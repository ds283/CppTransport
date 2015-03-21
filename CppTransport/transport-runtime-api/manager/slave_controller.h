//
// Created by David Seery on 19/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __slave_controller_H_
#define __slave_controller_H_


#include <list>
#include <set>
#include <vector>
#include <memory>
#include <functional>

#include "transport-runtime-api/models/model.h"
#include "transport-runtime-api/manager/instance_manager.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"

#include "transport-runtime-api/manager/mpi_operations.h"

#include "transport-runtime-api/manager/repository.h"
#include "transport-runtime-api/manager/json_repository_interface.h"
#include "transport-runtime-api/manager/data_manager.h"

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

		    //! Slave node: set repository
		    void set_repository(const MPI::data_request_payload& payload);


		    // SLAVE INTEGRATION TASKS

		  protected:

		    //! Slave node: Process a new integration task instruction
		    void process_task(const MPI::new_integration_payload& payload);

		    //! Slave node: process an integration task
		    void dispatch_integration_task(integration_task<number>* tk, const MPI::new_integration_payload& payload);

		    //! Slave node: process an integration queue
		    template <typename TaskObject, typename QueueObject, typename BatchObject>
		    void dispatch_integration_queue(TaskObject* tk, model<number>* m, QueueObject& queue,
		                                    const MPI::new_integration_payload& payload, BatchObject& batcher);

		    //! Push a temporary container to the master process
		    void push_temp_container(typename data_manager<number>::generic_batcher* batcher, unsigned int message, std::string log_message);


		    // SLAVE POSTINTEGRATION TASKS

		  protected:

		    //! Slave node: Process a new postintegration task instruction
		    void process_task(const MPI::new_postintegration_payload& payload);

		    //! Slave node: Process a postintegration task
		    void dispatch_postintegration_task(postintegration_task<number>* tk, const MPI::new_postintegration_payload& payload);

		    //! Slave node: process a postintegration queue
		    template <typename TaskObject, typename ParentTaskObject, typename QueueObject, typename BatchObject>
		    void dispatch_postintegration_queue(TaskObject* tk, ParentTaskObject* ptk, QueueObject& work,
		                                        const MPI::new_postintegration_payload& payload, BatchObject& batcher);

		    //! No-op push content for connexion to datapipe
		    void disallow_push_content(typename data_manager<number>::datapipe* pipe, typename derived_data::derived_product<number>* product);

		    //! Handler: zeta twopf task
		    void postintegration_handler(zeta_twopf_task<number>* tk, twopf_task<number>* ptk, work_queue<twopf_kconfig>& work,
		                                 typename data_manager<number>::zeta_twopf_batcher& batcher, typename data_manager<number>::datapipe& pipe);

		    //! Handler: zeta threepf task
		    void postintegration_handler(zeta_threepf_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
		                                 typename data_manager<number>::zeta_threepf_batcher& batcher, typename data_manager<number>::datapipe& pipe);

		    //! Handler: fNL task
		    void postintegration_handler(fNL_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
		                                 typename data_manager<number>::fNL_batcher& batcher, typename data_manager<number>::datapipe& pipe);

		    // SLAVE OUTPUT TASKS

		  protected:

		    //! Slave node: Process a new output task instruction
		    void process_task(const MPI::new_derived_content_payload& payload);

		    //! Slave node: Process an output task
		    void dispatch_output_task(output_task<number>* tk, const MPI::new_derived_content_payload& payload);

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

                case MPI::SET_REPOSITORY:
	                {
                    MPI::data_request_payload payload;
                    this->world.recv(MPI::RANK_MASTER, MPI::SET_REPOSITORY, payload);
                    this->set_repository(payload);
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
    void slave_controller<number>::set_repository(const MPI::data_request_payload& payload)
	    {
        boost::filesystem::path repo_path = payload.get_repository_path();

        try
	        {
            this->repo = repository_factory<number>(repo_path.string(), repository<number>::access_type::readonly,
                                                    this->error_handler, this->warning_handler, this->message_handler);
            this->repo->set_model_finder(this->model_finder);
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
            std::unique_ptr<typename repository<number>::task_record> record(this->repo->query_task(payload.get_task_name()));

            switch(record->get_type())
	            {
                case repository<number>::task_record::integration:
	                {
                    typename repository<number>::integration_task_record* int_rec = dynamic_cast<typename repository<number>::integration_task_record*>(record.get());

                    assert(int_rec != nullptr);
                    if(int_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    integration_task<number>* tk = int_rec->get_task();
                    this->dispatch_integration_task(tk, payload);
                    break;
	                }

                case repository<number>::task_record::output:
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_TASK_IS_OUTPUT << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
	                }

                case repository<number>::task_record::postintegration:
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

        twopf_task<number>* tka = nullptr;
        threepf_task<number>* tkb = nullptr;

        if((tka = dynamic_cast<twopf_task<number>*>(tk)) != nullptr)
	        {
            std::set<unsigned int> work_items = this->data_mgr->read_taskfile(payload.get_taskfile_path(), this->worker_number());
            work_item_filter<twopf_kconfig> filter(work_items);

            // create queues based on whatever devices are relevant for the backend
            context                   ctx  = m->backend_get_context();
            scheduler                 sch  = scheduler(ctx);
            work_queue<twopf_kconfig> work = sch.make_queue(m->backend_twopf_state_size(), *tka, filter);

            // construct a callback for the integrator to push new batches to the master
            typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                       std::bind(&slave_controller<number>::push_temp_container, this, std::placeholders::_1,
	                                                                                 MPI::INTEGRATION_DATA_READY, std::string("INTEGRATION_DATA_READY"));

            // construct a batcher to hold the output of the integration
            typename data_manager<number>::twopf_batcher batcher =
	                                                         this->data_mgr->create_temp_twopf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                     this->get_rank(), m, dispatcher);

            this->dispatch_integration_queue(tka, m, work, payload, batcher);
	        }
        else if((tkb = dynamic_cast<threepf_task<number>*>(tk)) != nullptr)
	        {
            std::set<unsigned int> work_items = this->data_mgr->read_taskfile(payload.get_taskfile_path(), this->worker_number());
            work_item_filter<threepf_kconfig> filter(work_items);

            // create queues based on whatever devices are relevant for the backend
            context                     ctx  = m->backend_get_context();
            scheduler                   sch  = scheduler(ctx);
            work_queue<threepf_kconfig> work = sch.make_queue(m->backend_threepf_state_size(), *tkb, filter);

            // construct a callback for the integrator to push new batches to the master
            typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                       std::bind(&slave_controller<number>::push_temp_container, this, std::placeholders::_1,
	                                                                                 MPI::INTEGRATION_DATA_READY, std::string("INTEGRATION_DATA_READY"));

            // construct a batcher to hold the output of the integration
            typename data_manager<number>::threepf_batcher batcher =
	                                                           this->data_mgr->create_temp_threepf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                         this->get_rank(), m, dispatcher);

            this->dispatch_integration_queue(tkb, m, work, payload, batcher);
	        }
        else
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
	    }


    template <typename number>
    template <typename TaskObject, typename QueueObject, typename BatchObject>
    void slave_controller<number>::dispatch_integration_queue(TaskObject* tk, model<number>* m, QueueObject& queue,
                                                           const MPI::new_integration_payload& payload, BatchObject& batcher)
	    {
        // dispatch integration to the underlying model
        assert(tk != nullptr);  // should be guaranteed
        assert(m != nullptr);   // should be guaranteed

        bool success = true;

        // keep track of wallclock time
        boost::timer::cpu_timer timer;

		    // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW INTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

        // perform the integration
        try
	        {
            m->backend_process_queue(queue, tk, batcher, true);    // 'true' = work silently
	        }
        catch(runtime_exception& xe)
	        {
            success = false;
            BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::error) << "-- Exception reported during integration: code=" << xe.get_exception_code() << ": " << xe.what();
	        }

        // close the batcher
        batcher.close();
        if(batcher.integrations_failed()) success = false;

        // all work is now done - stop the wallclock timer
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
            std::unique_ptr<typename repository<number>::task_record> record(this->repo->query_task(payload.get_task_name()));

            switch(record->get_type())
	            {
                case repository<number>::task_record::integration:
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_TASK_IS_INTEGRATION << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
	                }

                case repository<number>::task_record::output:
	                {
                    typename repository<number>::output_task_record* out_rec = dynamic_cast<typename repository<number>::output_task_record*>(record.get());

                    assert(out_rec != nullptr);
                    if(out_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    output_task<number>* tk = out_rec->get_task();
                    this->dispatch_output_task(tk, payload);
                    break;
	                }

                case repository<number>::task_record::postintegration:
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
    void slave_controller<number>::dispatch_output_task(output_task<number>* tk, const MPI::new_derived_content_payload& payload)
	    {
        assert(tk != nullptr);  // should be guaranteed

        std::set<unsigned int> work_items = this->data_mgr->read_taskfile(payload.get_taskfile_path(), this->worker_number());
        work_item_filter< output_task_element<number> > filter(work_items);

        // create a context and queue
        context ctx = context();
        ctx.add_device("CPU");
        scheduler sch = scheduler(ctx);
        work_queue< output_task_element<number> > work = sch.make_queue(*tk, filter);

        bool success = true;

        // keep track of CPU time
        boost::timer::cpu_timer timer;

        // set up output-group finder function
        typename data_manager<number>::datapipe::output_group_finder finder =
	                                                                     std::bind(&repository<number>::find_integration_task_output, this->repo, std::placeholders::_1, std::placeholders::_2);

        // set up content-dispatch function
        typename data_manager<number>::datapipe::dispatch_function dispatcher =
	                                                                   std::bind(&slave_controller<number>::push_derived_content, this, std::placeholders::_1, std::placeholders::_2);

        // acquire a datapipe which we can use to stream content from the databse
        typename data_manager<number>::datapipe pipe = this->data_mgr->create_datapipe(payload.get_logdir_path(), payload.get_tempdir_path(),
                                                                                       finder, dispatcher, this->get_rank(), timer);

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << "-- NEW OUTPUT TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << *tk;

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
                product->derive(pipe, task_tags);
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

        // close the datapipe
        pipe.close();

        // all work now done - stop the timer
        timer.stop();

        // notify master process that all work has been finished
        now = boost::posix_time::second_clock::universal_time();
        if(success) BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending FINISHED_DERIVED_CONTENT to master | finished at " << boost::posix_time::to_simple_string(now);
        else        BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::error)  << std::endl << "-- Worker reporting DERIVED_CONTENT_FAIL to master | finished at " << boost::posix_time::to_simple_string(now);

        MPI::finished_derived_payload finish_payload(pipe.get_database_time(), timer.elapsed().wall,
                                                     pipe.get_time_config_cache_hits(), pipe.get_time_config_cache_unloads(),
                                                     pipe.get_twopf_kconfig_cache_hits(), pipe.get_twopf_kconfig_cache_unloads(),
                                                     pipe.get_threepf_kconfig_cache_hits(), pipe.get_threepf_kconfig_cache_unloads(),
                                                     pipe.get_data_cache_hits(), pipe.get_data_cache_unloads(),
                                                     pipe.get_zeta_cache_hits(), pipe.get_zeta_cache_unloads(),
                                                     pipe.get_time_config_cache_evictions(), pipe.get_twopf_kconfig_cache_evictions(),
                                                     pipe.get_threepf_kconfig_cache_evictions(), pipe.get_data_cache_evictions(),
                                                     pipe.get_zeta_cache_evictions());

        this->world.isend(MPI::RANK_MASTER, success ? MPI::FINISHED_DERIVED_CONTENT : MPI::DERIVED_CONTENT_FAIL, finish_payload);
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
            std::unique_ptr<typename repository<number>::task_record> record(this->repo->query_task(payload.get_task_name()));

            switch(record->get_type())
	            {
                case repository<number>::task_record::integration:
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_TASK_IS_INTEGRATION << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
	                }

                case repository<number>::task_record::output:
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_TASK_IS_OUTPUT << " '" << payload.get_task_name() << "'";
                    throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
	                }

                case repository<number>::task_record::postintegration:
	                {
                    typename repository<number>::postintegration_task_record* pint_rec = dynamic_cast<typename repository<number>::postintegration_task_record*>(record.get());

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

        // create a context and work queue
        context ctx  = context();
        ctx.add_device("CPU");
        scheduler sch = scheduler(ctx);

        zeta_twopf_task<number>* z2pf = nullptr;
        zeta_threepf_task<number>* z3pf = nullptr;
        fNL_task<number>* zfNL = nullptr;

        if((z2pf = dynamic_cast<zeta_twopf_task<number>*>(tk)) != nullptr)
	        {
            twopf_task<number>* ptk = dynamic_cast<twopf_task<number>*>(z2pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_TWOPF_TASK << " '" << z2pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            std::set<unsigned int> work_items = this->data_mgr->read_taskfile(payload.get_taskfile_path(), this->worker_number());
            work_item_filter<twopf_kconfig> filter(work_items);

            work_queue<twopf_kconfig> work = sch.make_queue(sizeof(number), *ptk, filter);

            // construct a callback for the integrator to push new batches to the master
            typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                       std::bind(&slave_controller<number>::push_temp_container, this, std::placeholders::_1,
	                                                                                 MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            typename data_manager<number>::zeta_twopf_batcher batcher =
	                                                              this->data_mgr->create_temp_zeta_twopf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                               this->get_rank(), dispatcher);

            this->dispatch_postintegration_queue(z2pf, ptk, work, payload, batcher);
	        }
        else if((z3pf = dynamic_cast<zeta_threepf_task<number>*>(tk)) != nullptr)
	        {
            threepf_task<number>* ptk = dynamic_cast<threepf_task<number>*>(z3pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_TWOPF_TASK << " '" << z3pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            std::set<unsigned int> work_items = this->data_mgr->read_taskfile(payload.get_taskfile_path(), this->worker_number());
            work_item_filter<threepf_kconfig> filter(work_items);

            work_queue<threepf_kconfig> work = sch.make_queue(sizeof(number), *ptk, filter);

            // construct a callback for the integrator to push new batches to the master
            typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                       std::bind(&slave_controller<number>::push_temp_container, this, std::placeholders::_1,
	                                                                                 MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            typename data_manager<number>::zeta_threepf_batcher batcher =
	                                                                this->data_mgr->create_temp_zeta_threepf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                                   this->get_rank(), dispatcher);

            this->dispatch_postintegration_queue(z3pf, ptk, work, payload, batcher);
	        }
        else if((zfNL = dynamic_cast<fNL_task<number>*>(tk)) != nullptr)
	        {
            threepf_task<number>* ptk = dynamic_cast<threepf_task<number>*>(zfNL->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_TWOPF_TASK << " '" << zfNL->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            std::set<unsigned int> work_items = this->data_mgr->read_taskfile(payload.get_taskfile_path(), this->worker_number());
            work_item_filter<threepf_kconfig> filter(work_items);

            work_queue<threepf_kconfig> work = sch.make_queue(sizeof(number), *ptk, filter);

            // construct a callback for the integrator to push new batches to the master
            typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                       std::bind(&slave_controller<number>::push_temp_container, this, std::placeholders::_1,
	                                                                                 MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            typename data_manager<number>::fNL_batcher batcher =
	                                                       this->data_mgr->create_temp_fNL_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                 this->get_rank(), dispatcher, zfNL->get_template());

            this->dispatch_postintegration_queue(zfNL, ptk, work, payload, batcher);
	        }
        else
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
	    }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject, typename QueueObject, typename BatchObject>
    void slave_controller<number>::dispatch_postintegration_queue(TaskObject* tk, ParentTaskObject* ptk, QueueObject& work,
                                                               const MPI::new_postintegration_payload& payload, BatchObject& batcher)
	    {
        assert(tk != nullptr);    // should be guaranteed
        assert(ptk != nullptr);   // should be guaranteed

        bool success = true;

        // keep track of wallclock time
        boost::timer::cpu_timer timer;

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW POSTINTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

        // set up output-group finder function
        typename data_manager<number>::datapipe::output_group_finder finder =
	                                                                     std::bind(&repository<number>::find_integration_task_output, this->repo, std::placeholders::_1, std::placeholders::_2);

        // set up empty content-dispatch function -- this datapipe is not used to produce content
        typename data_manager<number>::datapipe::dispatch_function dispatcher =
	                                                                   std::bind(&slave_controller<number>::disallow_push_content, this, std::placeholders::_1, std::placeholders::_2);

        // acquire a datapipe which we can use to stream content from the databse
        typename data_manager<number>::datapipe pipe = this->data_mgr->create_datapipe(payload.get_logdir_path(), payload.get_tempdir_path(),
                                                                                       finder, dispatcher, this->get_rank(), timer, true);


        // perform the task
        try
	        {
            pipe.attach(ptk, ptk->get_model()->get_N_fields(), payload.get_tags());
            this->postintegration_handler(tk, ptk, work, batcher, pipe);
            pipe.detach();
	        }
        catch(runtime_exception& xe)
	        {
            success = false;
            BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::error) << "-- Exception reported during integration: code=" << xe.get_exception_code() << ": " << xe.what();
	        }

        // close the batcher
        batcher.close();

        // all work is now done - stop the wallclock timer
        timer.stop();

        // notify master process that all work has been finished (temporary containers will be deleted by the master node)
        now = boost::posix_time::second_clock::universal_time();
        if(success) BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending FINISHED_POSTINTEGRATION to master | finished at " << boost::posix_time::to_simple_string(now);
        else        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::error)  << std::endl << "-- Worker reporting POSTINTEGRATION_FAIL to master | finished at " << boost::posix_time::to_simple_string(now);

        MPI::finished_postintegration_payload outgoing_payload(pipe.get_database_time(), timer.elapsed().wall,
                                                               pipe.get_time_config_cache_hits(), pipe.get_time_config_cache_unloads(),
                                                               pipe.get_twopf_kconfig_cache_hits(), pipe.get_twopf_kconfig_cache_unloads(),
                                                               pipe.get_threepf_kconfig_cache_hits(), pipe.get_threepf_kconfig_cache_unloads(),
                                                               pipe.get_data_cache_hits(), pipe.get_data_cache_unloads(),
                                                               pipe.get_zeta_cache_hits(), pipe.get_zeta_cache_unloads(),
                                                               pipe.get_time_config_cache_evictions(), pipe.get_twopf_kconfig_cache_evictions(),
                                                               pipe.get_threepf_kconfig_cache_evictions(), pipe.get_data_cache_evictions(),
                                                               pipe.get_zeta_cache_evictions());

        this->world.isend(MPI::RANK_MASTER, success ? MPI::FINISHED_POSTINTEGRATION : MPI::POSTINTEGRATION_FAIL, outgoing_payload);
	    }


    template <typename number>
    void slave_controller<number>::postintegration_handler(zeta_twopf_task<number>* tk, twopf_task<number>* ptk, work_queue<twopf_kconfig>& work,
                                                        typename data_manager<number>::zeta_twopf_batcher& batcher,
                                                        typename data_manager<number>::datapipe& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const typename work_queue<twopf_kconfig>::device_queue queues = work[0];
        assert(queues.size() == 1);

        const typename work_queue<twopf_kconfig>::device_work_list list = queues[0];

        // get list of time values at which to sample
        const std::vector<time_config> time_list = ptk->get_time_config_list();

        // reslice to get a vector of serial numbers
        std::vector<unsigned int> time_sns(time_list.size());
        for(unsigned int i = 0; i < time_list.size(); i++)
	        {
            time_sns[i] = time_list[i].serial;
	        }

        // get list of kserial numbers
        std::vector<unsigned int> kconfig_sns(list.size());
        for(unsigned int i = 0; i < list.size(); i++)
	        {
            kconfig_sns[i] = list[i].serial;
	        }

        // set up cache handles
        typename data_manager<number>::datapipe::twopf_kconfig_handle& kc_handle = pipe.new_twopf_kconfig_handle(kconfig_sns);
        typename data_manager<number>::datapipe::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(time_sns);

        typename data_manager<number>::datapipe::twopf_kconfig_tag k_tag = pipe.new_twopf_kconfig_tag();
        const std::vector< typename data_manager<number>::twopf_configuration > k_values = kc_handle.lookup_tag(k_tag);

        for(unsigned int i = 0; i < list.size(); i++)
	        {
            typename data_manager<number>::datapipe::zeta_twopf_time_data_tag tag = pipe.new_zeta_twopf_time_data_tag(k_values[i]);
            // safe to use a reference here to avoid a copy
            const std::vector<number>& twopf = z_handle.lookup_tag(tag);

            for(unsigned int j = 0; j < time_sns.size(); j++)
	            {
                batcher.push_twopf(time_sns[j], kconfig_sns[i], twopf[j]);
	            }
            batcher.report_finished_block();
	        }
	    }


    template <typename number>
    void slave_controller<number>::postintegration_handler(zeta_threepf_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
                                                        typename data_manager<number>::zeta_threepf_batcher& batcher,
                                                        typename data_manager<number>::datapipe& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const typename work_queue<threepf_kconfig>::device_queue queues = work[0];
        assert(queues.size() == 1);

        const typename work_queue<threepf_kconfig>::device_work_list list = queues[0];

        // get list of time values at which to sample
        const std::vector<time_config> time_list = ptk->get_time_config_list();

        // reslice to get a vector of serial numbers
        std::vector<unsigned int> time_sns(time_list.size());
        for(unsigned int i = 0; i < time_list.size(); i++)
	        {
            time_sns[i] = time_list[i].serial;
	        }

        // get list of kserial numbers
        std::vector<unsigned int> kconfig_sns(list.size());
        for(unsigned int i = 0; i < list.size(); i++)
	        {
            kconfig_sns[i] = list[i].serial;
	        }

        // set up cache handles
        typename data_manager<number>::datapipe::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(kconfig_sns);
        typename data_manager<number>::datapipe::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(time_sns);

        typename data_manager<number>::datapipe::threepf_kconfig_tag k_tag = pipe.new_threepf_kconfig_tag();
        const std::vector< typename data_manager<number>::threepf_configuration > k_values = kc_handle.lookup_tag(k_tag);

        for(unsigned int i = 0; i < list.size(); i++)
	        {
            typename data_manager<number>::datapipe::zeta_threepf_time_data_tag tpf_tag = pipe.new_zeta_threepf_time_data_tag(k_values[i]);
            // safe to use a reference here to avoid a copy
            const std::vector<number>& threepf = z_handle.lookup_tag(tpf_tag);

            for(unsigned int j = 0; j < time_sns.size(); j++)
	            {
                batcher.push_threepf(time_sns[j], kconfig_sns[i], threepf[j]);
	            }

            typename data_manager<number>::datapipe::zeta_reduced_bispectrum_time_data_tag rbs_tag = pipe.new_zeta_reduced_bispectrum_time_data_tag(k_values[i]);
            // safe to use a reference here to avoid a copy
            const std::vector<number>& redbsp = z_handle.lookup_tag(rbs_tag);

            for(unsigned int j = 0; j < time_sns.size(); j++)
	            {
                batcher.push_reduced_bispectrum(time_sns[j], kconfig_sns[i], redbsp[j]);
	            }

            if(list[i].store_twopf_k1)
	            {
                typename data_manager<number>::twopf_configuration k1;

                k1.serial         = k_values[i].k1_serial;
                k1.k_comoving     = k_values[i].k1_comoving;
                k1.k_conventional = k_values[i].k1_conventional;

                typename data_manager<number>::datapipe::zeta_twopf_time_data_tag k1_tag = pipe.new_zeta_twopf_time_data_tag(k1);
                // safe to use a reference here to avoid a copy
                const std::vector<number>& k1_twopf = z_handle.lookup_tag(k1_tag);

                for(unsigned int j = 0; j < time_sns.size(); j++)
	                {
                    batcher.push_twopf(time_sns[j], k1.serial, k1_twopf[j]);
	                }
	            }

            if(list[i].store_twopf_k2)
	            {
                typename data_manager<number>::twopf_configuration k2;

                k2.serial         = k_values[i].k2_serial;
                k2.k_comoving     = k_values[i].k2_comoving;
                k2.k_conventional = k_values[i].k2_conventional;

                typename data_manager<number>::datapipe::zeta_twopf_time_data_tag k2_tag = pipe.new_zeta_twopf_time_data_tag(k2);
                // safe to use a reference here to avoid a copy
                const std::vector<number>& k2_twopf = z_handle.lookup_tag(k2_tag);

                for(unsigned int j = 0; j < time_sns.size(); j++)
	                {
                    batcher.push_twopf(time_sns[j], k2.serial, k2_twopf[j]);
	                }
	            }

            if(list[i].store_twopf_k3)
	            {
                typename data_manager<number>::twopf_configuration k3;

                k3.serial         = k_values[i].k3_serial;
                k3.k_comoving     = k_values[i].k3_comoving;
                k3.k_conventional = k_values[i].k3_conventional;

                typename data_manager<number>::datapipe::zeta_twopf_time_data_tag k3_tag = pipe.new_zeta_twopf_time_data_tag(k3);
                // safe to use a reference here to avoid a copy
                const std::vector<number>& k3_twopf = z_handle.lookup_tag(k3_tag);

                for(unsigned int j = 0; j < time_sns.size(); j++)
	                {
                    batcher.push_twopf(time_sns[j], k3.serial, k3_twopf[j]);
	                }
	            }

            batcher.report_finished_block();
	        }
	    }


    template <typename number>
    void slave_controller<number>::postintegration_handler(fNL_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
                                                        typename data_manager<number>::fNL_batcher& batcher,
                                                        typename data_manager<number>::datapipe& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const typename work_queue<threepf_kconfig>::device_queue queues = work[0];
        assert(queues.size() == 1);

        const typename work_queue<threepf_kconfig>::device_work_list list = queues[0];

        // get list of time values at which to sample
        const std::vector<time_config> time_list = ptk->get_time_config_list();

        // reslice to get a vector of serial numbers
        std::vector<unsigned int> time_sns(time_list.size());
        for(unsigned int i = 0; i < time_list.size(); i++)
	        {
            time_sns[i] = time_list[i].serial;
	        }

        // get list of kserial numbers
        std::vector<unsigned int> kconfig_sns(list.size());
        for(unsigned int i = 0; i < list.size(); i++)
	        {
            kconfig_sns[i] = list[i].serial;
	        }

        // set up cache handles
        typename data_manager<number>::datapipe::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(time_sns);

        typename data_manager<number>::datapipe::BT_time_data_tag BT_tag = pipe.new_BT_time_data_tag(tk->get_template(), kconfig_sns);
        typename data_manager<number>::datapipe::TT_time_data_tag TT_tag = pipe.new_TT_time_data_tag(tk->get_template(), kconfig_sns);

        const std::vector<number> BT = z_handle.lookup_tag(BT_tag);
        // safe to use a reference here to avoid a copy
        const std::vector<number>& TT = z_handle.lookup_tag(TT_tag);

        for(unsigned int j = 0; j < time_sns.size(); j++)
	        {
            batcher.push_fNL(time_sns[j], BT[j], TT[j]);
	        }

        batcher.report_finished_block();
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
