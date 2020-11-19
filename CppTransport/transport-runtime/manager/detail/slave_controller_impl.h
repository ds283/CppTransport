//
// Created by David Seery on 26/01/2016.
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

#ifndef CPPTRANSPORT_SLAVE_CONTROLLER_IMPL_H
#define CPPTRANSPORT_SLAVE_CONTROLLER_IMPL_H


#include "transport-runtime/manager/detail/slave_controller_decl.h"
#include "transport-runtime/manager/detail/slave_message_buffer.h"

#include "transport-runtime/defaults.h"


namespace transport
  {

    template <typename number>
    slave_controller<number>::slave_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
                                               local_environment& le, argument_cache& ac, model_manager<number>& f)
      : environment(e),
        world(w),
        local_env(le),
        arg_cache(ac),
        finder(f),
        data_mgr(data_manager_factory<number>(le, ac)),
        err(error_handler(le, ac)),
        warn(warning_handler(le, ac)),
        msg(message_handler(le, ac))
      {
        // create global busy/idle timer
        busyidle_timers.add_new_timer(CPPTRANSPORT_DEFAULT_TIMER);
      }


    template <typename number>
    void slave_controller<number>::wait_for_tasks()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        if(this->get_rank() == 0) throw runtime_exception(exception_type::MPI_ERROR, CPPTRANSPORT_WAIT_MASTER);

        bool finished = false;

        while(!finished)
          {
            // wait until a message is available from master, tracking the time we spend waiting
            timers.idle();
            boost::mpi::status stat = this->world.probe(MPI::RANK_MASTER);

            timers.busy();
            switch(stat.tag())
              {
                case MPI::NEW_INTEGRATION:
                  {
                    MPI::new_integration_payload payload;
                    this->world.recv(MPI::RANK_MASTER, MPI::NEW_INTEGRATION, payload);
                    this->process_task(payload);
                    break;
                  }

                case MPI::NEW_DERIVED_CONTENT:
                  {
                    MPI::new_derived_content_payload payload;
                    this->world.recv(MPI::RANK_MASTER, MPI::NEW_DERIVED_CONTENT, payload);
                    this->process_task(payload);
                    break;
                  }

                case MPI::NEW_POSTINTEGRATION:
                  {
                    MPI::new_postintegration_payload payload;
                    this->world.recv(MPI::RANK_MASTER, MPI::NEW_POSTINTEGRATION, payload);
                    this->process_task(payload);
                    break;
                  }

                case MPI::WORKER_SETUP:
                  {
                    MPI::slave_setup_payload payload;
                    this->world.recv(MPI::RANK_MASTER, MPI::WORKER_SETUP, payload);
                    this->initialize(payload);
                    break;
                  }

                case MPI::SET_LOCAL_CHECKPOINT:
                  {
                    unsigned int payload;
                    this->world.recv(MPI::RANK_MASTER, MPI::SET_LOCAL_CHECKPOINT, payload);
                    if(this->data_mgr) this->data_mgr->set_local_checkpoint_interval(payload);
                    break;
                  }

                case MPI::UNSET_LOCAL_CHECKPOINT:
                  {
                    this->world.recv(MPI::RANK_MASTER, MPI::UNSET_LOCAL_CHECKPOINT);
                    if(this->data_mgr) this->data_mgr->unset_local_checkpoint_interval();
                    break;
                  }
                
                case MPI::QUERY_PERFORMANCE_DATA:
                  {
                    this->world.recv(MPI::RANK_MASTER, MPI::QUERY_PERFORMANCE_DATA);
                     
                    MPI::performance_data_payload payload(this->busyidle_timers.get_load_average(CPPTRANSPORT_DEFAULT_TIMER));
                    boost::mpi::request msg = this->world.isend(MPI::RANK_MASTER, MPI::REPORT_PERFORMANCE_DATA, payload);
                    msg.wait();
                  }

                case MPI::TERMINATE:
                  {
                    this->world.recv(MPI::RANK_MASTER, MPI::TERMINATE);
                    finished = true;
                    break;
                  }

                default:
                  {
                    throw runtime_exception(exception_type::MPI_ERROR, CPPTRANSPORT_UNEXPECTED_MPI);
                  }
              }
          }
      }


    template <typename number>
    void slave_controller<number>::initialize(const MPI::slave_setup_payload& payload)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        try
          {
            boost::filesystem::path repo_path = payload.get_repository_path();

            // set up a repository
            this->repo = repository_factory<number>(repo_path.string(), this->finder, repository_mode::readonly, this->local_env, this->arg_cache);

            // overwrite current argument cache (by copy assignment) with the one provided in the payload
            // changes in the batcher/pipe capacities and the checkpoint interval will be visible to
            // the data manager, because it has a reference to the arg_cache member
            this->arg_cache = payload.get_argument_cache();

            // set names of external executables in local environment; note this will invalidate the locations
            // of any already-detected executables
            this->local_env.set_python_executable(this->arg_cache.get_python_executable());
            this->local_env.set_dot_executable(this->arg_cache.get_dot_executable());
            this->local_env.set_sendmail_executable(this->arg_cache.get_sendmail_executable());
          }
        catch (runtime_exception& xe)
          {
            if(xe.get_exception_code() == exception_type::REPO_NOT_FOUND)
              {
#ifdef TRACE_OUTPUT
                std::cout << "TRACE_OUTPUT L" << '\n';
#endif
                this->err(xe.what());
                repo = nullptr;
              }
            else throw;
          }
      }


    template <typename number>
    void slave_controller<number>::send_worker_data(model<number>* m)
      {
        assert(m != nullptr);
    
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // interrogate model instance for capacity and priority
        MPI::slave_information_payload payload(m->get_backend_type(), m->get_backend_memory(), m->get_backend_priority());

        // send worker identification payload, then wait until it has been received
        boost::mpi::request resp_msg = this->world.isend(MPI::RANK_MASTER, MPI::WORKER_IDENTIFICATION, payload);
        resp_msg.wait();
      }


    template <typename number>
    void slave_controller<number>::send_worker_data()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // no model instance, so default to a CPU with 0 capacity and unit priority
        MPI::slave_information_payload payload(worker_type::cpu, 0, 1);

        // send worker identification payload, then wait until it has been received
        boost::mpi::request resp_msg = this->world.isend(MPI::RANK_MASTER, MPI::WORKER_IDENTIFICATION, payload);
        resp_msg.wait();
      }


    template <typename number>
    void slave_controller<number>::process_task(const MPI::new_integration_payload& payload)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // ensure that a valid repository object has been constructed
        if(!this->repo) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_REPO_NOT_SET);

        // extract our task from the database
        // much of this is boiler-plate which is similar to master_process_task()
        // TODO: it would be nice to make this sharing more explicit, so the code isn't just duplicated
        try
          {
            // query a task record with the name we're looking for from the database
            std::unique_ptr< task_record<number> > record = this->repo->query_task(payload.get_task_name());

            switch(record->get_type())
              {
                case task_type::integration:
                  {
                    auto* int_rec = dynamic_cast< integration_task_record<number>* >(record.get());

                    assert(int_rec != nullptr);
                    if(int_rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                    integration_task<number>* tk = int_rec->get_task();
    
                    // create new timer for this task; the BusyIdle_Context manager
                    // ensures the timer is removed when the context manager is destroyed
                    BusyIdle_Context timing_context(payload.get_group_name(), this->busyidle_timers);

                    this->dispatch_integration_task(tk, payload);
                    break;
                  }

                case task_type::output:
                case task_type::postintegration:
                  {
                    throw runtime_exception(exception_type::RECORD_NOT_FOUND, payload.get_task_name());    // RECORD_NOT_FOUND expects task name in message
                  }
              }
          }
        catch(runtime_exception& xe)
          {
            if(xe.get_exception_code() == exception_type::RECORD_NOT_FOUND)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_MISSING_RECORD << " '" << xe.what() << "'" << CPPTRANSPORT_REPO_SKIPPING_TASK;
                this->err(msg.str());
              }
            else if(xe.get_exception_code() == exception_type::MISSING_MODEL_INSTANCE
                    || xe.get_exception_code() == exception_type::REPOSITORY_BACKEND_ERROR)
              {
                std::ostringstream msg;
                msg << xe.what() << " " << CPPTRANSPORT_REPO_FOR_TASK << " '" << payload.get_task_name() << "'" << CPPTRANSPORT_REPO_SKIPPING_TASK;
                this->err(msg.str());
              }
            else throw;
          }
      }


    template <typename number>
    void slave_controller<number>::dispatch_integration_task(integration_task<number>* tk, const MPI::new_integration_payload& payload)
      {
        assert(tk != nullptr);
    
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        model<number>* m = tk->get_model();
        assert(m != nullptr);

        // send scheduling information to the master process
        this->send_worker_data(m);

        twopf_task<number>* tka = nullptr;
        threepf_task<number>* tkb = nullptr;

        if((tka = dynamic_cast<twopf_task<number>*>(tk)) != nullptr)
          {
            // construct a callback for the integrator to push new batches to the master
            std::unique_ptr< slave_container_dispatch<number> > dispatcher = std::make_unique< slave_container_dispatch<number> >(*this, MPI::INTEGRATION_DATA_READY, std::string("INTEGRATION_DATA_READY"));

            // construct a batcher to hold the output of the integration
            twopf_batcher<number> batcher = this->data_mgr->create_temp_twopf_container(tka, payload.get_tempdir_path(), payload.get_logdir_path(),
                                                                                        this->get_rank(), payload.get_workgroup_number(), m, std::move(dispatcher));

            // write log header
            boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) <<  "-- NEW INTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << *tk;

            this->schedule_integration(tka, m, batcher, payload, m->backend_twopf_state_size());
          }
        else if((tkb = dynamic_cast<threepf_task<number>*>(tk)) != nullptr)
          {
            // construct a callback for the integrator to push new batches to the master
            std::unique_ptr< slave_container_dispatch<number> > dispatcher = std::make_unique< slave_container_dispatch<number> >(*this, MPI::INTEGRATION_DATA_READY, std::string("INTEGRATION_DATA_READY"));

            // construct a batcher to hold the output of the integration
            threepf_batcher<number> batcher = this->data_mgr->create_temp_threepf_container(tkb, payload.get_tempdir_path(), payload.get_logdir_path(),
                                                                                            this->get_rank(), payload.get_workgroup_number(), m, std::move(dispatcher));

            // write log header
            boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- NEW INTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << *tk;

            this->schedule_integration(tkb, m, batcher, payload, m->backend_threepf_state_size());
          }
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_UNKNOWN_DERIVED_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
      }


    template <typename number>
    template <typename TaskObject, typename BatchObject, typename PayloadObject>
    void slave_controller<number>::schedule_integration(TaskObject* tk, model<number>* m, BatchObject& batcher, const PayloadObject& payload, unsigned int state_size)
      {
        assert(tk != nullptr);  // should be guaranteed
        assert(m != nullptr);   // should be guaranteed
    
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // dispatch integration to the underlying model
        bool complete = false;
        while(!complete)
          {
            // wait for messages from scheduler, tracking idle time
            timers.idle();
            boost::mpi::status stat = this->world.probe(MPI::RANK_MASTER);

            timers.busy();
            switch(stat.tag())
              {
                case MPI::NEW_WORK_ASSIGNMENT:
                  {
                    MPI::work_assignment_payload assignment_payload;
                    this->world.recv(stat.source(), MPI::NEW_WORK_ASSIGNMENT, assignment_payload);

                    MPI::work_acknowledgment_payload ack_payload;
                    ack_payload.set_timestamp();
                    boost::mpi::request ack_msg = this->world.isend(MPI::RANK_MASTER, MPI::NEW_WORK_ACKNOWLEDGMENT, ack_payload);
                    ack_msg.wait();

                    const std::list<unsigned int>& work_items = assignment_payload.get_items();
                    auto filter = this->work_item_filter_factory(tk, work_items);

                    // create work queues based on whatever devices are relevant for our backend
                    context ctx = m->backend_get_context();
                    scheduler sch(ctx);
                    auto work = sch.make_queue(state_size, *tk, filter);

                    bool success = true;
                    batcher.begin_assignment();

                    slave_message_buffer
                      messages(this->environment, this->world,
                               [&](const std::string& m) -> void { BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::error) << m; });
                    slave_message_context msg_ctx(messages, tk->get_name());

                    // keep track of wallclock time
                    boost::timer::cpu_timer timer;

                    BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- NEW WORK ASSIGNMENT";

                    // perform the integration
                    try
                      {
                        m->backend_process_queue(work, tk, batcher, true);    // 'true' = work silently
                      }
                    catch(runtime_exception& xe)
                      {
                        success = false;
                        messages.push_back(xe.what());
                      }

                    // all work is now done - stop the wallclock timer
                    batcher.end_assignment();
                    timer.stop();

                    // notify master process that all work has been finished (temporary containers will be deleted by the master node)
                    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
                    if(success) BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Worker sending FINISHED_INTEGRATION to master | finished at " << boost::posix_time::to_simple_string(now);
                    else        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::error)  << "-- Worker reporting INTEGRATION_FAIL to master | finished at " << boost::posix_time::to_simple_string(now);

                    MPI::finished_integration_payload
                      outgoing_payload{batcher.get_integration_time(), batcher.get_max_integration_time(),
                                       batcher.get_min_integration_time(), batcher.get_batching_time(),
                                       batcher.get_max_batching_time(), batcher.get_min_batching_time(),
                                       timer.elapsed().wall, batcher.get_reported_integrations(),
                                       batcher.get_reported_refinements(), batcher.get_reported_failures(),
                                       batcher.get_failed_serials(),
                                       this->busyidle_timers.get_load_average(payload.get_group_name())};

                    boost::mpi::request finish_msg =
                      this->world.isend(MPI::RANK_MASTER, success ? MPI::FINISHED_INTEGRATION : MPI::INTEGRATION_FAIL, outgoing_payload);
                    finish_msg.wait();

                    BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Worker message sent to master | reported at " << boost::posix_time::to_simple_string(now);

                    break;
                  }

                case MPI::END_OF_WORK:
                  {
                    this->world.recv(stat.source(), MPI::END_OF_WORK);
                    complete = true;
                    BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Notified of end-of-work: preparing to shut down";

                    // close the batcher, flushing the current container to the master node if needed
                    batcher.close();

                    // send close-down acknowledgment to master
                    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
                    BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Worker sending WORKER_CLOSE_DOWN to master | close down at " << boost::posix_time::to_simple_string(now);
                    
                    MPI::performance_data_payload close_payload(this->busyidle_timers.get_load_average(payload.get_group_name()));
                    boost::mpi::request close_msg = this->world.isend(MPI::RANK_MASTER, MPI::WORKER_CLOSE_DOWN, close_payload);
                    close_msg.wait();

                    break;
                  }

                default:
                  {
                    BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "!! Received unexpected MPI message " << stat.tag() << " from master node; discarding";
                    this->world.recv(stat.source(), stat.tag());
                    break;
                  }
              }
          }
      }


    template <typename number>
    void slave_controller<number>::process_task(const MPI::new_derived_content_payload& payload)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // ensure that a valid repository object has been constructed
        if(!this->repo) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_REPO_NOT_SET);

        // extract our task from the database
        // much of this is boiler-plate which is similar to master_process_task()
        // TODO: it would be nice to make this sharing more explicit, so the code isn't just duplicated
        try
          {
            // query a task record with the name we're looking for from the database
            std::unique_ptr< task_record<number> > record = this->repo->query_task(payload.get_task_name());

            switch(record->get_type())
              {
                case task_type::integration:
                  {
                    throw runtime_exception(exception_type::RECORD_NOT_FOUND, payload.get_task_name());     // RECORD_NOT_FOUND expects task name in message
                  }

                case task_type::output:
                  {
                    auto* out_rec = dynamic_cast< output_task_record<number>* >(record.get());

                    assert(out_rec != nullptr);
                    if(out_rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                    output_task<number>* tk = out_rec->get_task();
    
                    // create new timer for this task; the BusyIdle_Context manager
                    // ensures the timer is removed when the context manager is destroyed
                    BusyIdle_Context timing_context(payload.get_group_name(), this->busyidle_timers);

                    this->schedule_output(tk, payload);
                    break;
                  }

                case task_type::postintegration:
                  {
                    throw runtime_exception(exception_type::RECORD_NOT_FOUND, payload.get_task_name());    // RECORD_NOT_FOUND expects task name in message
                  }
              }

          }
        catch (runtime_exception& xe)
          {
            if(xe.get_exception_code() == exception_type::RECORD_NOT_FOUND)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_MISSING_RECORD << " '" << xe.what() << "'" << CPPTRANSPORT_REPO_SKIPPING_TASK;
                this->err(msg.str());
              }
            else if(xe.get_exception_code() == exception_type::MISSING_MODEL_INSTANCE
                    || xe.get_exception_code() == exception_type::REPOSITORY_BACKEND_ERROR)
              {
                std::ostringstream msg;
                msg << xe.what() << " " << CPPTRANSPORT_REPO_FOR_TASK << " '" << payload.get_task_name() << "'" << CPPTRANSPORT_REPO_SKIPPING_TASK;
                this->err(msg.str());
              }
            else throw;
          }
      }


    template <typename number>
    void slave_controller<number>::schedule_output(output_task<number>* tk, const MPI::new_derived_content_payload& payload)
      {
        assert(tk != nullptr);  // should be guaranteed
    
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);
        
        // send scheduling information to the master process; here, report ourselves as a CPU
        // since there is currently no capacity to process output tasks on a GPU
        this->send_worker_data();

        // set up content-group finder function; must survive throughout lifetime of datapipe
        integration_content_finder<number>     i_finder(*this->repo);
        postintegration_content_finder<number> p_finder(*this->repo);

        // set up content-dispatch function; must survive throughout lifetime of datapipe
        slave_datapipe_dispatch<number> dispatcher(*this);

        // acquire a datapipe which we can use to stream content from the databse
        std::unique_ptr< datapipe<number> > pipe = this->data_mgr->create_datapipe(payload.get_logdir_path(), payload.get_tempdir_path(), i_finder, p_finder, dispatcher, this->get_rank());

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "-- NEW OUTPUT TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
        BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << *tk;
        
        bool complete = false;
        while(!complete)
          {
            // wait for messages from scheduler, tracking idle time
            timers.idle();
            boost::mpi::status stat = this->world.probe(MPI::RANK_MASTER);

            timers.busy();
            switch(stat.tag())
              {
                case MPI::NEW_WORK_ASSIGNMENT:
                  {
                    MPI::work_assignment_payload assignment_payload;
                    this->world.recv(stat.source(), MPI::NEW_WORK_ASSIGNMENT, assignment_payload);

                    MPI::work_acknowledgment_payload ack_payload;
                    ack_payload.set_timestamp();
                    boost::mpi::request ack_msg = this->world.isend(MPI::RANK_MASTER, MPI::NEW_WORK_ACKNOWLEDGMENT, ack_payload);
                    ack_msg.wait();

                    const std::list<unsigned int>& work_items = assignment_payload.get_items();
                    auto filter = this->work_item_filter_factory(tk, work_items);

                    // create work queues
                    context ctx;
                    ctx.add_device("CPU");
                    scheduler sch(ctx);
                    auto work = sch.make_queue(*tk, filter);

                    BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "-- NEW WORK ASSIGNMENT";

                    bool success = true;

                    // track content groups we use
                    std::list<std::string> content_groups;

                    // keep track of wallclock time
                    boost::timer::cpu_timer timer;
                    boost::timer::nanosecond_type processing_time = 0;
                    boost::timer::nanosecond_type min_processing_time = 0;
                    boost::timer::nanosecond_type max_processing_time = 0;

                    std::ostringstream work_msg;
                    work_msg << work;
                    BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << work_msg.str();

                    const typename work_queue< output_task_element<number> >::device_queue queues = work[0];
                    assert(queues.size() == 1);

                    const typename work_queue< output_task_element<number> >::device_work_list list = queues[0];

                    for(unsigned int i = 0; i < list.size(); ++i)
                      {
                        // set up message buffer; will push any errors to master node when it goes out of scope
                        slave_message_buffer messages(this->environment, this->world, [&](const std::string& m) -> void { BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::error) << m; });
                        slave_message_context ctx(messages, tk->get_name());

                        // get derived product
                        typename derived_data::derived_product<number>& product = list[i].get_product();

                        // merge command-line supplied tags with tags specified in the task
                        std::list<std::string> task_tags = list[i].get_tags();
                        std::list<std::string> command_line_tags = payload.get_tags();

                        task_tags.splice(task_tags.end(), command_line_tags);

                        BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "-- Processing derived product '" << product.get_name() << "'";

                        std::list<std::string> this_groups;

                        try
                          {
                            boost::timer::cpu_timer derive_timer;
                            this_groups = product.derive(*pipe, task_tags, messages, this->local_env, this->arg_cache);
                            content_groups.merge(this_groups);
                            derive_timer.stop();
                            processing_time += derive_timer.elapsed().wall;
                            if(max_processing_time == 0 || derive_timer.elapsed().wall > max_processing_time) max_processing_time = derive_timer.elapsed().wall;
                            if(min_processing_time == 0 || derive_timer.elapsed().wall < min_processing_time) min_processing_time = derive_timer.elapsed().wall;
                          }
                        catch(runtime_exception& xe)
                          {
                            success = false;
                            messages.push_back(xe.what());
                          }

                        // check that the datapipe was correctly detached
                        if(pipe->is_attached())
                          {
                            BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::error) << "!! Task manager detected that datapipe was not correctly detached after generating derived product '" << product.get_name() << "'";
                            pipe->detach();
                          }

                        BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "";
                      }

                    // collect content groups used during this derivation
                    content_groups.sort();
                    content_groups.unique();

                    // all work now done - stop the timer
                    timer.stop();

                    // notify master process that all work has been finished
                    now = boost::posix_time::second_clock::universal_time();
                    if(success) BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "-- Worker sending FINISHED_DERIVED_CONTENT to master | finished at " << boost::posix_time::to_simple_string(now);
                    else        BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::error)  << "-- Worker reporting DERIVED_CONTENT_FAIL to master | finished at " << boost::posix_time::to_simple_string(now);

                    MPI::finished_derived_payload
                      finish_payload{content_groups, pipe->get_database_time(), timer.elapsed().wall,
                                     static_cast<unsigned int>(list.size()), processing_time, min_processing_time, max_processing_time,
                                     pipe->get_time_config_cache_hits(), pipe->get_time_config_cache_unloads(),
                                     pipe->get_twopf_kconfig_cache_hits(), pipe->get_twopf_kconfig_cache_unloads(),
                                     pipe->get_threepf_kconfig_cache_hits(), pipe->get_threepf_kconfig_cache_unloads(),
                                     pipe->get_stats_cache_hits(), pipe->get_stats_cache_unloads(),
                                     pipe->get_data_cache_hits(), pipe->get_data_cache_unloads(),
                                     pipe->get_time_config_cache_evictions(), pipe->get_twopf_kconfig_cache_evictions(),
                                     pipe->get_threepf_kconfig_cache_evictions(), pipe->get_stats_cache_evictions(),
                                     pipe->get_data_cache_evictions(), this->busyidle_timers.get_load_average(payload.get_group_name())};

                    boost::mpi::request finish_msg =
                      this->world.isend(MPI::RANK_MASTER, success ? MPI::FINISHED_DERIVED_CONTENT : MPI::DERIVED_CONTENT_FAIL, finish_payload);
                    finish_msg.wait();

                    break;
                  }

                case MPI::END_OF_WORK:
                  {
                    this->world.recv(stat.source(), MPI::END_OF_WORK);
                    complete = true;
                    BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "-- Notified of end-of-work: preparing to shut down";

                    // close the datapipe
                    pipe->close();

                    // send close-down acknowledgment to master
                    now = boost::posix_time::second_clock::universal_time();
                    BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "-- Worker sending WORKER_CLOSE_DOWN to master | close down at " << boost::posix_time::to_simple_string(now);
    
                    MPI::performance_data_payload close_payload(this->busyidle_timers.get_load_average(payload.get_group_name()));
                    boost::mpi::request close_msg = this->world.isend(MPI::RANK_MASTER, MPI::WORKER_CLOSE_DOWN, close_payload);
                    close_msg.wait();

                    break;
                  }

                default:
                  {
                    BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "!! Received unexpected MPI message " << stat.tag() << " from master node; discarding";
                    this->world.recv(stat.source(), stat.tag());
                    break;
                  }
              }
          }

      }


    template <typename number>
    void slave_controller<number>::process_task(const MPI::new_postintegration_payload& payload)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // ensure that a valid repository object has been constructed
        if(!this->repo) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_REPO_NOT_SET);

        // extract our task from the database
        try
          {
            // lookup task by name from the repository
            auto record = this->repo->query_task(payload.get_task_name());

            switch(record->get_type())
              {
                case task_type::integration:
                case task_type::output:
                  {
                    throw runtime_exception(exception_type::RECORD_NOT_FOUND, payload.get_task_name());    // RECORD_NOT_FOUND expects task name in message
                  }

                case task_type::postintegration:
                  {
                    auto* pint_rec = dynamic_cast< postintegration_task_record<number>* >(record.get());

                    assert(pint_rec != nullptr);
                    if(pint_rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                    postintegration_task<number>* tk = pint_rec->get_task();
    
                    // create new timer for this task; the BusyIdle_Context manager
                    // ensures the timer is removed when the context manager is destroyed
                    BusyIdle_Context timing_context(payload.get_group_name(), this->busyidle_timers);

                    this->dispatch_postintegration_task(tk, payload);
                    break;
                  }
              }
          }
        catch(runtime_exception& xe)
          {
            if(xe.get_exception_code() == exception_type::RECORD_NOT_FOUND)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_MISSING_RECORD << " '" << xe.what() << "'" << CPPTRANSPORT_REPO_SKIPPING_TASK;
                this->err(msg.str());
              }
            else if(xe.get_exception_code() == exception_type::MISSING_MODEL_INSTANCE
                    || xe.get_exception_code() == exception_type::REPOSITORY_BACKEND_ERROR)
              {
                std::ostringstream msg;
                msg << xe.what() << " " << CPPTRANSPORT_REPO_FOR_TASK << " '" << payload.get_task_name() << "'" << CPPTRANSPORT_REPO_SKIPPING_TASK;
                this->err(msg.str());
              }
            else throw;
          }
      }


    template <typename number>
    void slave_controller<number>::dispatch_postintegration_task(postintegration_task<number>* tk, const MPI::new_postintegration_payload& payload)
      {
        assert(tk != nullptr);
    
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // send scheduling information to the master process; here, report ourselves as a CPU
        // since there is currently no capacity to process postintegration tasks on a GPU
        this->send_worker_data();

        zeta_twopf_task<number>* z2pf = nullptr;
        zeta_threepf_task<number>* z3pf = nullptr;
        fNL_task<number>* zfNL = nullptr;

        if((z2pf = dynamic_cast<zeta_twopf_task<number>*>(tk)) != nullptr)
          {
            // get parent task
            auto* ptk = dynamic_cast<twopf_task<number>*>(z2pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_TWOPF_TASK << " '" << z2pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
              }

            model<number>* m = ptk->get_model();

            // construct a callback for the postintegrator to push new batches to the master
            auto dispatcher = std::make_unique< slave_container_dispatch<number> >(*this, MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold postintegration output
            auto batcher = this->data_mgr->create_temp_zeta_twopf_container(z2pf, payload.get_tempdir_path(), payload.get_logdir_path(), this->get_rank(), m, std::move(dispatcher));

            // is this 2pf task paired?
            if(z2pf->is_paired())
              {
                // also need a callback for the paired integrator
                auto i_dispatcher = std::make_unique< slave_container_dispatch<number> >(*this, MPI::INTEGRATION_DATA_READY, std::string("INTEGRATION_DATA_READY"));

                // construct a batcher to hold integration output
                auto i_batcher = this->data_mgr->create_temp_twopf_container(ptk, payload.get_paired_tempdir_path(), payload.get_paired_logdir_path(), this->get_rank(), payload.get_paired_workgroup_number(), m, std::move(i_dispatcher));

                // pair batchers
                i_batcher.pair(&batcher);

                // write log header
                boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
                BOOST_LOG_SEV(i_batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- NEW PAIRED POSTINTEGRATION TASKS '" << tk->get_name() << "' & '" << ptk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
                BOOST_LOG_SEV(i_batcher.get_log(), generic_batcher::log_severity_level::normal) << *ptk;

                this->schedule_integration(ptk, m, i_batcher, payload, m->backend_twopf_state_size());
              }
            else
              {
                // write log header
                boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- NEW POSTINTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << *tk;

                this->schedule_postintegration(z2pf, ptk, payload, batcher);
              }
          }
        else if((z3pf = dynamic_cast<zeta_threepf_task<number>*>(tk)) != nullptr)
          {
            // get parent task
            auto* ptk = dynamic_cast<threepf_task<number>*>(z3pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_THREEPF_TASK << " '" << z3pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
              }

            model<number>* m = ptk->get_model();

            // construct a callback for the integrator to push new batches to the master
            auto p_dispatcher = std::make_unique< slave_container_dispatch<number> >(*this, MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            auto batcher = this->data_mgr->create_temp_zeta_threepf_container(z3pf, payload.get_tempdir_path(), payload.get_logdir_path(), this->get_rank(), m, std::move(p_dispatcher));

            if(z3pf->is_paired())
              {
                // also need a callback for the paired integrator
                auto i_dispatcher = std::make_unique< slave_container_dispatch<number> >(*this, MPI::INTEGRATION_DATA_READY, std::string("INTEGRATION_DATA_READY"));

                // construct a batcher to hold integration output
                threepf_batcher<number> i_batcher = this->data_mgr->create_temp_threepf_container(ptk, payload.get_paired_tempdir_path(), payload.get_paired_logdir_path(), this->get_rank(), payload.get_paired_workgroup_number(), m, std::move(i_dispatcher));

                // pair batchers
                i_batcher.pair(&batcher);

                // write log header
                boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
                BOOST_LOG_SEV(i_batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- NEW PAIRED POSTINTEGRATION TASKS '" << tk->get_name() << "' & '" << ptk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
                BOOST_LOG_SEV(i_batcher.get_log(), generic_batcher::log_severity_level::normal) << *ptk;

                this->schedule_integration(ptk, m, i_batcher, payload, m->backend_threepf_state_size());
              }
            else
              {
                // write log header
                boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- NEW POSTINTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << *tk;

                this->schedule_postintegration(z3pf, ptk, payload, batcher);
              }
          }
        else if((zfNL = dynamic_cast<fNL_task<number>*>(tk)) != nullptr)
          {
            // get parent task
            auto* ptk = dynamic_cast<zeta_threepf_task<number>*>(zfNL->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_ZETA_THREEPF_TASK << " '" << zfNL->get_parent_task()->get_name() << "'";
                throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
              }

            // get parent task
            auto* pptk = dynamic_cast<threepf_task<number>*>(ptk->get_parent_task());

            assert(pptk != nullptr);
            if(pptk == nullptr)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_THREEPF_TASK << " '" << ptk->get_parent_task()->get_name() << "'";
                throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
              }

            model<number>* m = pptk->get_model();

            // construct a callback for the integrator to push new batches to the master
            auto dispatcher = std::make_unique< slave_container_dispatch<number> >(*this, MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            auto batcher = this->data_mgr->create_temp_fNL_container(zfNL, payload.get_tempdir_path(), payload.get_logdir_path(), this->get_rank(), m, std::move(dispatcher), zfNL->get_template());

            this->schedule_postintegration(zfNL, ptk, payload, batcher);
          }
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_UNKNOWN_DERIVED_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
      }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject, typename BatchObject>
    void slave_controller<number>::schedule_postintegration(TaskObject* tk, ParentTaskObject* ptk,
                                                            const MPI::new_postintegration_payload& payload, BatchObject& batcher)
      {
        assert(tk != nullptr);    // should be guaranteed
        assert(ptk != nullptr);   // should be guaranteed
    
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- NEW POSTINTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << '\n';
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << *tk;

        // set up content-group finder functions; must survive throughout lifetime of datapipe
        integration_content_finder<number>     i_finder(*this->repo);
        postintegration_content_finder<number> p_finder(*this->repo);

        // set up empty content-dispatch function -- this datapipe is not used to produce content
        // must survive throughout lifetime of datapipe
        slave_null_dispatch_function<number> dispatcher(*this);

        // acquire a datapipe which we can use to stream content from the database
        auto pipe = this->data_mgr->create_datapipe(payload.get_logdir_path(), payload.get_tempdir_path(),
                                                    i_finder, p_finder, dispatcher, this->get_rank(), true);

        bool complete = false;
        while(!complete)
          {
            // wait for messages from scheduler, tracking idle time
            timers.idle();
            boost::mpi::status stat = this->world.probe(MPI::RANK_MASTER);

            timers.busy();
            switch(stat.tag())
              {
                case MPI::NEW_WORK_ASSIGNMENT:
                  {
                    MPI::work_assignment_payload assignment_payload;
                    this->world.recv(stat.source(), MPI::NEW_WORK_ASSIGNMENT, assignment_payload);

                    MPI::work_acknowledgment_payload ack_payload;
                    ack_payload.set_timestamp();
                    boost::mpi::request ack_msg = this->world.isend(MPI::RANK_MASTER, MPI::NEW_WORK_ACKNOWLEDGMENT, ack_payload);
                    ack_msg.wait();

                    const std::list<unsigned int>& work_items = assignment_payload.get_items();
                    auto filter = this->work_item_filter_factory(ptk, work_items);

                    // create work queues
                    context ctx;
                    ctx.add_device("CPU");
                    scheduler sch(ctx);
                    auto work = sch.make_queue(sizeof(number), *ptk, filter);

                    bool success = true;
                    batcher.begin_assignment();

                    slave_message_buffer
                      messages(this->environment, this->world,
                               [&](const std::string& m) -> void { BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::error) << m; });
                    slave_message_context msg_ctx(messages, tk->get_name());

                    // keep track of wallclock time
                    boost::timer::cpu_timer timer;

                    BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- NEW WORK ASSIGNMENT";

                    // perform the task
                    std::string group;
                    try
                      {
                        // attach datapipe to the best-match content group for the task ptk and specified tags
                        group = pipe->attach(ptk, payload.get_tags());

                        // use this content group to do whatever work is required
                        this->work_handler.postintegration_handler(tk, ptk, work, batcher, *pipe);

                        // detach datapipe
                        pipe->detach();
                      }
                    catch(runtime_exception& xe)
                      {
                        success = false;
                        messages.push_back(xe.what());
                      }

                    // inform the batcher we are at the end of this assignment
                    batcher.end_assignment();

                    // all work is now done - stop the wallclock timer
                    timer.stop();

                    // notify master process that all work has been finished (temporary containers will be deleted by the master node)
                    now = boost::posix_time::second_clock::universal_time();
                    if(success) BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Worker sending FINISHED_POSTINTEGRATION to master | finished at " << boost::posix_time::to_simple_string(now);
                    else        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::error)  << "-- Worker reporting POSTINTEGRATION_FAIL to master | finished at " << boost::posix_time::to_simple_string(now);

                    MPI::finished_postintegration_payload
                      outgoing_payload{group, pipe->get_database_time(), timer.elapsed().wall,
                                       batcher.get_items_processed(), batcher.get_processing_time(),
                                       batcher.get_max_processing_time(), batcher.get_min_processing_time(),
                                       pipe->get_time_config_cache_hits(), pipe->get_time_config_cache_unloads(),
                                       pipe->get_twopf_kconfig_cache_hits(), pipe->get_twopf_kconfig_cache_unloads(),
                                       pipe->get_threepf_kconfig_cache_hits(), pipe->get_threepf_kconfig_cache_unloads(),
                                       pipe->get_stats_cache_hits(), pipe->get_stats_cache_unloads(),
                                       pipe->get_data_cache_hits(), pipe->get_data_cache_unloads(),
                                       pipe->get_time_config_cache_evictions(), pipe->get_twopf_kconfig_cache_evictions(),
                                       pipe->get_threepf_kconfig_cache_evictions(), pipe->get_stats_cache_evictions(),
                                       pipe->get_data_cache_evictions(), this->busyidle_timers.get_load_average(payload.get_group_name())};

                    boost::mpi::request finish_msg =
                      this->world.isend(MPI::RANK_MASTER, success ? MPI::FINISHED_POSTINTEGRATION : MPI::POSTINTEGRATION_FAIL, outgoing_payload);
                    finish_msg.wait();

                    break;
                  }

                case MPI::END_OF_WORK:
                  {
                    this->world.recv(stat.source(), MPI::END_OF_WORK);
                    complete = true;
                    BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Notified of end-of-work: preparing to shut down";

                    // close the batcher, flushing the current container to the master node if required
                    batcher.close();

                    // send close-down acknowledgment to master
                    now = boost::posix_time::second_clock::universal_time();
                    BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Worker sending WORKER_CLOSE_DOWN to master | close down at " << boost::posix_time::to_simple_string(now);
    
                    MPI::performance_data_payload close_payload(this->busyidle_timers.get_load_average(payload.get_group_name()));
                    boost::mpi::request close_msg = this->world.isend(MPI::RANK_MASTER, MPI::WORKER_CLOSE_DOWN, close_payload);
                    close_msg.wait();

                    break;
                  }

                default:
                  {
                    BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "!! Received unexpected MPI message " << stat.tag() << " from master node; discarding";
                    this->world.recv(stat.source(), stat.tag());
                    break;
                  }
              }
          }
      }


    template <typename number>
    void slave_controller<number>::push_temp_container(generic_batcher& batcher, unsigned int message, std::string log_message)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Sending " << log_message << " message for container " << batcher.get_container_path();

        MPI::data_ready_payload payload(batcher.get_container_path());

        // advise master process that data is available in the named container
        boost::mpi::request push_msg = this->world.isend(MPI::RANK_MASTER, message, payload);
        push_msg.wait();
      }


    template <typename number>
    void slave_controller<number>::push_derived_content(datapipe<number>* pipe, typename derived_data::derived_product<number>* product,
                                                        const std::list<std::string>& used_groups)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        assert(pipe != nullptr);
        assert(product != nullptr);

        // FIXME: error message tag is possibly in the wrong namespace (but error message namespaces are totally confused anyway)
        if(pipe == nullptr) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);
        if(product == nullptr) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_NULL_DERIVED_PRODUCT);

        BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "-- Sending DERIVED_CONTENT_READY message for derived product '" << product->get_name() << "'";

        boost::filesystem::path product_filename = pipe->get_abs_tempdir_path() / product->get_filename();
        if(boost::filesystem::exists(product_filename))
          {
            MPI::content_ready_payload payload(product->get_name(), used_groups);
            boost::mpi::request ready_msg = this->world.isend(MPI::RANK_MASTER, MPI::DERIVED_CONTENT_READY, payload);
            ready_msg.wait();
          }
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_DATAMGR_DERIVED_PRODUCT_MISSING << " " << product_filename;
            throw runtime_exception(exception_type::DATAPIPE_ERROR, msg.str());
          }
      }


    template <typename number>
    void slave_controller<number>::disallow_push_content(datapipe<number>* pipe, typename derived_data::derived_product<number>* product)
      {
        assert(false);
      }

  }   // namespace transport


#endif //CPPTRANSPORT_SLAVE_CONTROLLER_IMPL_H
