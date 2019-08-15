//
// Created by David Seery on 20/12/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_SCHEDULER_H
#define CPPTRANSPORT_SCHEDULER_H


#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <set>
#include <sstream>
#include <stdexcept>

#include "transport-runtime/tasks/task.h"
#include "transport-runtime/tasks/integration_tasks.h"
#include "transport-runtime/tasks/output_tasks.h"
#include "transport-runtime/scheduler/context.h"
#include "transport-runtime/scheduler/device_queue_manager.h"


namespace transport
  {

    //! Abstract filter function, used to filter work items when adding to a schedule
		template <typename work_element>
    class abstract_filter
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor is default
        abstract_filter() = default;
        
        //! destructor is default
        virtual ~abstract_filter() = default;
        
        // FILTER INTERFACE
        
      public:

        //! filter an individual work item
        virtual bool filter(const work_element& config) const = 0;

      };


    //! Explicit empty filter
		template <typename work_element>
    class trivial_filter: public abstract_filter<work_element>
      {

        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor is default
        trivial_filter() = default;
        
        //! destructor is default
        ~trivial_filter() = default;

        
        // FILTER INTERFACE
        
      public:
    
        //! filter an individual work item
        bool filter(const work_element& config) const override { return(true); }

      };


    //! Filter function for work items -- used by slave nodes to filter out
    //! pieces of work intended for them
		template <typename work_element>
    class work_item_filter: public abstract_filter<work_element>
      {

        // CONSTRUCTOR, DESTRUCTOR
        
      public:

        //! Construct an empty filter
        work_item_filter() = default;

        //! Construct a filter from a predefined list of matching items
		    work_item_filter(const std::list<unsigned int>& filter_set)
			    {
				    items.clear();
		        std::copy(filter_set.begin(), filter_set.end(), std::inserter(items, items.begin()));
			    }
    
        //! Destructor is default
        ~work_item_filter() = default;
        
        
        // INTERFACE
        
      public:

        //! Add an item to the list of work-items included in this filter
        void add_work_item(unsigned int serial) { this->items.insert(serial); }

        //! Check whether a work-item is part of the filter
        bool filter(const work_element& config) const
          { return (this->items.find(config.get_serial()) != this->items.end()); }


        // WRITE SELF TO STREAM

      public:

        //! write details
        template <typename Stream> void write(Stream& out) const;


        // INTERNAL DATA

      private:

        //! std::set holding the serial number for each items we are supposed to process
        std::set<unsigned int> items;

      };


    template <typename work_element>
    template <typename Stream>
    void work_item_filter<work_element>::write(Stream& out) const
      {
        out << CPPTRANSPORT_FILTER_TAG;
        
        unsigned int c = 0;
        for(auto it : this->items)
          {
            out << (c > 0 ? ", " : " ") << it;
            ++c;
          }

        out << '\n';
      }


    //! Write a filter to a stream
		template <typename work_element, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const work_item_filter<work_element>& filter)
      {
        filter.write(out);
        return out;
      }


    //! A 'compute_scheduler' is a utility object that understands how to use a 'compute_context' to break
    //! work over all devices in the context
    class compute_scheduler
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constuctor captures context
        compute_scheduler(compute_context& c)
          : ctx(c)
          {
          }
        
        //! destructor is default
        ~compute_scheduler() = default;
        
        
        // SET UP QUEUES
        
      public:

        //! set up a work queue for a two-point function task, using a user-provided filter function
        template <typename number>
        device_queue_manager<twopf_kconfig_record> make_device_queues(unsigned int size, const twopf_task<number>& task, const abstract_filter<twopf_kconfig>& F);

        //! set up a work queue for a three-point function task, using a user-provided filter function
        template <typename number>
        device_queue_manager<threepf_kconfig_record> make_device_queues(unsigned int size, const threepf_task<number>& task, const abstract_filter<threepf_kconfig>& F);

        //! set up a work queue for a zeta three-point function task, using a user-provided filter function
        template <typename number>
        device_queue_manager<threepf_kconfig_record> make_device_queues(unsigned int size, const zeta_threepf_task<number>& task, const abstract_filter<threepf_kconfig>& F);

		    //! set up a work queue for an output task, using a user-provided filter function
		    template <typename number>
		    device_queue_manager< output_task_element<number> > make_device_queues(const output_task<number>& task, const abstract_filter< output_task_element<number> >& F);

        //! set up a work queue for a two-point function task, using no filter
        template <typename number>
        device_queue_manager<twopf_kconfig_record> make_device_queues(unsigned int size, const twopf_task<number>& task);

        //! set up a work queue for a three-point function task, using no filter
        template <typename number>
        device_queue_manager<threepf_kconfig_record> make_device_queues(unsigned int size, const threepf_task<number>& task);

		    //! set up a work queue for an output task, using no filter
		    template <typename number>
			  device_queue_manager< output_task_element<number> > make_device_queues(const output_task<number>& task);

        
        // INTERNAL DATA
        
      protected:
        
        //! capture compute context
        const compute_context& ctx;
        
      };


    // QUEUE-MAKING FUNCTIONS -- with a filter


    template <typename number>
    device_queue_manager<twopf_kconfig_record>
    compute_scheduler::make_device_queues(unsigned int size, const twopf_task<number>& task, const abstract_filter<twopf_kconfig>& F)
      {
        // set up an empty queue
        device_queue_manager<twopf_kconfig_record> work(this->ctx, size);

        const auto& twopf_db = task.get_twopf_database();
        for(auto t = twopf_db.record_cbegin(); t != twopf_db.record_cend(); ++t)
          {
            if(F.filter(*(*t))) work.enqueue_work_item(*t);
          }

        return work;
      }


    template <typename number>
    device_queue_manager<threepf_kconfig_record>
    compute_scheduler::make_device_queues(unsigned int size, const threepf_task<number>& task, const abstract_filter<threepf_kconfig>& F)
      {
        device_queue_manager<threepf_kconfig_record> work(this->ctx, size);

        const auto& threepf_db = task.get_threepf_database();
        for(auto t = threepf_db.record_cbegin(); t != threepf_db.record_cend(); ++t)
          {
            if(F.filter(*(*t))) work.enqueue_work_item(*t);
          }

        return work;
      }


    template <typename number>
    device_queue_manager<threepf_kconfig_record>
    compute_scheduler::make_device_queues(unsigned int size, const zeta_threepf_task<number>& task, const abstract_filter<threepf_kconfig>& F)
	    {
        device_queue_manager<threepf_kconfig_record> work(this->ctx, size);

        const auto& threepf_db = task.get_threepf_database();
        for(auto t = threepf_db.record_cbegin(); t != threepf_db.record_cend(); ++t)
          {
            if(F.filter(*(*t))) work.enqueue_work_item(*t);
          }

        return work;
	    }


		template <typename number>
		device_queue_manager< output_task_element<number> >
    compute_scheduler::make_device_queues(const output_task<number>& task, const abstract_filter< output_task_element<number> >& F)
			{
				device_queue_manager< output_task_element<number> > work(this->ctx, 1);

				const auto& config_list = task.get_elements();
        for(const auto& it : config_list)
					{
						if(F.filter(it)) work.enqueue_work_item(it);
					}

				return work;
			}


    // QUEUE-MAKING FUNCTIONS -- without a filter


    template <typename number>
    device_queue_manager<twopf_kconfig_record> compute_scheduler::make_device_queues(unsigned int size, const twopf_task<number>& task)
      {
        return this->make_device_queues(size, task, trivial_filter<twopf_kconfig>{});
      }


    template <typename number>
    device_queue_manager<threepf_kconfig_record> compute_scheduler::make_device_queues(unsigned int size, const threepf_task<number>& task)
      {
        return this->make_device_queues(size, task, trivial_filter<threepf_kconfig>{});
      }


		template <typename number>
		device_queue_manager< output_task_element<number> > compute_scheduler::make_device_queues(const output_task<number>& task)
			{
				return this->make_device_queues(task, trivial_filter< output_task_element<number> >{});
			}


  } // / namespace transport



#endif //CPPTRANSPORT_SCHEDULER_H
