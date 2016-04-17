//
// Created by David Seery on 22/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_INTEGRATION_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_INTEGRATION_H


#include "transport-runtime-api/manager/detail/master_controller_decl.h"

#include "transport-runtime-api/utilities/formatter.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


namespace transport
  {

    using namespace master_controller_impl;

    template <typename number>
    void master_controller<number>::dispatch_integration_task(integration_task_record<number>& rec, bool seeded, const std::string& seed_group,
                                                              const std::list<std::string>& tags)
      {
        // can't process a task if there are no workers
        if(this->world.size() == 1) throw runtime_exception(exception_type::MPI_ERROR, CPPTRANSPORT_TOO_FEW_WORKERS);

        integration_task<number>* tk = rec.get_task();
        model<number>* m = rec.get_task()->get_model();

        twopf_task<number>* tka = nullptr;
        threepf_task<number>* tkb = nullptr;

        if((tka = dynamic_cast< twopf_task<number>* >(tk)) != nullptr)
          {
            this->work_scheduler.set_state_size(m->backend_twopf_state_size());
            this->work_scheduler.prepare_queue(*tka);
            this->schedule_integration(rec, tka, seeded, seed_group, tags, slave_work_event::event_type::begin_twopf_assignment, slave_work_event::event_type::end_twopf_assignment);
          }
        else if((tkb = dynamic_cast< threepf_task<number>* >(tk)) != nullptr)
          {
            this->work_scheduler.set_state_size(m->backend_threepf_state_size());
            this->work_scheduler.prepare_queue(*tkb);
            this->schedule_integration(rec, tkb, seeded, seed_group, tags, slave_work_event::event_type::begin_threepf_assignment, slave_work_event::event_type::end_threepf_assignment);
          }
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_UNKNOWN_DERIVED_TASK << " '" << rec.get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
      }


    template <typename number>
    template <typename TaskObject>
    void master_controller<number>::schedule_integration(integration_task_record<number>& rec, TaskObject* tk,
                                                         bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                                         slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        // create an output writer to commit the result of this integration to the repository.
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::unique_ptr< integration_writer<number> > writer = this->repo->new_integration_task_content(rec, tags, this->get_rank(), 0, this->world.size());

        // initialize the writer
        this->data_mgr->initialize_writer(*writer);

        // write the various tables needed in the database
        this->data_mgr->create_tables(*writer, tk);

        // seed writer if a group has been provided; resets the work queue if required
        if(seeded) this->seed_writer(*writer, tk, seed_group);

        // register writer with the repository -- allows its debris to be recovered later if a crash occurs
        this->repo->register_writer(*writer);

        // set up aggregators
        integration_aggregator<number>     i_agg(*this, *writer);
        postintegration_aggregator<number> p_agg;
        derived_content_aggregator<number> d_agg;

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), base_writer::log_severity_level::normal) << "++ NEW INTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
        BOOST_LOG_SEV(writer->get_log(), base_writer::log_severity_level::normal) << *tk;

        // instruct workers to carry out the calculation
        // this call returns when all workers have signalled that their work is done
        bool success = this->integration_task_to_workers(*writer, i_agg, p_agg, d_agg, begin_label, end_label);

        journal_instrument instrument(this->journal, master_work_event::event_type::database_begin, master_work_event::event_type::database_end);

        // perform integrity check; updates writer with a list of missing serial numbers, if needed
        writer->check_integrity(tk);

        // close the writer
        this->data_mgr->close_writer(*writer);

        // commit output if successful; integrity failures are ignored, so containers can subsequently be used as a seed
        // if the writer is not committed it automatically aborts
        // committing (or aborting) the writer automatically deregisters it
        if(success) writer->commit();
      }


    template <typename number>
    template <typename TaskObject>
    std::set<unsigned int> master_controller<number>::seed_writer(integration_writer<number>& writer, TaskObject* tk, const std::string& seed_group)
      {
        // enumerate the content groups available for our own task
        integration_content_db db = this->repo->enumerate_integration_task_content(tk->get_name());

        // find the specified group in this list
        integration_content_db::const_iterator t = std::find_if(db.begin(), db.end(), OutputGroupFinder<integration_payload>(seed_group));

        if(t == db.end())   // no record found
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A << " '" << seed_group << "' " << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B << " '" << tk->get_name() << "'";
            this->warn(msg.str());
            return std::set<unsigned int>();
          }

        // mark writer as seeded
        writer.set_seed(seed_group);

        // get workgroup number used by seed
        unsigned int seed_workgroup = t->second->get_payload().get_workgroup_number();
        writer.set_workgroup_number(seed_workgroup+1);

        this->data_mgr->seed_writer(writer, tk, *t->second);
        this->work_scheduler.prepare_queue(t->second->get_payload().get_failed_serials());

        return(t->second->get_payload().get_failed_serials());
      }


    template <typename number>
    bool master_controller<number>::integration_task_to_workers(integration_writer<number>& writer,
                                                                integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                                                slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        assert(this->repo != nullptr);

        // set up a timer to keep track of the total wallclock time used in this integration
        boost::timer::cpu_timer wallclock_timer;

        // aggregate integration data reported by worker processes
        integration_metadata   i_metadata;
        output_metadata        o_metadata;      // unused
        std::list<std::string> content_groups;  // unused

        // get paths the workers will need
        boost::filesystem::path tempdir_path = writer.get_abs_tempdir_path();
        boost::filesystem::path logdir_path  = writer.get_abs_logdir_path();

        {
          // journal_instrument will log time spent doing MPI when it goes out of scope
          journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

          std::vector<boost::mpi::request> requests(this->world.size()-1);
          MPI::new_integration_payload payload(writer.get_task_name(), tempdir_path, logdir_path, writer.get_workgroup_number());

          for(unsigned int i = 0; i < this->world.size()-1; ++i)
            {
              requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_INTEGRATION, payload);
            }

          // wait for all messages to be received
          boost::mpi::wait_all(requests.begin(), requests.end());
          BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ All workers received NEW_INTEGRATION instruction";
        }

        bool success = this->poll_workers(i_agg, p_agg, d_agg, i_metadata, o_metadata, content_groups, writer, begin_label, end_label);

        // set wallclock time in metadata to our measured time
        wallclock_timer.stop();
        i_metadata.total_wallclock_time = wallclock_timer.elapsed().wall;

        // push task metadata to the writer
        writer.set_metadata(i_metadata);

        this->debrief_integration(writer, i_metadata);

        return(success);
      }


    template <typename number>
    void master_controller<number>::debrief_integration(integration_writer<number>& writer, integration_metadata& i_metadata)
      {
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ TASK COMPLETE (at " << boost::posix_time::to_simple_string(now) << "): FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total wallclock time required for for task '" << writer.get_task_name() << "' " << format_time(i_metadata.total_wallclock_time);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total aggregation time required by master process = " << format_time(i_metadata.total_aggregation_time);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ AGGREGATE PERFORMANCE STATISTICS";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Workers processed " << i_metadata.total_configurations << " individual integrations";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   " << i_metadata.total_failures << " integrations failed, and " << i_metadata.total_refinements << " integrations required mesh refinement (may not match individual k-configurations for some backends)";
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total integration time    = " << format_time(i_metadata.total_integration_time) << " | global mean integration time = " << format_time(i_metadata.total_integration_time/(i_metadata.total_configurations > 0 ? i_metadata.total_configurations : 1));
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Global min integration time = " << format_time(i_metadata.global_min_integration_time) << " | global max integration time = " << format_time(i_metadata.global_max_integration_time);
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Total batching time       = " << format_time(i_metadata.total_batching_time) << " | global mean batching time = " << format_time(i_metadata.total_batching_time/(i_metadata.total_configurations > 0 ? i_metadata.total_configurations : 1));
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++   Global min batching time = " << format_time(i_metadata.global_min_batching_time) << " | global max batching time = " << format_time(i_metadata.global_max_batching_time);
      }


    template <typename number>
    void master_controller<number>::aggregate_integration(integration_writer<number>& writer, unsigned int worker, unsigned int id,
                                                          MPI::data_ready_payload& payload, integration_metadata &metadata)
      {
        journal_instrument instrument(this->journal, master_work_event::event_type::aggregate_begin, master_work_event::event_type::aggregate_end, id);

        // set up a timer to measure how long we spend aggregating
        boost::timer::cpu_timer aggregate_timer;

        boost::filesystem::path ctr_path = payload.get_container_path();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ Beginning aggregation of temporary container '" << ctr_path.filename().string() << "'";
        bool success = true;

        try
          {
            writer.aggregate(payload.get_container_path());
          }
        catch(runtime_exception& xe)
          {
            if(xe.get_exception_code() == exception_type::DATA_CONTAINER_ERROR)   // trap data container errors (eg SQLITE key constraints) during aggregation
              {
                success = false;
                writer.set_fail(true);
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::error) << "!! Failed to aggregate container '" << ctr_path.filename().string() << "': " << xe.what();
              }
            else
              {
                throw xe;   // pass on to higher exception handler
              }
          }

        aggregate_timer.stop();

        // if aggregation proceeded normally, carry out housekeeping
        if(success)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ Aggregated temporary container '" << ctr_path.filename().string() << "' in time " << format_time(aggregate_timer.elapsed().wall);
            metadata.total_aggregation_time += aggregate_timer.elapsed().wall;

            // inform scheduler of a new aggregation
            this->work_scheduler.report_aggregation(aggregate_timer.elapsed().wall);

            // remove temporary container
//        BOOST_LOG_SEV(writer->get_log(), base_writer::log_severity_level::normal) << "++ Deleting temporary container '" << payload.get_container_path() << "'";
            if(!boost::filesystem::remove(payload.get_container_path()))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_DATACTR_REMOVE_TEMP << " '" << payload.get_container_path() << "'";
                this->err(msg.str());
              }
          }
      }


    template <typename number>
    void master_controller<number>::update_integration_metadata(MPI::finished_integration_payload& payload, integration_metadata& metadata)
      {
        // don't update wallclock time; this is set to be the time taken to complete the job measured on the master node
        // it is fixed once the integration is complete, in integration_task_to_workers()

        metadata.total_integration_time += payload.get_integration_time();

        if(metadata.global_max_integration_time == 0 || payload.get_max_integration_time() > metadata.global_max_integration_time) metadata.global_max_integration_time = payload.get_max_integration_time();
        if(metadata.global_min_integration_time == 0 || payload.get_min_integration_time() < metadata.global_min_integration_time) metadata.global_min_integration_time = payload.get_min_integration_time();

        metadata.total_batching_time += payload.get_batching_time();

        if(metadata.global_max_batching_time == 0 || payload.get_max_batching_time() > metadata.global_max_batching_time) metadata.global_max_batching_time = payload.get_max_batching_time();
        if(metadata.global_min_batching_time == 0 || payload.get_min_batching_time() < metadata.global_min_batching_time) metadata.global_min_batching_time = payload.get_min_batching_time();

        metadata.total_configurations += payload.get_num_integrations();
        metadata.total_failures += payload.get_num_failures();
        metadata.total_refinements += payload.get_num_refinements();
      }

  }   // namespace transport


#endif //CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_INTEGRATION_H
