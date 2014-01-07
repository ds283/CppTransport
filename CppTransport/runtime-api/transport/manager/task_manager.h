//
// Created by David Seery on 24/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __task_manager_H_
#define __task_manager_H_


#include <list>
#include <stdexcept>

#include "boost/mpi.hpp"

#include "transport/models/model.h"
#include "transport/manager/instance_manager.h"
#include "transport/tasks/task.h"
#include "transport/db-xml/repository.h"

#include "transport/scheduler/context.h"
#include "transport/scheduler/scheduler.h"
#include "transport/scheduler/work_queue.h"

#include "transport/messages_en.h"
#include "transport/exceptions.h"


#define __CPP_TRANSPORT_SWITCH_REPO     "-repo"
#define __CPP_TRANSPORT_SWITCH_TASK     "-task"
#define __CPP_TRANSPORT_SWITCH_RECOVER  "-recovery"

// name for worker devices
#define __CPP_TRANSPORT_WORKER_NAME     "mpi-worker-"


namespace transport
  {

    namespace MPI
      {

        // MPI messages
        namespace
          {

            const unsigned int NEW_TASK = 0;
            const unsigned int FINISHED_TASK = 1;
            const unsigned int TERMINATE = 99;

            const unsigned int RANK_MASTER = 0;

            class new_task_payload
              {
                std::string repo;
                std::string task;
              };

          }   // unnamed namespace

      }   // namespace MPI


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
        //! Construct a task manager using command-line arguments.
        //! The repository must exist and be named on the command line.
        task_manager(int argc, char* argv[]);

        //! Construct a task manager using a previously-constructed repository object.
        //! Usually this will be used only when creating a new repository.
        task_manager(int argc, char* argv[], repository<number>* r);

        //! Destroy a task manager.
        ~task_manager();

        // INTERFACE -- REPOSITORY MANAGEMENT

      public:
        //! Write a model/initial conditions/parameters combination to the repository
        void write_model(const initial_conditions<number>& ics, const model<number>* m);

        //! Write an integration task to the repository
        void write_integration(const twopf_task<number>& t, const model<number>* m);
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
        //! Process a 'task' job -- tasks are integrations, so this dispatches to the appropriate integrator
        void process_task(const job_descriptor& job);

        //! Dispatch a twopf 'task' (ie., integration) to the worker processes
        void dispatch_twopf_task(twopf_task<number>* tk, model<number>* m);

        //! Dispatch a threepf 'task' (ie., integration) to the worker processes
        void dispatch_threepf_task(threepf_task<number>* tk, model<number>* m);

        //! Make a 'device context' for the MPI worker processes
        context make_workers_context(void);

        //! Terminate all worker processes
        void terminate_workers(void);

        //! Map worker number to communicator rank
        constexpr unsigned int worker_rank(unsigned int worker_number) { return(worker_number+1); }

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

        //! Repository object
        repository<number>* repo;

        //! Queue of tasks to process
        std::list<job_descriptor> job_queue;
      };


    template <typename number>
    task_manager<number>::task_manager(int argc, char* argv[])
      : instance_manager<number>(), environment(argc, argv), repo(nullptr)
      {
        if(world.rank() == MPI::RANK_MASTER)
          {
            bool multiple_repo_warn = false;
            bool recovery = false;

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
                            repo = new repository<number>(repo_path, recovery);
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
                else if (static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_RECOVER)
                  {
                    if(this->repo != nullptr) this->warn(__CPP_TRANSPORT_RECOVER_NOEFFECT);
                    else recovery = true;
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
    task_manager<number>::task_manager(int argc, char* argv[], repository<number>* r)
      : instance_manager<number>(), environment(argc, argv), repo(r)
      {
        assert(repo != nullptr);
      }


    template <typename number>
    task_manager<number>::~task_manager()
      {
        delete this->repo;
      }


    template <typename number>
    void task_manager<number>::write_model(const initial_conditions<number>& ics, const model<number>* m)
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
        if(this->m    == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

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


    template <typename number>
    void task_manager<number>::execute_tasks()
      {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        if(this->repo == nullptr)
          {
            this->error(__CPP_TRANSPORT_REPO_NONE);
          }
        else
          {
            for(typename std::list<job_descriptor>::const_iterator t = this->job_queue.begin(); t != this->job_queue.end(); t++)
              {
                switch((*t).type)
                  {
                    case job_task:
                      this->process_task(*t);
                      break;

                    default:
                      throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_UNKNOWN_JOB_TYPE);
                  }
              }
          }

        // there is no more work, so ask all workers to shut down
        // and then exit ourselves
        this->terminate_workers();
      }


    template <typename number>
    void task_manager<number>::process_task(const job_descriptor& job)
      {
        try
          {
            model<number>* m = nullptr;
            task<number>* tk = this->repo->query_task(job.name, m, this->model_finder_factory());
            assert(m != nullptr);

            // set up work queues for this task, and then distributed to worker processes

            // dynamic_cast<> is a bit unsubtle, but we cannot predict in advance what type
            // of task will be returned
            if(dynamic_cast< threepf_task<number>* >(tk))
              {
                threepf_task<number>* three_task = dynamic_cast< threepf_task<number>* >(tk);
                this->dispatch_threepf_task(three_task, m);
              }
            else if(dynamic_cast< twopf_task<number>* >(tk))
              {
                twopf_task<number>* two_task = dynamic_cast< twopf_task<number>* >(tk);
                this->dispatch_twopf_task(two_task, m);
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
    void task_manager<number>::terminate_workers()
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
                case MPI::NEW_TASK:
                  std::cerr << "New task message for worker " << this->world.rank() << std::endl;
                  // receive the message
                  this->world.recv(MPI::RANK_MASTER, MPI::NEW_TASK);
                  break;

                case MPI::TERMINATE:
                  std::cerr << "worker " << this->world.rank() << " terminating" << std::endl;
                  // receive the message
                  this->world.recv(MPI::RANK_MASTER, MPI::TERMINATE);
                  finished = true;
                  break;

                default:
                  throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_UNEXPECTED_MPI);
              }
          }
      }


    template <typename number>
    void task_manager<number>::dispatch_twopf_task(twopf_task<number>* tk, model<number>* m)
      {
        // set up a work queue representing our workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        work_queue<twopf_kconfig> queue = sch.make_queue(m->backend_twopf_state_size(), *tk);
      }


    template <typename number>
    void task_manager<number>::dispatch_threepf_task(threepf_task<number>* tk, model<number>* m)
      {
        // set up a work queue representing our workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        work_queue<threepf_kconfig> queue = sch.make_queue(m->backend_twopf_state_size(), *tk);

        std::cout << queue;
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


  } // namespace transport


#endif //__task_manager_H_
