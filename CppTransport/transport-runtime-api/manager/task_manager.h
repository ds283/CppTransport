//
// Created by David Seery on 24/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __task_manager_H_
#define __task_manager_H_


#include <list>
#include <set>
#include <vector>

#include "transport-runtime-api/models/model.h"
#include "transport-runtime-api/manager/instance_manager.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/model_list.h"

#include "transport-runtime-api/manager/mpi_operations.h"

#include "transport-runtime-api/manager/repository.h"
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/scheduler/context.h"
#include "transport-runtime-api/scheduler/scheduler.h"
#include "transport-runtime-api/scheduler/work_queue.h"

#include "transport-runtime-api/utilities/formatter.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/timer/timer.hpp"


#define __CPP_TRANSPORT_SWITCH_REPO     "-repo"
#define __CPP_TRANSPORT_SWITCH_TASK     "-task"
#define __CPP_TRANSPORT_SWITCH_TAG      "-tag"

// name for worker devices
#define __CPP_TRANSPORT_WORKER_NAME     "mpi-worker-"

// default storage limit on nodes - 300 Mb
// on a machine with 8 workers, that would give 2400 Mb or 2.4 Gb
// this can be increased (either here, or when creating a
// task_manager object) on machines with more memory
#define __CPP_TRANSPORT_DEFAULT_STORAGE (300*1024*1024)


namespace transport
  {

    template <typename number>
    class task_manager : public instance_manager<number>
      {
      public:

        typedef enum { job_task } job_type;


        class job_descriptor
          {
          public:
            job_type               type;
            std::string            name;
		        std::list<std::string> tags;
          };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a task manager using command-line arguments. The repository must exist and be named on the command line.
        task_manager(int argc, char* argv[], unsigned int cp=__CPP_TRANSPORT_DEFAULT_STORAGE);

        //! Construct a task manager using a previously-constructed repository object. Usually this will be used only when creating a new repository.
        task_manager(int argc, char* argv[], repository<number>* r, unsigned int cp=__CPP_TRANSPORT_DEFAULT_STORAGE);

        //! Destroy a task manager.
        ~task_manager();


        // INTERFACE -- REPOSITORY MANAGEMENT

      public:

		    //! Return handle to repository
		    repository<number>* get_repository();

		    //! Return handle to data manager
		    data_manager<number>* get_data_manager();


        // INTERFACE -- MASTER-SLAVE API

      public:

        //! Query whether we are the master process
        bool is_master(void) const { return(this->world.rank() == MPI::RANK_MASTER); }

        //! Return MPI rank of this process
        unsigned int get_rank(void) const { return(this->world.rank()); }

		    //! Get worker number
		    unsigned int worker_number() { return(this->world.rank()-1); }

        //! If we are the master process, execute any queued tasks
        void execute_tasks(void);

        //! If we are a slave process, poll for instructions to perform work
        void wait_for_tasks(void);

      protected:

        //! Master node: Process a 'task' job -- tasks are integrations, so this dispatches to the appropriate integrator
        void master_process_task(const job_descriptor& job);

        //! Master node: Dispatch a twopf 'task' (ie., integration) to the worker processes
        void master_dispatch_twopf_task(twopf_task<number>* tk, model_list<number>& mlist, const std::list<std::string>& tags);

        //! Master node: Dispatch a threepf 'task' (ie., integration) to the worker processes
        void master_dispatch_threepf_task(threepf_task<number>* tk, model_list<number>& mlist, const std::list<std::string>& tags);

        //! Master node: Terminate all worker processes
        void master_terminate_workers(void);

        //! Master node: Push repository information to worker processes
        void master_push_repository(void);

        //! Master node: Pass new integration task to the workers
        void master_integration_task_to_workers(typename repository<number>::integration_writer& ctr, integration_task<number>* task_name, model<number>* m);

        //! Slave node: Process a new task instruction
        void slave_process_task(const MPI::new_integration_payload& payload);

        //! Slave node: Process a twopf task
        void slave_dispatch_twopf_task(twopf_task<number>* tk, model_list<number>& mlist, const MPI::new_integration_payload& payload, const work_item_filter& filter);

        //! Slave node: Process a threepf task
        void slave_dispatch_threepf_task(threepf_task<number>* tk, model_list<number>& mlist, const MPI::new_integration_payload& payload, const work_item_filter& filter);

        //! Slave node: set repository
        void slave_set_repository(const MPI::set_repository_payload& payload);

        //! Slave node: clean up after integration: wait for, and process, DELETE_CONTAINER messages until all temporary containers are gone
        void slave_clean_up(typename data_manager<number>::generic_batcher& batcher);

        //! Slave node: process queued DELETE_CONTAINER messages

        //! Note that this function takes a pointed to a batcher, even though we use references elsewhere.
        //! This is because batchers will dispatch to this function, setting a pointer to themselves using 'this'.
        //! Therefore we have to take a pointer; there's no way for an object to get a reference to itself.
        void slave_process_delete_containers(typename data_manager<number>::generic_batcher* batcher);

        //! Push a temporary container to the master process
        void slave_push_temp_container(typename data_manager<number>::generic_batcher* batcher, MPI::data_ready_payload::payload_type type);

        //! Make a 'device context' for the MPI worker processes
        context make_workers_context(void);


        // MPI utility functions

      protected:

        //! Map worker number to communicator rank
        constexpr unsigned int worker_rank(unsigned int worker_number) { return(worker_number+1); }

        //! Map communicator rank to worker number
        constexpr unsigned int worker_number(unsigned int worker_rank) { return(worker_rank-1); }


        // INTERFACE -- ERROR REPORTING

      public:

        //! Report an error
        void error(const std::string& msg) { std::cout << msg << std::endl; }

        //! Report a warning
        void warn(const std::string& msg) { std::cout << msg << std::endl; }

        // INTERNAL DATA

      protected:

        //! BOOST::MPI environment
        boost::mpi::environment environment;
        //! BOOST::MPI world communicator
        boost::mpi::communicator world;

        //! Repository manager instance
        repository<number>* repo;
        //! Data manager instance
        data_manager<number>* data_mgr;

        //! Queue of tasks to process
        std::list<job_descriptor> job_queue;

        //! Storage capacity per worker
        const unsigned int worker_capacity;

        //! Queue of temporary containers to be deleted (only relevant on slave nodes)
        std::list<std::string> temporary_container_queue;
      };


    template <typename number>
    task_manager<number>::task_manager(int argc, char* argv[], unsigned int cp)
      : instance_manager<number>(), environment(argc, argv),
        worker_capacity(cp),
        repo(nullptr), data_mgr(data_manager_factory<number>(cp))
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
                            repo = repository_factory<number>(repo_path);
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
                else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_TASK)
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
	              else if(static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_TAG)
	                {
		                if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_TAG);
		                else            tags.push_back(std::string(argv[++i]));
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
    task_manager<number>::task_manager(int argc, char* argv[], repository<number>* r, unsigned int cp)
      : instance_manager<number>(), environment(argc, argv),
        worker_capacity(cp),
        repo(r), data_mgr(data_manager_factory<number>(cp))
      {
        assert(repo != nullptr);
      }


    template <typename number>
    task_manager<number>::~task_manager()
      {
        delete this->repo;
      }


    // REPOSITORY INTERFACE


		template <typename number>
		repository<number>* task_manager<number>::get_repository()
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
    void task_manager<number>::execute_tasks()
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
                      this->master_process_task(*t);
                      break;

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
    void task_manager<number>::master_process_task(const job_descriptor& job)
      {
        try
          {
						// query a task with the name we're looking for from the database
		        model_list<number> mlist;
            task<number>* tk = this->repo->query_task(job.name, mlist, this->model_finder_factory());

		        // check that a model instance was returned -- we'll need this
            assert(mlist.size() > 0);

            // set up work queues for this task, and then distribute to worker processes

            // dynamic_cast<> is a bit unsubtle, but we cannot predict in advance what type
            // of task will be returned
            if(dynamic_cast< threepf_task<number>* >(tk) != nullptr)
              {
                threepf_task<number>* three_task = dynamic_cast< threepf_task<number>* >(tk);
                this->master_dispatch_threepf_task(three_task, mlist, job.tags);
              }
            else if(dynamic_cast< twopf_task<number>* >(tk) != nullptr)
              {
                twopf_task<number>* two_task = dynamic_cast< twopf_task<number>* >(tk);
                this->master_dispatch_twopf_task(two_task, mlist, job.tags);
              }
	          else if(dynamic_cast< output_task<number>* >(tk) != nullptr)
	            {
                std::cerr << "Output tasks not implemented yet!" << std::endl;
	            }
            else
              {
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK);
              }

            delete tk;
          }
        catch (runtime_exception xe)
          {
            if(xe.get_exception_code() == runtime_exception::TASK_NOT_FOUND)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << xe.what() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
              }
            else if(xe.get_exception_code() == runtime_exception::MODEL_NOT_FOUND)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_MISSING_MODEL_A << " '" << xe.what() << "' "
                  << __CPP_TRANSPORT_REPO_MISSING_MODEL_B << " '" << job.name << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
              }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE)
              {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << job.name << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
              }
            else if(xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
              {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << job.name << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
              }
            else
              {
                throw xe;
              }
          }
      }


    template <typename number>
    void task_manager<number>::master_dispatch_twopf_task(twopf_task<number>* tk, model_list<number>& mlist, const std::list<std::string>& tags)
      {
		    // can't process a task if there are no workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

		    // should have exactly one model instance
		    if(mlist.size() != 1) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_MODEL_LIST_MISMATCH);

        // set up a work queue representing our workers
        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

		    model<number>* m = mlist.front();

        work_queue<twopf_kconfig> queue = sch.make_queue(m->backend_twopf_state_size(), *tk);

        // create new output record in the repository database, and set up
        // paths to the integration database
        typename repository<number>::integration_writer ctr = this->repo->integration_new_output(tk, tags, m->get_backend(), this->get_rank());

        // create the data container
        this->data_mgr->create_container(ctr);

        // write the task distribution list -- this is subsequently read by the worker processes,
        // to find out which work items they should process
        this->data_mgr->create_taskfile(ctr, queue);

        // write the various tables needed in the database
        this->data_mgr->create_tables(ctr, tk, m->get_N_fields());

        // instruct workers to carry out the calculation
        // this call returns when all workers have signalled that their work is done
        this->master_integration_task_to_workers(ctr, tk, m);

        // close the data container
        this->data_mgr->close_container(ctr);
      }


    template <typename number>
    void task_manager<number>::master_dispatch_threepf_task(threepf_task<number>* tk, model_list<number>& mlist, const std::list<std::string>& tags)
      {
        // can't process a task if there are no workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        // should have exactly one model instance
        if(mlist.size() != 1) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_MODEL_LIST_MISMATCH);

        // set up a work queue representing our workers
        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        model<number>* m = mlist.front();

        work_queue<threepf_kconfig> queue = sch.make_queue(m->backend_threepf_state_size(), *tk);

        // create new output record in the repository database, and set up
        // paths to the integration database
        typename repository<number>::integration_writer ctr = this->repo->integration_new_output(tk, tags, m->get_backend(), this->get_rank());

        // create the data container
        this->data_mgr->create_container(ctr);

        // write the task distribution list -- this is subsequently read by the worker processes,
        // to find out which work items they should process
        this->data_mgr->create_taskfile(ctr, queue);

        // create the various tables needed in the database
        this->data_mgr->create_tables(ctr, tk, m->get_N_fields());

        // instruct workers to carry out the calculation
        // this call returns when all workers have signalled that their work is done
        this->master_integration_task_to_workers(ctr, tk, m);

        // close the data container
        this->data_mgr->close_container(ctr);
      }


    template <typename number>
    void task_manager<number>::master_integration_task_to_workers(typename repository<number>::integration_writer& ctr,
                                                                  integration_task<number>* tk, model<number>* m)
      {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ NEW INTEGRATION TASK '" << tk->get_name() << "'";
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << *tk;

        // set up a timer to keep track of the total wallclock time used in this integration
        boost::timer::cpu_timer wallclock_timer;
        // aggregate integration times reported by worker processes
        boost::timer::nanosecond_type total_work_time = 0;
        boost::timer::nanosecond_type total_aggregation_time = 0;

        // get paths the workers will need
        assert(this->repo != nullptr);
        boost::filesystem::path taskfile_path = ctr.taskfile_path();
        boost::filesystem::path tempdir_path  = ctr.temporary_files_path();
        boost::filesystem::path logdir_path   = ctr.log_directory_path();

        MPI::new_integration_payload payload(tk->get_name(), taskfile_path, tempdir_path, logdir_path);

        for(unsigned int i = 0; i < this->world.size()-1; i++)
          {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_INTEGRATION, payload);
          }

        // wait for all messages to be received
        boost::mpi::wait_all(requests.begin(), requests.end());

        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ All workers received NEW_INTEGRATION instruction";

        // poll workers, receiving data until workers are exhausted
        std::set<unsigned int> workers;
        for(unsigned int i = 0; i < this->world.size()-1; i++) workers.insert(i);
        while(workers.size() > 0)
          {
            BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) <<  "++ Master polling for DATA_READY messages";
            // wait until a message is available from a worker
            boost::mpi::status stat = this->world.probe();

            switch(stat.tag())
              {
                case MPI::DATA_READY:
                  {
                    MPI::data_ready_payload payload;
                    this->world.recv(stat.source(), MPI::DATA_READY, payload);
                    BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " sent aggregation request for container '" << payload.get_container_path() << "'";

                    // set up a timer to measure how long we spend batching
                    boost::timer::cpu_timer batching_timer;

                    // batch data into the main container
                    switch(payload.get_payload_type())
                      {
                        case MPI::data_ready_payload::twopf_payload:
                          {
                            this->data_mgr->aggregate_twopf_batch(ctr, payload.get_container_path(), m, tk);
                            break;
                          }

                        case MPI::data_ready_payload::threepf_payload:
                          {
                            this->data_mgr->aggregate_threepf_batch(ctr, payload.get_container_path(), m, tk);
                            break;
                          }

                        default:
                          assert(false);
                      }

                    batching_timer.stop();
                    BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Aggregated temporary container in time " << format_time(batching_timer.elapsed().wall);
                    total_aggregation_time += batching_timer.elapsed().wall;

                    // instruct worker to remove the temporary container
                    BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Sending worker " << stat.source() << " delete instruction for container '" << payload.get_container_path() << "'";
                    this->world.isend(stat.source(), MPI::DELETE_CONTAINER, payload.get_container_path());
                    break;
                  }

                case MPI::FINISHED_TASK:
                  {
                    boost::timer::nanosecond_type work_time = 0;
                    this->world.recv(stat.source(), MPI::FINISHED_TASK, work_time);
                    BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising finished task in CPU time " << format_time(work_time);

                    total_work_time += work_time;
                    workers.erase(this->worker_number(stat.source()));
                    break;
                  }

                default:
                  {
                    BOOST_LOG_SEV(ctr.get_log(), repository<number>::warning) << "++ Master received unexpected message " << stat.tag() << " waiting in the queue";
                    break;
                  }
              }
          }

        wallclock_timer.stop();
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Total wallclock time for task '" << tk->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++   " << wallclock_timer.format();
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Total integration time required by worker processes = " << format_time(total_work_time);
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Total aggregation time required by master process = " << format_time(total_aggregation_time);
      }


    template <typename number>
    context task_manager<number>::make_workers_context()
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


    // SLAVE FUNCTIONS


    template <typename number>
    void task_manager<number>::wait_for_tasks()
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
        boost::filesystem::path repo_path = payload.repository_path();

        try
          {
            this->repo = repository_factory<number>(repo_path.string(), repository<number>::access_type::readonly);
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

        std::set<unsigned int> work_items = this->data_mgr->read_taskfile(payload.taskfile_path(), this->worker_number());
        work_item_filter filter(work_items);

        // extract our task from the database
        // much of this is boiler-plate which is similar to master_process_task()
        // TODO: it would be nice to make this sharing more explicit, so the code isn't just duplicated
        try
          {
		        model_list<number> mlist;
            task<number>* tk = this->repo->query_task(payload.task_name(), mlist, this->model_finder_factory());
            assert(mlist.size() > 0);

            // dynamic_cast<> is a bit unsubtle, but we cannot predict in advance what type
            // of task will be returned
            if(dynamic_cast< threepf_task<number>* >(tk))
              {
                threepf_task<number>* three_task = dynamic_cast< threepf_task<number>* >(tk);
                this->slave_dispatch_threepf_task(three_task, mlist, payload, filter);
              }
            else if(dynamic_cast< twopf_task<number>* >(tk))
              {
                twopf_task<number>* two_task = dynamic_cast< twopf_task<number>* >(tk);
                this->slave_dispatch_twopf_task(two_task, mlist, payload, filter);
              }
            else
              {
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK);
              }

            delete tk;
          }
        catch (runtime_exception xe)
          {
            if(xe.get_exception_code() == runtime_exception::TASK_NOT_FOUND)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << xe.what() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
              }
            else if(xe.get_exception_code() == runtime_exception::MODEL_NOT_FOUND)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_MISSING_MODEL_A << " '" << xe.what() << "' "
                  << __CPP_TRANSPORT_REPO_MISSING_MODEL_B << " '" << payload.task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
              }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE)
              {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << payload.task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
              }
            else if(xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
              {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << payload.task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
              }
            else
              {
                throw xe;
              }
          }

      }


    template <typename number>
    void task_manager<number>::slave_dispatch_twopf_task(twopf_task<number>* tk, model_list<number>& mlist,
                                                         const MPI::new_integration_payload& payload,
                                                         const work_item_filter& filter)
      {
        // dispatch integration to the underlying model

        // keep track of CPU time
        boost::timer::cpu_timer timer;

        if(mlist.size() != 1)
	        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_MODEL_LIST_MISMATCH);

        model<number>* m = mlist.front();

        // create queues based on whatever devices are relevant for the backend
        context                   ctx  = m->backend_get_context();
        scheduler                 sch  = scheduler(ctx);
        work_queue<twopf_kconfig> work = sch.make_queue(m->backend_twopf_state_size(), *tk, filter);

        // make a temporary container object to hold the output of the integration
        typename data_manager<number>::container_dispatch_function dispatcher =
                                                                     std::bind(&task_manager<number>::slave_push_temp_container,
                                                                               this, std::placeholders::_1, MPI::data_ready_payload::twopf_payload);
        typename data_manager<number>::twopf_batcher batcher =
                                                       this->data_mgr->create_temp_twopf_container(payload.tempdir_path(), payload.logdir_path(),
                                                                                                   this->get_rank(), m->get_N_fields(), dispatcher, timer);

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW INTEGRATION TASK '" << tk->get_name() << "'";
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

        // perform the integration
        m->backend_process_twopf(work, tk, batcher, true);    // 'true' = work silently

        // close the batcher
        batcher.close();

        // all work is now done - stop the timer
        timer.stop();

        // wait until all temporary containers have been deleted, and then return
        this->slave_clean_up(batcher);

        // notify master process that all work has been finished
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- Slave sending FINISHED_TASK to master";
        this->world.isend(MPI::RANK_MASTER, MPI::FINISHED_TASK, timer.elapsed().wall);
      }


    template <typename number>
    void task_manager<number>::slave_dispatch_threepf_task(threepf_task<number>* tk, model_list<number>& mlist,
                                                           const MPI::new_integration_payload& payload,
                                                           const work_item_filter& filter)
      {
        // dispatch integration to the underlying model

        // keep track of CPU time
        boost::timer::cpu_timer timer;

				if(mlist.size() != 1)
					throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_MODEL_LIST_MISMATCH);

				model<number>* m = mlist.front();

        // create queues based on whatever devices are relevant for the backend
        context                     ctx  = m->backend_get_context();
        scheduler                   sch  = scheduler(ctx);
        work_queue<threepf_kconfig> work = sch.make_queue(m->backend_threepf_state_size(), *tk, filter);

        // make a temporary container object to hold the output of the integration
        typename data_manager<number>::container_dispatch_function dispatcher =
                                                                     std::bind(&task_manager<number>::slave_push_temp_container,
                                                                               this, std::placeholders::_1, MPI::data_ready_payload::threepf_payload);
        typename data_manager<number>::threepf_batcher batcher =
                                                         this->data_mgr->create_temp_threepf_container(payload.tempdir_path(), payload.logdir_path(),
                                                                                                       this->get_rank(), m->get_N_fields(), dispatcher, timer);

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW INTEGRATION TASK '" << tk->get_name() << "'";
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

        // perform the integration
        m->backend_process_threepf(work, tk, batcher, true);    // 'true' = work silently

        // close the batcher
        batcher.close();

        // all work is now done - stop the timer
        timer.stop();

        // wait until all temporary containers have been deleted, and then return
        this->slave_clean_up(batcher);

        // notify master process that all work has been finished
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- Slave sending FINISHED_TASK to master";
        this->world.isend(MPI::RANK_MASTER, MPI::FINISHED_TASK, timer.elapsed().wall);
      }


    template <typename number>
    void task_manager<number>::slave_clean_up(typename data_manager<number>::generic_batcher& batcher)
      {
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- Slave entering post-integration cleanup: waiting to delete containers";

        while(this->temporary_container_queue.size() > 0)
          {
            this->slave_process_delete_containers(&batcher);
          }
      }


    template <typename number>
    void task_manager<number>::slave_process_delete_containers(typename data_manager<number>::generic_batcher* batcher)
      {
//        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- Slave waiting for DELETE_CONTAINER instructions";

        // check for DELETE_CONTAINER instructions from the master node
        boost::optional<boost::mpi::status> stat = this->world.iprobe(MPI::RANK_MASTER, MPI::DELETE_CONTAINER);

        // work through all queued messages
        while(stat)
          {
            std::string payload;
            this->world.recv(MPI::RANK_MASTER, MPI::DELETE_CONTAINER, payload);
            BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal) << "-- Slave received delete instruction: " << payload;

            if(!boost::filesystem::remove(payload))
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_DATACTR_REMOVE_TEMP << " '" << payload << "'";
                this->error(msg.str());
              }
            this->temporary_container_queue.remove(payload);

            stat = this->world.iprobe(MPI::RANK_MASTER, MPI::DELETE_CONTAINER);
          }
      }


    // MPI UTILITY FUNCTIONS


    template <typename number>
    void task_manager<number>::master_terminate_workers()
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
    void task_manager<number>::master_push_repository()
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
    void task_manager<number>::slave_push_temp_container(typename data_manager<number>::generic_batcher* batcher,
                                                         MPI::data_ready_payload::payload_type type)
      {
        BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal) << "-- Sending DATA_READY message for container " << batcher->get_container_path();

        MPI::data_ready_payload payload(batcher->get_container_path().string(), type);

        // advise master process that data is available in the named container
        this->world.isend(MPI::RANK_MASTER, MPI::DATA_READY, payload);

        // add this container to the list of containers which should be deleted
        this->temporary_container_queue.push_back(batcher->get_container_path().string());

        // work through any queued DELETE_CONTAINER messages
        this->slave_process_delete_containers(batcher);
      }


  } // namespace transport


#endif //__task_manager_H_
