//
// Created by David Seery on 03/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __work_journal_H_
#define __work_journal_H_


#include <list>


#include "boost/date_time.hpp"


namespace transport
	{

		//! work_events record events
		class work_event
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! Create a work event for a given worker
				work_event();

				//! Create a work event for a given number, and a given timestamp
				work_event(boost::posix_time::ptime& t);

				virtual ~work_event() = default;


				// INTERFACE

		  public:

				//! Is this a master-record object?
				virtual bool is_master() const { return(false); };

				// PRIVATE DATA

		  private:

				//! cache time this work event was journaled
				boost::posix_time::ptime timestamp;

			};


		work_event::work_event()
			: timestamp(boost::posix_time::second_clock::universal_time())
			{
			}


		work_event::work_event(boost::posix_time::ptime& t)
			: timestamp(t)
			{
			}


		//! work events, specialized to master events
		class master_work_event: public work_event
			{

		  public:

				typedef enum
					{
				    aggregate_begin, aggregate_end
					} event_type;

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				master_work_event(event_type t);

				master_work_event(event_type t, boost::posix_time::ptime ts);

				virtual ~master_work_event() = default;


				// INTERFACE

		  public:

				virtual bool is_master() const override { return(true); }


				// PRIVATE DATA

		  private:

				//! cache event type
				event_type type;

			};


		master_work_event::master_work_event(typename master_work_event::event_type t)
			: work_event(),
			  type(t)
			{
			}


		master_work_event::master_work_event(typename master_work_event::event_type t, boost::posix_time::ptime ts)
			: work_event(ts),
				type(t)
			{
			}


		//! work events, specialized to slave events
		class slave_work_event: public work_event
			{

		  public:

				typedef enum
					{
				    begin_twopf_assignment, end_twopf_assignment,
						begin_threepf_assignment, end_threepf_assignment,
						begin_zeta_twopf_assignment, end_zeta_twopf_assignment,
						begin_zeta_threepf_assignment, end_zeta_threepf_assignment,
						begin_fNL_assignment, end_fNL_assignment,
						begin_output_assignment, end_output_assignment
					} event_type;

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				slave_work_event(unsigned int w, event_type t);

				slave_work_event(unsigned int w, event_type t, boost::posix_time::ptime ts);

				virtual ~slave_work_event() = default;


				// INTERFACE

				//! return worker number
				unsigned int get_worker_number() const { return(this->worker_number); }


				// PRIVATE DATA

		  private:

				//! cache event type
				event_type type;

		    //! cache worker number
		    unsigned int worker_number;

			};


		slave_work_event::slave_work_event(unsigned int w, typename slave_work_event::event_type t)
			: work_event(),
				type(t),
				worker_number(w)
			{
			}


    slave_work_event::slave_work_event(unsigned int w, typename slave_work_event::event_type t, boost::posix_time::ptime ts)
	    : work_event(ts),
	      type(t),
	      worker_number(w)
	    {
	    }


    //! a work journal records events in the timeline of a process,
    //! at a worker-level granularity.
		class work_journal
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! set up a work journal for N_workers workers (plus the master process)
				work_journal(unsigned int N);

				//! destructor
				~work_journal() = default;


				// INTERFACE -- JOURNAL ENTRIES

				//! push item to the journal
				void add_entry(const work_event& w);


				// PRIVATE DATA

		  private:

				//! cache number of workers
				unsigned int N_workers;

				//! journal
				std::list< work_event > journal;

			};


		work_journal::work_journal(unsigned int N)
			: N_workers(N)
			{
			}


		void work_journal::add_entry(const work_event& w)
			{
				this->journal.push_back(w);
			}


	}   // namespace transport


#endif //__work_journal_H_
