//
// Created by David Seery on 24/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __task_manager_H_
#define __task_manager_H_


#include <list>
#include <set>
#include <vector>

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


#define __CPP_TRANSPORT_SWITCH_REPO              "-repo"
#define __CPP_TRANSPORT_SWITCH_TAG               "-tag"
#define __CPP_TRANSPORT_SWITCH_CAPACITY          "-caches"
#define __CPP_TRANSPORT_SWITCH_BATCHER_CAPACITY  "-batch-cache"
#define __CPP_TRANSPORT_SWITCH_CACHE_CAPACITY    "-data-cache"
#define __CPP_TRANSPORT_SWITCH_ZETA_CAPACITY     "-zeta-cache"

#define __CPP_TRANSPORT_VERB_TASK                "task"
#define __CPP_TRANSPORT_VERB_GET                 "get"

#define __CPP_TRANSPORT_NOUN_TASK                "task"
#define __CPP_TRANSPORT_NOUN_PACKAGE             "package"
#define __CPP_TRANSPORT_NOUN_PRODUCT             "product"
#define __CPP_TRANSPORT_NOUN_CONTENT             "content"

// name for worker devices
#define __CPP_TRANSPORT_WORKER_NAME              "mpi-worker-"


namespace transport
  {

    //! Task manager is a managed interface to CppTransport's integration and output stacks,
    //! relying on a JSON-aware repository interface 'json_interface_repository'
    //! to handle storage and serialization, and MPI to handle task communication.
    template <typename number>
    class task_manager : public instance_manager<number>
      {
      public:

        typedef enum { job_task, job_get_package, job_get_task, job_get_product, job_get_content } job_type;


        class job_descriptor
          {
          public:
            job_type               type;
            std::string            name;
		        std::list<std::string> tags;

            std::string            output; // destination for output, if needed
          };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a task manager using command-line arguments. The repository must exist and be named on the command line.
        task_manager(int argc, char* argv[],
                     unsigned int bcp=__CPP_TRANSPORT_DEFAULT_BATCHER_STORAGE,
                     unsigned int pcp=__CPP_TRANSPORT_DEFAULT_PIPE_STORAGE,
                     unsigned int zcp=__CPP_TRANSPORT_DEFAULT_ZETA_CACHE_SIZE);

        //! Construct a task manager using a previously-constructed repository object. Usually this will be used only when creating a new repository.
        task_manager(int argc, char* argv[], json_interface_repository<number>* r,
                     unsigned int bcp=__CPP_TRANSPORT_DEFAULT_BATCHER_STORAGE,
                     unsigned int pcp=__CPP_TRANSPORT_DEFAULT_PIPE_STORAGE,
                     unsigned int zcp=__CPP_TRANSPORT_DEFAULT_ZETA_CACHE_SIZE);

        //! Destroy a task manager.
        ~task_manager();


        // INTERFACE -- REPOSITORY MANAGEMENT

      public:

		    //! Return handle to repository
		    json_interface_repository<number>* get_repository();

		    //! Return handle to data manager
		    data_manager<number>* get_data_manager();


        // INTERFACE -- MASTER-SLAVE API

      public:

        //! Query whether we are the master process
        bool is_master(void) const { return(this->world.rank() == MPI::RANK_MASTER); }

        //! Return MPI rank of this process
        unsigned int get_rank(void) const { return(static_cast<unsigned int>(this->world.rank())); }

		    //! Get worker number
		    unsigned int worker_number() { return(static_cast<unsigned int>(this->world.rank()-1)); }

        //! If we are the master process, execute any queued tasks
        void execute_tasks(void);

        //! If we are a slave process, poll for instructions to perform work
        void wait_for_tasks(void);


        // MPI utility functions

      protected:

        //! Map worker number to communicator rank
        constexpr unsigned int worker_rank(unsigned int worker_number) const { return(worker_number+1); }

        //! Map communicator rank to worker number
        constexpr unsigned int worker_number(unsigned int worker_rank) const { return(worker_rank-1); }


        // INTERFACE -- ERROR REPORTING

      public:

        //! Report an error
        void error(const std::string& msg) { std::cout << msg << std::endl; }

        //! Report a warning
        void warn(const std::string& msg) { std::cout << msg << std::endl; }

        //! Report a message
        void message(const std::string& msg) {std::cout << msg << std::endl; }


        // MASTER JOB HANDLING

      protected:

        //! Master node: Process a 'get' job
        void master_process_get(const job_descriptor& job);

        //! Master node: Process a 'task' job.
        //! Some tasks are integrations, others process the numerical output from an integration to product
        //! a derived data product (like fNL).
        //! This function schedules workers to process the task.
        void master_process_task(const job_descriptor& job);

        //! Master node: Terminate all worker processes
        void master_terminate_workers(void);

        //! Master node: Push repository information to worker processes
        void master_push_repository(void);


        // MASTER INTEGRATION TASKS

      protected:

        //! Master node: Dispatch an integration task to the worker processes.
        //! Makes a queue then invokes master_dispatch_integration_queue()
        void master_dispatch_integration_task(typename repository<number>::integration_task_record* rec, const std::list<std::string>& tags);

        //! Master node: Dispatch an integration queue to the worker processes.
        template <typename TaskObject, typename QueueObject>
        void master_dispatch_integration_queue(typename repository<number>::integration_task_record* rec,
                                               TaskObject* tk, QueueObject& queue, const std::list<std::string>& tags);

        //! Master node: Pass new integration task to the workers
        bool master_integration_task_to_workers(std::shared_ptr<typename repository<number>::integration_writer>& writer);

        //! Master node: respond to an aggregation request
        void aggregate_batch(std::shared_ptr<typename repository<number>::integration_writer>& writer, int source,
                             typename repository<number>::integration_metadata& metadata);

        //! Master node: update integration metadata after a worker has finished its tasks
        void update_integration_metadata(MPI::finished_integration_payload& payload, typename repository<number>::integration_metadata& metadata);


        // MASTER POSTINTEGRATION TASKS

      protected:

        //! Master node: Dispatch a postintegration task to the worker processes.
        void master_dispatch_postintegration_task(typename repository<number>::postintegration_task_record* rec, const std::list<std::string>& tags);

        //! Master node: Dispatch a postintegration queue to the worker processes
        template <typename TaskObject, typename ParentTaskObject, typename QueueObject>
        void master_dispatch_postintegration_queue(typename repository<number>::postintegration_task_record* rec,
                                                   TaskObject* tk, ParentTaskObject* ptk, QueueObject& queue, const std::list<std::string>& tags);

        //! Master node: Pass new postintegration task to workers
        bool master_postintegration_task_to_workers(std::shared_ptr<typename repository<number>::postintegration_writer>& writer, const std::list<std::string>& tags);

        //! Master node: respond to an aggregation request
        void aggregate_postprocess(std::shared_ptr<typename repository<number>::postintegration_writer>& writer, int source,
                                   typename repository<number>::output_metadata& metadata);


        // MASTER OUTPUT TASKS

      protected:

        //! Master node: Dispatch an output 'task' (ie., generation of derived data products) to the worker processes
        void master_dispatch_output_task(typename repository<number>::output_task_record* rec, const std::list<std::string>& tags);

        //! Master node: Pass new output task to the workers
        bool master_output_task_to_workers(std::shared_ptr<typename repository<number>::derived_content_writer>& writer, const std::list<std::string>& tags);

        //! Master node: respond to a notification of new derived content
        bool aggregate_content(std::shared_ptr<typename repository<number>::derived_content_writer>& writer, int source,
                               typename repository<number>::output_metadata& metadata);

        //! Master node: update output metadata after a worker has finished its tasks
        template <typename PayloadObject>
        void update_output_metadata(PayloadObject& payload, typename repository<number>::output_metadata& metadata);


        // SLAVE JOB HANDLING

      protected:

        //! Slave node: set repository
        void slave_set_repository(const MPI::set_repository_payload& payload);

        //! Make a 'device context' for the MPI worker processes
        context make_workers_context(void);


        // SLAVE INTEGRATION TASKS

      protected:

        //! Slave node: Process a new integration task instruction
        void slave_process_task(const MPI::new_integration_payload& payload);

        //! Slave node: process an integration task
        void slave_dispatch_integration_task(integration_task<number>* tk, const MPI::new_integration_payload& payload);

        //! Slave node: process an integration queue
        template <typename TaskObject, typename QueueObject, typename BatchObject>
        void slave_dispatch_integration_queue(TaskObject* tk, model<number>* m, QueueObject& queue,
                                              const MPI::new_integration_payload& payload, BatchObject& batcher);

        //! Push a temporary container to the master process
        void slave_push_temp_container(typename data_manager<number>::generic_batcher* batcher, unsigned int message, std::string log_message);


        // SLAVE POSTINTEGRATION TASKS

      protected:

        //! Slave node: Process a new postintegration task instruction
        void slave_process_task(const MPI::new_postintegration_payload& payload);

        //! Slave node: Process a postintegration task
        void slave_dispatch_postintegration_task(postintegration_task<number>* tk, const MPI::new_postintegration_payload& payload);

        //! Slave node: process a postintegration queue
        template <typename TaskObject, typename ParentTaskObject, typename QueueObject, typename BatchObject>
        void slave_dispatch_postintegration_queue(TaskObject* tk, ParentTaskObject* ptk, QueueObject& work,
                                                  const MPI::new_postintegration_payload& payload, BatchObject& batcher);

        //! No-op push content for connexion to datapipe
        void slave_disallow_push_content(typename data_manager<number>::datapipe* pipe, typename derived_data::derived_product<number>* product);

        //! Handler: zeta twopf task
        void slave_postintegration_handler(zeta_twopf_task<number>* tk, twopf_task<number>* ptk, work_queue<twopf_kconfig>& work,
                                           typename data_manager<number>::zeta_twopf_batcher& batcher, typename data_manager<number>::datapipe& pipe);

        //! Handler: zeta threepf task
        void slave_postintegration_handler(zeta_threepf_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
                                           typename data_manager<number>::zeta_threepf_batcher& batcher, typename data_manager<number>::datapipe& pipe);

        //! Handler: fNL task
        void slave_postintegration_handler(fNL_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
                                           typename data_manager<number>::fNL_batcher& batcher, typename data_manager<number>::datapipe& pipe);

        // SLAVE OUTPUT TASKS

      protected:

        //! Slave node: Process a new output task instruction
        void slave_process_task(const MPI::new_derived_content_payload& payload);

        //! Slave node: Process an output task
        void slave_dispatch_output_task(output_task<number>* tk, const MPI::new_derived_content_payload& payload);

        //! Push new derived content to the master process
        void slave_push_derived_content(typename data_manager<number>::datapipe* pipe, typename derived_data::derived_product<number>* product);


        // INTERNAL DATA

      protected:


		    // MPI ENVIRONMENT

        //! BOOST::MPI environment
        boost::mpi::environment environment;

        //! BOOST::MPI world communicator
        boost::mpi::communicator world;


		    // RUNTIME AGENTS

        //! Repository manager instance
        json_interface_repository<number>* repo;

        //! Data manager instance
        data_manager<number>* data_mgr;


		    // DATA AND STATE

        //! Queue of tasks to process
        std::list<job_descriptor> job_queue;

        //! Storage capacity per batcher
        unsigned int batcher_capacity;

        //! Data cache capacity per datapipe
        unsigned int pipe_data_capacity;

        //! Zeta cache capacity per datapipe
        unsigned int pipe_zeta_capacity;

      };


    template <typename number>
    task_manager<number>::task_manager(int argc, char* argv[], unsigned int bcp, unsigned int pcp, unsigned int zcp)
      : instance_manager<number>(),
        environment(argc, argv),
        batcher_capacity(bcp),
        pipe_data_capacity(pcp),
        pipe_zeta_capacity(zcp),
        repo(nullptr),
        data_mgr(data_manager_factory<number>(bcp, pcp, zcp))
      {
        if(world.rank() == MPI::RANK_MASTER)  // process command-line arguments if we are the master node
          {
            bool multiple_repo_warn = false;

            std::list<std::string> tags;

            for(unsigned int i = 1; i < argc; i++)
              {
                if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_REPO)
                  {
                    if(repo != nullptr)
                      {
                        ++i;
                        if(!multiple_repo_warn)
                          {
                            this->warn(__CPP_TRANSPORT_MULTIPLE_SET_REPO);
                            multiple_repo_warn = true;
                          }
                      }
                    else if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_REPO);
                    else
                      {
                        ++i;
                        std::string repo_path = static_cast<std::string>(argv[i]);
                        try
                          {
                            repo = repository_factory<number>(repo_path, repository<number>::access_type::readwrite,
                                                              std::bind(&task_manager<number>::error, this, std::placeholders::_1),
                                                              std::bind(&task_manager<number>::warn, this, std::placeholders::_1),
                                                              std::bind(&task_manager<number>::message, this, std::placeholders::_1));
		                        this->repo->set_model_finder(this->model_finder_factory());
                          }
                        catch (runtime_exception& xe)
                          {
                            if(xe.get_exception_code() == runtime_exception::REPO_NOT_FOUND)
                              {
                                this->error(xe.what());
                                repo = nullptr;
                              }
                            else
                              {
                                throw xe;
                              }
                          }
                      }
                  }
                else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_TAG)
                  {
                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_TAG);
                    else            tags.push_back(std::string(argv[++i]));
                  }
                else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_CAPACITY)
                  {
                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_CAPACITY);
                    else
                      {
                        ++i;
                        std::string capacity_str(argv[i]);
                        int capacity = boost::lexical_cast<int>(capacity_str);
                        capacity = capacity * 1024*1024;
                        if(capacity > 0)
                          {
                            this->batcher_capacity = this->pipe_data_capacity = static_cast<unsigned int>(capacity);
                            this->data_mgr->set_batcher_capacity(this->batcher_capacity);
                            this->data_mgr->set_data_capacity(this->pipe_data_capacity);
                          }
                        else
                          {
                            std::ostringstream msg;
                            msg << __CPP_TRANSPORT_EXPECTED_POSITIVE << " " << __CPP_TRANSPORT_SWITCH_CAPACITY;
                            this->error(msg.str());
                          }
                      }
                  }
                else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_BATCHER_CAPACITY)
                  {
                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_CAPACITY);
                    else
                      {
                        ++i;
                        std::string capacity_str(argv[i]);
                        int capacity = boost::lexical_cast<int>(capacity_str);
                        capacity = capacity * 1024*1024;
                        if(capacity > 0)
                          {
                            this->batcher_capacity = static_cast<unsigned int>(capacity);
                            this->data_mgr->set_batcher_capacity(this->batcher_capacity);
                          }
                        else
                          {
                            std::ostringstream msg;
                            msg << __CPP_TRANSPORT_EXPECTED_POSITIVE << " " << __CPP_TRANSPORT_SWITCH_BATCHER_CAPACITY;
                            this->error(msg.str());
                          }
                      }
                  }
                else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_CACHE_CAPACITY)
                  {
                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_CAPACITY);
                    else
                      {
                        ++i;
                        std::string capacity_str(argv[i]);
                        int capacity = boost::lexical_cast<int>(capacity_str);
                        capacity = capacity * 1024*1024;
                        if(capacity > 0)
                          {
                            this->pipe_data_capacity = static_cast<unsigned int>(capacity);
                            this->data_mgr->set_data_capacity(this->pipe_data_capacity);
                          }
                        else
                          {
                            std::ostringstream msg;
                            msg << __CPP_TRANSPORT_EXPECTED_POSITIVE << " " << __CPP_TRANSPORT_SWITCH_CACHE_CAPACITY;
                            this->error(msg.str());
                          }
                      }
                  }
                else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_ZETA_CAPACITY)
                  {
                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_CAPACITY);
                    else
                      {
                        ++i;
                        std::string capacity_str(argv[i]);
                        int capacity = boost::lexical_cast<int>(capacity_str);
                        capacity = capacity * 1024*1024;
                        if(capacity > 0)
                          {
                            this->pipe_zeta_capacity = static_cast<unsigned int>(capacity);
                            this->data_mgr->set_zeta_capacity(this->pipe_zeta_capacity);
                          }
                        else
                          {
                            std::ostringstream msg;
                            msg << __CPP_TRANSPORT_EXPECTED_POSITIVE << " " << __CPP_TRANSPORT_SWITCH_ZETA_CAPACITY;
                            this->error(msg.str());
                          }
                      }
                  }
                else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_VERB_TASK)
                  {
                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_TASK_ID);
                    else
                      {
                        ++i;

                        job_descriptor desc;
                        desc.type = job_task;
                        desc.name = argv[i];
		                    desc.tags = tags;

                        job_queue.push_back(desc);
		                    tags.clear();
                      }
                  }
                else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_VERB_GET)
                  {
                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_GET_TYPE);
                    ++i;

                    job_descriptor desc;

                    if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_NOUN_PACKAGE)      desc.type = job_get_package;
                    else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_NOUN_TASK)    desc.type = job_get_task;
                    else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_NOUN_PRODUCT) desc.type = job_get_product;
                    else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_NOUN_CONTENT) desc.type = job_get_content;
                    else
                      {
                        std::ostringstream msg;
                        msg << __CPP_TRANSPORT_UNKNOWN_GET_TYPE << " '" << argv[i] << "'";
                        this->error(msg.str());
                      }

                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_GET_NAME);
                    ++i;
                    desc.name = argv[i];

                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_GET_OUTPUT);
                    ++i;
                    desc.output = argv[i];

                    desc.tags = tags;

                    job_queue.push_back(desc);
                    tags.clear();
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_UNKNOWN_SWITCH << " '" << argv[i] << "'";
                    this->error(msg.str());
                  }
              }
          }
      }


    template <typename number>
    task_manager<number>::task_manager(int argc, char* argv[], json_interface_repository<number>* r, unsigned int bcp, unsigned int pcp, unsigned int zcp)
      : instance_manager<number>(), environment(argc, argv),
        batcher_capacity(bcp),
        pipe_data_capacity(pcp),
        pipe_zeta_capacity(zcp),
        repo(r),
        data_mgr(data_manager_factory<number>(bcp, pcp, zcp))
      {
        assert(repo != nullptr);

		    repo->set_model_finder(this->model_finder_factory());
      }


    template <typename number>
    task_manager<number>::~task_manager()
      {
        delete this->repo;
      }


    // REPOSITORY INTERFACE


		template <typename number>
		json_interface_repository<number>* task_manager<number>::get_repository()
			{
				assert(this->repo != nullptr);

				if(this->repo == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NOT_SET);

				return(this->repo);
			}


		template <typename number>
		data_manager<number>* task_manager<number>::get_data_manager()
			{
				assert(this->data_mgr != nullptr);

				if(this->data_mgr == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_DATAMGR_NOT_SET);

				return(this->data_mgr);
			}


    // MPI UTILITY FUNCTIONS


    template <typename number>
    void task_manager<number>::master_terminate_workers(void)
	    {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        for(unsigned int i = 0; i < this->world.size()-1; i++)
	        {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::TERMINATE);
	        }

        // wait for all messages to be received, then exit ourselves
        boost::mpi::wait_all(requests.begin(), requests.end());
	    }


    template <typename number>
    void task_manager<number>::master_push_repository(void)
	    {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        // we require this->repo not to be null,
        // but don't throw an exception since this condition should have been checked before calling
        assert(this->repo != nullptr);
        MPI::set_repository_payload payload(this->repo->get_root_path());

        for(unsigned int i = 0; i < this->world.size()-1; i++)
	        {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::SET_REPOSITORY, payload);
	        }

        // wait for all messages to be received, then return
        boost::mpi::wait_all(requests.begin(), requests.end());
	    }


    template <typename number>
    void task_manager<number>::slave_push_temp_container(typename data_manager<number>::generic_batcher* batcher, unsigned int message, std::string log_message)
	    {
        assert(batcher != nullptr);
        if(batcher == nullptr) throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_BATCHER);

        BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal) << "-- Sending " << log_message << " message for container " << batcher->get_container_path();

        MPI::data_ready_payload payload(batcher->get_container_path().string());

        // advise master process that data is available in the named container
        this->world.isend(MPI::RANK_MASTER, message, payload);
	    }


    template <typename number>
    void task_manager<number>::slave_push_derived_content(typename data_manager<number>::datapipe* pipe, typename derived_data::derived_product<number>* product)
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
    void task_manager<number>::slave_disallow_push_content(typename data_manager<number>::datapipe* pipe, typename derived_data::derived_product<number>* product)
	    {
        assert(false);
	    }

  } // namespace transport


// implementation of master and slave methods is broken into separate files due to size
#include "transport-runtime-api/manager/task_manager_master.h"
#include "transport-runtime-api/manager/task_manager_slave.h"


#endif //__task_manager_H_
