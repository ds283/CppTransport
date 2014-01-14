//
// Created by David Seery on 24/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __task_manager_H_
#define __task_manager_H_


#include <list>
#include <set>
#include <vector>

#include "transport/models/model.h"
#include "transport/manager/instance_manager.h"
#include "transport/tasks/task.h"

#include "transport/manager/repository.h"
#include "transport/manager/data_manager.h"

#include "transport/scheduler/context.h"
#include "transport/scheduler/scheduler.h"
#include "transport/scheduler/work_queue.h"

#include "transport/utilities/formatter.h"

#include "transport/messages_en.h"
#include "transport/exceptions.h"

#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/timer/timer.hpp"


#define __CPP_TRANSPORT_SWITCH_REPO     "-repo"
#define __CPP_TRANSPORT_SWITCH_TASK     "-task"

// name for worker devices
#define __CPP_TRANSPORT_WORKER_NAME     "mpi-worker-"

// default storage limit on nodes - 100 Mb
#define __CPP_TRANSPORT_DEFAULT_STORAGE (100*1024*1024)


namespace transport
  {

    namespace MPI
      {

        // MPI messages
        namespace
          {

            // MPI message tags
            const unsigned int NEW_INTEGRATION = 0;
            const unsigned int FINISHED_TASK = 1;
            const unsigned int DATA_READY = 2;
            const unsigned int DELETE_CONTAINER = 3;
            const unsigned int SET_REPOSITORY = 98;
            const unsigned int TERMINATE = 99;

            // MPI ranks
            const unsigned int RANK_MASTER = 0;

            // MPI message payloads

            class set_repository_payload
              {
              public:
                //! Null constructor (used for receiving messages)
                set_repository_payload()
                  {
                  }

                //! Value constructor (used for constructing messages to send)
                set_repository_payload(const boost::filesystem::path& rp)
                  : repository(rp.string())
                  {
                  }

                boost::filesystem::path repository_path() const { return(boost::filesystem::path(this->repository)); }

              private:
                //! Pathname to repository
                std::string repository;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & repository;
                  }
              };


            class new_integration_payload
              {
              public:
                //! Null constructor (used for receiving messages)
                new_integration_payload()
                  {
                  }

                //! Value constructor (used for constructing messages to send)
                new_integration_payload(const boost::filesystem::path& tk,
                                        const boost::filesystem::path& tk_f,
                                        const boost::filesystem::path& tmp_d,
                                        const boost::filesystem::path& log_d)
                  : task(tk.string()), taskfile(tk_f.string()), tempdir(tmp_d.string()), logdir(log_d.string())
                  {
                  }

                const std::string& task_name()          const { return(this->task); }
                boost::filesystem::path taskfile_path() const { return(boost::filesystem::path(this->taskfile)); }
                boost::filesystem::path tempdir_path()  const { return(boost::filesystem::path(this->tempdir)); }
                boost::filesystem::path logdir_path()   const { return(boost::filesystem::path(this->logdir)); }

              private:
                //! Name of task, to be looked up in repository database
                std::string task;

                //! Pathname to taskfile
                std::string taskfile;

                //! Pathname to directory for temporary files
                std::string tempdir;

                //! Pathname to directory for log files
                std::string logdir;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & task;
                    ar & taskfile;
                    ar & tempdir;
                    ar & logdir;
                  }
              };


            class data_ready_payload
              {
              public:

                typedef enum { twopf_payload, threepf_payload } payload_type;

                //! Null constructor (used for receiving messages)
                data_ready_payload()
                  {
                  }

                //! Value constructor (used for sending messages)
                data_ready_payload(const std::string& p, payload_type t)
                  : container_path(p), payload(t)
                  {
                  }

                //! Get container path
                const std::string& get_container_path() const { return(this->container_path); }

                //! Get payload type
                payload_type get_payload_type() const { return(this->payload); }

              private:
                //! Path to container
                std::string container_path;

                //! Type of container
                payload_type payload;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & container_path;
                    ar & payload;
                  }
              };


          }   // unnamed namespace

      }   // namespace MPI


    class work_item_filter: public abstract_filter
      {
      public:
        //! Construct an empty filter
        work_item_filter()
          {
          }

        work_item_filter(const std::set<unsigned int>& filter_set)
        : items(filter_set)
          {
          }

        //! Add an item to the list of work-items the filter allows
        void add_work_item(unsigned int serial) { this->items.insert(serial); }

        //! Check whether a work-item is part of the filter
        bool filter(const twopf_kconfig& config)   const { return(this->items.find(config.serial) != this->items.end()); }
        bool filter(const threepf_kconfig& config) const { return(this->items.find(config.serial) != this->items.end()); }

        friend std::ostream& operator<<(std::ostream& out, const work_item_filter& filter);

      private:
        //! std::set holding work items that we are supposed to process
        std::set<unsigned int> items;
      };


    std::ostream& operator<<(std::ostream& out, const work_item_filter& filter)
      {
        std::cerr << __CPP_TRANSPORT_FILTER_TAG;
        for(std::set<unsigned int>::iterator t = filter.items.begin(); t != filter.items.end(); t++)
          {
            std::cerr << (t != filter.items.begin() ? ", " : " ") << *t;
          }
        std::cerr << std::endl;
        return(out);
      }


    template <typename number>
    class task_manager : public instance_manager<number>
      {
      public:
        typedef enum { job_task } job_type;


        class job_descriptor
          {
          public:
            job_type    type;
            std::string name;
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
        //! Write a model/initial conditions/parameters combination (a 'package') to the repository
        void write_package(const initial_conditions<number>& ics, const model<number>* m);

        //! Write a twopf integration task to the repository
        void write_integration(const twopf_task<number>& t, const model<number>* m);
        //! Write a threepf integration task to the repository
        void write_integration(const threepf_task<number>& t, const model<number>* m);


        // INTERFACE -- MASTER-SLAVE API

      public:
        //! Query whether we are the master process
        bool is_master(void) const { return(this->world.rank() == MPI::RANK_MASTER); }

        //! Return rank of this process
        unsigned int get_rank(void) const { return(this->world.rank()); }

        //! If we are the master process, execute any queued tasks
        void execute_tasks(void);

        //! If we are a slave process, poll for instructions to perform work
        void wait_for_tasks(void);

      protected:
        //! Master node: Process a 'task' job -- tasks are integrations, so this dispatches to the appropriate integrator
        void master_process_task(const job_descriptor& job);

        //! Master node: Dispatch a twopf 'task' (ie., integration) to the worker processes
        void master_dispatch_twopf_task(twopf_task<number>* tk, model<number>* m);

        //! Master node: Dispatch a threepf 'task' (ie., integration) to the worker processes
        void master_dispatch_threepf_task(threepf_task<number>* tk, model<number>* m);

        //! Master node: Terminate all worker processes
        void master_terminate_workers(void);

        //! Master node: Push repository information to worker processes
        void master_push_repository(void);

        //! Master node: Pass new integration task to the workers
        void master_task_to_workers(typename repository<number>::integration_container& ctr, task<number>* task_name, model<number>* m);

        //! Slave node: Process a new task instruction
        void slave_process_task(const MPI::new_integration_payload& payload);

        //! Slave node: Process a twopf task
        void slave_dispatch_twopf_task(twopf_task<number>* tk, model<number>* m, const MPI::new_integration_payload& payload, const work_item_filter& filter);

        //! Slave node: Process a threepf task
        void slave_dispatch_threepf_task(threepf_task<number>* tk, model<number>* m, const MPI::new_integration_payload& payload, const work_item_filter& filter);

        //! Slave node: set repository
        void slave_set_repository(const MPI::set_repository_payload& payload);

        //! Slave node: wait until an instruction has been received to delete all containers
        void slave_wait_temp_containers_deleted(const boost::timer::cpu_times& elapsed, typename data_manager<number>::generic_batcher& batcher);

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

        //! Get worker number
        unsigned int worker_number() { return(this->world.rank()-1); }


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
                            repo = repository_factory<number>(repo_path, this->get_rank() == MPI::RANK_MASTER);   // run recovery if we are the master node
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
                else if (static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_TASK)
                  {
                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_TASK_ID);
                    else
                      {
                        ++i;
                        job_descriptor desc;
                        desc.type = job_task;
                        desc.name = argv[i];
                        job_queue.push_back(desc);
                      }
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
    void task_manager<number>::write_package(const initial_conditions<number>& ics, const model<number>* m)
      {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_REPO_WRITE_SLAVE);

        assert(this->repo != nullptr);

        if(this->repo == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NOT_SET);

        this->repo->write_package(ics, m);
      }


    template <typename number>
    void task_manager<number>::write_integration(const twopf_task<number>& t, const model<number>* m)
      {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_REPO_WRITE_SLAVE);

        assert(this->repo != nullptr);
        assert(m != nullptr);

        if(this->repo == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NOT_SET);
        if(m          == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        this->repo->write_integration(t, m);
      }


    template <typename number>
    void task_manager<number>::write_integration(const threepf_task<number>& t, const model<number>* m)
      {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_REPO_WRITE_SLAVE);

        assert(this->repo != nullptr);
        assert(m != nullptr);

        if(this->repo == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NOT_SET);
        if(m          == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        this->repo->write_integration(t, m);
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
            model<number>* m = nullptr;
            task<number>* tk = this->repo->query_task(job.name, m, this->model_finder_factory());
            assert(m != nullptr);

            // set up work queues for this task, and then distribute to worker processes

            // dynamic_cast<> is a bit unsubtle, but we cannot predict in advance what type
            // of task will be returned
            if(dynamic_cast< threepf_task<number>* >(tk))
              {
                threepf_task<number>* three_task = dynamic_cast< threepf_task<number>* >(tk);
                this->master_dispatch_threepf_task(three_task, m);
              }
            else if(dynamic_cast< twopf_task<number>* >(tk))
              {
                twopf_task<number>* two_task = dynamic_cast< twopf_task<number>* >(tk);
                this->master_dispatch_twopf_task(two_task, m);
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
            else if(xe.get_exception_code() == runtime_exception::BADLY_FORMED_XML)
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
    void task_manager<number>::master_dispatch_twopf_task(twopf_task<number>* tk, model<number>* m)
      {
        // set up a work queue representing our workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        work_queue<twopf_kconfig> queue = sch.make_queue(m->backend_twopf_state_size(), *tk);

        // create new output record in the repository XML database, and set up
        // paths to the integration SQL database
        typename repository<number>::integration_container ctr = this->repo->integration_new_output(tk, this->worker_number());

        // create the data container
        this->data_mgr->create_container(ctr);

        // write the task distribution list -- this is subsequently read by the worker processes,
        // to find out which work items they should process
        this->data_mgr->create_taskfile(ctr, queue);

        // write the various tables needed in the database
        this->data_mgr->create_tables(ctr, tk, m->get_N_fields());

        // instruct workers to carry out the calculation
        // this call returns when all workers have signalled that their work is done
        this->master_task_to_workers(ctr, tk, m);

        // close the data container
        this->data_mgr->close_container(ctr);
      }


    template <typename number>
    void task_manager<number>::master_dispatch_threepf_task(threepf_task<number>* tk, model<number>* m)
      {
        // set up a work queue representing our workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        work_queue<threepf_kconfig> queue = sch.make_queue(m->backend_threepf_state_size(), *tk);

        // create new output record in the repository XML database, and set up
        // paths to the integration SQL database
        typename repository<number>::integration_container ctr = this->repo->integration_new_output(tk, this->get_rank());

        // create the data container
        this->data_mgr->create_container(ctr);

        // write the task distribution list -- this is subsequently read by the worker processes,
        // to find out which work items they should process
        this->data_mgr->create_taskfile(ctr, queue);

        // create the various tables needed in the database
        this->data_mgr->create_tables(ctr, tk, m->get_N_fields());

        // instruct workers to carry out the calculation
        // this call returns when all workers have signalled that their work is done
        this->master_task_to_workers(ctr, tk, m);

        // close the data container
        this->data_mgr->close_container(ctr);
      }


    template <typename number>
    void task_manager<number>::master_task_to_workers(typename repository<number>::integration_container& ctr,
                                                      task<number>* tk, model<number>* m)
      {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ NEW INTEGRATION TASK '" << tk->get_name() << "'";
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << *tk;

        // set up a timer to keep track of the total wallclock time used in this integration
        boost::timer::cpu_timer wallclock_timer;
        // aggregate cpu times reported by worker processes
        boost::timer::nanosecond_type total_cpu_time = 0;

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

                    // instruct worker to remove the temporary container
                    this->world.isend(stat.source(), MPI::DELETE_CONTAINER, payload.get_container_path());
                    break;
                  }

                case MPI::FINISHED_TASK:
                  {
                    boost::timer::nanosecond_type cpu_time = 0;
                    this->world.recv(stat.source(), MPI::FINISHED_TASK, cpu_time);
                    BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising finished task in CPU time " << format_time(cpu_time);

                    total_cpu_time += cpu_time;
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
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Total wallclock time for task '" << tk->get_name() << "'" << wallclock_timer.format();
        BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "++ Total CPU time required by worker processes = " << format_time(total_cpu_time);
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
            this->repo = repository_factory<number>(repo_path.string());
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
            model<number>* m = nullptr;
            task<number>* tk = this->repo->query_task(payload.task_name(), m, this->model_finder_factory());
            assert(m != nullptr);

            // dynamic_cast<> is a bit unsubtle, but we cannot predict in advance what type
            // of task will be returned
            if(dynamic_cast< threepf_task<number>* >(tk))
              {
                threepf_task<number>* three_task = dynamic_cast< threepf_task<number>* >(tk);
                this->slave_dispatch_threepf_task(three_task, m, payload, filter);
              }
            else if(dynamic_cast< twopf_task<number>* >(tk))
              {
                twopf_task<number>* two_task = dynamic_cast< twopf_task<number>* >(tk);
                this->slave_dispatch_twopf_task(two_task, m, payload, filter);
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
            else if(xe.get_exception_code() == runtime_exception::BADLY_FORMED_XML)
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
    void task_manager<number>::slave_dispatch_twopf_task(twopf_task<number>* tk, model<number>* m,
                                                         const MPI::new_integration_payload& payload,
                                                         const work_item_filter& filter)
      {
        // dispatch integration to the underlying model

        // keep track of CPU time
        boost::timer::cpu_timer timer;

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
                                                                                                   this->get_rank(), m->get_N_fields(), dispatcher);

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW INTEGRATION TASK '" << tk->get_name() << "'";
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

        // perform the integration
        m->backend_process_twopf(work, tk, batcher, true);    // 'true' = work silently

        // close the batcher
        batcher.close();

        // all work is now done - stop the timer
        timer.stop();

        // wait until all temporary containers have been deleted
        this->slave_wait_temp_containers_deleted(timer.elapsed(), batcher);
      }


    template <typename number>
    void task_manager<number>::slave_dispatch_threepf_task(threepf_task<number>* tk, model<number>* m,
                                                           const MPI::new_integration_payload& payload,
                                                           const work_item_filter& filter)
      {
        // dispatch integration to the underlying model

        // keep track of CPU time
        boost::timer::cpu_timer timer;

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
                                                                                                       this->get_rank(), m->get_N_fields(), dispatcher);

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- NEW INTEGRATION TASK '" << tk->get_name() << "'";
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *tk;

        // perform the integration
        m->backend_process_threepf(work, tk, batcher, true);    // 'true' = work silently

        // close the batcher
        batcher.close();

        // all work is now done - stop the timer
        timer.stop();

        // wait until all temporary containers have been deleted, and then return
        this->slave_wait_temp_containers_deleted(timer.elapsed(), batcher);
      }


    template <typename number>
    void task_manager<number>::slave_wait_temp_containers_deleted(const boost::timer::cpu_times& elapsed,
                                                                  typename data_manager<number>::generic_batcher& batcher)
      {
        while(this->temporary_container_queue.size() > 0)
          {
            BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- Slave waiting for DELETE_CONTAINER instructions";
            // wait until a message is available from master
            boost::mpi::status stat = this->world.probe(MPI::RANK_MASTER);

            switch(stat.tag())
              {
                case MPI::DELETE_CONTAINER:
                  {
                    std::string payload;
                    this->world.recv(MPI::RANK_MASTER, MPI::DELETE_CONTAINER, payload);
                    BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- Slave received delete instruction: " << payload;

                    if(!boost::filesystem::remove(payload))
                      {
                        std::ostringstream msg;
                        msg << __CPP_TRANSPORT_DATACTR_REMOVE_TEMP << " '" << payload << "'";
                        this->error(msg.str());
                      }
                    this->temporary_container_queue.remove(payload);
                    break;
                  }
              }
          }
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "-- Slave sending FINISHED_TASK to master";
        this->world.isend(MPI::RANK_MASTER, MPI::FINISHED_TASK, elapsed.user);
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
      }


  } // namespace transport


#endif //__task_manager_H_
