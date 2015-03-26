//
// Created by David Seery on 19/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __slave_controller_H_
#define __slave_controller_H_


#include <list>
#include <vector>
#include <memory>
#include <functional>

#include "transport-runtime-api/models/model.h"
#include "transport-runtime-api/manager/instance_manager.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"

#include "transport-runtime-api/manager/mpi_operations.h"

#include "transport-runtime-api/repository/repository.h"
#include "transport-runtime-api/manager/data_manager.h"
#include "transport-runtime-api/manager/slave_work_handler.h"

#include "transport-runtime-api/scheduler/context.h"
#include "transport-runtime-api/scheduler/scheduler.h"
#include "transport-runtime-api/scheduler/work_queue.h"

#include "transport-runtime-api/utilities/formatter.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/timer/timer.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


namespace transport
	{

    // SLAVE FUNCTIONS

		template <typename number>
		class slave_controller
			{

		  public:

		    //! Error-reporting callback object
		    typedef std::function<void(const std::string&)> error_callback;

		    //! Warning callback object
		    typedef std::function<void(const std::string&)> warning_callback;

		    //! Message callback object
		    typedef std::function<void(const std::string&)> message_callback;


				// CONSTRUCTOR, DESTRUCTOR

		  public:

		    //! construct a slave controller object
		    //! unlike a master controller, there is no option to supply a repository;
		    //! one has to be provided by a master controller over MPI later
		    slave_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
		                     const typename instance_manager<number>::model_finder& f,
		                     error_callback err, warning_callback warn, message_callback msg,
		                     unsigned int bcp = __CPP_TRANSPORT_DEFAULT_BATCHER_STORAGE,
		                     unsigned int pcp = __CPP_TRANSPORT_DEFAULT_PIPE_STORAGE,
		                     unsigned int zcp = __CPP_TRANSPORT_DEFAULT_ZETA_CACHE_SIZE);

		    //! destroy a slave manager object
		    ~slave_controller();


				// INTERFACE

		  public:

		    //! poll for instructions to perform work
		    void wait_for_tasks(void);


				// MPI FUNCTIONS

		  protected:

		    //! Get worker number
		    unsigned int worker_number() { return(static_cast<unsigned int>(this->world.rank()-1)); }

		    //! Return MPI rank of this process
		    unsigned int get_rank(void) const { return(static_cast<unsigned int>(this->world.rank())); }


		    // SLAVE JOB HANDLING

		  protected:

		    //! Slave node: initialize ourselves
		    void initialize(const MPI::slave_setup_payload& payload);

				//! Slave node: Pass scheduling data to the master node
				void send_worker_data(model<number>* m);

				//! Slave node: Pass scheduling data to the master node
				void send_worker_data(void);


		    // SLAVE INTEGRATION TASKS

		  protected:

		    //! Slave node: Process a new integration task instruction
		    void process_task(const MPI::new_integration_payload& payload);

		    //! Slave node: process an integration task
		    void dispatch_integration_task(integration_task<number>* tk, const MPI::new_integration_payload& payload);

		    //! Slave node: process an integration queue
		    template <typename TaskObject, typename BatchObject>
		    void schedule_integration(TaskObject* tk, model<number>* m, const MPI::new_integration_payload& payload,
		                              BatchObject& batcher, unsigned int state_size);

		    //! Push a temporary container to the master process
		    void push_temp_container(typename data_manager<number>::generic_batcher* batcher, unsigned int message, std::string log_message);

				//! Construct a work item filter for a twopf task
				work_item_filter<twopf_kconfig> work_item_filter_factory(twopf_task<number>* tk, const std::list<unsigned int>& items) const { return work_item_filter<twopf_kconfig>(items); }

		    //! Construct a work item filter for a threepf task
		    work_item_filter<threepf_kconfig> work_item_filter_factory(threepf_task<number>* tk, const std::list<unsigned int>& items) const { return work_item_filter<threepf_kconfig>(items); }

				//! Construct a work item filter factory for an output task
				work_item_filter< output_task_element<number> > work_item_filter_factory(output_task<number>* tk, const std::list<unsigned int>& items) const { return work_item_filter< output_task_element<number> >(items); }


		    // SLAVE POSTINTEGRATION TASKS

		  protected:

		    //! Slave node: Process a new postintegration task instruction
		    void process_task(const MPI::new_postintegration_payload& payload);

		    //! Slave node: Process a postintegration task
		    void dispatch_postintegration_task(postintegration_task<number>* tk, const MPI::new_postintegration_payload& payload);

		    //! Slave node: process a postintegration queue
		    template <typename TaskObject, typename ParentTaskObject, typename BatchObject>
		    void schedule_postintegration(TaskObject* tk, ParentTaskObject* ptk,
		                                  const MPI::new_postintegration_payload& payload, BatchObject& batcher);

		    //! No-op push content for connexion to datapipe
		    void disallow_push_content(typename data_manager<number>::datapipe* pipe, typename derived_data::derived_product<number>* product);


		    // SLAVE OUTPUT TASKS

		  protected:

		    //! Slave node: Process a new output task instruction
		    void process_task(const MPI::new_derived_content_payload& payload);

		    //! Slave node: Process an output task
		    void schedule_output(output_task<number>* tk, const MPI::new_derived_content_payload& payload);

		    //! Push new derived content to the master process
		    void push_derived_content(typename data_manager<number>::datapipe* pipe, typename derived_data::derived_product<number>* product);


		    // INTERNAL DATA

		  protected:


		    // MPI ENVIRONMENT

		    //! BOOST::MPI environment
		    boost::mpi::environment& environment;

		    //! BOOST::MPI world communicator
		    boost::mpi::communicator& world;


				// MODEL FINDER REFERENCE
		    const typename instance_manager<number>::model_finder model_finder;

		    // RUNTIME AGENTS

		    //! Repository manager instance
		    json_interface_repository<number>* repo;

		    //! Data manager instance
		    data_manager<number>* data_mgr;

				//! Handler for postintegration and output tasks
				slave_work_handler<number> work_handler;


		    // DATA AND STATE

		    //! Storage capacity per batcher
		    unsigned int batcher_capacity;

		    //! Data cache capacity per datapipe
		    unsigned int pipe_data_capacity;

		    //! Zeta cache capacity per datapipe
		    unsigned int pipe_zeta_capacity;


				// ERROR CALLBACKS

				//! error callback
				error_callback error_handler;

				//! warning callback
				warning_callback warning_handler;

				//! message callback
				message_callback message_handler;

			};


    template <typename number>
    slave_controller<number>::slave_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
                                               const typename instance_manager<number>::model_finder& f,
                                               error_callback err, warning_callback warn, message_callback msg,
                                               unsigned int bcp, unsigned int pcp, unsigned int zcp)
	    : environment(e),
	      world(w),
	      model_finder(f),
	      repo(nullptr),
	      data_mgr(data_manager_factory<number>(bcp, pcp, zcp)),
	      batcher_capacity(bcp),
	      pipe_data_capacity(pcp),
	      pipe_zeta_capacity(zcp),
	      error_handler(err),
	      warning_handler(warn),
	      message_handler(msg)
	    {
	    }


		template <typename number>
		slave_controller<number>::~slave_controller()
			{
				// delete repository instance, if it is set
				if(this->repo != nullptr)
					{
						delete this->repo;
					}
			}


    template <typename number>
    void slave_controller<number>::wait_for_tasks(void)
	    {
        if(this->get_rank() == 0) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_WAIT_MASTER);

        bool finished = false;

        while(!finished)
	        {
            // wait until a message is available from master
            boost::mpi::status stat = this->world.probe(MPI::RANK_MASTER);

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

                case MPI::INFORMATION_REQUEST:
	                {
                    MPI::slave_setup_payload payload;
                    this->world.recv(MPI::RANK_MASTER, MPI::INFORMATION_REQUEST, payload);
                    this->initialize(payload);
                    break;
	                }

                case MPI::TERMINATE:
	                {
                    this->world.recv(MPI::RANK_MASTER, MPI::TERMINATE);
                    finished = true;
                    break;
	                }

                default:
	                throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_UNEXPECTED_MPI);
	            }
	        }
	    }


    template <typename number>
    void slave_controller<number>::initialize(const MPI::slave_setup_payload& payload)
	    {
        try
	        {
            boost::filesystem::path repo_path = payload.get_repository_path();

            this->repo = repository_factory<number>(repo_path.string(), repository<number>::access_type::readonly,
                                                    this->error_handler, this->warning_handler, this->message_handler);
            this->repo->set_model_finder(this->model_finder);

		        this->data_mgr->set_batcher_capacity(payload.get_batcher_capacity());
		        this->data_mgr->set_data_capacity(payload.get_data_capacity());
		        this->data_mgr->set_zeta_capacity(payload.get_zeta_capacity());
	        }
        catch (runtime_exception& xe)
	        {
            if(xe.get_exception_code() == runtime_exception::REPO_NOT_FOUND)
	            {
                this->error_handler(xe.what());
                repo = nullptr;
	            }
            else
	            {
                throw xe;
	            }
	        }
	    }


		template <typename number>
		void slave_controller<number>::send_worker_data(model<number>* m)
			{
				assert(m != nullptr);

				typename model<number>::backend_type btype = m->get_backend_type();
		    MPI::slave_information_payload::worker_type wtype;

				if(btype == model<number>::cpu) wtype = MPI::slave_information_payload::cpu;
				else if(btype == model<number>::gpu) wtype = MPI::slave_information_payload::gpu;

		    MPI::slave_information_payload payload(wtype, m->get_backend_memory(), m->get_backend_priority());

				this->world.isend(MPI::RANK_MASTER, MPI::INFORMATION_RESPONSE, payload);
			}


		template <typename number>
		void slave_controller<number>::send_worker_data(void)
			{
		    MPI::slave_information_payload payload(MPI::slave_information_payload::cpu, 0, 1);

		    this->world.isend(MPI::RANK_MASTER, MPI::INFORMATION_RESPONSE, payload);
			}


    template <typename number>
    void slave_controller<number>::process_task(const MPI::new_integration_payload& payload)
	    {
        // ensure that a valid repository object has been constructed
        if(this->repo == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NOT_SET);

        // extract our task from the database
        // much of this is boiler-plate which is similar to master_process_task()
        // TODO: it would be nice to make this sharing more explicit, so the code isn't just duplicated
        try
	        {
            // query a task record with the name we're looking for from the database
            std::unique_ptr< task_record<number> > record(this->repo->query_task(payload.get_task_name()));

            switch(record->get_type())
	            {
                case task_record<number>::integration:
	                {
                    integration_task_record<number>* int_rec = dynamic_cast< integration_task_record<number>* >(record.get());

                    assert(int_rec != nullptr);
                    if(int_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    integration_task<number>* tk = int_rec->get_task();
                    this->dispatch_integration_task(tk, payload);
                    break;
	                }

                case task_record<number>::output:
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_TASK_IS_OUTPUT << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
	                }

                case task_record<number>::postintegration:
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_TASK_IS_POSTINTEGRATION << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
	                }

                default:
	                {
                    assert(false);

                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_UNKNOWN_RECORD_TYPE << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	                }
	            }
	        }
        catch(runtime_exception xe)
	        {
            if(xe.get_exception_code() == runtime_exception::RECORD_NOT_FOUND)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_MISSING_RECORD << " '" << xe.what() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error_handler(msg.str());
	            }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE
	                  || xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
	            {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << payload.get_task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error_handler(msg.str());
	            }
            else
	            {
                throw xe;
	            }
	        }
	    }


    template <typename number>
    void slave_controller<number>::dispatch_integration_task(integration_task<number>* tk, const MPI::new_integration_payload& payload)
	    {
        assert(tk != nullptr);

        model<number>* m = tk->get_model();
        assert(m != nullptr);

		    // send scheduling information to the master process
		    this->send_worker_data(m);

        twopf_task<number>* tka = nullptr;
        threepf_task<number>* tkb = nullptr;

        if((tka = dynamic_cast<twopf_task<number>*>(tk)) != nullptr)
	        {
            // construct a callback for the integrator to push new batches to the master
            typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                       std::bind(&slave_controller<number>::push_temp_container, this, std::placeholders::_1,
	                                                                                 MPI::INTEGRATION_DATA_READY, std::string("INTEGRATION_DATA_READY"));

            // construct a batcher to hold the output of the integration
            typename data_manager<number>::twopf_batcher batcher =
	                                                         this->data_mgr->create_temp_twopf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                     this->get_rank(), m, dispatcher);

            this->schedule_integration(tka, m, payload, batcher, m->backend_twopf_state_size());
	        }
        else if((tkb = dynamic_cast<threepf_task<number>*>(tk)) != nullptr)
	        {
            // construct a callback for the integrator to push new batches to the master
            typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                       std::bind(&slave_controller<number>::push_temp_container, this, std::placeholders::_1,
	                                                                                 MPI::INTEGRATION_DATA_READY, std::string("INTEGRATION_DATA_READY"));

            // construct a batcher to hold the output of the integration
            typename data_manager<number>::threepf_batcher batcher =
	                                                           this->data_mgr->create_temp_threepf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                         this->get_rank(), m, dispatcher);

            this->schedule_integration(tkb, m, payload, batcher, m->backend_threepf_state_size());
	        }
        else
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
	    }


    template <typename number>
    template <typename TaskObject, typename BatchObject>
    void slave_controller<number>::schedule_integration(TaskObject* tk, model<number>* m, const MPI::new_integration_payload& payload,
                                                        BatchObject& batcher, unsigned int state_size)
	    {
        // dispatch integration to the underlying model

        assert(tk != nullptr);  // should be guaranteed
        assert(m != nullptr);   // should be guaranteed

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- NEW INTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

		    bool complete = false;
		    while(!complete)
			    {
				    // wait for messages from scheduler
		        boost::mpi::status stat = this->world.probe(MPI::RANK_MASTER);

				    switch(stat.tag())
					    {
				        case MPI::NEW_WORK_ASSIGNMENT:
					        {
				            MPI::work_assignment_payload payload;
						        this->world.recv(stat.source(), MPI::NEW_WORK_ASSIGNMENT, payload);

				            const std::list<unsigned int>& work_items = payload.get_items();
						        auto filter = this->work_item_filter_factory(tk, work_items);

						        // create work queues based on whatever devices are relevant for our backend
						        context ctx = m->backend_get_context();
						        scheduler sch(ctx);
						        auto work = sch.make_queue(state_size, *tk, filter);

				            bool success = true;
						        batcher.begin_assignment();

				            // keep track of wallclock time
				            boost::timer::cpu_timer timer;

						        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW WORK ASSIGNMENT";

				            // perform the integration
				            try
					            {
				                m->backend_process_queue(work, tk, batcher, true);    // 'true' = work silently
					            }
				            catch(runtime_exception& xe)
					            {
				                success = false;
				                BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::error) << "-- Exception reported during integration: code=" << xe.get_exception_code() << ": " << xe.what();
					            }
				            if(batcher.integrations_failed()) success = false;

				            // all work is now done - stop the wallclock timer
						        batcher.end_assignment();
				            timer.stop();

				            // notify master process that all work has been finished (temporary containers will be deleted by the master node)
				            now = boost::posix_time::second_clock::universal_time();
				            if(success) BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending FINISHED_INTEGRATION to master | finished at " << boost::posix_time::to_simple_string(now);
				            else        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::error)  << std::endl << "-- Worker reporting INTEGRATION_FAIL to master | finished at " << boost::posix_time::to_simple_string(now);

				            MPI::finished_integration_payload outgoing_payload(batcher.get_integration_time(),
				                                                               batcher.get_max_integration_time(), batcher.get_min_integration_time(),
				                                                               batcher.get_batching_time(),
				                                                               batcher.get_max_batching_time(), batcher.get_min_batching_time(),
				                                                               timer.elapsed().wall,
				                                                               batcher.get_reported_integrations());

				            this->world.isend(MPI::RANK_MASTER, success ? MPI::FINISHED_INTEGRATION : MPI::INTEGRATION_FAIL, outgoing_payload);

						        break;
					        };

				        case MPI::END_OF_WORK:
					        {
						        this->world.recv(stat.source(), MPI::END_OF_WORK);
						        complete = true;
						        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- Notified of end-of-work: preparing to shut down";

				            // close the batcher, flushing the current container to the master node if needed
				            batcher.close();

						        // send close-down acknowledgment to master
				            now = boost::posix_time::second_clock::universal_time();
						        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending WORKER_CLOSE_DOWN to master | close down at " << boost::posix_time::to_simple_string(now);
						        this->world.isend(MPI::RANK_MASTER, MPI::WORKER_CLOSE_DOWN);

						        break;
					        };

				        default:
					        {
						        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "!! Received unexpected MPI message " << stat.tag() << " from master node; discarding";
						        this->world.recv(stat.source(), stat.tag());
						        break;
					        }
					    }
			    }
	    }


    template <typename number>
    void slave_controller<number>::process_task(const MPI::new_derived_content_payload& payload)
	    {
        // ensure that a valid repository object has been constructed
        if(this->repo == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NOT_SET);

        // extract our task from the database
        // much of this is boiler-plate which is similar to master_process_task()
        // TODO: it would be nice to make this sharing more explicit, so the code isn't just duplicated
        try
	        {
            // query a task record with the name we're looking for from the database
            std::unique_ptr< task_record<number> > record(this->repo->query_task(payload.get_task_name()));

            switch(record->get_type())
	            {
                case task_record<number>::integration:
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_TASK_IS_INTEGRATION << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
	                }

                case task_record<number>::output:
	                {
                    output_task_record<number>* out_rec = dynamic_cast< output_task_record<number>* >(record.get());

                    assert(out_rec != nullptr);
                    if(out_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    output_task<number>* tk = out_rec->get_task();
                    this->schedule_output(tk, payload);
                    break;
	                }

                case task_record<number>::postintegration:
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_TASK_IS_POSTINTEGRATION << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
	                }

                default:
	                {
                    assert(false);

                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_UNKNOWN_RECORD_TYPE << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	                }
	            }

	        }
        catch (runtime_exception xe)
	        {
            if(xe.get_exception_code() == runtime_exception::RECORD_NOT_FOUND)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_MISSING_RECORD << " '" << xe.what() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error_handler(msg.str());
	            }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE
	                  || xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
	            {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << payload.get_task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error_handler(msg.str());
	            }
            else
	            {
                throw xe;
	            }
	        }
	    }


    template <typename number>
    void slave_controller<number>::schedule_output(output_task<number>* tk, const MPI::new_derived_content_payload& payload)
	    {
        assert(tk != nullptr);  // should be guaranteed

        // send scheduling information to the master process; here, report ourselves as a CPU
        // since there is currently no capacity to process output tasks on a GPU
        this->send_worker_data();

        // set up output-group finder function
        typename data_manager<number>::datapipe::output_group_finder finder =
	                                                                     std::bind(&repository<number>::find_integration_task_output, this->repo, std::placeholders::_1, std::placeholders::_2);

        // set up content-dispatch function
        typename data_manager<number>::datapipe::dispatch_function dispatcher =
	                                                                   std::bind(&slave_controller<number>::push_derived_content, this, std::placeholders::_1, std::placeholders::_2);

        // acquire a datapipe which we can use to stream content from the databse
        typename data_manager<number>::datapipe pipe = this->data_mgr->create_datapipe(payload.get_logdir_path(), payload.get_tempdir_path(),
                                                                                       finder, dispatcher, this->get_rank());

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "-- NEW OUTPUT TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << *tk;

		    bool complete = false;
		    while(!complete)
			    {
				    // wait for messages from scheduler
		        boost::mpi::status stat = this->world.probe(MPI::RANK_MASTER);

				    switch(stat.tag())
					    {
				        case MPI::NEW_WORK_ASSIGNMENT:
					        {
				            MPI::work_assignment_payload assignment_payload;
						        this->world.recv(stat.source(), MPI::NEW_WORK_ASSIGNMENT, assignment_payload);

						        const std::list<unsigned int>& work_items = assignment_payload.get_items();
						        auto filter = this->work_item_filter_factory(tk, work_items);

						        // create work queues
						        context ctx;
						        ctx.add_device("CPU");
						        scheduler sch(ctx);
						        auto work = sch.make_queue(*tk, filter);

				            bool success = true;

				            // keep track of wallclock time
				            boost::timer::cpu_timer timer;
				            boost::timer::nanosecond_type processing_time = 0;
				            boost::timer::nanosecond_type min_processing_time = 0;
				            boost::timer::nanosecond_type max_processing_time = 0;

				            std::ostringstream work_msg;
				            work_msg << work;
				            BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << work_msg.str();

				            const typename work_queue< output_task_element<number> >::device_queue queues = work[0];
				            assert(queues.size() == 1);

				            const typename work_queue< output_task_element<number> >::device_work_list list = queues[0];

				            for(unsigned int i = 0; i < list.size(); i++)
					            {
				                typename derived_data::derived_product<number>* product = list[i].get_product();

				                assert(product != nullptr);
				                if(product == nullptr)
					                {
				                    std::ostringstream msg;
				                    msg << __CPP_TRANSPORT_TASK_NULL_DERIVED_PRODUCT << " '" << tk->get_name() << "'";
				                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
					                }

				                // merge command-line supplied tags with tags specified in the task
				                std::list<std::string> task_tags = list[i].get_tags();
				                std::list<std::string> command_line_tags = payload.get_tags();

				                task_tags.splice(task_tags.end(), command_line_tags);

				                BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << "-- Processing derived product '" << product->get_name() << "'";

				                try
					                {
				                    boost::timer::cpu_timer derive_timer;
				                    product->derive(pipe, task_tags);
						                derive_timer.stop();
						                processing_time += derive_timer.elapsed().wall;
						                if(max_processing_time == 0 || derive_timer.elapsed().wall > max_processing_time) max_processing_time = derive_timer.elapsed().wall;
						                if(min_processing_time == 0 || derive_timer.elapsed().wall < min_processing_time) min_processing_time = derive_timer.elapsed().wall;
					                }
				                catch(runtime_exception& xe)
					                {
				                    success = false;
				                    BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::error) << "!! Exception reported while processing: code=" << xe.get_exception_code() << ": " << xe.what();
					                }

				                // check that the datapipe was correctly detached
				                if(pipe.is_attached())
					                {
				                    BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::error) << "!! Task manager detected that datapipe was not correctly detached after generating derived product '" << product->get_name() << "'";
				                    pipe.detach();
					                }

				                BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << "";
					            }

				            // all work now done - stop the timer
				            timer.stop();

				            // notify master process that all work has been finished
				            now = boost::posix_time::second_clock::universal_time();
				            if(success) BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending FINISHED_DERIVED_CONTENT to master | finished at " << boost::posix_time::to_simple_string(now);
				            else        BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::error)  << std::endl << "-- Worker reporting DERIVED_CONTENT_FAIL to master | finished at " << boost::posix_time::to_simple_string(now);

				            MPI::finished_derived_payload finish_payload(pipe.get_database_time(), timer.elapsed().wall,
				                                                         list.size(), processing_time,
				                                                         min_processing_time, max_processing_time,
				                                                         pipe.get_time_config_cache_hits(), pipe.get_time_config_cache_unloads(),
				                                                         pipe.get_twopf_kconfig_cache_hits(), pipe.get_twopf_kconfig_cache_unloads(),
				                                                         pipe.get_threepf_kconfig_cache_hits(), pipe.get_threepf_kconfig_cache_unloads(),
				                                                         pipe.get_data_cache_hits(), pipe.get_data_cache_unloads(),
				                                                         pipe.get_zeta_cache_hits(), pipe.get_zeta_cache_unloads(),
				                                                         pipe.get_time_config_cache_evictions(), pipe.get_twopf_kconfig_cache_evictions(),
				                                                         pipe.get_threepf_kconfig_cache_evictions(), pipe.get_data_cache_evictions(),
				                                                         pipe.get_zeta_cache_evictions());

				            this->world.isend(MPI::RANK_MASTER, success ? MPI::FINISHED_DERIVED_CONTENT : MPI::DERIVED_CONTENT_FAIL, finish_payload);

						        break;
					        }

				        case MPI::END_OF_WORK:
					        {
				            this->world.recv(stat.source(), MPI::END_OF_WORK);
				            complete = true;
				            BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "-- Notified of end-of-work: preparing to shut down";

				            // close the datapipe
				            pipe.close();

				            // send close-down acknowledgment to master
				            now = boost::posix_time::second_clock::universal_time();
				            BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending WORKER_CLOSE_DOWN to master | close down at " << boost::posix_time::to_simple_string(now);
				            this->world.isend(MPI::RANK_MASTER, MPI::WORKER_CLOSE_DOWN);

				            break;
					        }

				        default:
					        {
				            BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << "!! Received unexpected MPI message " << stat.tag() << " from master node; discarding";
				            this->world.recv(stat.source(), stat.tag());
				            break;
					        }
					    };
			    }

	    }


    template <typename number>
    void slave_controller<number>::process_task(const MPI::new_postintegration_payload& payload)
	    {
        // ensure that a valid repository object has been constructed
        if(this->repo == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NOT_SET);

        // extract our task from the database
        try
	        {
            // query a task record with the name we're looking for from the database
            std::unique_ptr< task_record<number> > record(this->repo->query_task(payload.get_task_name()));

            switch(record->get_type())
	            {
                case task_record<number>::integration:
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_TASK_IS_INTEGRATION << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
	                }

                case task_record<number>::output:
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_TASK_IS_OUTPUT << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
	                }

                case task_record<number>::postintegration:
	                {
                    postintegration_task_record<number>* pint_rec = dynamic_cast< postintegration_task_record<number>* >(record.get());

                    assert(pint_rec != nullptr);
                    if(pint_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    postintegration_task<number>* tk = pint_rec->get_task();
                    this->dispatch_postintegration_task(tk, payload);
                    break;
	                }

                default:
	                {
                    assert(false);

                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_UNKNOWN_RECORD_TYPE << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	                }
	            }
	        }
        catch(runtime_exception xe)
	        {
            if(xe.get_exception_code() == runtime_exception::RECORD_NOT_FOUND)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_MISSING_RECORD << " '" << xe.what() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error_handler(msg.str());
	            }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE
	                  || xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
	            {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << payload.get_task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error_handler(msg.str());
	            }
            else
	            {
                throw xe;
	            }
	        }
	    }


    template <typename number>
    void slave_controller<number>::dispatch_postintegration_task(postintegration_task<number>* tk, const MPI::new_postintegration_payload& payload)
	    {
        assert(tk != nullptr);

        // send scheduling information to the master process; here, report ourselves as a CPU
		    // since there is currently no capacity to process postintegration tasks on a GPU
        this->send_worker_data();

        zeta_twopf_task<number>* z2pf = nullptr;
        zeta_threepf_task<number>* z3pf = nullptr;
        fNL_task<number>* zfNL = nullptr;

        if((z2pf = dynamic_cast<zeta_twopf_task<number>*>(tk)) != nullptr)
	        {
		        // get parent task
            twopf_task<number>* ptk = dynamic_cast<twopf_task<number>*>(z2pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_TWOPF_TASK << " '" << z2pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            // construct a callback for the integrator to push new batches to the master
            typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                       std::bind(&slave_controller<number>::push_temp_container, this, std::placeholders::_1,
	                                                                                 MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            typename data_manager<number>::zeta_twopf_batcher batcher =
	                                                              this->data_mgr->create_temp_zeta_twopf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                               this->get_rank(), dispatcher);

            this->schedule_postintegration(z2pf, ptk, payload, batcher);
	        }
        else if((z3pf = dynamic_cast<zeta_threepf_task<number>*>(tk)) != nullptr)
	        {
		        // get parent task
            threepf_task<number>* ptk = dynamic_cast<threepf_task<number>*>(z3pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_TWOPF_TASK << " '" << z3pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            // construct a callback for the integrator to push new batches to the master
            typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                       std::bind(&slave_controller<number>::push_temp_container, this, std::placeholders::_1,
	                                                                                 MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            typename data_manager<number>::zeta_threepf_batcher batcher =
	                                                                this->data_mgr->create_temp_zeta_threepf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                                   this->get_rank(), dispatcher);

            this->schedule_postintegration(z3pf, ptk, payload, batcher);
	        }
        else if((zfNL = dynamic_cast<fNL_task<number>*>(tk)) != nullptr)
	        {
		        // get parent task
            threepf_task<number>* ptk = dynamic_cast<threepf_task<number>*>(zfNL->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_TWOPF_TASK << " '" << zfNL->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            // construct a callback for the integrator to push new batches to the master
            typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                       std::bind(&slave_controller<number>::push_temp_container, this, std::placeholders::_1,
	                                                                                 MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            typename data_manager<number>::fNL_batcher batcher =
	                                                       this->data_mgr->create_temp_fNL_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                 this->get_rank(), dispatcher, zfNL->get_template());

            this->schedule_postintegration(zfNL, ptk, payload, batcher);
	        }
        else
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
	    }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject, typename BatchObject>
    void slave_controller<number>::schedule_postintegration(TaskObject* tk, ParentTaskObject* ptk,
                                                            const MPI::new_postintegration_payload& payload, BatchObject& batcher)
	    {
        assert(tk != nullptr);    // should be guaranteed
        assert(ptk != nullptr);   // should be guaranteed

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- NEW POSTINTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

		    bool complete = false;
		    while(!complete)
			    {
				    // wait for messages from scheduler
		        boost::mpi::status stat = this->world.probe(MPI::RANK_MASTER);

				    switch(stat.tag())
					    {
				        case MPI::NEW_WORK_ASSIGNMENT:
					        {
				            MPI::work_assignment_payload assignment_payload;
						        this->world.recv(stat.source(), MPI::NEW_WORK_ASSIGNMENT, assignment_payload);

				            const std::list<unsigned int>& work_items = assignment_payload.get_items();
						        auto filter = this->work_item_filter_factory(ptk, work_items);

						        // create work queues
						        context ctx;
				            ctx.add_device("CPU");
						        scheduler sch(ctx);
						        auto work = sch.make_queue(sizeof(number), *ptk, filter);

				            bool success = true;
						        batcher.begin_assignment();

				            // keep track of wallclock time
				            boost::timer::cpu_timer timer;

				            BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW WORK ASSIGNMENT";

				            // set up output-group finder function
				            typename data_manager<number>::datapipe::output_group_finder finder =
					                                                                         std::bind(&repository<number>::find_integration_task_output, this->repo, std::placeholders::_1, std::placeholders::_2);

				            // set up empty content-dispatch function -- this datapipe is not used to produce content
				            typename data_manager<number>::datapipe::dispatch_function dispatcher =
					                                                                       std::bind(&slave_controller<number>::disallow_push_content, this, std::placeholders::_1, std::placeholders::_2);

				            // acquire a datapipe which we can use to stream content from the databse
				            typename data_manager<number>::datapipe pipe = this->data_mgr->create_datapipe(payload.get_logdir_path(), payload.get_tempdir_path(),
				                                                                                           finder, dispatcher, this->get_rank(), true);

				            // perform the task
				            try
					            {
				                pipe.attach(ptk, ptk->get_model()->get_N_fields(), payload.get_tags());
				                this->work_handler.postintegration_handler(tk, ptk, work, batcher, pipe);
				                pipe.detach();
					            }
				            catch(runtime_exception& xe)
					            {
				                success = false;
				                BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::error) << "-- Exception reported during integration: code=" << xe.get_exception_code() << ": " << xe.what();
					            }

				            // inform the batcher we are at the end of this assignment
				            batcher.end_assignment();

				            // all work is now done - stop the wallclock timer
				            timer.stop();

				            // notify master process that all work has been finished (temporary containers will be deleted by the master node)
				            now = boost::posix_time::second_clock::universal_time();
				            if(success) BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending FINISHED_POSTINTEGRATION to master | finished at " << boost::posix_time::to_simple_string(now);
				            else        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::error)  << std::endl << "-- Worker reporting POSTINTEGRATION_FAIL to master | finished at " << boost::posix_time::to_simple_string(now);

				            MPI::finished_postintegration_payload outgoing_payload(pipe.get_database_time(), timer.elapsed().wall,
				                                                                   batcher.get_items_processed(), batcher.get_processing_time(),
				                                                                   batcher.get_max_processing_time(), batcher.get_min_processing_time(),
				                                                                   pipe.get_time_config_cache_hits(), pipe.get_time_config_cache_unloads(),
				                                                                   pipe.get_twopf_kconfig_cache_hits(), pipe.get_twopf_kconfig_cache_unloads(),
				                                                                   pipe.get_threepf_kconfig_cache_hits(), pipe.get_threepf_kconfig_cache_unloads(),
				                                                                   pipe.get_data_cache_hits(), pipe.get_data_cache_unloads(),
				                                                                   pipe.get_zeta_cache_hits(), pipe.get_zeta_cache_unloads(),
				                                                                   pipe.get_time_config_cache_evictions(), pipe.get_twopf_kconfig_cache_evictions(),
				                                                                   pipe.get_threepf_kconfig_cache_evictions(), pipe.get_data_cache_evictions(),
				                                                                   pipe.get_zeta_cache_evictions());

				            this->world.isend(MPI::RANK_MASTER, success ? MPI::FINISHED_POSTINTEGRATION : MPI::POSTINTEGRATION_FAIL, outgoing_payload);

						        break;
					        }

				        case MPI::END_OF_WORK:
					        {
						        this->world.recv(stat.source(), MPI::END_OF_WORK);
						        complete = true;
						        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- Notified of end-of-work: preparing to shut down";

						        // close the batcher, flushing the current container to the master node if required
						        batcher.close();

						        // send close-down acknowledgment to master
						        now = boost::posix_time::second_clock::universal_time();
						        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending WORKER_CLOSE_DOWN to master | close down at " << boost::posix_time::to_simple_string(now);
						        this->world.isend(MPI::RANK_MASTER, MPI::WORKER_CLOSE_DOWN);

						        break;
					        }

				        default:
					        {
				            BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "!! Received unexpected MPI message " << stat.tag() << " from master node; discarding";
				            this->world.recv(stat.source(), stat.tag());
				            break;
					        }
					    };
			    }
	    }


    template <typename number>
    void slave_controller<number>::push_temp_container(typename data_manager<number>::generic_batcher* batcher, unsigned int message, std::string log_message)
	    {
        assert(batcher != nullptr);
        if(batcher == nullptr) throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_BATCHER);

        BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal) << "-- Sending " << log_message << " message for container " << batcher->get_container_path();

        MPI::data_ready_payload payload(batcher->get_container_path().string());

        // advise master process that data is available in the named container
        this->world.isend(MPI::RANK_MASTER, message, payload);
	    }


    template <typename number>
    void slave_controller<number>::push_derived_content(typename data_manager<number>::datapipe* pipe, typename derived_data::derived_product<number>* product)
	    {
        assert(pipe != nullptr);
        assert(product != nullptr);

        // FIXME: error message tag is possibly in the wrong namespace (but error message namespaces are totally confused anyway)
        if(pipe == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE);
        if(product == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DERIVED_PRODUCT);

        BOOST_LOG_SEV(pipe->get_log(), data_manager<number>::normal) << "-- Sending DERIVED_CONTENT_READY message for derived product '" << product->get_name() << "'";

        boost::filesystem::path product_filename = pipe->get_abs_tempdir_path() / product->get_filename();
        if(boost::filesystem::exists(product_filename))
	        {
            MPI::content_ready_payload payload(product->get_name());
            this->world.isend(MPI::RANK_MASTER, MPI::DERIVED_CONTENT_READY, payload);
	        }
        else
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATAMGR_DERIVED_PRODUCT_MISSING << " " << product_filename;
            throw runtime_exception(runtime_exception::DATAPIPE_ERROR, msg.str());
	        }
	    }


    template <typename number>
    void slave_controller<number>::disallow_push_content(typename data_manager<number>::datapipe* pipe, typename derived_data::derived_product<number>* product)
	    {
        assert(false);
	    }

	}


#endif //__slave_controller_H_
