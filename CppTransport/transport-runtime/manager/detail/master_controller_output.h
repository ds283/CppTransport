//
// Created by David Seery on 22/01/2016.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_OUTPUT_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_OUTPUT_H


#include "transport-runtime/manager/detail/master_controller_decl.h"

#include "transport-runtime/utilities/formatter.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"


namespace transport
  {

    using namespace master_controller_impl;

    template <typename number>
    void master_controller<number>::dispatch_output_task(output_task_record<number>& rec, const std::list<std::string>& tags)
      {
        // can't process a task if there are no workers
        if(this->world.size() <= 1) throw runtime_exception(exception_type::MPI_ERROR, CPPTRANSPORT_TOO_FEW_WORKERS);

        output_task<number>* tk = rec.get_task();

        this->work_scheduler.set_state_size(sizeof(number));
        this->work_scheduler.prepare_queue(*tk);

        // set up a derived_content_writer object to coordinate logging, output destination and commits into the repository.
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::unique_ptr< derived_content_writer<number> > writer = this->repo->new_output_task_content(rec, tags, this->get_rank(), this->world.size());
    
        // create new timer for this task; the BusyIdle_Context manager
        // ensures the timer is removed when the context manager is destroyed
        BusyIdle_Context timing_context(writer->get_name(), this->busyidle_timers);

        // set up the writer for us
        this->data_mgr->initialize_writer(*writer);

        // register writer with the repository -- allows its debris to be recovered later if a crash occurs
        this->repo->register_writer(*writer);

        // set up aggregators
        integration_aggregator<number>     i_agg;
        postintegration_aggregator<number> p_agg;
        derived_content_aggregator<number> d_agg(*this, *writer);

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), base_writer::log_severity_level::normal) << "++ NEW OUTPUT TASK '" << writer->get_name() << "@" << tk->get_name()
                                                                                  << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
        BOOST_LOG_SEV(writer->get_log(), base_writer::log_severity_level::normal) << *tk;

        // instruct workers to carry out their tasks
        bool success = this->output_task_to_workers(*writer, tags, i_agg, p_agg, d_agg, slave_work_event::event_type::begin_output_assignment, slave_work_event::event_type::end_output_assignment);

        journal_instrument instrument(this->journal, master_work_event::event_type::database_begin, master_work_event::event_type::database_end);

        // close the writer
        this->data_mgr->close_writer(*writer);

        // commit output to the database if successful; if not, the writer automatically aborts
        // committing (or aborting) the writer automatically deregisters it
        if(success) writer->commit();
      }


    template <typename number>
    bool master_controller<number>::output_task_to_workers(derived_content_writer<number>& writer, const std::list<std::string>& tags,
                                                           integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                                           slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        assert(this->repo != nullptr);

        // set up a timer to keep track of the total wallclock time used in this task
        boost::timer::cpu_timer wallclock_timer;

        // aggregate cache information
        integration_metadata   i_metadata;      // unused
        output_metadata        o_metadata = writer.get_metadata();
        std::list<std::string> content_groups;

        // get paths the workers will need
        boost::filesystem::path tempdir_path = writer.get_abs_tempdir_path();
        boost::filesystem::path logdir_path  = writer.get_abs_logdir_path();

        {
          // journal_instrument will log time spent doing MPI when it goes out of scope
          journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

          std::vector<boost::mpi::request> requests(this->world.size()-1);
          MPI::new_derived_content_payload payload(writer.get_task_name(), writer.get_name(), tempdir_path, logdir_path, tags);

          for(unsigned int i = 0; i < this->world.size()-1; ++i)
            {
              requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_DERIVED_CONTENT, payload);
            }

          // wait for all messages to be received
          boost::mpi::wait_all(requests.begin(), requests.end());
          BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ All workers received NEW_DERIVED_CONTENT instruction";
        }

        // we keep track of the global content group list, but content groups are also tracked
        // on a product-by-product basis as they are emplaced during the task
        bool success = this->poll_workers(i_agg, p_agg, d_agg, i_metadata, o_metadata, content_groups, writer, begin_label, end_label);

        writer.set_metadata(o_metadata);
        writer.set_content_groups(content_groups);
        wallclock_timer.stop();
        this->debrief_output(writer, o_metadata, wallclock_timer);

        return(success);
      }


    template <typename number>
    template <typename WriterObject>
    void master_controller<number>::debrief_output(WriterObject& writer, output_metadata& o_metadata, boost::timer::cpu_timer& wallclock_timer)
      {
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ TASK COMPLETE (at " << boost::posix_time::to_simple_string(now) << "): FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total wallclock time for task '" << writer.get_task_name() << "' " << format_time(wallclock_timer.elapsed().wall);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ AGGREGATE CACHE STATISTICS";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total work time required by worker processes      = " << format_time(o_metadata.work_time);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total aggregation time required by master process = " << format_time(o_metadata.aggregation_time);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total time spent reading database                 = " << format_time(o_metadata.db_time);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total time-configuration cache hits               = " << o_metadata.time_config_hits << ", unloads = " << o_metadata.time_config_unloads;
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total twopf k-config cache hits                   = " << o_metadata.twopf_kconfig_hits << ", unloads = " << o_metadata.twopf_kconfig_unloads;
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total threepf k-config cache hits                 = " << o_metadata.threepf_kconfig_hits << ", unloads = " << o_metadata.threepf_kconfig_unloads;
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total statistics cache hits                       = " << o_metadata.stats_hits << ", unloads = " << o_metadata.stats_unloads;
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total data cache hits                             = " << o_metadata.data_hits << ", unloads = " << o_metadata.data_unloads;
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total time config cache evictions                 = " << format_time(o_metadata.time_config_evictions);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total twopf k-config cache evictions              = " << format_time(o_metadata.twopf_kconfig_evictions);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total threepf k-config cache evictions            = " << format_time(o_metadata.threepf_kconfig_evictions);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total statistics cache evictions                  = " << format_time(o_metadata.stats_evictions);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total data cache evictions                        = " << format_time(o_metadata.data_evictions);
      }


    template <typename number>
    bool master_controller<number>::aggregate_content(derived_content_writer<number>& writer, unsigned int worker, unsigned int id,
                                                      MPI::content_ready_payload& payload, output_metadata& metadata)
      {
        journal_instrument instrument(this->journal, master_work_event::event_type::aggregate_begin, master_work_event::event_type::aggregate_end, id);

        // set up a timer to measure how long we spend aggregating
        boost::timer::cpu_timer aggregate_timer;

        bool success = writer.aggregate(payload.get_product_name(), payload.get_content_groups());

        aggregate_timer.stop();
        metadata.aggregation_time += aggregate_timer.elapsed().wall;

        // inform scheduler of a new aggregation
        this->work_scheduler.report_aggregation(aggregate_timer.elapsed().wall);

        return (success);
      }


    template <typename number>
    template <typename PayloadObject>
    void master_controller<number>::update_output_metadata(PayloadObject& payload, output_metadata& metadata)
      {
        metadata.work_time                 += payload.get_wallclock_time();
        metadata.db_time                   += payload.get_database_time();
        metadata.time_config_hits          += payload.get_time_config_hits();
        metadata.time_config_unloads       += payload.get_time_config_unloads();
        metadata.time_config_evictions     += payload.get_time_config_evictions();
        metadata.twopf_kconfig_hits        += payload.get_twopf_kconfig_hits();
        metadata.twopf_kconfig_unloads     += payload.get_twopf_kconfig_unloads();
        metadata.twopf_kconfig_evictions   += payload.get_twopf_kconfig_evictions();
        metadata.threepf_kconfig_hits      += payload.get_threepf_kconfig_hits();
        metadata.threepf_kconfig_unloads   += payload.get_threepf_kconfig_unloads();
        metadata.threepf_kconfig_evictions += payload.get_threepf_kconfig_evictions();
        metadata.stats_hits                += payload.get_stats_hits();
        metadata.stats_unloads             += payload.get_stats_unloads();
        metadata.stats_evictions           += payload.get_stats_evictions();
        metadata.data_hits                 += payload.get_data_hits();
        metadata.data_unloads              += payload.get_data_unloads();
        metadata.data_evictions            += payload.get_data_evictions();
      }


    template <typename number>
    template <typename PayloadObject>
    void master_controller<number>::update_content_group_list(PayloadObject& payload, std::list<std::string>& groups)
      {
        std::list<std::string> this_group = payload.get_content_groups();

        this_group.sort();
        groups.merge(this_group);
        groups.unique();
      }

  }   // namespace transport


#endif //CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_OUTPUT_H
