//
// Created by David Seery on 22/03/15.
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


#ifndef CPPTRANSPORT_MASTER_SCHEDULER_H
#define CPPTRANSPORT_MASTER_SCHEDULER_H


#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <cmath>
#include <random>

#include "transport-runtime/manager/mpi_operations.h"

#include "transport-runtime/repository/writers/generic_writer.h"

#include "transport-runtime/reporting/key_value.h"
#include "transport-runtime/utilities/formatter.h"

#include "transport-runtime/exceptions.h"
#include "transport-runtime/messages.h"

#include "boost/timer/timer.hpp"


// target work assignment of 1 minute's worth of work, expressed in nanosecond
// 1 microsecond = 1000 nanosecond
// 1 millisecond = 1000 microsecond
// 1 second = 1000 milisecond
// 1 minute = 60 seconds
#define CPPTRANSPORT_DEFAULT_SCHEDULING_GRANULARITY (boost::timer::nanosecond_type(1)*60*1000*1000*1000)

#define CPPTRANSPORT_SCHEDULER_MINIMUM_UPDATE_TIME  (boost::timer::nanosecond_type(5)*60*1000*1000*1000)

//#define CPPTRANSPORT_DEBUG_SCHEDULER


namespace transport
	{
    
    class worker_scheduler;
    
    class work_assignment
      {
      
      public:
        
        //! construct a work assignment record
        work_assignment(unsigned int w, const std::list<unsigned int>& i)
          : worker(w),
            items(i)
          {
          }
        
        ~work_assignment() = default;
        
        // EXTRACT INFORMATION
      
      public:
        
        //! get worker number
        unsigned int get_worker() const { return(this->worker); }
        
        //! get work items
        const std::list<unsigned int>& get_items() const { return(this->items); }
        
        // INTERNAL DATA
      
      private:
        
        //! worker number
        unsigned int worker;
        
        //! work items
        const std::list<unsigned int> items;
      };

    
    //! Worker information class
    class worker_information
      {
  
      public:
        
        friend class worker_scheduler;
    
        //! construct a worker information record
        worker_information()
          : type(worker_type::cpu),
            capacity(0),
            priority(0),
            initialized(false),
            assigned(false),
            active(true),
            items(0),
            time(0)
          {
          }
    
        //! destructor
        ~worker_information() = default;
    
    
        // INTERFACE -- INTERROGATE FOR GENERAL INFORMATION
  
      public:
    
        //! get worker type
        worker_type get_type() const { return(this->type); }
    
        //! get worker number
        unsigned int get_number() const { return(this->number); }
    
        //! get worker capacity
        unsigned int get_capacity() const { return(this->capacity); }
    
        //! get worker priority
        unsigned int get_priority() const { return(this->priority); }
    
        //! get initialization status
        bool get_initialization_status() const { return(this->initialized); }
    
      private:
        
        //! set data for this worker
        //! this is a private method; only friend classes may use it
        void set_data(unsigned int n, worker_type t, unsigned int c, unsigned int p)
          {
            this->number = n;
            this->type = t;
            this->capacity = c;
            this->priority = p;
            this->initialized = true;
          }
    
    
        // INTERFACE -- ASSIGNMENT MANAGEMENT
  
      public:
    
        //! is this worker currently assigned?
        bool is_assigned() const { return(this->assigned); }
    
        //! is this worker currently active?
        bool is_active() const { return(this->active); }
        
      private:
    
        //! set assignment status
        void mark_assigned(bool status) { this->assigned = status; }
    
        //! set active states
        void mark_active(bool status) { this->active = status; }
    
    
        // INTERFACE -- TIMING METADATA
  
      public:
    
        // get total time for this worker
        boost::timer::nanosecond_type get_total_time() const { return(this->time); }
    
        //! get running mean time per work item for this workers
        boost::timer::nanosecond_type get_mean_time_per_work_item() const { return(this->items == 0 ? this->time : this->time / this->items); }
        
      private:
    
        //! update timing data
        void update_timing_data(boost::timer::nanosecond_type t, unsigned int n) { this->time += t; this->items += n; }
    
    
        // INTERFACE -- GENERAL METADATA
  
      public:
    
        // get total number of items processed
        unsigned int get_number_items() const { return(this->items); }
    
    
        // INTERNAL DATA
  
      private:
    
        //! worker number
        unsigned int number;
    
        //! capacity type -- are integrations on this worker limited by memory?
        worker_type type;
    
        //! worker's memory capacity (for integrations only)
        unsigned int capacity;
    
        //! worker's priority
        unsigned int priority;
    
        //! received initialization data from this worker?
        bool initialized;
    
        //! is this worker currently assigned?
        bool assigned;
    
        //! is this worker currently active?
        bool active;
    
        //! total time used to process items on this worker
        boost::timer::nanosecond_type time;
    
        //! total number of items processed on this worker
        unsigned int items;
    
      };

        
    class worker_scheduler
	    {
        
		    // CONSTRUCTOR, DESTRUCTOR

      public:

		    //! construct a scheduler object
		    worker_scheduler(unsigned int nw)
		      : number_workers(nw),
            waiting_for_setup(0),
		        state_size(0),
		        unassigned(0),
		        active(0),
		        has_cpus(false),
		        has_gpus(false),
		        max_work_allocation(1),
		        current_granularity(CPPTRANSPORT_DEFAULT_SCHEDULING_GRANULARITY),
		        total_aggregation_time(0),
		        number_aggregations(0),
		        total_work_time(0),
            estimated_completion(boost::posix_time::not_a_date_time),
            estimated_cpu_time(0),
		        work_items_completed(0),
		        work_items_in_flight(0),
            finished(false)
			    {
            // set up the random number generator
            urng.seed(rng());
			    }

		    ~worker_scheduler() = default;


		    // INTERFACE -- GENERAL FUNCTIONS

      public:

		    //! reset scheduler and all scheduling data; prepare for new scheduling
		    //! task with the given number of workers
		    void reset();

		    //! initialization complete and ready to proceed with scheduling?
		    bool is_ready() const { return(this->waiting_for_setup == 0); }

		    //! initialize a worker
		    //! reduces count of workers waiting for initialization if successful, and logs the data using the supplied WriterObject
		    //! otherwise, logs an error.
		    void initialize_worker(boost::log::sources::severity_logger< base_writer::log_severity_level >& log, unsigned int worker, MPI::slave_information_payload& payload);

		    //! set current state size; used when assigning work to GPUs
		    void set_state_size(unsigned int size) { this->state_size = size; }


		    // INTERFACE -- MANAGE WORK QUEUE

      public:

				//! build a work queue for twopf task
		    template <typename number>
		    void prepare_queue(twopf_task<number>& task);

		    //! build a work queue for a threepf task
		    template <typename number>
		    void prepare_queue(threepf_task<number>& task);

        //! build a work queue for zeta twopf task
        template <typename number>
        void prepare_queue(zeta_twopf_task<number>& task);

        //! build a work queue for a zeta threepf task
        template <typename number>
        void prepare_queue(zeta_threepf_task<number>& task);

		    //! build a work queue for an output task
		    template <typename number>
		    void prepare_queue(output_task<number>& task);

        //! build a work queue using specified serial numbers (used when seeding tasks)
        void prepare_queue(const std::set<unsigned int>& list);

		    //! current queue exhausted? ie., finished all current work?
		    bool is_finished() const { return(this->queue.size() == 0); }

		    //! get remaining queue size
		    unsigned int get_queue_size() const { return(this->queue.size()); }

		    //! finalize queue setup; should be called before generating work assignments
		    void complete_queue_setup();

        //! check whether an update is available
        bool update_available();

		    //! check for an update string
		    void populate_update_information(reporting::key_value& updates, reporting::key_value& notifications);

        //! get current estimated time-of-completion
        const boost::posix_time::ptime& get_estimated_completion() const { return this->estimated_completion; }
        
        //! get current estimtaed CPU time
        const boost::timer::nanosecond_type& get_estimated_CPU_time() const { return this->estimated_cpu_time; }


		    // INTERFACE -- MANAGE WORK ASSIGNMENTS

      public:

		    //! does assignable work remain?
		    bool assignable() const;

		    //! generate work assignments
		    std::list<work_assignment> assign_work(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log);

		    //! mark a worker as assigned
		    void mark_assigned(const work_assignment& assignment);

		    //! mark a worker as unassigned, updating its mean time per work_item
		    void mark_unassigned(unsigned int worker, boost::timer::nanosecond_type time, unsigned int items);

		    //! mark a worker as inactive, meaning that it has closed down after running out of work
		    void mark_inactive(unsigned int worker);

		    //! are all workers inactive?
		    bool all_inactive() const { return(this->active == 0); }

        //! get number of active workers
        unsigned int get_number_active() const { return(this->active); }


        // INTERFACE -- MANAGE WORKER DATA AND METADATA

      public:
        
        //! query for individual worker data, returned in read-only format
        const worker_information& operator[](unsigned int worker) const;
        
        //! query for size
        size_t size() const { return this->number_workers; }

        //! advise a new aggregation time
        void report_aggregation(boost::timer::nanosecond_type time);


		    // INTERNAL API

      protected:

		    //! build a work queue from a list of work items
		    template <typename WorkItem>
		    void build_queue(const std::vector<WorkItem>& q);

        //! build a work queue for a database of configurations
        template <typename Database>
        void build_queue(const Database& db);


		    // SCHEDULING STRATEGIES

      protected:

		    //! schedule work for a pool of CPU only workers
		    std::list<work_assignment> assign_work_cpu_only_strategy(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log);

		    //! schedule work for a pool of GPU only workers
		    std::list<work_assignment> assign_work_gpu_only_strategy(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log);

		    //! schedule work for a mixed pool of CPU and GPU workers
		    std::list<work_assignment> assign_work_mixed_strategy(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log);


		    // INTERNAL DATA

      private:


		    // WORKER POOL
        
        //! Number of workers in the pool
        const unsigned int number_workers;

        //! Information about workers
        std::vector<worker_information> worker_data;

		    //! Number of workers still to be initialized
		    unsigned int waiting_for_setup;

		    //! Number of workers currently unassigned
		    unsigned int unassigned;

		    //! Number of workers currently active
		    unsigned int active;

		    //! Current state size; used when assigning work to GPUs
		    unsigned int state_size;

        //! CPUs in our pool of workers?
        bool has_cpus;

        //! GPUs in our list of workers?
        bool has_gpus;


		    // QUEUE AND ALLOCATION DATA

		    //! Queue of work items
		    std::list<unsigned int> queue;

		    //! Maximum number of work items to be allocated in one shot
		    unsigned int max_work_allocation;

		    //! Current scheduling granularity
		    boost::timer::nanosecond_type current_granularity;

		    //! Keep track of total time spent aggregating, so we can work out a mean aggregation time
		    boost::timer::nanosecond_type total_aggregation_time;

		    //! Keep track of total number of aggregations, so we can work out a mean aggregation time
		    unsigned int number_aggregations;


		    // CURRENT STATUS

		    //! Keep track of time elapsed
		    boost::timer::cpu_timer timer;

		    //! Keep track of total time spent doing work, to estimate a time-to-completion
		    boost::timer::nanosecond_type total_work_time;

		    //! Keep track of total number of work items which have been fully processed, to estimate a time-to-complation
		    unsigned int work_items_completed;

		    //! Keep track of total number of work items which are still in-flight
		    unsigned int work_items_in_flight;

		    //! Points at which to emit updates
		    std::list< unsigned int > update_stack;

        //! work complete?
        bool finished;
        
        
        // ESTIMATED TIME TO COMPLETION
    
        //! Current estimate of completion time
        boost::posix_time::ptime estimated_completion;
    
        //! Current estimate of expected CPU time
        boost::timer::nanosecond_type estimated_cpu_time;


        // RANDOM NUMBER GENERATORS

        std::random_device rng;

        std::mt19937 urng;

	    };


		void worker_scheduler::reset()
			{
				this->worker_data.clear();
				this->worker_data.resize(this->number_workers);

				this->waiting_for_setup = this->number_workers;
				this->unassigned = 0;
				this->active = 0;

				this->has_cpus = false;
				this->has_gpus = false;

				this->update_stack.clear();
        this->finished = false;

				// reset metadata and statistics
				this->total_aggregation_time = 0;
				this->total_work_time = 0;
				this->number_aggregations = 0;
				this->work_items_completed = 0;
				this->work_items_in_flight = 0;
				this->timer.start();
			}


		void worker_scheduler::initialize_worker(boost::log::sources::severity_logger< base_writer::log_severity_level >& log, unsigned int worker, MPI::slave_information_payload& payload)
			{
		    if(!(this->worker_data[worker].get_initialization_status()))
			    {
		        worker_type type = payload.get_type();
            switch(type)
              {
                case worker_type::cpu:
                  this->has_cpus = true;
                  break;

                case worker_type::gpu:
                  this->has_gpus = true;
                  break;
              }

		        this->worker_data[worker].set_data(worker, type, payload.get_capacity(), payload.get_priority());
				    this->waiting_for_setup--;

		        std::ostringstream msg;
		        msg << "** Worker " << worker+1 << " identified as ";
            switch(type)
              {
                case worker_type::cpu:
                  msg << "CPU";
                  break;

                case worker_type::gpu:
                  msg << "GPU, available memory = " << format_memory(payload.get_capacity());
                  break;
              }

		        msg << " and priority " << payload.get_priority();

		        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << msg.str();

				    this->unassigned++;
				    this->active++;
			    }
		    else
			    {
		        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Unexpected double identification for worker  " << worker;
			    }

			}


		template <typename number>
		void worker_scheduler::prepare_queue(twopf_task<number>& task)
			{
				this->build_queue(task.get_twopf_database());
			}


		template <typename number>
		void worker_scheduler::prepare_queue(threepf_task<number>& task)
			{
				this->build_queue(task.get_threepf_database());
			}


		template <typename number>
		void worker_scheduler::prepare_queue(zeta_twopf_task<number>& task)
			{
				this->build_queue(task.get_twopf_database());
			}


		template <typename number>
		void worker_scheduler::prepare_queue(zeta_threepf_task<number>& task)
			{
				this->build_queue(task.get_threepf_database());
			}


		template <typename number>
		void worker_scheduler::prepare_queue(output_task<number>& task)
			{
        // TODO: move output tasks to a database system?
				this->build_queue(task.get_elements());
			}


    // TODO: this version of build_queue() is required only for output tasks, which don't use the database variant. Refactor output_task so it can be removed?
		template <typename WorkItem>
		void worker_scheduler::build_queue(const std::vector<WorkItem>& q)
			{
				this->queue.clear();

				// build a queue of work items from the serial numbers of each work item
				for(typename std::vector<WorkItem>::const_iterator t = q.begin(); t != q.end(); ++t)
					{
						this->queue.push_back(t->get_serial());
					}

				// sort into ascending order of serial number, and remove any duplicates
				// (note duplicate removal using unique() requires a sorted list)
				this->queue.sort();
				this->queue.unique();

        // shuffle items; work items with nearby serial numbers typically have similar properties and therefore similar
        // integration times. That can bias the average time-per-item low or high at the beginnng of the integration,
        // making the remaining-time estimate unreliable
        // shuffling attempt to alleviate that problem a bit
        std::vector<unsigned int> temp(this->queue.size());
        std::copy(this->queue.begin(), this->queue.end(), temp.begin());
        std::shuffle(temp.begin(), temp.end(), this->urng);
        std::copy(temp.begin(), temp.end(), this->queue.begin());
			}


    template <typename Database>
    void worker_scheduler::build_queue(const Database& db)
      {
        this->queue.clear();

        // build a queue of work items from the serial numbers of each work item
        for(typename Database::const_config_iterator t = db.config_begin(); t != db.config_end(); ++t)
          {
            this->queue.push_back(t->get_serial());
          }

        // sort into ascending order of serial number, and remove any duplicates
        // (note duplicate removal using unique() requires a sorted list)
        this->queue.sort();
        this->queue.unique();

        // shuffle items; work items with nearby serial numbers typically have similar properties and therefore similar
        // integration times. That can bias the average time-per-item low or high at the beginnng of the integration,
        // making the remaining-time estimate unreliable
        // shuffling attempt to alleviate that problem a bit
        std::vector<unsigned int> temp(this->queue.size());
        std::copy(this->queue.begin(), this->queue.end(), temp.begin());
        std::shuffle(temp.begin(), temp.end(), this->urng);
        std::copy(temp.begin(), temp.end(), this->queue.begin());
      }


    void worker_scheduler::prepare_queue(const std::set<unsigned int>& list)
      {
        // copy serial numbers from list into temporary vector
        std::vector<unsigned int> temp;
        temp.reserve(list.size());
        std::copy(list.begin(), list.end(), std::back_inserter(temp));

        // shuffle
        std::shuffle(temp.begin(), temp.end(), this->urng);

        // copy shuffled numbers back into queue
        this->queue.clear();
        std::copy(temp.begin(), temp.end(), std::back_inserter(this->queue));
      }


		void worker_scheduler::complete_queue_setup()
			{
				// set maximum work allocation to force multiple scheduling adjustments during
				// the lifetime of the task.
				// This is intended to prevent lots of k-configurations being allocated in one go,
				// and then the system inadvertently running out of work with some cores
				// left unallocated for a long period.
				if(this->worker_data.size() > 0)
					{
				    this->max_work_allocation = std::max(static_cast<unsigned int>(this->queue.size() / (5*this->worker_data.size())), static_cast<unsigned int>(1));
					}
				else
					{
						this->max_work_allocation = 1;
					}

				unsigned int update_interval = static_cast<unsigned int>(this->queue.size() / 10);
				if(update_interval > 0)
					{
				    unsigned int count = update_interval;
				    while(count < this->queue.size())
					    {
				        this->update_stack.push_front(count);
				        count += update_interval;
					    }
					}
			}


		bool worker_scheduler::assignable() const
			{
				// are there unassigned workers and work items left for them to process?
				return(this->queue.size() > 0 && this->unassigned > 0);
			}


		void worker_scheduler::mark_assigned(const work_assignment& assignment)
			{
				// check that there are unassigned workers
				if(this->unassigned == 0) throw runtime_exception(exception_type::SCHEDULING_ERROR, CPPTRANSPORT_SCHEDULING_NO_UNASSIGNED);

				// if this worker is already assigned, then an error must have occurred
				if(this->worker_data[assignment.get_worker()].is_assigned()) throw runtime_exception(exception_type::SCHEDULING_ERROR, CPPTRANSPORT_SCHEDULING_ALREADY_ASSIGNED);

				// mark this worker as assigned
				this->worker_data[assignment.get_worker()].mark_assigned(true);
				this->unassigned--;

				// remove assigned work items from the queue
        for(const unsigned int& item : assignment.get_items())
					{
						// we're guaranteed only one instance of this work item exists in the queue
				    std::list<unsigned int>::iterator u = std::find(this->queue.begin(), this->queue.end(), item);

						if(u == this->queue.end())
							{
						    std::ostringstream msg;
								msg << CPPTRANSPORT_SCHEDULING_ASSIGN_NOT_EXIST << " " << item << ", " << CPPTRANSPORT_SCHEDULING_ASSIGN_WORKER << " " << assignment.get_worker();
								throw runtime_exception(exception_type::SCHEDULING_ERROR, msg.str());
							}
						else
							{
								this->queue.erase(u);
								this->work_items_in_flight++;
							}
					}
			}


		void worker_scheduler::mark_unassigned(unsigned int worker, boost::timer::nanosecond_type time, unsigned int items)
			{
        if(worker >= this->worker_data.size()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_SCHEDULING_INDEX_OUT_OF_RANGE);
        
				// if this worker is not already assigned, an error must have occurred
				if(!this->worker_data[worker].is_assigned()) throw runtime_exception(exception_type::SCHEDULING_ERROR, CPPTRANSPORT_SCHEDULING_NOT_ALREADY_ASSIGNED);

				this->worker_data[worker].update_timing_data(time, items);
				this->worker_data[worker].mark_assigned(false);
				this->unassigned++;

				this->work_items_completed += items;
		    if(this->work_items_in_flight >= items)
			    {
		        this->work_items_in_flight -= items;
			    }
		    else
			    {
				    throw runtime_exception(exception_type::SCHEDULING_ERROR, CPPTRANSPORT_SCHEDULING_OVERRELEASE_INFLIGHT);
			    }

				this->total_work_time += time;
			}


		void worker_scheduler::mark_inactive(unsigned int worker)
			{
        if(worker >= this->worker_data.size()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_SCHEDULING_INDEX_OUT_OF_RANGE);

				// if this worker is already inactive, an error must have occurred
				if(!this->worker_data[worker].is_active()) throw runtime_exception(exception_type::SCHEDULING_ERROR, CPPTRANSPORT_SCHEDULING_ALREADY_INACTIVE);

				this->worker_data[worker].mark_active(false);
				this->active--;

				if(this->active == 0 && this->work_items_in_flight > 0) throw runtime_exception(exception_type::SCHEDULING_ERROR, CPPTRANSPORT_SCHEDULING_UNDER_INFLIGHT);
			}
    
    
    const worker_information& worker_scheduler::operator[](unsigned int worker) const
      {
        if(worker >= this->worker_data.size()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_SCHEDULING_INDEX_OUT_OF_RANGE);
        
        return this->worker_data[worker];
      }


		void worker_scheduler::report_aggregation(boost::timer::nanosecond_type time)
			{
				this->total_aggregation_time += time;
				this->number_aggregations++;

		    boost::timer::nanosecond_type mean_aggregation_time = this->total_aggregation_time / this->number_aggregations;

				// we want to allocate work in chunks large enough that they last
				// many times the time required to aggregate, otherwise the workers will spend
				// a lot of time waiting around for the master to process MPI messages
				// in between aggregations

        // note we don't have to worry about how many items are left;
        // the scheduler will never allocate more than an equitable amount of work per-worker,
        // no matter how large the granularity becomes
				if(10*mean_aggregation_time > CPPTRANSPORT_DEFAULT_SCHEDULING_GRANULARITY)
					{
						this->current_granularity = 10*mean_aggregation_time;
					}
				else
					{
						this->current_granularity = CPPTRANSPORT_DEFAULT_SCHEDULING_GRANULARITY;
					}
			}


    bool worker_scheduler::update_available()
      {
        bool result = false;

        if(this->timer.elapsed().wall > CPPTRANSPORT_SCHEDULER_MINIMUM_UPDATE_TIME)
          {
            if(this->update_stack.size() > 0)     // any updates remaining in the queue?
              {
                if(this->update_stack.front() > this->queue.size() +
                                                this->work_items_in_flight)   // wait until items remaining (including those in flight) is small enough
                  {
                    result = true;
                  }
              }

            if(this->queue.size() == 0 && !this->finished)
              {
                result = true;
              }
          }

        return result;
      }


    void worker_scheduler::populate_update_information(reporting::key_value& updates, reporting::key_value& notifications)
			{
        updates.reset();

				if(this->timer.elapsed().wall > CPPTRANSPORT_SCHEDULER_MINIMUM_UPDATE_TIME)
					{
						if(this->update_stack.size() > 0)     // any updates remaining in the queue?
							{
								if(this->update_stack.front() > this->queue.size() + this->work_items_in_flight)   // wait until items remaining (including those in flight) is small enough
									{
                    // pop work items from front of queue, in case we need to skip multiple updates
										while(this->update_stack.size() > 0 && this->update_stack.front() > this->queue.size() + this->work_items_in_flight)
											{
												this->update_stack.pop_front();
											}

								    std::ostringstream complete_msg;
										complete_msg << std::setprecision(3);
										complete_msg << 100.0 * (static_cast<double>(this->work_items_completed)
											/ (static_cast<double>(this->work_items_completed + this->work_items_in_flight + this->queue.size()))) << "%";

                    updates.insert_back(CPPTRANSPORT_WORKER_SCHEDULER_WORK_ITEMS_PROCESSED, boost::lexical_cast<std::string>(this->work_items_completed));
                    updates.insert_back(CPPTRANSPORT_WORKER_SCHEDULER_WORK_ITEMS_INFLIGHT, boost::lexical_cast<std::string>(this->work_items_in_flight));
                    updates.insert_back(CPPTRANSPORT_WORKER_SCHEDULER_REMAIN, boost::lexical_cast<std::string>(this->queue.size()));
                    updates.insert_back(CPPTRANSPORT_WORKER_SCHEDULER_COMPLETE, complete_msg.str());

								    boost::timer::nanosecond_type mean_time_per_item = this->total_work_time / this->work_items_completed;
                    updates.insert_back(CPPTRANSPORT_WORKER_SCHEDULER_MEAN_TIME_PER_ITEM, format_time(mean_time_per_item));
                    updates.insert_back(CPPTRANSPORT_WORKER_SCHEDULER_TARGET_DURATION, format_time(this->current_granularity));

								    boost::timer::nanosecond_type total_wallclock_time         = this->timer.elapsed().wall;
								    boost::timer::nanosecond_type mean_wallclock_time_per_item = total_wallclock_time / this->work_items_completed;
								    boost::timer::nanosecond_type estimated_time_remaining     = mean_wallclock_time_per_item * static_cast<unsigned int>(this->queue.size() + this->work_items_in_flight);

                    boost::posix_time::time_duration duration = boost::posix_time::seconds(estimated_time_remaining / (1000 * 1000 * 1000));

                    // update cached expected completion time and expected total CPU time
                    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
								    this->estimated_completion = now + duration;
                    this->estimated_cpu_time = this->total_work_time + mean_time_per_item * (this->work_items_in_flight + this->queue.size());

                    std::ostringstream estimate_msg;
                    estimate_msg << boost::posix_time::to_simple_string(this->estimated_completion) << " ("
                      << format_time(estimated_time_remaining) << " " << CPPTRANSPORT_WORKER_SCHEDULER_FROM_NOW << ")";

                    notifications.insert_back(CPPTRANSPORT_WORKER_SCHEDULER_COMPLETION_ESTIMATE, estimate_msg.str());
                    notifications.insert_back(CPPTRANSPORT_WORKER_SCHEDULER_CPU_TIME_ESTIMATE, format_time(this->estimated_cpu_time));
									}
							}

            if(this->queue.size() == 0 && !this->finished)
              {
                this->finished = true;

                boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
                notifications.insert_back(CPPTRANSPORT_WORKER_SCHEDULER_WORK_COMPLETE, boost::posix_time::to_simple_string(now));
              }
					}
			}


		std::list<work_assignment> worker_scheduler::assign_work(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log)
			{
		    // generate a work assignment

		    if(this->has_cpus && !this->has_gpus)
			    {
		        // CPU only scheduling strategy
		        return this->assign_work_cpu_only_strategy(log);
			    }
		    else if(this->has_gpus && !this->has_cpus)
			    {
		        // GPU only scheduling strategy
		        return this->assign_work_gpu_only_strategy(log);
			    }
		    else
			    {
		        // mixed CPU & GPU scheduling strategy
		        return this->assign_work_mixed_strategy(log);
			    }
			}


		std::list<work_assignment> worker_scheduler::assign_work_cpu_only_strategy(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log)
			{
				// schedule work for a CPU only pool
				// the strategy is to avoid cores becoming idle because they have run out of work

		    // build a list of workers requiring assignments
		    // storing a list of iterators is OK here; they would be invalidated by operations on this->worker_data,
		    // but we won't be amending it within this function
		    std::list< std::vector<worker_information>::iterator > workers;

		    for(std::vector<worker_information>::iterator t = this->worker_data.begin(); t != this->worker_data.end(); ++t)
			    {
		        if(!t->is_assigned()) workers.push_back(t);
			    }

				// sort into ascending order of mean time per item
				struct MeanTimeComparator
					{
						bool operator()(const std::vector<worker_information>::iterator& A, const std::vector<worker_information>::iterator& B)
							{
								if(A->get_total_time() == 0) return(true);
								if(B->get_total_time() == 0) return(false);

								// at this stage, getting mean time per worker should be safe -- no divide by zero possibility
						    return(A->get_mean_time_per_work_item() < B->get_mean_time_per_work_item());
							}
					};
				workers.sort(MeanTimeComparator());

				// step through list of workers requiring assignments, assigning work to the fastest first.
				// Note that workers who have not yet had any assignments will be at the top of the queue
		    std::list<work_assignment> assignment_list;

				// try to prevent large chunks of work from being allocated -- this can mean one worker gobbles up all the work items,
        // leaving other workers idle
        // worker_scale is the geometric mean of (i) the number of workers requiring new assignments, and (ii) the total number
        // of workers
        // the maximum allocation per worker is computed by dividing the remaining number of items in the queue by this
        // geometric mean
				assert(workers.size() > 0);
				double worker_scale = sqrt(workers.size() * this->worker_data.size());

				unsigned int max_allocation_per_worker = std::max(static_cast<unsigned int>(1), static_cast<unsigned int>(floor(static_cast<double>(this->queue.size()) / worker_scale)));

				// set up an iterator to point at the next item of work
		    std::list<unsigned int>::iterator next_item = this->queue.begin();

				// loop through workers, allocating work from the queue
#ifdef CPPTRANSPORT_DEBUG_SCHEDULER
				BOOST_LOG_SEV(log, generic_writer::normal) << "%% BEGIN NEW SCHEDULE (max work allocation=" << this->max_work_allocation << ", max allocation per worker=" << max_allocation_per_worker << ")";
#endif
				for(typename std::list< std::vector<worker_information>::iterator >::iterator t = workers.begin(); next_item != this->queue.end() && t != workers.end(); ++t)
					{
				    std::list<unsigned int> items;

						// is this a worker which has not yet had any assignment?
						if((*t)->get_total_time() == 0)
							{
#ifdef CPPTRANSPORT_DEBUG_SCHEDULER
								BOOST_LOG_SEV(log, generic_writer::normal) << "%% Worker " << (*t)->get_number()+1 << " has not yet been allocated work; allocating 1 item";
#endif
								// if so, assign just a single work item to get a sense of how long it takes this worker to process
								items.push_back(*next_item);
								next_item++;
							}
						else
							{
								// allocate enough work items to fill up the current scheduling granularity (begins at 60 seconds)
								// or the mean allocation per worker, whichever is smaller
						    boost::timer::nanosecond_type time_per_item             = (*t)->get_mean_time_per_work_item();
						    boost::timer::nanosecond_type items_per_granularity     = time_per_item > 0 ? this->current_granularity / time_per_item : 1.0;
						    unsigned int                  int_items_per_granularity = std::max(static_cast<unsigned int>(1), static_cast<unsigned int>(floor(items_per_granularity)));

                // unit of work is the smallest of (i) the current maximum allocation, currently fixed at 1/5 of the
                // original queue size, and (ii) the number of items needed to fill
                // out the current scheduling granularity
								unsigned int unit_of_work = std::min(this->max_work_allocation, int_items_per_granularity);
								if(unit_of_work == 0) unit_of_work = 1;

                // actual number of work items allocated is the smallest of the unit of work and
                // the maximum permitted allocation based on the current queue size

                // this means there are two caps on the number of work items a worker can be allocated
                // 1 - the maximum allocation of 1/5 original queue size, fixed at the beginning (alterable in principle but not used in current implementation)
                // 2 - the maximum allocation taking into account current queue size and number of workers needing new jobs
								unsigned int num_work_items = std::min(unit_of_work, max_allocation_per_worker);

#ifdef CPPTRANSPORT_DEBUG_SCHEDULER
								BOOST_LOG_SEV(log, generic_writer::normal) << "%% Worker " << (*t)->get_number()+1 << " mean time-per-item = " << format_time(time_per_item)
										<< " -> granularity = " << granularity_int
										<< ". Allocated " << num_work_items << " items";
#endif

								for(unsigned int i = 0; next_item != this->queue.end() && i < num_work_items; ++i)
									{
										items.push_back(*next_item);
										next_item++;
									}
							}

						assignment_list.push_back(work_assignment((*t)->get_number(), items));
					}

				return(assignment_list);
			}


		std::list<work_assignment> worker_scheduler::assign_work_gpu_only_strategy(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log)
			{
				// currently we schedule work just by breaking it up between all workers
				// TODO: in future, this should be replaced by a more intelligent scheduler

				// build a list of workers requiring assignments
				// storing a list of iterators is OK here; they would be invalidated by operations on this->worker_data,
				// but we won't be amending it within this function
		    std::list< std::vector<worker_information>::iterator > workers;

				for(std::vector<worker_information>::iterator t = this->worker_data.begin(); t != this->worker_data.end(); ++t)
					{
						if(!t->is_assigned()) workers.push_back(t);
					}

				if(workers.size() != this->unassigned) throw runtime_exception(exception_type::SCHEDULING_ERROR, CPPTRANSPORT_SCHEDULING_UNASSIGNED_MISMATCH);

				// divide available work items between all unassigned workers
				unsigned int items_per_worker = this->queue.size() / workers.size();
				unsigned int items_left_over  = this->queue.size() % workers.size();

		    std::list<work_assignment> assignment_list;

				unsigned int c = 0;
		    std::list<unsigned int>::iterator next_item = this->queue.begin();

				for(typename std::list< std::vector<worker_information>::iterator >::iterator t = workers.begin(); next_item != this->queue.end() && t != workers.end(); ++t, ++c)
					{
				    std::list<unsigned int> items;

						for(unsigned int i = 0; next_item != this->queue.end() && i < items_per_worker + (c < items_left_over ? 1 : 0); ++i)
							{
								items.push_back(*next_item);
								next_item++;
							}

						assignment_list.push_back(work_assignment((*t)->get_number(), items));
					}

				return(assignment_list);
			}


    std::list<work_assignment> worker_scheduler::assign_work_mixed_strategy(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log)
	    {
				throw runtime_exception(exception_type::RUNTIME_ERROR, "Mixed CPU/GPU scheduling is not yet implemented");
	    }


	}   // namespace transport


#endif //CPPTRANSPORT_MASTER_SCHEDULER_H
