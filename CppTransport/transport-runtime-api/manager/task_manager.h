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


    // MASTER FUNCTIONS


    template <typename number>
    void task_manager<number>::execute_tasks(void)
      {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        if(this->repo == nullptr) this->error(__CPP_TRANSPORT_REPO_NONE);
        else
          {
            // push repository information to all workers
            this->master_push_repository();

            for(typename std::list<job_descriptor>::const_iterator t = this->job_queue.begin(); t != this->job_queue.end(); t++)
              {
                switch((*t).type)
                  {
                    case job_task:
                      {
                        this->master_process_task(*t);
                        break;
                      }

                    case job_get_product:
                    case job_get_task:
                    case job_get_package:
                    case job_get_content:
                      {
                        this->master_process_get(*t);
                        break;
                      }

                    default:
                      throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_UNKNOWN_JOB_TYPE);
                  }
              }
          }

        // there is no more work, so ask all workers to shut down
        // and then exit ourselves
        this->master_terminate_workers();
      }


    template <typename number>
    void task_manager<number>::master_process_get(const job_descriptor& job)
      {
        try
          {
            std::string document;

            switch(job.type)
              {
                case job_get_package:
                  {
                    document = this->repo->export_JSON_package_record(job.name);
                    break;
                  }

                case job_get_task:
                  {
                    document = this->repo->export_JSON_task_record(job.name);
                    break;
                  }

                case job_get_product:
                  {
                    document = this->repo->export_JSON_product_record(job.name);
                    break;
                  }

                case job_get_content:
                  {
                    document = this->repo->export_JSON_content_record(job.name);
                    break;
                  }

                default:
                  assert(false);
              }

            std::ofstream out;
            out.open(job.output);
            if(out.is_open() && !out.fail())
              {
                out << document;
              }
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_OPEN_OUTPUT_FAIL << " '" << job.output << "'";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
              }
            out.close();
          }
        catch(runtime_exception& xe)
          {
            this->error(xe.what());
          }
      }


    template <typename number>
    void task_manager<number>::master_process_task(const job_descriptor& job)
      {
        try
          {
						// query a task record with the name we're looking for from the database
            std::unique_ptr<typename repository<number>::task_record> record(this->repo->query_task(job.name));

            switch(record->get_type())
              {
                case repository<number>::task_record::integration:
                  {
                    typename repository<number>::integration_task_record* int_rec = dynamic_cast<typename repository<number>::integration_task_record*>(record.get());

                    assert(int_rec != nullptr);
                    if(int_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    this->master_dispatch_integration_task(int_rec, job.tags);
                    break;
                  }

                case repository<number>::task_record::output:
                  {
                    typename repository<number>::output_task_record* out_rec = dynamic_cast<typename repository<number>::output_task_record*>(record.get());

                    assert(out_rec != nullptr);
                    if(out_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    this->master_dispatch_output_task(out_rec, job.tags);
                    break;
                  }

                case repository<number>::task_record::postintegration:
                  {
                    typename repository<number>::postintegration_task_record* pint_rec = dynamic_cast<typename repository<number>::postintegration_task_record*>(record.get());

                    assert(pint_rec != nullptr);
                    if(pint_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    this->master_dispatch_postintegration_task(pint_rec, job.tags);
                    break;
                  }

                default:
                  {
                    assert(false);

                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_UNKNOWN_RECORD_TYPE << " '" << job.name << "'";
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
                this->error(msg.str());
              }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE
	                  || xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
	            {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << job.name << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
	            }
            else throw xe;
          }
      }


    template <typename number>
    void task_manager<number>::master_dispatch_integration_task(typename repository<number>::integration_task_record* rec, const std::list<std::string>& tags)
      {
        assert(rec != nullptr);

        // can't process a task if there are no workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        // set up a work queue representing our workers
        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        integration_task<number>* tk = rec->get_task();
        model<number>* m = rec->get_task()->get_model();

        twopf_task<number>* tka = nullptr;
        threepf_task<number>* tkb = nullptr;

        if((tka = dynamic_cast< twopf_task<number>* >(tk)) != nullptr)
          {
            work_queue<twopf_kconfig> queue = sch.make_queue(m->backend_twopf_state_size(), *tka);
            this->master_dispatch_integration_queue(rec, tka, queue, tags);
          }
        else if((tkb = dynamic_cast< threepf_task<number>* >(tk)) != nullptr)
          {
            work_queue<threepf_kconfig> queue = sch.make_queue(m->backend_threepf_state_size(), *tkb);
            this->master_dispatch_integration_queue(rec, tkb, queue, tags);
          }
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << rec->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
      }


    template <typename number>
    template <typename TaskObject, typename QueueObject>
    void task_manager<number>::master_dispatch_integration_queue(typename repository<number>::integration_task_record* rec,
                                                                 TaskObject* tk, QueueObject& queue, const std::list<std::string>& tags)
      {
        assert(rec != nullptr);

        // create an output writer to commit the result of this integration to the repository.
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::shared_ptr<typename repository<number>::integration_writer> writer = this->repo->new_integration_task_content(rec, tags, this->get_rank());

        // initialize the writer
        this->data_mgr->initialize_writer(writer);

        // write the task distribution list -- this is subsequently read by the worker processes,
        // to find out which work items they should process
        this->data_mgr->create_taskfile(writer, queue);

        // write the various tables needed in the database
        this->data_mgr->create_tables(writer, tk);

        // instruct workers to carry out the calculation
        // this call returns when all workers have signalled that their work is done
        bool success = this->master_integration_task_to_workers(writer);

        // close the writer
        this->data_mgr->close_writer(writer);

        // commit output if successful
        if(success) writer->commit();
      }


    template <typename number>
    bool task_manager<number>::master_integration_task_to_workers(std::shared_ptr<typename repository<number>::integration_writer>& writer)
      {
        assert(this->repo != nullptr);
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        bool success = true;

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ NEW INTEGRATION TASK '" << writer->get_record()->get_name() << "'" << std::endl;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << *(writer->get_record()->get_task());

        // set up a timer to keep track of the total wallclock time used in this integration
        boost::timer::cpu_timer wallclock_timer;

        // aggregate integration times reported by worker processes
        typename repository<number>::integration_metadata metadata;

        // get paths the workers will need
        boost::filesystem::path taskfile_path = writer->get_abs_taskfile_path();
        boost::filesystem::path tempdir_path  = writer->get_abs_tempdir_path();
        boost::filesystem::path logdir_path   = writer->get_abs_logdir_path();

        MPI::new_integration_payload payload(writer->get_record()->get_name(), taskfile_path, tempdir_path, logdir_path);

        for(unsigned int i = 0; i < this->world.size()-1; i++)
          {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_INTEGRATION, payload);
          }

        // wait for all messages to be received
        boost::mpi::wait_all(requests.begin(), requests.end());

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ All workers received NEW_INTEGRATION instruction";

        // poll workers, receiving data until workers are exhausted
        std::set<unsigned int> workers;
        for(unsigned int i = 0; i < this->world.size()-1; i++) workers.insert(i);
        while(workers.size() > 0)
          {
            BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) <<  "++ Master polling for INTEGRATION_DATA_READY messages";
            // wait until a message is available from a worker
            boost::mpi::status stat = this->world.probe();

            switch(stat.tag())
              {
                case MPI::INTEGRATION_DATA_READY:
                  {
                    this->aggregate_batch(writer, stat.source(), metadata);
                    break;
                  }

                case MPI::FINISHED_INTEGRATION:
                  {
                    MPI::finished_integration_payload payload;
                    this->world.recv(stat.source(), MPI::FINISHED_INTEGRATION, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising finished task in wallclock time " << format_time(payload.get_wallclock_time());

                    this->update_integration_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    break;
                  }

                case MPI::INTEGRATION_FAIL:
                  {
                    MPI::finished_integration_payload payload;
                    this->world.recv(stat.source(), MPI::INTEGRATION_FAIL, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising integration failure (successful tasks consumed wallclock time " << format_time(payload.get_wallclock_time()) << ")";

                    this->update_integration_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    success = false;
                    break;
                  }

                default:
                  {
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::warning) << "++ Master received unexpected message " << stat.tag() << " waiting in the queue";
                    break;
                  }
              }
          }

        writer->set_metadata(metadata);

        wallclock_timer.stop();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ TASK COMPLETE: FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total wallclock time for task '" << writer->get_record()->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total wallclock time required by worker processes = " << format_time(metadata.total_wallclock_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total aggregation time required by master process = " << format_time(metadata.total_aggregation_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ AGGREGATE CACHE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Workers processed " << metadata.total_configurations << " individual integrations";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total integration time    = " << format_time(metadata.total_integration_time) << " | global mean integration time = " << format_time(metadata.total_integration_time/metadata.total_configurations);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Min mean integration time = " << format_time(metadata.min_mean_integration_time) << " | global min integration time = " << format_time(metadata.global_min_integration_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Max mean integration time = " << format_time(metadata.max_mean_integration_time) << " | global max integration time = " << format_time(metadata.global_max_integration_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total batching time       = " << format_time(metadata.total_batching_time) << " | global mean batching time = " << format_time(metadata.total_batching_time/metadata.total_configurations);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Min mean batching time    = " << format_time(metadata.min_mean_batching_time) << " | global min batching time = " << format_time(metadata.global_min_batching_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Max mean batching time    = " << format_time(metadata.max_mean_batching_time) << " | global max batching time = " << format_time(metadata.global_max_batching_time);

        return(success);
      }


    template <typename number>
    void task_manager<number>::aggregate_batch(std::shared_ptr<typename repository<number>::integration_writer>& writer, int source,
                                               typename repository<number>::integration_metadata& metadata)
      {
        MPI::data_ready_payload payload;
        this->world.recv(source, MPI::INTEGRATION_DATA_READY, payload);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << source << " sent aggregation notification for container '" << payload.get_container_path() << "'";

        // set up a timer to measure how long we spend batching
        boost::timer::cpu_timer batching_timer;

        writer->aggregate(payload.get_container_path());

        batching_timer.stop();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Aggregated temporary container '" << payload.get_container_path() << "' in time " << format_time(batching_timer.elapsed().wall);
        metadata.total_aggregation_time += batching_timer.elapsed().wall;

        // remove temporary container
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Deleting temporary container '" << payload.get_container_path() << "'";
        if(!boost::filesystem::remove(payload.get_container_path()))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATACTR_REMOVE_TEMP << " '" << payload.get_container_path() << "'";
            this->error(msg.str());
          }
      }


    template <typename number>
    void task_manager<number>::update_integration_metadata(MPI::finished_integration_payload& payload, typename repository<number>::integration_metadata& metadata)
      {
        metadata.total_wallclock_time += payload.get_wallclock_time();

        metadata.total_integration_time += payload.get_integration_time();
        boost::timer::nanosecond_type mean_integration_time = payload.get_integration_time() / payload.get_num_integrations();
        if(metadata.max_mean_integration_time == 0 || mean_integration_time > metadata.max_mean_integration_time) metadata.max_mean_integration_time = mean_integration_time;
        if(metadata.min_mean_integration_time == 0 || mean_integration_time < metadata.min_mean_integration_time) metadata.min_mean_integration_time = mean_integration_time;

        if(metadata.global_max_integration_time == 0 || payload.get_max_integration_time() > metadata.global_max_integration_time) metadata.global_max_integration_time = payload.get_max_integration_time();
        if(metadata.global_min_integration_time == 0 || payload.get_min_integration_time() < metadata.global_min_integration_time) metadata.global_min_integration_time = payload.get_min_integration_time();

        metadata.total_batching_time += payload.get_batching_time();
        boost::timer::nanosecond_type mean_batching_time = payload.get_batching_time() / payload.get_num_integrations();
        if(metadata.max_mean_batching_time == 0 || mean_batching_time > metadata.max_mean_batching_time) metadata.max_mean_batching_time = mean_batching_time;
        if(metadata.min_mean_batching_time == 0 || mean_batching_time < metadata.min_mean_batching_time) metadata.min_mean_batching_time = mean_batching_time;

        if(metadata.global_max_batching_time == 0 || payload.get_max_batching_time() > metadata.global_max_batching_time) metadata.global_max_batching_time = payload.get_max_batching_time();
        if(metadata.global_min_batching_time == 0 || payload.get_min_batching_time() < metadata.global_min_batching_time) metadata.global_min_batching_time = payload.get_min_batching_time();

        metadata.total_configurations += payload.get_num_integrations();
      }


    template <typename number>
    context task_manager<number>::make_workers_context(void)
      {
        context ctx;

        // add devices to this context
        // in this implementation, we assume that all workers are symmetric and therefore we would like
        // the work queues to be balanced
        // In principle, this can be changed if we have more information about the workers
        for(unsigned int i = 0; i < this->world.size()-1; i++)
          {
            std::ostringstream name;
            name << __CPP_TRANSPORT_WORKER_NAME << i;
            ctx.add_device(name.str());
          }

        return(ctx);
      }


    template <typename number>
    void task_manager<number>::master_dispatch_output_task(typename repository<number>::output_task_record* rec, const std::list<std::string>& tags)
      {
        // can't process a task if there are no workers
        if(this->world.size() <= 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        // set up a work queue representing our workers
        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        output_task<number>* tk = rec->get_task();
        work_queue< output_task_element<number> > queue = sch.make_queue(*tk);

        // set up an derived_content_writer object to coordinate logging, output destination and commits into the repository.
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::shared_ptr<typename repository<number>::derived_content_writer> writer = this->repo->new_output_task_content(rec, tags, this->get_rank());

        // set up the writer for us
        this->data_mgr->initialize_writer(writer);

        // write a task distribution list -- subsequently read by the worker processes
        // to find out which work items they should process
        this->data_mgr->create_taskfile(writer, queue);

        // instruct workers to carry out their tasks
        bool success = this->master_output_task_to_workers(writer, tags);

        // close the writer
        this->data_mgr->close_writer(writer);

        // commit output to the database if successful
        if(success) writer->commit();
      }


    template <typename number>
    bool task_manager<number>::master_output_task_to_workers(std::shared_ptr<typename repository<number>::derived_content_writer>& writer, const std::list<std::string>& tags)
      {
        assert(this->repo != nullptr);
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

		    bool success = true;

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ NEW OUTPUT TASK '" << writer->get_record()->get_name() << "'";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << *(writer->get_record()->get_task());

        // set up a timer to keep track of the total wallclock time used in this task
        boost::timer::cpu_timer wallclock_timer;

        // aggregate cache information
		    typename repository<number>::output_metadata metadata;

        // get paths the workers will need
        boost::filesystem::path taskfile_path = writer->get_abs_taskfile_path();
        boost::filesystem::path tempdir_path  = writer->get_abs_tempdir_path();
        boost::filesystem::path logdir_path   = writer->get_abs_logdir_path();

        MPI::new_derived_content_payload payload(writer->get_record()->get_name(), taskfile_path, tempdir_path, logdir_path, tags);

        for(unsigned int i = 0; i < this->world.size()-1; i++)
          {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_DERIVED_CONTENT, payload);
          }

        // wait for all messages to be received
        boost::mpi::wait_all(requests.begin(), requests.end());

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ All workers received NEW_DERIVED_CONTENT instruction";

        // poll workers, receiving data until workers are exhausted
        std::set<unsigned int> workers;
        for(unsigned int i = 0; i < this->world.size()-1; i++) workers.insert(i);
        while(workers.size() > 0)
          {
            BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Master polling for DERIVED_CONTENT_READY messages";
            // wait until a message is available from a worker
            boost::mpi::status stat = this->world.probe();

            switch(stat.tag())
              {
                case MPI::DERIVED_CONTENT_READY:
                  {
		                if(!this->aggregate_content(writer, stat.source(), metadata)) success = false;
                    break;
                  }

                case MPI::FINISHED_DERIVED_CONTENT:
                  {
                    MPI::finished_derived_payload payload;
                    this->world.recv(stat.source(), MPI::FINISHED_DERIVED_CONTENT, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising finished producing derived content in CPU time " << format_time(payload.get_cpu_time());

		                this->update_output_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    break;
                  }

                case MPI::DERIVED_CONTENT_FAIL:
	                {
                    MPI::finished_derived_payload payload;
		                this->world.recv(stat.source(), MPI::DERIVED_CONTENT_FAIL, payload);
		                BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising failure to produce derived content (successful tasks consumed wallclock time " << format_time(payload.get_cpu_time()) << ")";

		                this->update_output_metadata(payload, metadata);

		                workers.erase(this->worker_number(stat.source()));
		                success = false;
		                break;
	                }

                default:
                  {
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::warning) << "++ Master received unexpected message " << stat.tag() << " waiting in the queue";
                    break;
                  }
              }
          }

        writer->set_metadata(metadata);

        wallclock_timer.stop();

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ TASK COMPLETE -- FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total wallclock time for task '" << writer->get_record()->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
		    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ AGGREGATE CACHE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total work time required by worker processes      = " << format_time(metadata.work_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total aggregation time required by master process = " << format_time(metadata.aggregation_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time spent reading database                 = " << format_time(metadata.db_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time-configuration cache hits               = " << metadata.time_config_hits << ", unloads = " << metadata.time_config_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total twopf k-config cache hits                   = " << metadata.twopf_kconfig_hits << ", unloads = " << metadata.twopf_kconfig_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total threepf k-config cache hits                 = " << metadata.threepf_kconfig_hits << ", unloads = " << metadata.threepf_kconfig_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total data cache hits                             = " << metadata.data_hits << ", unloads = " << metadata.data_unloads;
		    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total zeta cache hits                             = " << metadata.zeta_hits << ", unloads = " << metadata.zeta_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
		    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time config cache evictions                 = " << format_time(metadata.time_config_evictions);
		    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total twopf k-config cache evictions              = " << format_time(metadata.twopf_kconfig_evictions);
		    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total threepf k-config cache evictions            = " << format_time(metadata.threepf_kconfig_evictions);
		    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total data cache evictions                        = " << format_time(metadata.data_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total zeta cache evictions                        = " << format_time(metadata.zeta_evictions);

		    return(success);
      }


    template <typename number>
    bool task_manager<number>::aggregate_content(std::shared_ptr<typename repository<number>::derived_content_writer>& writer, int source,
                                                 typename repository<number>::output_metadata& metadata)
	    {
        MPI::content_ready_payload payload;
        this->world.recv(source, MPI::DERIVED_CONTENT_READY, payload);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << source << " sent content-ready notification";

		    // set up a timer to measure how long we spend aggregating
        boost::timer::cpu_timer aggregate_timer;

        bool success = writer->aggregate(payload.get_product_name());

		    aggregate_timer.stop();
        metadata.aggregation_time += aggregate_timer.elapsed().wall;

		    return(success);
	    }


		template <typename number>
    template <typename PayloadObject>
		void task_manager<number>::update_output_metadata(PayloadObject& payload, typename repository<number>::output_metadata& metadata)
			{
		    metadata.work_time                 += payload.get_cpu_time();
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
		    metadata.data_hits                 += payload.get_data_hits();
		    metadata.data_unloads              += payload.get_data_unloads();
        metadata.data_evictions            += payload.get_data_evictions();
        metadata.zeta_hits                 += payload.get_zeta_hits();
        metadata.zeta_unloads              += payload.get_zeta_unloads();
        metadata.zeta_evictions            += payload.get_zeta_evictions();
			}


    template <typename number>
    void task_manager<number>::master_dispatch_postintegration_task(typename repository<number>::postintegration_task_record* rec, const std::list<std::string>& tags)
      {
        assert(rec != nullptr);

        // can't process a task if there are no workers
        if(this->world.size() <= 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        // set up a work queue representing our workers
        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        postintegration_task<number>* tk = rec->get_task();

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
                msg << __CPP_TRANSPORT_EXPECTED_TWOPF_TASK << " '" << z2pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
              }

            work_queue<twopf_kconfig> queue = sch.make_queue(sizeof(number), *ptk);
            this->master_dispatch_postintegration_queue(rec, z2pf, ptk, queue, tags);
          }
        else if((z3pf = dynamic_cast< zeta_threepf_task<number>* >(tk)) != nullptr)
          {
            threepf_task<number>* ptk = dynamic_cast<threepf_task<number>*>(z3pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_THREEPF_TASK << " '" << z3pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
              }

            work_queue<threepf_kconfig> queue = sch.make_queue(sizeof(number), *ptk);
            this->master_dispatch_postintegration_queue(rec, z3pf, ptk, queue, tags);
          }
        else if((zfNL = dynamic_cast< fNL_task<number>* >(tk)) != nullptr)
          {
            threepf_task<number>* ptk = dynamic_cast<threepf_task<number>*>(zfNL->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_THREEPF_TASK << " '" << zfNL->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
              }

            work_queue<threepf_kconfig> queue = sch.make_queue(sizeof(number), *ptk);
            this->master_dispatch_postintegration_queue(rec, zfNL, ptk, queue, tags);
          }
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << rec->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
      }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject, typename QueueObject>
    void task_manager<number>::master_dispatch_postintegration_queue(typename repository<number>::postintegration_task_record* rec,
                                                                     TaskObject* tk, ParentTaskObject* ptk, QueueObject& queue, const std::list<std::string>& tags)
      {
        assert(rec != nullptr);

        // create an output writer to commit our results into the repository
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::shared_ptr<typename repository<number>::postintegration_writer> writer = this->repo->new_postintegration_task_content(rec, tags, this->get_rank());

        // initialize the writer
        this->data_mgr->initialize_writer(writer);

        // write the task distribution list
        this->data_mgr->create_taskfile(writer, queue);

        // create new tables needed in the database
        this->data_mgr->create_tables(writer, tk);

        // instruct workers to carry out the calculation
        bool success = this->master_postintegration_task_to_workers(writer, tags);

        // close the writer
        this->data_mgr->close_writer(writer);

        // commit output if successful
        if(success) writer->commit();
      }


    template <typename number>
    bool task_manager<number>::master_postintegration_task_to_workers(std::shared_ptr<typename repository<number>::postintegration_writer>& writer, const std::list<std::string>& tags)
      {
        assert(this->repo != nullptr);
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        bool success = true;

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ NEW POSTINTEGRATION TASK '" << writer->get_record()->get_name() << "'" << std::endl;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << *(writer->get_record()->get_task());

        // set up a timer to keep track of the total wallclock time used
        boost::timer::cpu_timer wallclock_timer;

        // aggregate cache information
        typename repository<number>::output_metadata metadata;

        // get paths the workers will need
        boost::filesystem::path taskfile_path = writer->get_abs_taskfile_path();
        boost::filesystem::path tempdir_path  = writer->get_abs_tempdir_path();
        boost::filesystem::path logdir_path   = writer->get_abs_logdir_path();

        MPI::new_postintegration_payload payload(writer->get_record()->get_name(), taskfile_path, tempdir_path, logdir_path, tags);

        for(unsigned int i = 0; i < this->world.size()-1; i++)
          {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_POSTINTEGRATION, payload);
          }

        // wait for all messages to be received
        boost::mpi::wait_all(requests.begin(), requests.end());

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ All workers received NEW_POSTINTEGRATION instruction";

        // poll workers, receiving data until workers are exhausted
        std::set<unsigned int> workers;
        for(unsigned int i = 0; i < this->world.size()-1; i++) workers.insert(i);
        while(workers.size() > 0)
          {
            BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) <<  "++ Master polling for POSTINTEGRATION_DATA_READY messages";
            // wait until a message is available from a worker
            boost::mpi::status stat = this->world.probe();

            switch(stat.tag())
              {
                case MPI::POSTINTEGRATION_DATA_READY:
                  {
                    this->aggregate_postprocess(writer, stat.source(), metadata);
                    break;
                  }

                case MPI::FINISHED_POSTINTEGRATION:
                  {
                    MPI::finished_postintegration_payload payload;
                    this->world.recv(stat.source(), MPI::FINISHED_POSTINTEGRATION, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising finished postintegration processing in wallclock time " << format_time(payload.get_cpu_time());

                    this->update_output_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    break;
                  }

                case MPI::POSTINTEGRATION_FAIL:
                  {
                    MPI::finished_postintegration_payload payload;
                    this->world.recv(stat.source(), MPI::POSTINTEGRATION_FAIL, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising failure to complete postintegration processing (successful tasks consumed wallclock time " << format_time(payload.get_cpu_time()) << ")";

                    this->update_output_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    success = false;
                    break;
                  }

                default:
                  {
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::warning) << "++ Master received unexpected message " << stat.tag() << " waiting in the queue";
                    break;
                  }
              }
          }

		    writer->set_metadata(metadata);

        wallclock_timer.stop();

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ TASK COMPLETE -- FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total wallclock time for task '" << writer->get_record()->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ AGGREGATE CACHE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total work time required by worker processes      = " << format_time(metadata.work_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total aggregation time required by master process = " << format_time(metadata.aggregation_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time spent reading database                 = " << format_time(metadata.db_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time-configuration cache hits               = " << metadata.time_config_hits << ", unloads = " << metadata.time_config_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total twopf k-config cache hits                   = " << metadata.twopf_kconfig_hits << ", unloads = " << metadata.twopf_kconfig_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total threepf k-config cache hits                 = " << metadata.threepf_kconfig_hits << ", unloads = " << metadata.threepf_kconfig_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total data cache hits                             = " << metadata.data_hits << ", unloads = " << metadata.data_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total zeta cache hits                             = " << metadata.zeta_hits << ", unloads = " << metadata.zeta_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time config cache evictions                 = " << format_time(metadata.time_config_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total twopf k-config cache evictions              = " << format_time(metadata.twopf_kconfig_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total threepf k-config cache evictions            = " << format_time(metadata.threepf_kconfig_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total data cache evictions                        = " << format_time(metadata.data_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total zeta cache evictions                        = " << format_time(metadata.zeta_evictions);

        return(success);
      }


    template <typename number>
    void task_manager<number>::aggregate_postprocess(std::shared_ptr<typename repository<number>::postintegration_writer>& writer, int source,
                                                     typename repository<number>::output_metadata& metadata)
      {
        MPI::data_ready_payload payload;
        this->world.recv(source, MPI::POSTINTEGRATION_DATA_READY, payload);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << source << " sent aggregation notification for container '" << payload.get_container_path() << "'";

        // set up a timer to measure how long we spend batching
        boost::timer::cpu_timer aggregation_timer;

        writer->aggregate(payload.get_container_path());

        aggregation_timer.stop();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Aggregated temporary container '" << payload.get_container_path() << "' in time " << format_time(aggregation_timer.elapsed().wall);
        metadata.aggregation_time += aggregation_timer.elapsed().wall;

        // remove temporary container
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Deleting temporary container '" << payload.get_container_path() << "'";
        if(!boost::filesystem::remove(payload.get_container_path()))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATACTR_REMOVE_TEMP << " '" << payload.get_container_path() << "'";
            this->error(msg.str());
          }
      }


    // SLAVE FUNCTIONS


    template <typename number>
    void task_manager<number>::wait_for_tasks(void)
      {
        if(this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_WAIT_MASTER);

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
                    this->slave_process_task(payload);
                    break;
                  }

                case MPI::NEW_DERIVED_CONTENT:
	                {
                    MPI::new_derived_content_payload payload;
		                this->world.recv(MPI::RANK_MASTER, MPI::NEW_DERIVED_CONTENT, payload);
		                this->slave_process_task(payload);
		                break;
	                }

                case MPI::NEW_POSTINTEGRATION:
                  {
                    MPI::new_postintegration_payload payload;
                    this->world.recv(MPI::RANK_MASTER, MPI::NEW_POSTINTEGRATION, payload);
                    this->slave_process_task(payload);
                    break;
                  }

                case MPI::SET_REPOSITORY:
                  {
                    MPI::set_repository_payload payload;
                    this->world.recv(MPI::RANK_MASTER, MPI::SET_REPOSITORY, payload);
                    this->slave_set_repository(payload);
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
    void task_manager<number>::slave_set_repository(const MPI::set_repository_payload& payload)
      {
        boost::filesystem::path repo_path = payload.get_repository_path();

        try
          {
            this->repo = repository_factory<number>(repo_path.string(), repository<number>::access_type::readonly,
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


    template <typename number>
    void task_manager<number>::slave_process_task(const MPI::new_integration_payload& payload)
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
                    this->slave_dispatch_integration_task(tk, payload);
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
                this->error(msg.str());
	            }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE
	                  || xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
	            {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << payload.get_task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
	            }
            else
	            {
                throw xe;
	            }
	        }
	    }


    template <typename number>
    void task_manager<number>::slave_dispatch_integration_task(integration_task<number>* tk, const MPI::new_integration_payload& payload)
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
                                                                         std::bind(&task_manager<number>::slave_push_temp_container, this, std::placeholders::_1,
                                                                                   MPI::INTEGRATION_DATA_READY, std::string("INTEGRATION_DATA_READY"));

            // construct a batcher to hold the output of the integration
            typename data_manager<number>::twopf_batcher batcher =
                                                           this->data_mgr->create_temp_twopf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
                                                                                                       this->get_rank(), m, dispatcher);

            this->slave_dispatch_integration_queue(tka, m, work, payload, batcher);
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
                                                                         std::bind(&task_manager<number>::slave_push_temp_container, this, std::placeholders::_1,
                                                                                   MPI::INTEGRATION_DATA_READY, std::string("INTEGRATION_DATA_READY"));

            // construct a batcher to hold the output of the integration
            typename data_manager<number>::threepf_batcher batcher =
                                                             this->data_mgr->create_temp_threepf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
                                                                                                           this->get_rank(), m, dispatcher);

            this->slave_dispatch_integration_queue(tkb, m, work, payload, batcher);
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
    void task_manager<number>::slave_dispatch_integration_queue(TaskObject* tk, model<number>* m, QueueObject& queue,
                                                                const MPI::new_integration_payload& payload, BatchObject& batcher)
      {
        // dispatch integration to the underlying model
        assert(tk != nullptr);  // should be guaranteed
        assert(m != nullptr);   // should be guaranteed

        bool success = true;

        // keep track of wallclock time
        boost::timer::cpu_timer timer;

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW INTEGRATION TASK '" << tk->get_name() << "'" << std::endl;
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
        if(success) BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending FINISHED_INTEGRATION to master";
        else        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::error)  << std::endl << "-- Worker reporting INTEGRATION_FAIL to master";

        MPI::finished_integration_payload outgoing_payload(batcher.get_integration_time(),
                                                           batcher.get_max_integration_time(), batcher.get_min_integration_time(),
                                                           batcher.get_batching_time(),
                                                           batcher.get_max_batching_time(), batcher.get_min_batching_time(),
                                                           timer.elapsed().wall,
                                                           batcher.get_reported_integrations());

        this->world.isend(MPI::RANK_MASTER, success ? MPI::FINISHED_INTEGRATION : MPI::INTEGRATION_FAIL, outgoing_payload);
      }


    template <typename number>
    void task_manager<number>::slave_process_task(const MPI::new_derived_content_payload& payload)
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
                    this->slave_dispatch_output_task(tk, payload);
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
                this->error(msg.str());
	            }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE
                    || xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
	            {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << payload.get_task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
	            }
            else
	            {
                throw xe;
	            }
	        }
	    }


		template <typename number>
		void task_manager<number>::slave_dispatch_output_task(output_task<number>* tk, const MPI::new_derived_content_payload& payload)
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
			    std::bind(&task_manager<number>::slave_push_derived_content, this, std::placeholders::_1, std::placeholders::_2);

		    // acquire a datapipe which we can use to stream content from the databse
		    typename data_manager<number>::datapipe pipe = this->data_mgr->create_datapipe(payload.get_logdir_path(), payload.get_tempdir_path(),
                                                                                       finder, dispatcher, this->get_rank(), timer);

				BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << "-- NEW OUTPUT TASK '" << tk->get_name() << "'" << std::endl;
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
				if(success) BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending FINISHED_DERIVED_CONTENT to master";
				else        BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::error)  << std::endl << "-- Worker reporting DERIVED_CONTENT_FAIL to master";

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
    void task_manager<number>::slave_process_task(const MPI::new_postintegration_payload& payload)
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
                    this->slave_dispatch_postintegration_task(tk, payload);
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
                this->error(msg.str());
              }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE
              || xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
              {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << payload.get_task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
              }
            else
              {
                throw xe;
              }
          }
      }


    template <typename number>
    void task_manager<number>::slave_dispatch_postintegration_task(postintegration_task<number>* tk, const MPI::new_postintegration_payload& payload)
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
                                                                         std::bind(&task_manager<number>::slave_push_temp_container, this, std::placeholders::_1,
                                                                                   MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            typename data_manager<number>::zeta_twopf_batcher batcher =
                                                                this->data_mgr->create_temp_zeta_twopf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
                                                                                                                 this->get_rank(), dispatcher);

            this->slave_dispatch_postintegration_queue(z2pf, ptk, work, payload, batcher);
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
                                                                         std::bind(&task_manager<number>::slave_push_temp_container, this, std::placeholders::_1,
                                                                                   MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            typename data_manager<number>::zeta_threepf_batcher batcher =
                                                                  this->data_mgr->create_temp_zeta_threepf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
                                                                                                                     this->get_rank(), dispatcher);

            this->slave_dispatch_postintegration_queue(z3pf, ptk, work, payload, batcher);
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
                                                                         std::bind(&task_manager<number>::slave_push_temp_container, this, std::placeholders::_1,
                                                                                   MPI::POSTINTEGRATION_DATA_READY, std::string("POSTINTEGRATION_DATA_READY"));

            // construct batcher to hold output
            typename data_manager<number>::fNL_batcher batcher =
                                                         this->data_mgr->create_temp_fNL_container(payload.get_tempdir_path(), payload.get_logdir_path(),
                                                                                                   this->get_rank(), dispatcher, zfNL->get_template());

            this->slave_dispatch_postintegration_queue(zfNL, ptk, work, payload, batcher);
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
    void task_manager<number>::slave_dispatch_postintegration_queue(TaskObject* tk, ParentTaskObject* ptk, QueueObject& work,
                                                                    const MPI::new_postintegration_payload& payload, BatchObject& batcher)
      {
        assert(tk != nullptr);    // should be guaranteed
        assert(ptk != nullptr);   // should be guaranteed

        bool success = true;

        // keep track of wallclock time
        boost::timer::cpu_timer timer;

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW POSTINTEGRATION TASK '" << tk->get_name() << "'" << std::endl;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

        // set up output-group finder function
        typename data_manager<number>::datapipe::output_group_finder finder =
                                                                       std::bind(&repository<number>::find_integration_task_output, this->repo, std::placeholders::_1, std::placeholders::_2);

        // set up empty content-dispatch function -- this datapipe is not used to produce content
        typename data_manager<number>::datapipe::dispatch_function dispatcher =
                                                                     std::bind(&task_manager<number>::slave_disallow_push_content, this, std::placeholders::_1, std::placeholders::_2);

        // acquire a datapipe which we can use to stream content from the databse
        typename data_manager<number>::datapipe pipe = this->data_mgr->create_datapipe(payload.get_logdir_path(), payload.get_tempdir_path(),
                                                                                       finder, dispatcher, this->get_rank(), timer, true);


        // perform the task
        try
          {
            pipe.attach(ptk, ptk->get_model()->get_N_fields(), payload.get_tags());
            this->slave_postintegration_handler(tk, ptk, work, batcher, pipe);
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
        if(success) BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending FINISHED_POSTINTEGRATION to master";
        else        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::error)  << std::endl << "-- Worker reporting POSTINTEGRATION_FAIL to master";

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
    void task_manager<number>::slave_postintegration_handler(zeta_twopf_task<number>* tk, twopf_task<number>* ptk, work_queue<twopf_kconfig>& work,
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
    void task_manager<number>::slave_postintegration_handler(zeta_threepf_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
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
    void task_manager<number>::slave_postintegration_handler(fNL_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
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


#endif //__task_manager_H_
