//
// Created by David Seery on 20/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __work_queue_H_
#define __work_queue_H_


#include <iostream>
#include <assert.h>
#include <vector>
#include <sstream>
#include <stdexcept>


#include "transport/messages_en.h"
#include "transport/scheduler/context.h"
#import "context.h"


namespace transport
  {

    // forward reference for overload of operator<<
    template <typename ItemType, typename SizeFunctor> class work_queue;

    // overload operator<< to write a work_queue to a stream
    template <typename ItemType, typename SizeFunctor>
    std::ostream& operator<<(std::ostream& out, work_queue<ItemType, SizeFunctor>& obj);

    template <typename ItemType, typename SizeFunctor>
    class work_queue
      {
      public:

        class device_work_queue
          {
          public:
            device_work_queue(unsigned int size)
              : state_size(0)
              {
              }

            // compute memory required by items in the queue
            unsigned int get_memory_required() const { return(this->state_size * this->queue.size()); }

            // queue an work item on this list
            void enqueue_item(const ItemType& item) { queue.push_back(item); }

            // return number of items queued on this list
            unsigned int size() const { return(this->queue.size()); }

            // overload subscripting operator to access individual items
            const ItemType& operator[](unsigned int d) const
              {
                if(d < this->queue.size())
                  {
                    return(this->queue[d]);
                  }
                else if(this->queue.size() > 0)
                  {
                    return(this->queue.back());
                  }
                else
                  {
                    throw std::out_of_range(__CPP_TRANSPORT_WORK_QUEUE_RANGE);
                  }
              }

          protected:
            std::vector<ItemType> queue;

            // memory required for the state vector
            unsigned int state_size;
          };

        class device_queue
          {
          public:
            device_queue(const context::device& dev, unsigned int size)
              : device(dev), total_items(0), state_size(size)
              {
                // push empty queue onto list
                queue_list.push_back(device_work_queue(state_size));
              }

            // get memory requirements for the current queue
            unsigned int get_memory_required() const { return(this->queue_list.back().get_memory_required()); }

            // return device associated with this queue
            const context::device& get_device() const { return(this->device); }

            // return fractional weight of device associated with this queue
            double get_weight() const { return(this->device.get_fractional_weight()); }

            // return number of queues on this device
            unsigned int size() const { return(this->queue_list.size()); }

            // access queues by overloading subscript operator
            const device_work_queue& operator[](unsigned int d) const
              {
                assert(d < this->queue_list.size());

                if(d < this->queue_list.size())
                  {
                    return(this->queue_list[d]);
                  }
                else
                  {
                    return (this->queue_list.back());
                  }
              }

            // push item to current queue
            void enqueue_item(const ItemType& item)
              {
                if(this->device.get_mem_type() == context::device::bounded && this->get_memory_required() > this->device.get_mem_size())
                  {
                    this->new_queue();
                  }
                this->queue_list.back().enqueue_item(item);
                this->total_items++;
              }

            // get total number of items enqueued on the device
            unsigned int get_total_items() const { return(this->total_items); }

          protected:
            // create a new queue on the device
            void new_queue() { this->queue_list.push_back(device_work_queue(this->state_size)); }

            // set of queues of work items for this device
            // there can be multiple queues on devices with fixed memory
            std::vector<device_work_queue> queue_list;

            // device corresponding to this queue
            const context::device& device;

            // memory required for the state vector
            unsigned int state_size;

            // total number of items enqueued on this device
            unsigned int total_items;
          };

        work_queue(const context& c, const SizeFunctor sf, const std::vector<double>& ks);;

        // add a work item to the back of the queue
        void enqueue_work_item(const ItemType& item);

      protected:
        // clear all queues and start again
        void clear();

        const context& ctx;

        // vector of device queues, holding all various work items
        // we have been asked to enqueue
        std::vector<device_queue> device_list;

        // total number of work items we are holding, summed over all devices and queues
        // used to work out which queue to push the next item to
        unsigned int              total_items;

        // flat lookup table of k-modes
        const std::vector<double> k_lookup;

        // size functor: calculates the size of a state vector
        const SizeFunctor         state_size;
      };


    template <typename ItemType, typename SizeFunctor>
    work_queue<ItemType, SizeFunctor>::work_queue(const context& c, const SizeFunctor sf, const std::vector<double>& ks)
      : ctx(c), k_lookup(ks), total_items(0), state_size(sf)
      {
        this->clear();

        assert(ctx.size() > 0);

        if(ctx.size() == 0)
          {
            std::cerr << __CPP_TRANSPORT_NO_DEVICES << std::endl;
            exit(1);  // this error shouldn't happen. TODO: tidy this up; could do with a proper error handler
          }
      }


    template <typename ItemType, typename SizeFunctor>
    void work_queue<ItemType, SizeFunctor>::clear()
      {
        // set up queue for the number of devices in our context
        this->device_list.clear();

        for(unsigned int i = 0; i < this->ctx.size(); i++)
          {
            this->device_list.push_back(device_queue(this->ctx[i], this->state_size()));
          }
      }


    template <typename ItemType, typename SizeFunctor>
    void work_queue<ItemType, SizeFunctor>::enqueue_work_item(const ItemType& item)
      {
        // check whether all indices are within bounds
        if(item.validate_index(this->k_lookup.size()))
          {
            bool inserted = false;
            for(std::vector<device_queue>::iterator t = this->device_list.begin(); t != this->device_list.end(); t++)
              {
                // enqueue this work item on the first device whose queue is too small
                if((*t).get_total_items() < static_cast<unsigned int>((*t).get_weight() * this->total_items))
                  {
                    (*t).enqueue_item(item);
                    inserted = true;
                    break;
                  }
              }

            if(!inserted) this->devices[0].enqueue_item(item);
            this->total_items++;
          }
      }


    template <typename ItemType, typename SizeFunctor>
    std::ostream& operator<<(std::ostream& out, work_queue<ItemType, SizeFunctor>& obj)
      {
        out << __CPP_TRANSPORT_WORK_QUEUE_OUTPUT_A << " " << obj.ctx.size() << __CPP_TRANSPORT_WORK_QUEUE_OUTPUT_B << std::endl << std::endl;

        unsigned int d = 0;
        for(std::vector<work_queue::device_queue>::const_iterator t = obj.queue_list.begin(); t != obj.queue_list.end(); t++, d++)
          {
            out << d << ". " << (*t).get_device().get_name() << " (" << __CPP_TRANSPORT_WORK_QUEUE_WEIGHT << " = " << (*t).get_weight() << "), ";
            if((*t).get_device().get_mem_type() == context::device::bounded)
              {
                out << __CPP_TRANSPORT_WORK_QUEUE_MAXMEM << " = " << format_memory((*t).get_device().get_mem_size());
              }
            else
              {
                out << __CPP_TRANSPORT_WORK_QUEUE_UNBOUNDED;
              }
            out << std::endl;

            out << (*t).size() << " " << __CPP_TRANSPORT_WORK_QUEUE_QUEUES << std::endl << std::endl;
            for(unsigned int i = 0; i < (*t).size(); i++)
              {
                work_queue::device_work_queue& work = (*t)[i];

                out << "  " << __CPP_TRANSPORT_WORK_QUEUE_QUEUE << " " << i << std::endl;
                for(unsigned int j = 0; j < work.size(); j++)
                  {
                    out << "  " << work[j];
                  }
                out << std::endl;
              }
          }
      }

  }



#endif //__work_queue_H_
