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


#ifndef CPPTRANSPORT_WORK_QUEUE_H
#define CPPTRANSPORT_WORK_QUEUE_H


#include <iostream>
#include <assert.h>
#include <vector>
#include <sstream>
#include <stdexcept>


#include "transport-runtime/messages.h"
#include "transport-runtime/scheduler/context.h"
#include "transport-runtime/utilities/formatter.h"


namespace transport
  {
    
    namespace work_queue_impl
      {
    
        //! Hold a list of work items for a specific device. The queue for a device is a collection of these work lists.
        template <typename QueueManager>
        class device_work_list
          {
            
            // TYPES
            
          public:
            
            //! type of work item
            using item_type = typename QueueManager::item_type;

            
            // CONSTRUCTOR, DESTRUCTOR
            
          public:
    
            //! constructor captures size of state vector
            device_work_list(unsigned int sz=0)
              : state_size(sz)
              {
              }
            
            //! destructor is default
            ~device_work_list() = default;
            
            
            // INTERFACE
            
          public:
    
            //! Compute the memory required to integrate all the items in this list
            size_t get_memory_required() const { return(this->state_size * this->work_list.size()); }
    
            //! Add a work item on this list
            void enqueue_item(const item_type item) { work_list.push_back(std::move(item)); }
    
            //! Return the number of items sitting in this list
            size_t size() const { return(this->work_list.size()); }
    
            //! Access an individual item in the list
            const item_type& operator[](unsigned int d) const;
            
          private:
    
            //! std::vector holding the list of work items
            std::vector<item_type> work_list;
    
            //! Memory required for the state vector, used when computing the memory required to integrate items in the list
            unsigned int state_size;
    
          };
    
    
        template <typename QueueManager>
        const typename device_work_list<QueueManager>::item_type&
        device_work_list<QueueManager>::operator[](unsigned int d) const
          {
            if(d < this->work_list.size()) return(this->work_list[d]);
            
            if(this->work_list.size() > 0)  return(this->work_list.back());

            throw std::out_of_range(CPPTRANSPORT_WORK_LIST_RANGE);
          }
    
    
        //! Hold a queue of work items for a specific device. The queue may be broken into a collection of work lists.
        template <typename ContextManager, typename QueueManager>
        class device_queue
          {
            
            // TYPES
            
          public:
            
            //! device type
            using device_type = typename ContextManager::device;
            
            //! work list type
            using work_list_type = device_work_list<QueueManager>;
            
            //! work item type
            using item_type = typename QueueManager::item_type;
      
            
          public:
        
            device_queue(const device_type& dev, unsigned int size)
              : device(dev),
                total_items(0),
                state_size(size)
              {
                // push empty queue onto list
                queue_list.emplace_back(state_size);
              }
        
            //! Compute memory required to integrate all items in this queue
            size_t get_memory_required() const { return(this->queue_list.back().get_memory_required()); }
        
            //! Return device associated with this queue
            const device_type& get_device() const { return(this->device); }
        
            //! Return fractional weight of device associated with this queue
            double get_weight() const { return(this->device.get_fractional_weight()); }
        
            //! Return number of work lists associated with this queue
            size_t size() const { return(this->queue_list.size()); }
        
            //! Access individual work lists
            const work_list_type& operator[](unsigned int d) const;
        
            //! Add an item to the queue.
        
            //! If the memory required for the integration is larger than the assigned capacity of the device,
            //! then a new work list is created and the work-item (and subsequent items) are added to the new list.
            void enqueue_item(const item_type& item);
        
            //! Return total number of items queued on the device, over all work lists
            size_t get_total_items() const { return(this->total_items); }
      
          protected:

            //! Create a new work list on the device
            void new_queue() { this->queue_list.emplace_back(this->state_size); }
      
            
            // INTERNAL DATA
            
          private:
        
            //! std::vector holding the work lists for this device
            std::vector< work_list_type > queue_list;
        
            //! device corresponding to this queue
            const device_type& device;
        
            //! Memory required for the state vector in a single integration, used when estimating the memory required to integrate a work list
            unsigned int state_size;
        
            //! Motal number of items queued on this device, over all work lists
            unsigned int total_items;
        
          };
    
    
        template <typename ContextManager, typename QueueManager>
        const typename device_queue<ContextManager, QueueManager>::work_list_type&
        device_queue<ContextManager, QueueManager>::operator[](unsigned int d) const
          {
            assert(d < this->queue_list.size());
    
            if(d < this->queue_list.size()) return(this->queue_list[d]);

            throw std::out_of_range(CPPTRANSPORT_DEVICE_QUEUE_RANGE);
          }
    
    
        template <typename ContextManager, typename QueueManager>
        void device_queue<ContextManager, QueueManager>::enqueue_item(const item_type& item)
          {
            if(this->device.get_mem_type() == compute_context::device::memory_type::bounded
               && this->get_memory_required() > this->device.get_mem_size())
              {
                this->new_queue();
              }

            this->queue_list.back().enqueue_item(item);
            this->total_items++;
          }
    
      }   // namespace work_queue_impl


    template <typename ItemType>
    class work_queue
      {
        
        // TYPES

      public:
    
        //! work item type
        using item_type = ItemType;

        //! type for device queue
        using device_queue = work_queue_impl::device_queue< compute_context, work_queue<ItemType> >;
        
        //! type for work list
        using device_work_list = typename device_queue::work_list_type;

        
        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures a compute context and size of state vector
        work_queue(const compute_context& c, unsigned int size);
        
        //! destructor is default
        ~work_queue() = default;
        
        
        // QUEUE MANAGEMENT
        
      public:

        //! Add a work item to the queue.

        //! The work item will be assigned to one of the available devices in a way which tries to balance
        //! the length of the queues to match the predefined device weighting
        void enqueue_work_item(const ItemType& item);

        //! Query number of devices to which we are assigning work
        size_t size() const { return(ctx.size()); }

        //! Query total number of work-items enqueued, counting all devices
        size_t get_total_items() const { return(this->total_items); }

        //! Access the work queue for an individual device
        const device_queue& operator[](unsigned int d) const;


        // WRITE SELF TO STREAM

      public:

        //! write to stream
        template <typename Stream> void write(Stream& out);


      protected:

        //! Clear all queues
        void clear();


      private:

        //! Device context
        const compute_context& ctx;

        //! std::vector holding queues for each device
        std::vector<device_queue> device_list;

        //! Total number of work items we are holding, summed over all devices
        unsigned int total_items;

        //! Memory required for the state vector in a single integration, used when estimating the memory required to integrate a work list
        unsigned int state_size;

      };


    template <typename ItemType>
    work_queue<ItemType>::work_queue(const compute_context& c, unsigned int size)
      : ctx(c),
        total_items(0),
        state_size(size)
      {
        this->clear();

        assert(ctx.size() > 0);
        if(ctx.size() == 0) throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_NO_DEVICES);
      }


    template <typename ItemType>
    void work_queue<ItemType>::clear()
      {
        // set up queue for the number of devices in our compute context
        this->device_list.clear();

        for(unsigned int i = 0; i < this->ctx.size(); ++i)
          {
            this->device_list.emplace_back(this->ctx[i], this->state_size);
          }
      }


    template <typename ItemType>
    void work_queue<ItemType>::enqueue_work_item(const ItemType& item)
      {
        bool inserted = false;
        for(auto& q : this->device_list)
          {
            // compute difference between current weight and desired weight
            // is positive if queue is too short, negative if queue is too long
            double this_delta = (static_cast<double>(q.get_total_items()) / this->total_items) - q.get_weight();

            // insert this item into the first queue which is shorter than desired
            if(this_delta < 0.0)
              {
                q.enqueue_item(item);
                inserted = true;
                break;
              }
          }

        if(!inserted) this->device_list.begin()->enqueue_item(item);
        ++this->total_items;
      }


    template <typename ItemType>
    template <typename Stream>
    void work_queue<ItemType>::write(Stream& out)
      {
        out << CPPTRANSPORT_WORK_QUEUE_OUTPUT_A << " " << this->ctx.size() << " "
        << (this->ctx.size() > 1 ? CPPTRANSPORT_WORK_QUEUE_OUTPUT_B : CPPTRANSPORT_WORK_QUEUE_OUTPUT_C)
        << '\n' << '\n';

        unsigned int d = 0;
        for(const auto& q : this->device_list)
          {
            out << d << ". " << q.get_device().get_name() << " (" << CPPTRANSPORT_WORK_QUEUE_WEIGHT << " = " << q.get_weight() << "), ";
            if(q.get_device().get_mem_type() == compute_context::device::memory_type::bounded)
              {
                out << CPPTRANSPORT_WORK_QUEUE_MAXMEM << " = " << format_memory(q.get_device().get_mem_size());
              }
            else
              {
                out << CPPTRANSPORT_WORK_QUEUE_UNBOUNDED;
              }
            out << '\n';

            // loop through the queues on this device, emitting them:
            out << "   " << q.size() << " "
            << (q.size() > 1 ? CPPTRANSPORT_WORK_QUEUE_QUEUES : CPPTRANSPORT_WORK_QUEUE_QUEUE)
            << '\n' << '\n';

            for(unsigned int i = 0; i < q.size(); ++i)
              {
                const typename work_queue<ItemType>::device_work_list& work = q[i];

                out << "   ** " << CPPTRANSPORT_WORK_QUEUE_QUEUE_NAME << " " << i << '\n';
                for(unsigned int j = 0; j < work.size(); ++j)
                  {
                    out << "     " << work[j];
                  }
                out << '\n';
              }
            
            ++d;
          }
      }
    
    
    template <typename ItemType>
    const typename work_queue<ItemType>::device_queue&
    work_queue<ItemType>::operator[](unsigned int d) const
      {
        assert(d < this->device_list.size());
    
        if(d < this->device_list.size()) return(this->device_list[d]);

        throw std::out_of_range(CPPTRANSPORT_WORK_QUEUE_RANGE);
      }
    
    
    template <typename ItemType, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, work_queue<ItemType>& obj)
      {
        obj.write(out);
        return(out);
      }

  }



#endif //CPPTRANSPORT_WORK_QUEUE_H
