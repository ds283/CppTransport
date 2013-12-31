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

        // REPOSITORY-MANAGEMENT API

        //! Write a model/initial conditions/parameters combination to the repository
        void write_model(const initial_conditions<number>& ics, const model<number>* m);

        //! Write an integration task to the repository
        void write_integration(const twopf_task<number>& t, const model<number>* m);
        void write_integration(const threepf_task<number>& t, const model<number>* m);

        // MASTER-SLAVE API

        // query whether we are the master process
        bool is_master(void) const { return(this->world.rank() == __CPP_TRANSPORT_RANK_MASTER); }

        // if we are a slave process, poll for instructions to perform work
        void wait_for_tasks(void);

        // INTERNAL DATA

      protected:
        boost::mpi::environment environment;
        boost::mpi::communicator world;

        repository<number>* repo;
      };


    template <typename number>
    task_manager<number>::task_manager(int argc, char* argv[])
      : instance_manager<number>(), environment(argc, argv), repo(nullptr)
      {
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


  } // namespace transport


#endif //__task_manager_H_
