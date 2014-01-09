//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __data_manager_H_
#define __data_manager_H_


#include "transport/tasks/task.h"
#include "transport/manager/repository.h"


namespace transport
  {

    template <typename number>
    class data_manager
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:
        //! Destroy the data_manager instance. In practice this would always be delegated to an implementation class
        virtual ~data_manager()
          {
          }


        // INTERFACE -- CONTAINER HANDLING

      public:
        //! Create a new container. Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void create_container(repository<number>* repo, typename repository<number>::integration_container& ctr) = 0;

        //! Open an existing container
        virtual void open_container(repository<number>* repo, typename repository<number>::integration_container& ctr) = 0;

        //! Close an open container
        virtual void close_container(typename repository<number>::integration_container& ctr) = 0;


        // INTERFACE -- WRITE INDEX TABLES

      public:
        //! Create the table of sample times
        virtual void create_time_sample_table(typename repository<number>::integration_container& ctr, task<number>* tk) = 0;

        //! Create the table of twopf sample configurations
        virtual void create_twopf_sample_table(typename repository<number>::integration_container& ctr, twopf_list_task<number>* tk) = 0;

        //! Create the table of threepf sample configurations
        virtual void create_threepf_sample_table(typename repository<number>::integration_container& ctr, threepf_task<number>* tk) = 0;


        // INTERFACE -- TASK FILES

      public:
        //! Create a list of task assignments, over a number of devices, from a work queue of twopf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_container& ctr, const work_queue<twopf_kconfig>& queue) = 0;

        //! Create a list of task assignments, over a number of devices, from a work queue of threepf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_container& ctr, const work_queue<threepf_kconfig>& queue) = 0;
      };

  }   // namespace transport



#endif //__data_manager_H_
