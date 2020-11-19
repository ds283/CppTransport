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

#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_POSTINTEGRATION_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_POSTINTEGRATION_H


#include "transport-runtime/manager/detail/master_controller_decl.h"

#include "transport-runtime/utilities/formatter.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"


namespace transport
  {

    template <typename number>
    void master_controller<number>::dispatch_postintegration_task(postintegration_task_record<number>& rec, bool seeded, const std::string& seed_group,
                                                                  const std::list<std::string>& tags)
      {
        // can't process a task if there are no workers
        if(this->world.size() <= 1) throw runtime_exception(exception_type::MPI_ERROR, CPPTRANSPORT_TOO_FEW_WORKERS);

        postintegration_task<number>* tk = rec.get_task();

        zeta_twopf_task<number>*   z2pf = nullptr;
        zeta_threepf_task<number>* z3pf = nullptr;
        fNL_task<number>*          zfNL = nullptr;

        if((z2pf = dynamic_cast< zeta_twopf_task<number>* >(tk)) != nullptr)
          {
            twopf_task<number>* ptk = dynamic_cast<twopf_task<number>*>(z2pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_TWOPF_TASK << " '" << z2pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
              }

            // is this 2pf task paired?
            if(z2pf->is_paired())
              {
                model<number>* m = ptk->get_model();
                this->work_scheduler.set_state_size(m->backend_twopf_state_size());
                this->work_scheduler.prepare_queue(*ptk);
                this->schedule_paired_postintegration(rec, z2pf, ptk, seeded, seed_group, tags, slave_work_event::event_type::begin_twopf_assignment, slave_work_event::event_type::end_twopf_assignment);
              }
            else
              {
                this->validate_content_group(ptk, tags);    // ensure a suitable content group is attached to the parent task before trying to schedule this postintegration
                this->work_scheduler.set_state_size(sizeof(number));
                this->work_scheduler.prepare_queue(*ptk);
                this->schedule_postintegration(rec, z2pf, seeded, seed_group, tags, slave_work_event::event_type::begin_zeta_twopf_assignment, slave_work_event::event_type::end_zeta_twopf_assignment);
              }
          }
        else if((z3pf = dynamic_cast< zeta_threepf_task<number>* >(tk)) != nullptr)
          {
            threepf_task<number>* ptk = dynamic_cast<threepf_task<number>*>(z3pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_THREEPF_TASK << " '" << z3pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
              }

            // is this 3pf task apired?
            if(z3pf->is_paired())
              {
                model<number>* m = ptk->get_model();
                this->work_scheduler.set_state_size(m->backend_threepf_state_size());
                this->work_scheduler.prepare_queue(*ptk);
                this->schedule_paired_postintegration(rec, z3pf, ptk, seeded, seed_group, tags, slave_work_event::event_type::begin_threepf_assignment, slave_work_event::event_type::end_threepf_assignment);
              }
            else
              {
                this->validate_content_group(ptk, tags);    // ensure a suitable content group is attached to the parent task before trying to schedule this postintegration
                this->work_scheduler.set_state_size(sizeof(number));
                this->work_scheduler.prepare_queue(*ptk);
                this->schedule_postintegration(rec, z3pf, seeded, seed_group, tags, slave_work_event::event_type::begin_zeta_threepf_assignment, slave_work_event::event_type::end_zeta_threepf_assignment);
              }
          }
        else if((zfNL = dynamic_cast< fNL_task<number>* >(tk)) != nullptr)
          {
            zeta_threepf_task<number>* ptk = dynamic_cast<zeta_threepf_task<number>*>(zfNL->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_ZETA_THREEPF_TASK << " '" << zfNL->get_parent_task()->get_name() << "'";
                throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
              }

            this->validate_content_group(ptk, tags);    // ensure a suitable content group is attached to the parent task before trying to schedule this postintegration
            this->work_scheduler.set_state_size(sizeof(number));
            this->work_scheduler.prepare_queue(*ptk);
            this->schedule_postintegration(rec, zfNL, false, "", tags, slave_work_event::event_type::begin_fNL_assignment, slave_work_event::event_type::end_fNL_assignment);
          }
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_UNKNOWN_DERIVED_TASK << " '" << rec.get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
      }


    template <typename number>
    void master_controller<number>::validate_content_group(integration_task<number>* tk, const std::list<std::string>& tags)
      {
        assert(tk != nullptr);

        integration_content_finder<number> finder(*this->repo);

        // check whether finder can locate a suitable content group for this parent task;
        // if not, an exception will be thrown which is caught back in the main task processing loop, so this task will be aborted
        auto group = finder(tk->get_name(), tags);
      }


    template <typename number>
    void master_controller<number>::validate_content_group(postintegration_task<number>* tk, const std::list<std::string>& tags)
      {
        assert(tk != nullptr);

        postintegration_content_finder<number> finder(*this->repo);

        // check whether finder can locate a suitable content group for this parent task;
        // if not, an exception will be thrown which is caught back in the main task processing loop, so this task will be aborted
        auto group = finder(tk->get_name(), tags);
      }


    template <typename number>
    template <typename TaskObject>
    void master_controller<number>::schedule_postintegration(postintegration_task_record<number>& rec, TaskObject* tk,
                                                             bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                                             slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        // create an output writer to commit our results into the repository
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        auto writer = this->repo->new_postintegration_task_content(rec, tags, this->get_rank(), this->world.size());
    
        // create new timer for this task; the BusyIdle_Context manager
        // ensures the timer is removed when the context manager is destroyed
        BusyIdle_Context timing_context(writer->get_name(), this->busyidle_timers);

        // initialize the writer
        this->data_mgr->initialize_writer(*writer);

        // create new tables needed in the database
        this->data_mgr->create_tables(*writer, tk);

        // seed writer if a group has been provided; resets the work queue if required
        if(seeded) this->seed_writer(*writer, tk, seed_group);

        // register writer with the repository -- allows its debris to be recovered later if a crash occurs
        this->repo->register_writer(*writer);

        // set up aggregators
        integration_aggregator<number>     i_agg;
        postintegration_aggregator<number> p_agg(*this, *writer);
        derived_content_aggregator<number> d_agg;

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), base_writer::log_severity_level::normal) << "++ NEW POSTINTEGRATION TASK '" << writer->get_name() << "@" << tk->get_name()
                                                                                  << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
        BOOST_LOG_SEV(writer->get_log(), base_writer::log_severity_level::normal) << *tk;

        // instruct workers to carry out the calculation
        bool success = this->postintegration_task_to_workers(*writer, tags, i_agg, p_agg, d_agg, begin_label, end_label);

        // close the writer; performs integrity check and finalization step
        journal_instrument instrument(this->journal, master_work_event::event_type::database_begin, master_work_event::event_type::database_end);
        this->data_mgr->close_writer(*writer);
        this->aggregation_profiles.push_back(std::move(writer->get_aggregation_profiler()));

        // commit output if successful; integrity failures are ignored, so containers can subsequently be used as a seed
        if(success || this->arg_cache.get_commit_failed() == true) writer->commit();
      }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject>
    void master_controller<number>::schedule_paired_postintegration(postintegration_task_record<number>& rec, TaskObject* tk, ParentTaskObject* ptk,
                                                                    bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                                                    slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        auto pre_prec = this->repo->query_task(ptk->get_name());
        integration_task_record<number>* prec = dynamic_cast< integration_task_record<number>* >(pre_prec.get());

        assert(prec != nullptr);
        if(prec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

        // check whether the parent integration task has a default checkpoint interval
        // if so, instruct workers to change their interval unless we have been overriden by the command line
        Checkpoint_Context<number> checkpoint_context(*this);
        if(ptk->has_default_checkpoint() && this->arg_cache.get_checkpoint_interval() == 0)
          {
            boost::optional<unsigned int> interval = ptk->get_default_checkpoint();
            if(interval)
              {
                checkpoint_context.requires_unset();
                this->set_local_checkpoint_interval(*interval);
              }
          }

        // create an output writer for the postintegration task
        auto p_writer = this->repo->new_postintegration_task_content(rec, tags, this->get_rank(), this->world.size());
        this->data_mgr->initialize_writer(*p_writer);
        this->data_mgr->create_tables(*p_writer, tk);

        // create an output writer for the integration task; use suffix option to add "-paired" to distinguish the different content groups
        auto i_writer = this->repo->new_integration_task_content(*prec, tags, this->get_rank(), 0, this->world.size(), "paired");
        this->data_mgr->initialize_writer(*i_writer);
        this->data_mgr->create_tables(*i_writer, ptk);
    
        // create new timer for this task; the BusyIdle_Context manager
        // ensures the timer is removed when the context manager is destroyed
        // note that the timer gets the name of the integration content group, not the postintegration content group
        // this is because it's the integration writer that is passed to poll_workers(), and which will be used to interrogate
        // for load data during execution of the task
        BusyIdle_Context timing_context(i_writer->get_name(), this->busyidle_timers);

        // pair
        p_writer->set_pair(true);
        p_writer->set_parent_group(i_writer->get_name());

        // seed writers if a group has been provided; resets the work queue if required
        if(seeded) this->seed_writer_pair(*i_writer, *p_writer, tk, ptk, seed_group);

        // register writers with the repository -- allows their debris to be recovered later if a crash occurs
        this->repo->register_writer(*i_writer);
        this->repo->register_writer(*p_writer);

        // set up aggregators
        integration_aggregator<number>     i_agg(*this, *i_writer);
        postintegration_aggregator<number> p_agg(*this, *p_writer);
        derived_content_aggregator<number> d_agg;

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(i_writer->get_log(), base_writer::log_severity_level::normal) << "++ NEW PAIRED POSTINTEGRATION TASKS '" << p_writer->get_name() << "@" << tk->get_name()
                                                                                    << "' & '" << i_writer->get_name() << "@" << ptk->get_name()
                                                                                    << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
        BOOST_LOG_SEV(i_writer->get_log(), base_writer::log_severity_level::normal) << *ptk;

        // instruct workers to carry out the calculation
        bool success = this->paired_postintegration_task_to_workers(*i_writer, *p_writer, tags, i_agg, p_agg, d_agg, begin_label, end_label);

        // close both writers; performs integrity check, synchronizes missing serial numbers and performs finalization step
        journal_instrument instrument(this->journal, master_work_event::event_type::database_begin, master_work_event::event_type::database_end);
        this->data_mgr->close_writer(*i_writer, *p_writer);
        this->aggregation_profiles.push_back(std::move(i_writer->get_aggregation_profiler()));
        this->aggregation_profiles.push_back(std::move(p_writer->get_aggregation_profiler()));

        // commit output if successful; integrity failures are ignored, so containers can subsequently be used as a seed
        // if the writers are not committed they automatically abort
        // committing (or aborting) the writers automatically deregisters them
        if(success || this->arg_cache.get_commit_failed() == true)
          {
            i_writer->commit();
            p_writer->commit();
          }
      }


    template <typename number>
    template <typename TaskObject>
    std::set<unsigned int> master_controller<number>::seed_writer(postintegration_writer<number>& writer, TaskObject* tk, const std::string& seed_group)
      {
        // enumerate the content groups available for our own task
        postintegration_content_db db = this->repo->enumerate_postintegration_task_content(tk->get_name());

        // find the specified group in this list, by name
        auto t = std::find_if(db.begin(), db.end(), OutputGroupFinder<postintegration_payload>(seed_group));

        if(t == db.end())   // no record found
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A << " '" << seed_group << "' " << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B << " '" << tk->get_name() << "'";
            throw runtime_exception(exception_type::SEEDING_ERROR, msg.str());
          };

        // mark writer as seeded
        writer.set_seed(seed_group);

        // seeded writer inherits metadata from its seed content group
        writer.set_metadata(t->second->get_payload().get_metadata());

        this->data_mgr->seed_writer(writer, tk, *(t->second));
        this->work_scheduler.prepare_queue(t->second->get_payload().get_failed_serials());

        return(t->second->get_payload().get_failed_serials());
      }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject>
    std::set<unsigned int> master_controller<number>::seed_writer_pair(integration_writer<number>& i_writer,
                                                                        postintegration_writer<number>& p_writer,
                                                                        TaskObject* tk, ParentTaskObject* ptk, const std::string& seed_group)
      {
        // enumerate the content groups available for our own task
        postintegration_content_db db = this->repo->enumerate_postintegration_task_content(tk->get_name());

        // find the specified group in this list
        auto t = std::find_if(db.begin(), db.end(), OutputGroupFinder<postintegration_payload>(seed_group));

        if(t == db.end())   // no record found
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A << " '" << seed_group << "' " << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B << " '" << tk->get_name() << "'";
            throw runtime_exception(exception_type::SEEDING_ERROR, msg.str());
          }

        // find parent content group for the seed
        std::string parent_seed_name = t->second->get_payload().get_parent_group();

        // check that same k-configurations are missing from both content groups in the pair
        // currently we assume this to be true, although the integrity check for paired writers
        // doesn't enforce it (yet)
        // note, this->seed_writer() will make i_writer inherit metadata from the seed content group
        std::set<unsigned int> integration_serials = this->seed_writer(i_writer, ptk, parent_seed_name);

        if(i_writer.is_seeded())
          {
            std::set<unsigned int> postintegration_serials = t->second->get_payload().get_failed_serials();

            // minimal check is that each content group is missing the same number of serial numbers
            if(postintegration_serials.size() != integration_serials.size())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_A << " '" << t->second->get_name() << "' "
                << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_B << " '" << parent_seed_name << "' "
                << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_C;
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
              }

            // now check more carefully that the missing serial numbers are the same
            std::set<unsigned int> diff;
            std::set_difference(integration_serials.begin(), integration_serials.end(),
                                postintegration_serials.begin(), postintegration_serials.end(), std::inserter(diff, diff.begin()));

            if(diff.size() > 0)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_A << " '" << t->second->get_name() << "' "
                << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_B << " '" << parent_seed_name << "' "
                << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_C;
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
              }

            p_writer.set_seed(seed_group);

            // seeded writer inherits metadata from its seed content group
            p_writer.set_metadata(t->second->get_payload().get_metadata());

            this->data_mgr->seed_writer(p_writer, tk, *(t->second));
          }

        return(t->second->get_payload().get_failed_serials());
      }


    template <typename number>
    bool master_controller<number>::postintegration_task_to_workers(postintegration_writer<number>& writer, const std::list<std::string>& tags,
                                                                    integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                                                    slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        assert(this->repo != nullptr);

        // set up a timer to keep track of the total wallclock time used
        boost::timer::cpu_timer wallclock_timer;

        // aggregate cache information
        integration_metadata   i_metadata;  // unused
        output_metadata        o_metadata = writer.get_metadata();
        std::list<std::string> content_groups;

        // get paths the workers will need
        boost::filesystem::path tempdir_path = writer.get_abs_tempdir_path();
        boost::filesystem::path logdir_path  = writer.get_abs_logdir_path();

        {
          // journal_instrument will log time spent doing MPI when it goes out of scope
          journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

          std::vector<boost::mpi::request> requests(this->world.size()-1);
          MPI::new_postintegration_payload payload(writer.get_task_name(), writer.get_name(), tempdir_path, logdir_path, tags);

          for(unsigned int i = 0; i < this->world.size()-1; ++i)
            {
              requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_POSTINTEGRATION, payload);
            }

          // wait for all messages to be received
          boost::mpi::wait_all(requests.begin(), requests.end());
          BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
            << "++ All workers received NEW_POSTINTEGRATION instruction";
        }

        bool success = this->poll_workers(i_agg, p_agg, d_agg, i_metadata, o_metadata, content_groups, writer, begin_label, end_label);

        if(content_groups.size() > 1) throw runtime_exception(exception_type::RUNTIME_ERROR,  CPPTRANSPORT_POSTINTEGRATION_MULTIPLE_GROUPS);

        writer.set_parent_group(content_groups.front());
        writer.set_metadata(o_metadata);
        wallclock_timer.stop();
        this->debrief_output(writer, o_metadata, wallclock_timer);

        return(success);
      }


    template <typename number>
    bool master_controller<number>::paired_postintegration_task_to_workers(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer,
                                                                           const std::list<std::string>& tags,
                                                                           integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                                                           slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        assert(this->repo != nullptr);

        // set up a timer to keep track of the total wallclock time used
        boost::timer::cpu_timer wallclock_timer;

        // aggregate cache information
        integration_metadata   i_metadata = i_writer.get_metadata();
        output_metadata        o_metadata = p_writer.get_metadata();
        std::list<std::string> content_groups;  // unused

        // reset count of failed configurations (can be nonzero if inherited from a seed group)
        i_metadata.total_failures = 0;

        // get paths needed by different batchers on each worker
        boost::filesystem::path i_tempdir_path = i_writer.get_abs_tempdir_path();
        boost::filesystem::path i_logdir_path  = i_writer.get_abs_logdir_path();

        boost::filesystem::path p_tempdir_path = p_writer.get_abs_tempdir_path();
        boost::filesystem::path p_logdir_path  = p_writer.get_abs_logdir_path();

        {
          // journal instrument will log time spent doing MPI when it goes out of scope
          journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

          std::vector<boost::mpi::request> requests(this->world.size()-1);
          MPI::new_postintegration_payload payload(p_writer.get_task_name(), p_writer.get_name(), p_tempdir_path, p_logdir_path, tags, i_tempdir_path, i_logdir_path, i_writer.get_workgroup_number());

          for(unsigned int i = 0; i < this->world.size()-1; ++i)
            {
              requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_POSTINTEGRATION, payload);
            }

          // wait for all messages to be received
          boost::mpi::wait_all(requests.begin(), requests.end());
          BOOST_LOG_SEV(i_writer.get_log(), base_writer::log_severity_level::notification)
            << "++ All workers received NEW_POSTINTEGRATION instruction";
        }

        bool success = this->poll_workers(i_agg, p_agg, d_agg, i_metadata, o_metadata, content_groups, i_writer, begin_label, end_label);

        if(content_groups.size() > 1) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_POSTINTEGRATION_MULTIPLE_GROUPS);

        // set wallclock time in integration metadata to our measured time
        wallclock_timer.stop();
        i_metadata.total_wallclock_time = wallclock_timer.elapsed().wall;

        // push task metadata to the writers
        i_writer.set_metadata(i_metadata);
        p_writer.set_metadata(o_metadata);

        this->debrief_integration(i_writer, i_metadata);

        return(success);
      }


    template <typename number>
    void master_controller<number>::aggregate_postprocess(postintegration_writer<number>& writer, unsigned int worker, unsigned int id,
                                                          MPI::data_ready_payload& payload, output_metadata& metadata)
      {
        journal_instrument instrument(this->journal, master_work_event::event_type::aggregate_begin, master_work_event::event_type::aggregate_end, id);

        // set up a timer to measure how long we spend batching
        boost::timer::cpu_timer aggregate_timer;

        boost::filesystem::path ctr_path = payload.get_container_path();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "++ Beginning aggregation of temporary container '" << ctr_path.filename().string() << "'";
        bool success = true;

        try
          {
            writer.aggregate(ctr_path);
          }
        catch(runtime_exception& xe)
          {
            if(xe.get_exception_code() == exception_type::DATA_CONTAINER_ERROR)   // trap data container errors (eg SQLite key constraints) during aggregation
              {
                success = false;
                writer.set_fail(true);
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::error) << "!! Failed to aggregate container '" << ctr_path.filename().string() << "': " << xe.what();
              }
            else throw;
          }

        aggregate_timer.stop();

        // if aggregation proceeded normally, carry out housekeeping
        if(success)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ Aggregated temporary container '" << ctr_path.filename().string() << "' in time " << format_time(aggregate_timer.elapsed().wall);
            metadata.aggregation_time += aggregate_timer.elapsed().wall;

            // inform scheduler of a new aggregation
            this->work_scheduler.report_aggregation(aggregate_timer.elapsed().wall);

            // remove temporary container
//        BOOST_LOG_SEV(writer->get_log(), base_writer::log_severity_level::normal) << "++ Deleting temporary container '" << payload.get_container_path() << "'";
#ifndef CPPPTRANSPORT_NO_REMOVE_INTERMEDIATE_CONTAINERS
            if(!boost::filesystem::remove(ctr_path))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_DATACTR_REMOVE_TEMP << " '" << ctr_path.string() << "'";
                this->err(msg.str());
              }
#endif
          }
      }

  }   // namespace transport


#endif //CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_POSTINTEGRATION_H
