//
// Created by David Seery on 20/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __scheduler_H_
#define __scheduler_H_


#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "transport/tasks/task.h"
#include "transport/scheduler/context.h"
#include "transport/scheduler/work_queue.h"


namespace transport
  {

    class scheduler
      {
      public:
        scheduler(context& c)
          : ctx(c)
          {
          }

        // set up a work queue for a two-point function
        template <typename number>
        work_queue<twopf_kconfig> make_queue(unsigned int size, const twopf_task<number>& task);

        // set up a work queue for a three-point function
        template <typename number>
        work_queue<threepf_kconfig> make_queue(unsigned int size, const threepf_task<number>& task);

      protected:
        const context&    ctx;
      };


    template <typename number>
    work_queue<twopf_kconfig> scheduler::make_queue(unsigned int size, const twopf_task<number>& task)
      {
        work_queue<twopf_kconfig> work(this->ctx, size);

        const std::vector<twopf_kconfig>& config_list = task.get_sample();
        for(std::vector<twopf_kconfig>::const_iterator t = config_list.begin(); t != config_list.end(); t++)
          {
            work.enqueue_work_item(*t);
          }

        return(work);
      }


    template <typename number>
    work_queue<threepf_kconfig> scheduler::make_queue(unsigned int size, const threepf_task<number>& task)
      {
        work_queue<threepf_kconfig> work(this->ctx, size);

        const std::vector<threepf_kconfig>& config_list = task.get_sample();
        for(std::vector<threepf_kconfig>::const_iterator t = config_list.begin(); t != config_list.end(); t++)
          {
            work.enqueue_work_item(*t);
          }

        return(work);
      }

  } // / namespace transport



#endif //__scheduler_H_
