//
// Created by David Seery on 22/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_POSTINTEGRATION_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_POSTINTEGRATION_H


#include "transport-runtime-api/manager/detail/master_controller_decl.h"

#include "transport-runtime-api/utilities/formatter.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


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
    template <typename TaskObject>
    void master_controller<number>::schedule_postintegration(postintegration_task_record<number>& rec, TaskObject* tk,
                                                             bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                                             slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        // create an output writer to commit our results into the repository
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::unique_ptr< postintegration_writer<number> > writer = this->repo->new_postintegration_task_content(rec, tags, this->get_rank());

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
        BOOST_LOG_SEV(writer->get_log(), base_writer::log_severity_level::normal) << "++ NEW POSTINTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
        BOOST_LOG_SEV(writer->get_log(), base_writer::log_severity_level::normal) << *tk;

        // instruct workers to carry out the calculation
        bool success = this->postintegration_task_to_workers(*writer, tags, i_agg, p_agg, d_agg, begin_label, end_label);

        journal_instrument instrument(this->journal, master_work_event::event_type::database_begin, master_work_event::event_type::database_end);

        // perform integrity check; updates writer with a valid list of missing serial numbers if needed
        writer->check_integrity(tk);

        // close the writer
        this->data_mgr->close_writer(*writer);

        // commit output if successful; integrity failures are ignored, so containers can subsequently be used as a seed
        if(success) writer->commit();
      }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject>
    void master_controller<number>::schedule_paired_postintegration(postintegration_task_record<number>& rec, TaskObject* tk, ParentTaskObject* ptk,
                                                                    bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                                                    slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        std::unique_ptr< task_record<number> > pre_prec = this->repo->query_task(ptk->get_name());
        integration_task_record<number>* prec = dynamic_cast< integration_task_record<number>* >(pre_prec.get());

        assert(prec != nullptr);
        if(prec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

        // create an output writer for the postintegration task
        std::unique_ptr< postintegration_writer<number> > p_writer = this->repo->new_postintegration_task_content(rec, tags, this->get_rank());
        this->data_mgr->initialize_writer(*p_writer);
        this->data_mgr->create_tables(*p_writer, tk);

        // create an output writer for the integration task; use suffix option to add "-paired" to distinguish the different output groups
        std::unique_ptr<integration_writer<number> > i_writer = this->repo->new_integration_task_content(*prec, tags, this->get_rank(), 0, "paired");
        this->data_mgr->initialize_writer(*i_writer);
        this->data_mgr->create_tables(*i_writer, ptk);

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
        BOOST_LOG_SEV(i_writer->get_log(), base_writer::log_severity_level::normal) << "++ NEW PAIRED POSTINTEGRATION TASKS '" << tk->get_name() << "' & '" << ptk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
        BOOST_LOG_SEV(i_writer->get_log(), base_writer::log_severity_level::normal) << *ptk;

        // instruct workers to carry out the calculation
        bool success = this->paired_postintegration_task_to_workers(*i_writer, *p_writer, tags, i_agg, p_agg, d_agg, begin_label, end_label);

        journal_instrument instrument(this->journal, master_work_event::event_type::database_begin, master_work_event::event_type::database_end);

        // perform integrity check
        // the integrity check updates each writer with a valid list of missing serial numbers, if needed
        i_writer->check_integrity(ptk);
        p_writer->check_integrity(tk);

        // ensure missing serial numbers are synchronized
        this->data_mgr->synchronize_missing_serials(*i_writer, *p_writer, ptk, tk);

        // close both writers
        this->data_mgr->close_writer(*i_writer);
        this->data_mgr->close_writer(*p_writer);

        // commit output if successful; integrity failures are ignored, so containers can subsequently be used as a seed
        if(success)
          {
            i_writer->commit();
            p_writer->commit();
          }
      }


    template <typename number>
    template <typename TaskObject>
    std::list<unsigned int> master_controller<number>::seed_writer(postintegration_writer<number>& writer, TaskObject* tk, const std::string& seed_group)
      {
        // enumerate the content groups available for our own task
        std::list< std::unique_ptr< output_group_record<postintegration_payload> > > list = this->repo->enumerate_postintegration_task_content(tk->get_name());

        // find the specified group in this list
        std::list< std::unique_ptr< output_group_record<postintegration_payload> > >::const_iterator t = std::find_if(list.begin(), list.end(),
                                                                                                                      OutputGroupFinder<postintegration_payload>(seed_group));

        if(t == list.end())   // no record found
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A << " '" << seed_group << "' " << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B << " '" << tk->get_name() << "'";
            this->warn(msg.str());
            return std::list<unsigned int>{};
          };

        // mark writer as seeded
        writer.set_seed(seed_group);

        this->data_mgr->seed_writer(writer, tk, *(*t));
        this->work_scheduler.prepare_queue((*t)->get_payload().get_failed_serials());

        return((*t)->get_payload().get_failed_serials());
      }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject>
    std::list<unsigned int> master_controller<number>::seed_writer_pair(integration_writer<number>& i_writer,
                                                                        postintegration_writer<number>& p_writer,
                                                                        TaskObject* tk, ParentTaskObject* ptk, const std::string& seed_group)
      {
        // enumerate the content groups available for our own task
        std::list< std::unique_ptr< output_group_record<postintegration_payload> > > list = this->repo->enumerate_postintegration_task_content(tk->get_name());

        // find the specified group in this list
        std::list< std::unique_ptr< output_group_record<postintegration_payload> > >::const_iterator t = std::find_if(list.begin(), list.end(),
                                                                                                                      OutputGroupFinder<postintegration_payload>(seed_group));

        if(t == list.end())   // no record found
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A << " '" << seed_group << "' " << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B << " '" << tk->get_name() << "'";
            this->warn(msg.str());
            return std::list<unsigned int>{};
          }

        // find parent content group for the seed
        std::string parent_seed_name = (*t)->get_payload().get_parent_group();

        // check that same k-configurations are missing from both content groups in the pair
        // currently we assume this to be true, although the integrity check for paired writers
        // doesn't enforce it (yet)
        std::list<unsigned int> integration_serials = this->seed_writer(i_writer, ptk, parent_seed_name);

        if(i_writer.is_seeded())
          {
            std::list<unsigned int> postintegration_serials = (*t)->get_payload().get_failed_serials();

            // minimal check is that each content group is missing the same number of serial numbers
            if(postintegration_serials.size() != integration_serials.size())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_A << " '" << (*t)->get_name() << "' "
                << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_B << " '" << parent_seed_name << "' "
                << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_C;
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
              }

            // now check more carefully that the missing serial numbers are the same
            std::list<unsigned int> diff;
            std::set_difference(integration_serials.begin(), integration_serials.end(),
                                postintegration_serials.begin(), postintegration_serials.end(), std::back_inserter(diff));

            if(diff.size() > 0)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_A << " '" << (*t)->get_name() << "' "
                << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_B << " '" << parent_seed_name << "' "
                << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_C;
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
              }

            p_writer.set_seed(seed_group);
            this->data_mgr->seed_writer(p_writer, tk, *(*t));
          }

        return((*t)->get_payload().get_failed_serials());
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
        output_metadata        o_metadata;
        std::list<std::string> content_groups;

        // get paths the workers will need
        boost::filesystem::path tempdir_path = writer.get_abs_tempdir_path();
        boost::filesystem::path logdir_path  = writer.get_abs_logdir_path();

        {
          // journal_instrument will log time spent doing MPI when it goes out of scope
          journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

          std::vector<boost::mpi::request> requests(this->world.size()-1);
          MPI::new_postintegration_payload payload(writer.get_task_name(), tempdir_path, logdir_path, tags);

          for(unsigned int i = 0; i < this->world.size()-1; ++i)
            {
              requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_POSTINTEGRATION, payload);
            }

          // wait for all messages to be received
          boost::mpi::wait_all(requests.begin(), requests.end());
          BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ All workers received NEW_POSTINTEGRATION instruction";
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
        integration_metadata   i_metadata;
        output_metadata        o_metadata;
        std::list<std::string> content_groups;  // unused

        // get paths needed by different batchers on each worker
        boost::filesystem::path i_tempdir_path = i_writer.get_abs_tempdir_path();
        boost::filesystem::path i_logdir_path  = i_writer.get_abs_logdir_path();

        boost::filesystem::path p_tempdir_path = p_writer.get_abs_tempdir_path();
        boost::filesystem::path p_logdir_path  = p_writer.get_abs_logdir_path();

        {
          // journal instrument will log time spent doing MPI when it goes out of scope
          journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

          std::vector<boost::mpi::request> requests(this->world.size()-1);
          MPI::new_postintegration_payload payload(p_writer.get_task_name(), p_tempdir_path, p_logdir_path, tags, i_tempdir_path, i_logdir_path, i_writer.get_workgroup_number());

          for(unsigned int i = 0; i < this->world.size()-1; ++i)
            {
              requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_POSTINTEGRATION, payload);
            }

          // wait for all messages to be received
          boost::mpi::wait_all(requests.begin(), requests.end());
          BOOST_LOG_SEV(i_writer.get_log(), base_writer::log_severity_level::normal) << "++ All workers received NEW_POSTINTEGRATION instruction";
        }

        bool success = this->poll_workers(i_agg, p_agg, d_agg, i_metadata, o_metadata, content_groups, i_writer, begin_label, end_label);

        if(content_groups.size() > 1) throw runtime_exception(exception_type::RUNTIME_ERROR,  CPPTRANSPORT_POSTINTEGRATION_MULTIPLE_GROUPS);

        i_writer.set_metadata(i_metadata);
        p_writer.set_metadata(o_metadata);

        wallclock_timer.stop();
        this->debrief_integration(i_writer, i_metadata, wallclock_timer);

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
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "++ Beginning aggregation of temporary container '" << ctr_path.filename().string() << "'";
        bool success = true;

        try
          {
            writer.aggregate(payload.get_container_path());
          }
        catch(runtime_exception& xe)
          {
            if(xe.get_exception_code() == exception_type::DATA_CONTAINER_ERROR)   // trap data container errors (eg SQLite key constraints) during aggregation
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
            metadata.aggregation_time += aggregate_timer.elapsed().wall;

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

  }   // namespace transport


#endif //CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_POSTINTEGRATION_H
