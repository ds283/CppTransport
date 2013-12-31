//
// Created by David Seery on 24/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __task_manager_H_
#define __task_manager_H_


#include <list>
#include <stdexcept>

#include "boost/mpi/environment.hpp"
#include "boost/mpi/communicator.hpp"

#include "transport/models/model.h"
#include "transport/manager/instance_manager.h"
#include "transport/db-xml/repository.h"
#include "transport/messages_en.h"


#define __CPP_TRANSPORT_RANK_MASTER (0)


#define __CPP_TRANSPORT_SWITCH_REPO "-repo"
#define __CPP_TRANSPORT_SWITCH_TASK "-task"


namespace transport
  {

    template <typename number>
    class task_manager : public instance_manager<number>
      {
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

        // INTERFACE -- REPOSITORY-MANAGEMENT

        //! Write a model/initial conditions/parameters combination to the repository
        void write_model(const initial_conditions<number>& ics, const model<number>* m);

        //! Write an integration task to the repository
        void write_integration(const twopf_task<number>& t, const model<number>* m);
        void write_integration(const threepf_task<number>& t, const model<number>* m);

        // INTERFACE -- MASTER-SLAVE API

        //! Query whether we are the master process
        bool is_master(void) const { return(this->world.rank() == __CPP_TRANSPORT_RANK_MASTER); }

        //! If we are the master process, execute any queued tasks
        void execute_tasks(void);

        //! If we are a slave process, poll for instructions to perform work
        void wait_for_tasks(void);

        // INTERFACE -- ERROR REPORTING

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
        std::list<std::string> task_queue;
      };


    template <typename number>
    task_manager<number>::task_manager(int argc, char* argv[])
      : instance_manager<number>(), environment(argc, argv), repo(nullptr)
      {
        if(world.rank() == __CPP_TRANSPORT_RANK_MASTER)
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
                        repo = new repository<number>(repo_path);
                      }
                  }
                else if (static_cast<std::string>(argv[i]) == __CPP_TRANSPORT_SWITCH_TASK)
                  {
                    if(i+1 >= argc) this->error(__CPP_TRANSPORT_EXPECTED_TASK_ID);
                    else
                      {
                        ++i;
                        task_queue.push_back(static_cast<std::string>(argv[i]));
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
        assert(this->repo != nullptr);

        if(this->repo != nullptr)
          {
            if(!this->is_master()) throw std::runtime_error(__CPP_TRANSPORT_REPO_WRITE_SLAVE);
            this->repo->write_model(ics, m);
          }
        else
          {
            throw std::runtime_error(__CPP_TRANSPORT_REPO_NOT_SET);
          }
      }


    template <typename number>
    void task_manager<number>::write_integration(const twopf_task<number>& t, const model<number>* m)
      {
        assert(this->repo != nullptr);
        assert(m != nullptr);

        if(this->repo == nullptr) throw std::runtime_error(__CPP_TRANSPORT_REPO_NOT_SET);
        if(this->m    == nullptr) throw std::runtime_error(__CPP_TRANSPORT_REPO_NULL_MODEL);

        if(!this->is_master()) throw std::runtime_error(__CPP_TRANSPORT_REPO_WRITE_SLAVE);
        this->repo->write_integration(t, m);
      }


    template <typename number>
    void task_manager<number>::write_integration(const threepf_task<number>& t, const model<number>* m)
      {
        assert(this->repo != nullptr);
        assert(m != nullptr);

        if(this->repo == nullptr) throw std::runtime_error(__CPP_TRANSPORT_REPO_NOT_SET);
        if(m          == nullptr) throw std::runtime_error(__CPP_TRANSPORT_REPO_NULL_MODEL);

        if(!this->is_master()) throw std::runtime_error(__CPP_TRANSPORT_REPO_WRITE_SLAVE);
        this->repo->write_integration(t, m);
      }


    template <typename number>
    void task_manager<number>::execute_tasks()
      {
        for(std::list<std::string>::const_iterator t = this->task_queue.begin(); t != this->task_queue.end(); t++)
          {
            std::cerr << "Task: name = " << *t << std::endl;
          }
      }


    template <typename number>
    void task_manager<number>::wait_for_tasks()
      {

      }


  } // namespace transport


#endif //__task_manager_H_
