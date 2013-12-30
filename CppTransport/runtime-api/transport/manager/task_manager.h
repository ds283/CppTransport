//
// Created by David Seery on 24/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __task_manager_H_
#define __task_manager_H_


#include <list>

#include "boost/mpi/environment.hpp"
#include "boost/mpi/communicator.hpp"

#include "transport/models/model.h"
#include "transport/manager/instance_manager.h"


namespace transport
  {

    template <typename number>
    class task_manager : public instance_manager<number>
      {
        // CONSTRUCTOR, DESTRUCTOR

      public:
        task_manager(int argc, char* argv[]);
        ~task_manager();

        // MASTER-SLAVE API

        // query whether we are the master process
        bool is_master(void) { return(this->world.rank() == 0); }

        // if we are a slave process, poll for instructions to perform work
        void wait_for_tasks(void);

        // INTERNAL DATA

      protected:
        boost::mpi::environment environment;
        boost::mpi::communicator world;
      };


    template <typename number>
    task_manager<number>::task_manager(int argc, char* argv[])
      : instance_manager<number>(), environment(argc, argv)
      {
      }


    template <typename number>
    task_manager<number>::~task_manager()
      {
      }


  } // namespace transport


#endif //__task_manager_H_
