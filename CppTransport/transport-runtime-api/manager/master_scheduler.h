//
// Created by David Seery on 22/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __master_scheduler_H_
#define __master_scheduler_H_


#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <cmath>

#include "transport-runtime-api/manager/mpi_operations.h"

#include "transport-runtime-api/repository/writers/generic_writer.h"

#include "transport-runtime-api/utilities/formatter.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/messages.h"

#include "boost/timer/timer.hpp"


// target work assignment of 1 minute's worth of work, expressed in nanosecond
// 1 microsecond = 1000 nanosecond
// 1 millisecond = 1000 microsecond
// 1 second = 1000 milisecond
// 1 minute = 60 seconds
#define __CPP_TRANSPORT_DEFAULT_SCHEDULING_GRANULARITY (boost::timer::nanosecond_type(1)*60*1000*1000*1000)

#define __CPP_TRANSPORT_SCHEDULER_MINIMUM_UPDATE_TIME  (boost::timer::nanosecond_type(5)*60*1000*1000*1000)

//#define __CPP_TRANSPORT_DEBUG_SCHEDULER


namespace transport
	{

    class master_scheduler
	    {

      public:

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

      public:

        //! Labels for types of workers
        typedef enum { cpu, gpu } worker_type;

        //! Worker information class
        class worker_information
	        {

          public:

            //! construct a worker information record
            worker_information()
	            : type(cpu),
	              capacity(0),
	              priority(0),
	              initialized(false),
	              assigned(false),
	              active(true),
                items(0),
                time(0)
	            {
	            }

            // INTERFACE

          public:

            //! get worker type
            worker_type get_type() const { return(this->type); }

		        //! get worker number
		        unsigned int get_number() const { return(this->number); }

            //! get worker capacity
            unsigned int get_capacity() const { return(this->capacity); }

		        //! is this worker currently assigned?
		        bool is_assigned() const { return(this->assigned); }

		        //! set assignment status
		        void mark_assigned(bool status) { this->assigned = status; }

		        //! is this worker currently active?
		        bool is_active() const { return(this->active); }

		        //! set active states
		        void mark_active(bool status) { this->active = status; }

            //! get worker priority
            unsigned int get_priority() const { return(this->priority); }

            //! get initialization status
            bool get_initialization_status() const { return(this->initialized); }

            //! set data
            void set_data(unsigned int n, worker_type t, unsigned int c, unsigned int p) { this->number = n; this->type = t; this->capacity = c; this->priority = p; this->initialized = true; }

		        //! update timing data
		        void update_timing_data(boost::timer::nanosecond_type t, unsigned int n) { this->time += t; this->items += n; }

		        // get total time for this worker
		        boost::timer::nanosecond_type get_total_time() const { return(this->time); }

		        //! get running mean time per work item for this workers
		        boost::timer::nanosecond_type get_mean_time_per_work_item() const { return(this->items == 0 ? this->time : this->time / this->items); }

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

		        //! total time to process items on this worker
		        boost::timer::nanosecond_type time;

		        //! total number of items processed on this worker
		        unsigned int items;

	        };


		    // CONSTRUCTOR, DESTRUCTOR

      public:

		    //! construct a scheduler object
		    master_scheduler()
		      : waiting_for_setup(0),
		        state_size(0),
		        unassigned(0),
		        active(0),
		        has_cpus(false),
		        has_gpus(false),
		        max_work_allocation(1),
		        current_granularity(__CPP_TRANSPORT_DEFAULT_SCHEDULING_GRANULARITY),
		        total_aggregation_time(0),
		        number_aggregations(0),
		        total_work_time(0),
		        number_work(0)
			    {
			    }

		    ~master_scheduler() = default;


		    // INTERFACE -- GENERAL FUNCTIONS

      public:

		    //! reset scheduler and all scheduling data; prepare for new scheduling
		    //! task with the given number of workers
		    void reset(unsigned int worker);

		    //! initialization complete and ready to proceed with scheduling?
		    bool ready() const { return(this->waiting_for_setup == 0); }

		    //! initialize a worker
		    //! reduces count of workers waiting for initialization if successful, and logs the data using the supplied WriterObject
		    //! otherwise, logs an error.
		    template <typename number, typename WriterObject>
		    void initialize_worker(WriterObject& writer, unsigned int worker, MPI::slave_information_payload& payload);

		    //! set current state; used when assigning work to GPUs
		    void set_state_size(unsigned int size) { this->state_size = size; }

        //! advise a new aggregation time
        void report_aggregation(boost::timer::nanosecond_type time);


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

		    //! current queue exhausted? ie., finished all current work?
		    bool finished() const { return(this->queue.size() == 0); }

		    //! get remaining queue size
		    unsigned int get_queue_size() const { return(this->queue.size()); }

		    //! finalize queue setup; should be called before generating work assignments
		    void complete_queue_setup();

		    //! check for an update string
		    bool generate_update_string(std::string& msg);

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


		    // INTERNAL API

      protected:

		    //! build a work queue from a list of work items
		    template <typename WorkItem>
		    void build_queue(const std::vector<WorkItem>& q);


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


		    // STATUS AND TIME-TO-COMPLETION

		    //! Keep track of time
		    boost::timer::cpu_timer timer;

		    //! Keep track of total time spent doing work, to estimate a time-to-completion
		    boost::timer::nanosecond_type total_work_time;

		    //! Keep track of total number of work items, to estimate a time-to-complation
		    unsigned number_work;

		    //! Points at which to emit updates
		    std::list< unsigned int > update_stack;

	    };


		void master_scheduler::reset(unsigned int workers)
			{
				this->worker_data.clear();
				this->worker_data.resize(workers);

				this->waiting_for_setup = workers;
				this->unassigned = 0;
				this->active = 0;

				this->has_cpus = false;
				this->has_gpus = false;

				this->update_stack.clear();
			}


		template <typename number, typename WriterObject>
		void master_scheduler::initialize_worker(WriterObject& writer, unsigned int worker, MPI::slave_information_payload& payload)
			{
		    if(!(this->worker_data[worker].get_initialization_status()))
			    {
		        worker_type type = cpu;
		        if(payload.get_type() == MPI::slave_information_payload::cpu)
			        {
		            type = cpu;
				        this->has_cpus = true;
			        }
		        else if(payload.get_type() == MPI::slave_information_payload::gpu)
			        {
		            type = gpu;
				        this->has_gpus = true;
			        }

		        this->worker_data[worker].set_data(worker, type, payload.get_capacity(), payload.get_priority());
				    this->waiting_for_setup--;

		        std::ostringstream msg;
		        msg << "** Worker " << worker << " identified as ";
		        if(type == cpu)
			        {
		            msg << "CPU";
			        }
		        else if(type == gpu)
			        {
		            msg << "GPU, available memory = " << format_memory(payload.get_capacity());
			        }

		        msg << " and priority " << payload.get_priority();

		        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << msg.str();

				    this->unassigned++;
				    this->active++;
			    }
		    else
			    {
		        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "!! Unexpected double identification for worker  " << worker;
			    }

			}


		template <typename number>
		void master_scheduler::prepare_queue(twopf_task<number>& task)
			{
				this->build_queue(task.get_twopf_kconfig_list());
			}


		template <typename number>
		void master_scheduler::prepare_queue(threepf_task<number>& task)
			{
				this->build_queue(task.get_threepf_kconfig_list());
			}


		template <typename number>
		void master_scheduler::prepare_queue(zeta_twopf_task<number>& task)
			{
				this->build_queue(task.get_twopf_kconfig_list());
			}


		template <typename number>
		void master_scheduler::prepare_queue(zeta_threepf_task<number>& task)
			{
				this->build_queue(task.get_threepf_kconfig_list());
			}


		template <typename number>
		void master_scheduler::prepare_queue(output_task<number>& task)
			{
				this->build_queue(task.get_elements());
			}


		template <typename WorkItem>
		void master_scheduler::build_queue(const std::vector<WorkItem>& q)
			{
				this->queue.clear();

				// build a queue of work items from the serial numbers of each work item
				for(typename std::vector<WorkItem>::const_iterator t = q.begin(); t != q.end(); t++)
					{
						this->queue.push_back(t->get_serial());
					}

				// sort into ascending order of serial number, and remove any duplicates
				// (note duplicate removal using unique() requires a sorted list)
				this->queue.sort();
				this->queue.unique();
			}


		void master_scheduler::complete_queue_setup()
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
				unsigned int count = update_interval;
				while(count < this->queue.size())
					{
						this->update_stack.push_back(count);
						count += update_interval;
					}
			}


		bool master_scheduler::assignable() const
			{
				// are there unassigned workers and work items left for them to process?
				return(this->queue.size() > 0 && this->unassigned > 0);
			}


		void master_scheduler::mark_assigned(const work_assignment& assignment)
			{
				// check that there are unassigned workers
				if(this->unassigned == 0) throw runtime_exception(runtime_exception::SCHEDULING_ERROR, __CPP_TRANSPORT_SCHEDULING_NO_UNASSIGNED);

				// if this worker is already assigned, then an error must have occurred
				if(this->worker_data[assignment.get_worker()].is_assigned()) throw runtime_exception(runtime_exception::SCHEDULING_ERROR, __CPP_TRANSPORT_SCHEDULING_ALREADY_ASSIGNED);

				// mark this worker as assigned
				this->worker_data[assignment.get_worker()].mark_assigned(true);
				this->unassigned--;

				// remove assigned work items from the queue
				for(std::list<unsigned int>::const_iterator t = assignment.get_items().begin(); t != assignment.get_items().end(); t++)
					{
						// we're guaranteed only one instance of this work item exists in the queue
				    std::list<unsigned int>::iterator u = std::find(this->queue.begin(), this->queue.end(), *t);

						if(u == this->queue.end())
							{
						    std::ostringstream msg;
								msg << __CPP_TRANSPORT_SCHEDULING_ASSIGN_NOT_EXIST << " " << *t << ", " << __CPP_TRANSPORT_SCHEDULING_ASSIGN_WORKER << " " << assignment.get_worker();
								throw runtime_exception(runtime_exception::SCHEDULING_ERROR, msg.str());
							}
						else
							{
								this->queue.erase(u);
							}
					}
			}


		void master_scheduler::mark_unassigned(unsigned int worker, boost::timer::nanosecond_type time, unsigned int items)
			{
				// if this worker is not already assigned, an error must have occurred
				if(!this->worker_data[worker].is_assigned()) throw runtime_exception(runtime_exception::SCHEDULING_ERROR, __CPP_TRANSPORT_SCHEDULING_NOT_ALREADY_ASSIGNED);

				this->worker_data[worker].update_timing_data(time, items);
				this->worker_data[worker].mark_assigned(false);
				this->unassigned++;

				this->number_work += items;
				this->total_work_time += time;
			}


		void master_scheduler::mark_inactive(unsigned int worker)
			{
				// if this worker is already inactive, an error must have occurred
				if(!this->worker_data[worker].is_active()) throw runtime_exception(runtime_exception::SCHEDULING_ERROR, __CPP_TRANSPORT_SCHEDULING_ALREADY_INACTIVE);

				this->worker_data[worker].mark_active(false);
				this->active--;
			}


		void master_scheduler::report_aggregation(boost::timer::nanosecond_type time)
			{
				this->total_aggregation_time += time;
				this->number_aggregations++;

		    boost::timer::nanosecond_type mean_aggregation_time = this->total_aggregation_time / this->number_aggregations;

				// we want to allocate work in chunks large enough that they last
				// many times the time required to aggregate, otherwise the workers will spend
				// a lot of time waiting around for the master to process MPI messages
				// in between aggregations
				if(10*mean_aggregation_time > __CPP_TRANSPORT_DEFAULT_SCHEDULING_GRANULARITY)
					{
						this->current_granularity = 10*mean_aggregation_time;
					}
				else
					{
						this->current_granularity = __CPP_TRANSPORT_DEFAULT_SCHEDULING_GRANULARITY;
					}
			}


		bool master_scheduler::generate_update_string(std::string& msg)
			{
				bool result = false;
				msg.clear();

				if(this->timer.elapsed().wall > __CPP_TRANSPORT_SCHEDULER_MINIMUM_UPDATE_TIME)
					{
						if(this->update_stack.size() > 0)
							{
								if(this->update_stack.front() < this->queue.size())
									{
										result = true;
										while(this->update_stack.size() > 0 && this->update_stack.front() < this->queue.size())
											{
												this->update_stack.pop_front();
											}

								    std::ostringstream percent_stream;
										percent_stream << std::setprecision(3);
										percent_stream << 100.0 * (static_cast<double>(this->number_work) / (static_cast<double>(this->number_work + this->queue.size()))) << "%";

								    std::ostringstream msg_stream;
										msg_stream << this->number_work << " " << __CPP_TRANSPORT_MASTER_SCHEDULER_WORK_ITEMS_PROCESSED
											<< ", " << this->queue.size() << " " << __CPP_TRANSPORT_MASTER_SCHEDULER_REMAIN
											<< " (~" << percent_stream.str() << " " << __CPP_TRANSPORT_MASTER_SCHEDULER_COMPLETE << ")";

								    boost::timer::nanosecond_type mean_time_per_item = this->total_work_time / this->number_work;
										msg_stream << " | " << __CPP_TRANSPORT_MASTER_SCHEDULER_MEAN_TIME_PER_ITEM << " " << format_time(mean_time_per_item);

										msg_stream << " | " << __CPP_TRANSPORT_MASTER_SCHEDULER_TARGET_DURATION << " " << format_time(this->current_granularity);

								    boost::timer::nanosecond_type total_wallclock_time         = this->timer.elapsed().wall;
								    boost::timer::nanosecond_type mean_wallclock_time_per_item = total_wallclock_time / this->number_work;
								    boost::timer::nanosecond_type estimated_time_remaining     = mean_wallclock_time_per_item * static_cast<unsigned int>(this->queue.size());

								    boost::posix_time::time_duration duration        = boost::posix_time::seconds(estimated_time_remaining / (1000 * 1000 * 1000));
								    boost::posix_time::ptime         now             = boost::posix_time::second_clock::local_time();
								    boost::posix_time::ptime         completion_time = now + duration;

										msg_stream << " | " << __CPP_TRANSPORT_MASTER_SCHEDULER_COMPLETION_ESTIMATE << " "
											<< boost::posix_time::to_simple_string(completion_time) << " ("
											<< format_time(estimated_time_remaining) << ")";

										msg = msg_stream.str();
									}
							}
					}

				return(result);
			}


		std::list<master_scheduler::work_assignment> master_scheduler::assign_work(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log)
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


		std::list<master_scheduler::work_assignment> master_scheduler::assign_work_cpu_only_strategy(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log)
			{
				// schedule work for a CPU only pool
				// the strategy is to avoid cores becoming idle because they have run out of work

		    // build a list of workers requiring assignments
		    // storing a list of iterators is OK here; they would be invalidated by operations on this->worker_data,
		    // but we won't be amending it within this function
		    std::list< std::vector<master_scheduler::worker_information>::iterator > workers;

		    for(std::vector<master_scheduler::worker_information>::iterator t = this->worker_data.begin(); t != this->worker_data.end(); t++)
			    {
		        if(!t->is_assigned()) workers.push_back(t);
			    }

				// sort into ascending order of mean time per item
				struct MeanTimeComparator
					{
						bool operator()(const std::vector<master_scheduler::worker_information>::iterator& A, const std::vector<master_scheduler::worker_information>::iterator& B)
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

				// try to prevent large chunks of work from being allocate
				assert(workers.size() > 0);
				double worker_scale = sqrt(workers.size() * this->worker_data.size());
				unsigned int max_allocation_per_worker = std::max(static_cast<unsigned int>(1), static_cast<unsigned int>(floor(static_cast<double>(this->queue.size()) / worker_scale)));

				// set up an iterator to point at the next item of work
		    std::list<unsigned int>::iterator next_item = this->queue.begin();

				// loop through workers, allocating work from the queue
#ifdef __CPP_TRANSPORT_DEBUG_SCHEDULER
				BOOST_LOG_SEV(log, generic_writer::normal) << "%% BEGIN NEW SCHEDULE (max work allocation=" << this->max_work_allocation << ", max allocation per worker=" << max_allocation_per_worker << ")";
#endif
				for(typename std::list< std::vector<master_scheduler::worker_information>::iterator >::iterator t = workers.begin(); next_item != this->queue.end() && t != workers.end(); t++)
					{
				    std::list<unsigned int> items;

						// is this a worker which has not yet had any assignment?
						if((*t)->get_total_time() == 0)
							{
#ifdef __CPP_TRANSPORT_DEBUG_SCHEDULER
								BOOST_LOG_SEV(log, generic_writer::normal) << "%% Worker " << (*t)->get_number()+1 << " has not yet been allocated work; allocating 1 item";
#endif
								// if so, assign just a single work item to get a sense of how long it takes this worker to process
								items.push_back(*next_item);
								next_item++;
							}
						else
							{
								// allocate up to __CPP_TRANSPORT_DEFAULT_SCHEDULING_GRANULARITY of work,
								// or the mean allocation per worker, whichever is smaller
						    boost::timer::nanosecond_type time_per_item   = (*t)->get_mean_time_per_work_item();
						    boost::timer::nanosecond_type granularity     = time_per_item > 0 ? this->current_granularity / (*t)->get_mean_time_per_work_item() : 1.0;
						    unsigned int                  granularity_int = std::max(static_cast<unsigned int>(1), static_cast<unsigned int>(floor(granularity)));

								unsigned int unit_of_work = std::min(this->max_work_allocation, granularity_int);
								if(unit_of_work == 0) unit_of_work = 1;

								unsigned int num_work_items = std::min(unit_of_work, max_allocation_per_worker);

#ifdef __CPP_TRANSPORT_DEBUG_SCHEDULER
								BOOST_LOG_SEV(log, generic_writer::normal) << "%% Worker " << (*t)->get_number()+1 << " mean time-per-item = " << format_time(time_per_item)
										<< " -> granularity = " << granularity_int
										<< ". Allocated " << num_work_items << " items";
#endif

								for(unsigned int i = 0; next_item != this->queue.end() && i < num_work_items; i++)
									{
										items.push_back(*next_item);
										next_item++;
									}
							}

						assignment_list.push_back(master_scheduler::work_assignment((*t)->get_number(), items));
					}

				return(assignment_list);
			}


		std::list<master_scheduler::work_assignment> master_scheduler::assign_work_gpu_only_strategy(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log)
			{
				// currently we schedule work just by breaking it up between all workers
				// TODO: in future, this should be replaced by a more intelligent scheduler

				// build a list of workers requiring assignments
				// storing a list of iterators is OK here; they would be invalidated by operations on this->worker_data,
				// but we won't be amending it within this function
		    std::list< std::vector<master_scheduler::worker_information>::iterator > workers;

				for(std::vector<master_scheduler::worker_information>::iterator t = this->worker_data.begin(); t != this->worker_data.end(); t++)
					{
						if(!t->is_assigned()) workers.push_back(t);
					}

				if(workers.size() != this->unassigned) throw runtime_exception(runtime_exception::SCHEDULING_ERROR, __CPP_TRANSPORT_SCHEDULING_UNASSIGNED_MISMATCH);

				// divide available work items between all unassigned workers
				unsigned int items_per_worker = this->queue.size() / workers.size();
				unsigned int items_left_over  = this->queue.size() % workers.size();

		    std::list<work_assignment> assignment_list;

				unsigned int c = 0;
		    std::list<unsigned int>::iterator next_item = this->queue.begin();

				for(typename std::list< std::vector<master_scheduler::worker_information>::iterator >::iterator t = workers.begin(); next_item != this->queue.end() && t != workers.end(); t++, c++)
					{
				    std::list<unsigned int> items;

						for(unsigned int i = 0; next_item != this->queue.end() && i < items_per_worker + (c < items_left_over ? 1 : 0); i++)
							{
								items.push_back(*next_item);
								next_item++;
							}

						assignment_list.push_back(master_scheduler::work_assignment((*t)->get_number(), items));
					}

				return(assignment_list);
			}


    std::list<master_scheduler::work_assignment> master_scheduler::assign_work_mixed_strategy(boost::log::sources::severity_logger<generic_writer::log_severity_level>& log)
	    {
				throw runtime_exception(runtime_exception::RUNTIME_ERROR, "Mixed CPU/GPU scheduling is not yet implemented");
	    }


	}   // namespace transport


#endif //__master_scheduler_H_
