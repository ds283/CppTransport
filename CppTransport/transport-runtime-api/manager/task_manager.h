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
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"

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
        unsigned int get_rank(void) const { return(static_cast<unsigned int>(this->world.rank())); }

		    //! Get worker number
		    unsigned int worker_number() { return(static_cast<unsigned int>(this->world.rank()-1)); }

        //! If we are the master process, execute any queued tasks
        void execute_tasks(void);

        //! If we are a slave process, poll for instructions to perform work
        void wait_for_tasks(void);


      protected:

        //! Master node: Process a 'task' job.
		    //! Some tasks are integrations, others process the numerical output from an integration to product
		    //! a derived data product (like fNL).
		    //! This function schedules workers to process the task.
        void master_process_task(const job_descriptor& job);

        //! Master node: Dispatch a twopf 'task' (ie., integration) to the worker processes
        void master_dispatch_twopf_task(twopf_task<number>* tk, model<number>* m, const std::list<std::string>& tags);

        //! Master node: Dispatch a threepf 'task' (ie., integration) to the worker processes
        void master_dispatch_threepf_task(threepf_task<number>* tk, model<number>* m, const std::list<std::string>& tags);

		    //! Master node: Dispatch an output 'task' (ie., generation of derived data products) to the worker processes
		    void master_dispatch_output_task(output_task<number>* tk, model<number>* m, const std::list<std::string>& tags);

        //! Master node: Terminate all worker processes
        void master_terminate_workers(void);

        //! Master node: Push repository information to worker processes
        void master_push_repository(void);

        //! Master node: Pass new integration task to the workers
        void master_integration_task_to_workers(typename repository<number>::integration_writer& ctr,
                                                integration_task<number>* task_name, model<number>* m);

		    //! Master node: Pass new output task to the workers
		    void master_output_task_to_workers(typename repository<number>::derived_content_writer& writer,
		                                       output_task<number>* task_name, const std::list<std::string>& tags);

        //! Slave node: Process a new integration task instruction
        void slave_process_task(const MPI::new_integration_payload& payload);

		    //! Slave node: Process a new output task instruction
		    void slave_process_task(const MPI::new_derived_content_payload& payload);

        //! Slave node: Process a twopf task
        void slave_dispatch_twopf_task(twopf_task<number>* tk, model<number>* m,
                                       const MPI::new_integration_payload& payload, const work_item_filter<twopf_kconfig>& filter);

        //! Slave node: Process a threepf task
        void slave_dispatch_threepf_task(threepf_task<number>* tk, model<number>* m,
                                         const MPI::new_integration_payload& payload, const work_item_filter<threepf_kconfig>& filter);

		    //! Slave node: Process an output task
		    void slave_dispatch_output_task(output_task<number>* tk, model<number>* m,
		                                    const MPI::new_derived_content_payload& payload, const work_item_filter< output_task_element<number> >& filter);

        //! Slave node: set repository
        void slave_set_repository(const MPI::set_repository_payload& payload);

        //! Push a temporary container to the master process
        void slave_push_temp_container(typename data_manager<number>::generic_batcher* batcher, MPI::data_ready_payload::payload_type type);

		    //! Push new derived content to the master process
        void slave_push_derived_content(typename data_manager<number>::datapipe* pipe);

        //! Make a 'device context' for the MPI worker processes
        context make_workers_context(void);


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


        // INTERNAL DATA

      protected:


		    // MPI ENVIRONMENT

        //! BOOST::MPI environment
        boost::mpi::environment environment;

        //! BOOST::MPI world communicator
        boost::mpi::communicator world;


		    // RUNTIME AGENTS

        //! Repository manager instance
        repository<number>* repo;

        //! Data manager instance
        data_manager<number>* data_mgr;


		    // DATA AND STATE

        //! Queue of tasks to process
        std::list<job_descriptor> job_queue;

        //! Storage capacity per worker
        const unsigned int worker_capacity;

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
                            repo = repository_factory<number>(repo_path, repository<number>::access_type::readwrite,
                                                              std::bind(&task_manager<number>::error, this, std::placeholders::_1),
                                                              std::bind(&task_manager<number>::warn, this, std::placeholders::_1));
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
            model<number>* m = nullptr;
            task<number>* tk = this->repo->lookup_task(job.name, m, this->model_finder_factory());

            // set up work queues for this task, and then distribute to worker processes

            // dynamic_cast<> is a bit unsubtle, but we cannot predict in advance what type
            // of task will be returned
            if(dynamic_cast< threepf_task<number>* >(tk) != nullptr)
              {
                threepf_task<number>* three_task = dynamic_cast< threepf_task<number>* >(tk);
                this->master_dispatch_threepf_task(three_task, m, job.tags);
              }
            else if(dynamic_cast< twopf_task<number>* >(tk) != nullptr)
              {
                twopf_task<number>* two_task = dynamic_cast< twopf_task<number>* >(tk);
                this->master_dispatch_twopf_task(two_task, m, job.tags);
              }
	          else if(dynamic_cast< output_task<number>* >(tk) != nullptr)
	            {
		            output_task<number>* out_task = dynamic_cast< output_task<number>* >(tk);
		            this->master_dispatch_output_task(out_task, m, job.tags);
	            }
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << job.name << "'";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
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
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE
	                  || xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
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
    void task_manager<number>::master_dispatch_twopf_task(twopf_task<number>* tk, model<number>* m, const std::list<std::string>& tags)
      {
        assert(m != nullptr);   // should be guaranteed
        assert(tk != nullptr);

		    // can't process a task if there are no workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        // set up a work queue representing our workers
        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        work_queue<twopf_kconfig> queue = sch.make_queue(m->backend_twopf_state_size(), *tk);

        // create new output record in the repository database, and set up
        // paths to the integration database
        typename repository<number>::integration_writer writer = this->repo->new_integration_task_output(tk, tags, m, this->get_rank());

        // create the data writer
        this->data_mgr->initialize_writer(writer);

        // write the task distribution list -- this is subsequently read by the worker processes,
        // to find out which work items they should process
        this->data_mgr->create_taskfile(writer, queue);

        // write the various tables needed in the database
        this->data_mgr->create_tables(writer, tk, m->get_N_fields());

        // instruct workers to carry out the calculation
        // this call returns when all workers have signalled that their work is done
        this->master_integration_task_to_workers(writer, tk, m);

        // close the data writer
        this->data_mgr->close_writer(writer);

        // commit integration output to the database
        writer.commit();
      }


    template <typename number>
    void task_manager<number>::master_dispatch_threepf_task(threepf_task<number>* tk, model<number>* m, const std::list<std::string>& tags)
      {
        assert(m != nullptr);   // should be guaranteed
        assert(tk != nullptr);  // should be guaranteed

        // can't process a task if there are no workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        // set up a work queue representing our workers
        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        work_queue<threepf_kconfig> queue = sch.make_queue(m->backend_threepf_state_size(), *tk);

        // create new output record in the repository database, and set up
        // paths to the integration database
        typename repository<number>::integration_writer writer = this->repo->new_integration_task_output(tk, tags, m, this->get_rank());

        // create the data writer
        this->data_mgr->initialize_writer(writer);

        // write the task distribution list -- this is subsequently read by the worker processes,
        // to find out which work items they should process
        this->data_mgr->create_taskfile(writer, queue);

        // create the various tables needed in the database
        this->data_mgr->create_tables(writer, tk, m->get_N_fields());

        // instruct workers to carry out the calculation
        // this call returns when all workers have signalled that their work is done
        this->master_integration_task_to_workers(writer, tk, m);

        // close the data writer
        this->data_mgr->close_writer(writer);

		    // commit integration output to the database
		    writer.commit();
      }


    template <typename number>
    void task_manager<number>::master_integration_task_to_workers(typename repository<number>::integration_writer& ctr,
                                                                  integration_task<number>* tk, model<number>* m)
      {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ NEW INTEGRATION TASK '" << tk->get_name() << "'" << std::endl;
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << *tk;

        // set up a timer to keep track of the total wallclock time used in this integration
        boost::timer::cpu_timer wallclock_timer;

        // aggregate integration times reported by worker processes
        boost::timer::nanosecond_type total_wallclock_time = 0;
        boost::timer::nanosecond_type total_aggregation_time = 0;

        boost::timer::nanosecond_type total_integration_time = 0;
        boost::timer::nanosecond_type min_mean_integration_time = 0;
        boost::timer::nanosecond_type max_mean_integration_time = 0;
        boost::timer::nanosecond_type global_min_integration_time = 0;
        boost::timer::nanosecond_type global_max_integration_time = 0;
        boost::timer::nanosecond_type total_batching_time = 0;
        boost::timer::nanosecond_type min_mean_batching_time = 0;
        boost::timer::nanosecond_type max_mean_batching_time = 0;
        boost::timer::nanosecond_type global_min_batching_time = 0;
        boost::timer::nanosecond_type global_max_batching_time = 0;

        unsigned int num_integrations = 0;

        // get paths the workers will need
        assert(this->repo != nullptr);
        boost::filesystem::path taskfile_path = ctr.get_abs_taskfile_path();
        boost::filesystem::path tempdir_path  = ctr.get_abs_tempdir_path();
        boost::filesystem::path logdir_path   = ctr.get_abs_logdir_path();

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
            BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) <<  "++ Master polling for INTEGRATION_DATA_READY messages";
            // wait until a message is available from a worker
            boost::mpi::status stat = this->world.probe();

            switch(stat.tag())
              {
                case MPI::INTEGRATION_DATA_READY:
                  {
                    MPI::data_ready_payload payload;
                    this->world.recv(stat.source(), MPI::INTEGRATION_DATA_READY, payload);
                    BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " sent aggregation notification for container '" << payload.get_container_path() << "'";

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

		                // remove temporary container
				            if(!boost::filesystem::remove(payload.get_container_path()))
				              {
				                std::ostringstream msg;
				                msg << __CPP_TRANSPORT_DATACTR_REMOVE_TEMP << " '" << payload.get_container_path() << "'";
				                this->error(msg.str());
				              }
                    break;
                  }

                case MPI::FINISHED_INTEGRATION:
                  {
                    MPI::finished_integration_payload payload;
                    this->world.recv(stat.source(), MPI::FINISHED_INTEGRATION, payload);
                    BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising finished task in wallclock time " << format_time(payload.get_wallclock_time());

                    total_wallclock_time += payload.get_wallclock_time();

                    total_integration_time += payload.get_integration_time();
                    boost::timer::nanosecond_type mean_integration_time = payload.get_integration_time() / payload.get_num_integrations();
                    if(max_mean_integration_time == 0 || mean_integration_time > max_mean_integration_time) max_mean_integration_time = mean_integration_time;
                    if(min_mean_integration_time == 0 || mean_integration_time < min_mean_integration_time) min_mean_integration_time = mean_integration_time;

                    if(global_max_integration_time == 0 || payload.get_max_integration_time() > global_max_integration_time) global_max_integration_time = payload.get_max_integration_time();
                    if(global_min_integration_time == 0 || payload.get_min_integration_time() < global_min_integration_time) global_min_integration_time = payload.get_min_integration_time();

                    total_batching_time += payload.get_batching_time();
                    boost::timer::nanosecond_type mean_batching_time = payload.get_batching_time() / payload.get_num_integrations();
                    if(max_mean_batching_time == 0 || mean_batching_time > max_mean_batching_time) max_mean_batching_time = mean_batching_time;
                    if(min_mean_batching_time == 0 || mean_batching_time < min_mean_batching_time) min_mean_batching_time = mean_batching_time;

                    if(global_max_batching_time == 0 || payload.get_max_batching_time() > global_max_batching_time) global_max_batching_time = payload.get_max_batching_time();
                    if(global_min_batching_time == 0 || payload.get_min_batching_time() < global_min_batching_time) global_min_batching_time = payload.get_min_batching_time();

                    num_integrations += payload.get_num_integrations();

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

        typename repository<number>::integration_metadata timings(total_wallclock_time, total_aggregation_time,
                                                             total_integration_time, min_mean_integration_time, max_mean_integration_time,
                                                             global_min_integration_time, global_max_integration_time,
                                                             total_batching_time, min_mean_batching_time, max_mean_batching_time,
                                                             global_min_batching_time, global_max_batching_time,
                                                             num_integrations);
        ctr.set_metadata(timings);

        wallclock_timer.stop();
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ TASK COMPLETE: FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Total wallclock time for task '" << tk->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Total wallclock time required by worker processes = " << format_time(total_wallclock_time);
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Total aggregation time required by master process = " << format_time(total_aggregation_time);
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Workers processed " << num_integrations << " individual integrations";
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Total integration time    = " << format_time(total_integration_time) << " | global mean integration time = " << format_time(total_integration_time/num_integrations);
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Min mean integration time = " << format_time(min_mean_integration_time) << " | global min integration time = " << format_time(global_min_integration_time);
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Max mean integration time = " << format_time(max_mean_integration_time) << " | global max integration time = " << format_time(global_max_integration_time);
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Total batching time       = " << format_time(total_batching_time) << " | global mean batching time = " << format_time(total_batching_time/num_integrations);
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Min mean batching time    = " << format_time(min_mean_batching_time) << " | global min batching time = " << format_time(global_min_batching_time);
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Max mean batching time    = " << format_time(max_mean_batching_time) << " | global max bathcing time = " << format_time(global_max_batching_time);
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
    void task_manager<number>::master_dispatch_output_task(output_task<number>* tk, model<number>* m, const std::list<std::string>& tags)
	    {
        assert(tk != nullptr);  // should be guaranteed
        assert(m == nullptr);   // should be guaranteed

		    // can't process a task if there are no workers
		    if(this->world.size() <= 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

		    // set up a work queue representing our workers
		    context ctx = this->make_workers_context();
		    scheduler sch = scheduler(ctx);

		    work_queue< output_task_element<number> > queue = sch.make_queue(*tk);

		    // set up an derived_content_writer object to coordinate logging, output destination and commits into the repository
		    typename repository<number>::derived_content_writer writer = this->repo->new_output_task_output(tk, tags, this->get_rank());

		    // set up the writer for us
        this->data_mgr->initialize_writer(writer);

		    // write a task distribution list -- subsequently read by the worker processes
		    // to find out which work items they should process
		    this->data_mgr->create_taskfile(writer, queue);

		    // instruct workers to carry out their tasks
		    this->master_output_task_to_workers(writer, tk, tags);

		    // close the writer
		    this->data_mgr->close_writer(writer);

		    // commit output to the database
		    writer.commit();
	    }


		template <typename number>
		void task_manager<number>::master_output_task_to_workers(typename repository<number>::derived_content_writer& writer,
		                                                         output_task<number>* tk, const std::list<std::string>& tags)
			{
				if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

		    std::vector<boost::mpi::request> requests(this->world.size()-1);

				BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++ NEW OUTPUT TASK '" << tk->get_name() << "'";
				BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << *tk;

		    // set up a timer to keep track of the total wallclock time used in this task
		    boost::timer::cpu_timer wallclock_timer;

		    // aggregate work times reported by worker processes
		    boost::timer::nanosecond_type total_work_time = 0;
		    boost::timer::nanosecond_type total_db_time = 0;
		    boost::timer::nanosecond_type total_aggregation_time = 0;

				// aggregate cache information
				unsigned int total_time_config_hits        = 0;
				unsigned int total_twopf_kconfig_hits      = 0;
				unsigned int total_threepf_kconfig_hits    = 0;
				unsigned int total_data_hits = 0;
		    unsigned int total_time_config_unloads     = 0;
		    unsigned int total_twopf_kconfig_unloads   = 0;
		    unsigned int total_threepf_kconfig_unloads = 0;
		    unsigned int total_data_unloads = 0;

		    // get paths the workers will need
		    assert(this->repo != nullptr);
		    boost::filesystem::path taskfile_path = writer.get_abs_taskfile_path();
		    boost::filesystem::path tempdir_path  = writer.get_abs_tempdir_path();
		    boost::filesystem::path logdir_path   = writer.get_abs_logdir_path();

		    MPI::new_derived_content_payload payload(tk->get_name(), taskfile_path, tempdir_path, logdir_path, tags);

		    for(unsigned int i = 0; i < this->world.size()-1; i++)
			    {
		        requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_DERIVED_CONTENT, payload);
			    }

		    // wait for all messages to be received
		    boost::mpi::wait_all(requests.begin(), requests.end());

		    BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++ All workers received NEW_DERIVED_CONTENT instruction";

		    // poll workers, receiving data until workers are exhausted
		    std::set<unsigned int> workers;
		    for(unsigned int i = 0; i < this->world.size()-1; i++) workers.insert(i);
		    while(workers.size() > 0)
			    {
		        BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++ Master polling for DERIVED_CONTENT_READY messages";
		        // wait until a message is available from a worker
		        boost::mpi::status stat = this->world.probe();

		        switch(stat.tag())
			        {
		            case MPI::DERIVED_CONTENT_READY:
			            {
		                MPI::content_ready_payload payload;
		                this->world.recv(stat.source(), MPI::DERIVED_CONTENT_READY, payload);
		                BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " sent content-ready notification";

		                break;
			            }

		            case MPI::FINISHED_DERIVED_CONTENT:
			            {
		                MPI::derived_finished_payload payload;
		                this->world.recv(stat.source(), MPI::FINISHED_DERIVED_CONTENT, payload);

		                BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising finished task in CPU time " << format_time(payload.get_cpu_time());

		                total_work_time               += payload.get_cpu_time();
				            total_db_time                 += payload.get_database_time();
				            total_time_config_hits        += payload.get_time_config_hits();
				            total_time_config_unloads     += payload.get_time_config_unloads();
				            total_twopf_kconfig_hits      += payload.get_twopf_kconfig_hits();
				            total_twopf_kconfig_unloads   += payload.get_twopf_kconfig_unloads();
				            total_threepf_kconfig_hits    += payload.get_threepf_kconfig_hits();
				            total_threepf_kconfig_unloads += payload.get_threepf_kconfig_unloads();
				            total_data_hits               += payload.get_data_hits();
				            total_data_unloads            += payload.get_data_unloads();

		                workers.erase(this->worker_number(stat.source()));
		                break;
			            }

		            default:
			            {
		                BOOST_LOG_SEV(writer.get_log(), repository<number>::warning) << "++ Master received unexpected message " << stat.tag() << " waiting in the queue";
		                break;
			            }
			        }
			    }

		    typename repository<number>::output_metadata timings(total_work_time, total_db_time, total_aggregation_time,
		                                                         total_time_config_hits, total_time_config_unloads,
		                                                         total_twopf_kconfig_hits, total_twopf_kconfig_unloads,
		                                                         total_threepf_kconfig_hits, total_threepf_kconfig_unloads,
		                                                         total_data_hits, total_data_unloads);
		    writer.set_metadata(timings);

        wallclock_timer.stop();
		    BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "";
				BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++ TASK COMPLETE -- FINAL USAGE STATISTICS";
		    BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++   Total wallclock time for task '" << tk->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
		    BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++     " << wallclock_timer.format();
				BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++ AGGREGATE CACHE STATISTICS";
		    BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++   Total work time required by worker processes      = " << format_time(total_work_time);
		    BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++   Total aggregation time required by master process = " << format_time(total_aggregation_time);
				BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++   Total time spent reading database                 = " << format_time(total_db_time);
				BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++   Total time-configuration cache hits               = " << total_time_config_hits << ", unloads = " << total_time_config_unloads;
				BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++   Total twopf k-config cache hits                   = " << total_twopf_kconfig_hits << ", unloads = " << total_twopf_kconfig_unloads;
				BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++   Total threepf k-config cache hits                 = " << total_threepf_kconfig_hits << ", unloads = " << total_threepf_kconfig_unloads;
				BOOST_LOG_SEV(writer.get_log(), repository<number>::normal) << "++   Total data cache hits                             = " << total_data_hits << ", unloads = " << total_data_unloads;
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
                                                    std::bind(&task_manager<number>::warn, this, std::placeholders::_1));
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
            model<number>* m = nullptr;
            task<number>* tk = this->repo->lookup_task(payload.get_task_name(), m, this->model_finder_factory());

            // dynamic_cast<> is a bit unsubtle, but we cannot predict in advance what type
            // of task will be returned
            if(dynamic_cast< threepf_task<number>* >(tk))
	            {
                std::set<unsigned int> work_items = this->data_mgr->read_taskfile(payload.get_taskfile_path(), this->worker_number());
                work_item_filter<threepf_kconfig> filter(work_items);

                threepf_task<number>* three_task = dynamic_cast< threepf_task<number>* >(tk);
                this->slave_dispatch_threepf_task(three_task, m, payload, filter);
	            }
            else if(dynamic_cast< twopf_task<number>* >(tk))
	            {
                std::set<unsigned int> work_items = this->data_mgr->read_taskfile(payload.get_taskfile_path(), this->worker_number());
                work_item_filter<twopf_kconfig> filter(work_items);

                twopf_task<number>* two_task = dynamic_cast< twopf_task<number>* >(tk);
                this->slave_dispatch_twopf_task(two_task, m, payload, filter);
	            }
            else
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << payload.get_task_name() << "'";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	            }

            delete tk;
	        }
        catch(runtime_exception xe)
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
	                << __CPP_TRANSPORT_REPO_MISSING_MODEL_B << " '" << payload.get_task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
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
    void task_manager<number>::slave_process_task(const MPI::new_derived_content_payload& payload)
	    {
		    // ensure that a valid repository object has been constructed
		    if(this->repo == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NOT_SET);

		    // extract our task from the database
        // much of this is boiler-plate which is similar to master_process_task()
        // TODO: it would be nice to make this sharing more explicit, so the code isn't just duplicated
        try
	        {
            model<number>* m = nullptr;
            task<number>* tk = this->repo->lookup_task(payload.get_task_name(), m, this->model_finder_factory());

            // dynamic_cast<> is a bit unsubtle, but we cannot predict in advance what type
            // of task will be returned
            if(dynamic_cast< output_task<number>* >(tk))
	            {
                std::set<unsigned int> work_items = this->data_mgr->read_taskfile(payload.get_taskfile_path(), this->worker_number());
                work_item_filter< output_task_element<number> > filter(work_items);

                output_task<number>* out_task = dynamic_cast< output_task<number>* >(tk);
                this->slave_dispatch_output_task(out_task, m, payload, filter);
	            }
            else
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << payload.get_task_name() << "'";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
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
	                << __CPP_TRANSPORT_REPO_MISSING_MODEL_B << " '" << payload.get_task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
	            }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE)
	            {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << payload.get_task_name() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
	            }
            else if(xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
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
    void task_manager<number>::slave_dispatch_twopf_task(twopf_task<number>* tk, model<number>* m,
                                                         const MPI::new_integration_payload& payload,
                                                         const work_item_filter<twopf_kconfig>& filter)
      {
        // dispatch integration to the underlying model
        assert(tk != nullptr);  // should be guaranteed
        assert(m != nullptr);   // should be guaranteed

        // keep track of wallclock time
        boost::timer::cpu_timer timer;

        // create queues based on whatever devices are relevant for the backend
        context                   ctx  = m->backend_get_context();
        scheduler                 sch  = scheduler(ctx);
        work_queue<twopf_kconfig> work = sch.make_queue(m->backend_twopf_state_size(), *tk, filter);

        // construct a callback for the integrator to push new batches to the master
        typename data_manager<number>::container_dispatch_function dispatcher =
                                                                     std::bind(&task_manager<number>::slave_push_temp_container,
                                                                               this, std::placeholders::_1, MPI::data_ready_payload::twopf_payload);

        // construct a batcher to hold the output of the integration
        typename data_manager<number>::twopf_batcher batcher =
                                                       this->data_mgr->create_temp_twopf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
                                                                                                   this->get_rank(), m, dispatcher);

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW INTEGRATION TASK '" << tk->get_name() << "'" << std::endl;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

        // perform the integration
        m->backend_process_twopf(work, tk, batcher, true);    // 'true' = work silently

        // close the batcher
        batcher.close();

        // all work is now done - stop the wallclock timer
        timer.stop();

        // notify master process that all work has been finished (temporary containers will be deleted by the master node)
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- Worker sending FINISHED_INTEGRATION to master";
        MPI::finished_integration_payload            outgoing_payload(batcher.get_integration_time(),
                                                                      batcher.get_max_integration_time(), batcher.get_min_integration_time(),
                                                                      batcher.get_batching_time(),
                                                                      batcher.get_max_batching_time(), batcher.get_min_batching_time(),
                                                                      timer.elapsed().wall,
                                                                      batcher.get_reported_integrations());
        this->world.isend(MPI::RANK_MASTER, MPI::FINISHED_INTEGRATION, outgoing_payload);
      }


    template <typename number>
    void task_manager<number>::slave_dispatch_threepf_task(threepf_task<number>* tk, model<number>* m,
                                                           const MPI::new_integration_payload& payload,
                                                           const work_item_filter<threepf_kconfig>& filter)
      {
        // dispatch integration to the underlying model
        assert(tk != nullptr);  // should be guaranteed
        assert(m != nullptr);   // should be guaranteed

        // keep track of wallclock time
        boost::timer::cpu_timer timer;

        // create queues based on whatever devices are relevant for the backend
        context                     ctx  = m->backend_get_context();
        scheduler                   sch  = scheduler(ctx);
        work_queue<threepf_kconfig> work = sch.make_queue(m->backend_threepf_state_size(), *tk, filter);

        // construct a callback for the integrator to push new batches to the master
        typename data_manager<number>::container_dispatch_function dispatcher =
	                                                                   std::bind(&task_manager<number>::slave_push_temp_container,
	                                                                             this, std::placeholders::_1, MPI::data_ready_payload::threepf_payload);

        // construct a batcher to hold the output of the integration
        typename data_manager<number>::threepf_batcher batcher =
	                                                       this->data_mgr->create_temp_threepf_container(payload.get_tempdir_path(), payload.get_logdir_path(),
	                                                                                                     this->get_rank(), m, dispatcher);

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW INTEGRATION TASK '" << tk->get_name() << "'" << std::endl;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

        // perform the integration
        m->backend_process_threepf(work, tk, batcher, true);    // 'true' = work silently

        // close the batcher
        batcher.close();

        // all work is now done - stop the wallclock timer
        timer.stop();

        // notify master process that all work has been finished (temporary containers will be deleted by the master node)
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << std::endl << "-- Worker sending FINISHED_INTEGRATION to master";
        MPI::finished_integration_payload outgoing_payload(batcher.get_integration_time(),
                                                           batcher.get_max_integration_time(), batcher.get_min_integration_time(),
                                                           batcher.get_batching_time(),
                                                           batcher.get_max_batching_time(), batcher.get_min_batching_time(),
                                                           timer.elapsed().wall,
                                                           batcher.get_reported_integrations());
        this->world.isend(MPI::RANK_MASTER, MPI::FINISHED_INTEGRATION, outgoing_payload);
      }


		template <typename number>
		void task_manager<number>::slave_dispatch_output_task(output_task<number>* tk, model<number>* m,
																												  const MPI::new_derived_content_payload& payload,
																												  const work_item_filter< output_task_element<number> >& filter)
			{
        assert(tk != nullptr);  // should be guaranteed
        assert(m == nullptr);   // should be guaranteed

				// keep track of CPU time
		    boost::timer::cpu_timer timer;

        // set up output-group finder function
        typename data_manager<number>::output_group_finder finder =
                                                             std::bind(&repository<number>::find_integration_task_output_group, this->repo, std::placeholders::_1, std::placeholders::_2);

        // set up content-dispatch function
        typename data_manager<number>::derived_content_dispatch_function dispatcher =
                                                                           std::bind(&task_manager<number>::slave_push_derived_content, this, std::placeholders::_1);

		    // acquire a datapipe which we can use to stream content from the databse
		    typename data_manager<number>::datapipe pipe = this->data_mgr->create_datapipe(payload.get_logdir_path(), payload.get_tempdir_path(),
                                                                                       finder, dispatcher,
		                                                                                   this->get_rank(), timer);

				BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << "-- NEW OUTPUT TASK '" << tk->get_name() << "'" << std::endl;
				BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << *tk;

		    // create a context and queue
				context ctx = context();
				ctx.add_device("CPU");
				scheduler sch = scheduler(ctx);
				work_queue< output_task_element<number> > work = sch.make_queue(*tk, filter);

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

            product->derive(pipe, task_tags);

            // check that the datapipe was correctly detached
            if(pipe.is_attached())
              {
                BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::error) << "-- Task manager detected that datapipe was not correctly detached after generating derived product '" << product->get_name() << "'";
                pipe.detach();
              }

            BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << "";
					}

				// close the datapipe
				pipe.close();

				// all work now done - stop the timer
				timer.stop();

				// notify master process that all work has been finished
				BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << "-- Worker sending FINISHED_DERIVED_CONTENT to master";

		    MPI::derived_finished_payload finish_payload(pipe.get_database_time(), timer.elapsed().wall,
		                                                 pipe.get_time_config_cache_hits(), pipe.get_time_config_cache_unloads(),
		                                                 pipe.get_twopf_kconfig_cache_hits(), pipe.get_twopf_kconfig_cache_unloads(),
		                                                 pipe.get_threepf_kconfig_cache_hits(), pipe.get_threepf_kconfig_cache_unloads(),
		                                                 pipe.get_data_cache_hits(), pipe.get_data_cache_unloads());
		    this->world.isend(MPI::RANK_MASTER, MPI::FINISHED_DERIVED_CONTENT, finish_payload);
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
    void task_manager<number>::slave_push_temp_container(typename data_manager<number>::generic_batcher* batcher,
                                                         MPI::data_ready_payload::payload_type type)
      {
        BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal) << "-- Sending INTEGRATION_DATA_READY message for container " << batcher->get_container_path();

        MPI::data_ready_payload payload(batcher->get_container_path().string(), type);

        // advise master process that data is available in the named container
        this->world.isend(MPI::RANK_MASTER, MPI::INTEGRATION_DATA_READY, payload);
      }


    template <typename number>
    void task_manager<number>::slave_push_derived_content(typename data_manager<number>::datapipe* pipe)
	    {
        assert(pipe != nullptr);

        // FIXME: error message tag is possibly in the wrong namespace (but error message namespaces are totally confused anyway)
        if(pipe == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE);
	    }


  } // namespace transport


#endif //__task_manager_H_
