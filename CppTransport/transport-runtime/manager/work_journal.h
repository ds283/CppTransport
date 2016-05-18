//
// Created by David Seery on 03/04/15.
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


#ifndef CPPTRANSPORT_WORK_JOURNAL_H
#define CPPTRANSPORT_WORK_JOURNAL_H


#include <list>
#include <memory>
#include <cstdlib>

#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/argument_cache.h"

#include "transport-runtime/utilities/plot_environment.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

#include "boost/date_time.hpp"
#include "boost/filesystem/operations.hpp"

#include "json/json.h"


// set default minimum time interval for instruments to be 1 second
#define CPPTRANSPORT_JOURNAL_MINIMUM_TIMESPAN         (1)

#define CPPTRANSPORT_JOURNAL_JSON_TYPE                "event-type"
#define CPPTRANSPORT_JOURNAL_JSON_MASTER_EVENT        "master-event"
#define CPPTRANSPORT_JOURNAL_JSON_SLAVE_EVENT         "slave-event"

#define CPPTRANSPORT_JOURNAL_JSON_TIMESTAMP           "timestamp"
#define CPPTRANSPORT_JOURNAL_JSON_ID                  "id"
#define CPPTRANSPORT_JOURNAL_JSON_WORKER_NUMBER       "worker"

#define CPPTRANSPORT_JOURNAL_JSON_EVENT               "event"

#define CPPTRANSPORT_JOURNAL_JSON_AGGREGATE_BEGIN     "aggregation-begin"
#define CPPTRANSPORT_JOURNAL_JSON_AGGREGATE_END       "aggregation-end"
#define CPPTRANSPORT_JOURNAL_JSON_MPI_BEGIN           "mpi-begin"
#define CPPTRANSPORT_JOURNAL_JSON_MPI_END             "mpi-end"
#define CPPTRANSPORT_JOURNAL_JSON_DATABASE_BEGIN      "database-begin"
#define CPPTRANSPORT_JOURNAL_JSON_DATABASE_END        "database-end"

#define CPPTRANSPORT_JOURNAL_JSON_TWOPF_BEGIN         "twopf-begin"
#define CPPTRANSPORT_JOURNAL_JSON_TWOPF_END           "twopf-end"
#define CPPTRANSPORT_JOURNAL_JSON_THREEPF_BEGIN       "threepf-begin"
#define CPPTRANSPORT_JOURNAL_JSON_THREEPF_END         "threepf-end"
#define CPPTRANSPORT_JOURNAL_JSON_ZETA_TWOPF_BEGIN    "zeta-twopf-begin"
#define CPPTRANSPORT_JOURNAL_JSON_ZETA_TWOPF_END      "zeta-twopf-end"
#define CPPTRANSPORT_JOURNAL_JSON_ZETA_THREEPF_BEGIN  "zeta-threepf-begin"
#define CPPTRANSPORT_JOURNAL_JSON_ZETA_THREEPF_END    "zeta-threepf-end"
#define CPPTRANSPORT_JOURNAL_JSON_FNL_BEGIN           "fNL-begin"
#define CPPTRANSPORT_JOURNAL_JSON_FNL_END             "fNL-end"
#define CPPTRANSPORT_JOURNAL_JSON_OUTPUT_BEGIN        "derived-content-begin"
#define CPPTRANSPORT_JOURNAL_JSON_OUTPUT_END          "derived-content-end"
#define CPPTRANSPORT_JOURNAL_JSON_INTEGRATION_AGG     "integration-aggregation-request"
#define CPPTRANSPORT_JOURNAL_JSON_POSTINTEGRATION_AGG "postintegration-aggregation-request"
#define CPPTRANSPORT_JOURNAL_JSON_OUTPUT_AGG          "derived-content-aggregation-request"


namespace transport
	{

		//! Policy object representing information about the environment while formatting bars
		class Gantt_environment
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! constructor
				Gantt_environment(std::string ao, std::string d_n, std::string dt_n, std::string du_n, double h)
		      : axes_object_str(ao),
		        date_namespace_str(d_n),
		        dt_namespace_str(dt_n),
		        dateutil_namespace_str(du_n),
						height_val(h)
					{
					}


				// INTERFACE

		  public:

				const std::string& axes_object() const { return(this->axes_object_str); }

				const std::string& date_namespace() const { return(this->date_namespace_str); }

				const std::string& dt_namespace() const { return(this->dt_namespace_str); }

				const std::string& dateutil_namespace() const { return(this->dateutil_namespace_str); }

				double height() const { return(this->height_val); }


				// INTERNAL DATA

		  protected:

				//! name of axes object
				std::string axes_object_str;

				//! name of date namespace
				std::string date_namespace_str;

				//! name of dt namespace
				std::string dt_namespace_str;

				//! name of dateutil namespace
				std::string dateutil_namespace_str;

				//! height of bars
				double height_val;

			};

		//! Gantt_bar represents a time duration on a Gantt chart
		class Gantt_bar
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! constructor
				Gantt_bar(boost::posix_time::ptime b, boost::posix_time::ptime e, std::string c)
					: begin_time(b),
		        end_time(e),
		        colour(c),
						alpha(0.6)
					{
					}


				// INTERFACE

		  public:

				//! Produce a std::string representing Python code to format the Gantt chart bar
				std::string format(unsigned int unique_id, double y, Gantt_environment& env) const;

		  private:

				//! colour of this bar
				std::string colour;

				//! start time
				boost::posix_time::ptime begin_time;

				//! end time
				boost::posix_time::ptime end_time;

		    //! alpha channel
				double alpha;

			};


    std::string Gantt_bar::format(unsigned int unique_id, double y, Gantt_environment& env) const
	    {
        std::ostringstream begin_label;
        std::ostringstream end_label;
        std::ostringstream insn;

		    begin_label << "time_begin" << unique_id;
		    end_label << "time_end" << unique_id;

		    insn << begin_label.str() << " = " << env.date_namespace() << ".date2num(" << env.dateutil_namespace() << ".parse('" << this->begin_time << "'))" << '\n';
		    insn << end_label.str()   << " = " << env.date_namespace() << ".date2num(" << env.dateutil_namespace() << ".parse('" << this->end_time << "'))" << '\n';

		    double bar_height = 0.8*env.height();
		    double bar_pad    = 0.1*env.height();

        insn << env.axes_object() << ".barh(" << y+bar_pad << ", " << end_label.str() << "-" << begin_label.str()
	        << ", height=" << bar_height << ", left=" << begin_label.str() << ", color='" << this->colour << "', alpha=" << this->alpha << ", align='edge', linewidth=0)";

		    return(insn.str());
	    }


		//! Gantt_milestone represents a time on a Gantt chart
		class Gantt_milestone
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! constructor
				Gantt_milestone(boost::posix_time::ptime m, std::string c)
		      : milestone_time(m),
						colour(c)
					{
					}


				// INTERFACE

		  public:

				//! Produce a std::string representing Python code to format the milestone
				std::string format(unsigned int unique_id, double y, Gantt_environment& env) const;

		  private:

				//! colour of this milestone
				std::string colour;

				//! milestone time
				boost::posix_time::ptime milestone_time;

			};


		std::string Gantt_milestone::format(unsigned int unique_id, double y, Gantt_environment& env) const
			{
		    std::ostringstream time_label;
		    std::ostringstream insn;

				time_label << "milestone" << unique_id;

				insn << time_label.str() << " = " << env.date_namespace() << ".date2num(" << env.dateutil_namespace() << ".parse('" << this->milestone_time << "'))" << '\n';

				double bar_height = 0.8*env.height();
				double size       = bar_height/3.0;
				double bar_pad    = 0.1*env.height();

				insn << env.axes_object() << ".plot([" << time_label.str() << "], [" << y+bar_pad+bar_height/2.0 << "], marker='D', mfc='" << this->colour << "', mec='black')";

				return(insn.str());
			}


		//! work_events record events
		class work_event
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! Create a work event for a given worker
				work_event();

				//! Create a work event for a given number, and a given timestamp and id
				work_event(boost::posix_time::ptime& t, unsigned int i);

				virtual ~work_event() = default;


				// INTERFACE

		  public:

				//! Is this a master-record object?
        virtual bool is_master() const = 0;

				//! Get timestamp
				boost::posix_time::ptime get_timestamp() const { return(this->timestamp); }

				//! Set id
				void set_id(unsigned int i) { this->id = i; }

				//! Get id
				unsigned int get_id() const { return(this->id); }


        // CONVERSIONS

      public:

        //! convert to JSON
        virtual void as_JSON(Json::Value& writer) const = 0;


				// CLONE

		  public:

				//! Clone
				virtual work_event* clone() const = 0;


				// PRIVATE DATA

		  private:

				//! cache time this work event was journaled
				boost::posix_time::ptime timestamp;

				//! internal ID number (interpretation depends on concrete class)
				unsigned int id;

			};


		work_event::work_event()
			: timestamp(boost::posix_time::second_clock::universal_time()),
				id(0)
			{
			}


		work_event::work_event(boost::posix_time::ptime& t, unsigned int i)
			: timestamp(t),
				id(i)
			{
			}


		//! work events, specialized to master events
		class master_work_event: public work_event
			{

		  public:

        enum class event_type
					{
				    aggregate_begin, aggregate_end,
						MPI_begin, MPI_end,
            database_begin, database_end
					};

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				master_work_event(event_type t);

				master_work_event(event_type t, boost::posix_time::ptime ts, unsigned int id=0);

				virtual ~master_work_event() = default;


				// INTERFACE

		  public:

				virtual bool is_master() const override { return(true); }

				event_type   get_type()  const { return(this->type); }


        // CONVERSIONS

      public:

        //! convert to JSON
        virtual void as_JSON(Json::Value& writer) const override;


		    // CLONE

		  public:

		    //! Clone
		    virtual master_work_event* clone() const override { return new master_work_event(dynamic_cast< const master_work_event& >(*this)); }


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


		master_work_event::master_work_event(typename master_work_event::event_type t, boost::posix_time::ptime ts, unsigned int id)
			: work_event(ts, id),
				type(t)
			{
			}


    void master_work_event::as_JSON(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_JOURNAL_JSON_TYPE]      = std::string(CPPTRANSPORT_JOURNAL_JSON_MASTER_EVENT);
        writer[CPPTRANSPORT_JOURNAL_JSON_TIMESTAMP] = boost::posix_time::to_simple_string(this->get_timestamp());
        writer[CPPTRANSPORT_JOURNAL_JSON_ID]        = this->get_id();

        switch(this->get_type())
          {
            case master_work_event::event_type::aggregate_begin:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_AGGREGATE_BEGIN);
              break;

            case master_work_event::event_type::aggregate_end:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_AGGREGATE_END);
              break;

            case master_work_event::event_type::MPI_begin:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_MPI_BEGIN);
              break;

            case master_work_event::event_type::MPI_end:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_MPI_END);
              break;

            case master_work_event::event_type::database_begin:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_DATABASE_BEGIN);
              break;

            case master_work_event::event_type::database_end:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_DATABASE_END);
              break;
          }
      }


		inline bool is_duration_delimiter(master_work_event::event_type type)
			{
				if(type == master_work_event::event_type::aggregate_begin
					|| type == master_work_event::event_type::aggregate_end
					|| type == master_work_event::event_type::MPI_begin
					|| type == master_work_event::event_type::MPI_end
					|| type == master_work_event::event_type::database_begin
					|| type == master_work_event::event_type::database_end) return(true);

				return(false);
			}


		inline bool is_milestone(master_work_event::event_type type)
			{
				return(false);
			}


		//! work events, specialized to slave events
		class slave_work_event: public work_event
			{

		  public:

				enum class event_type
					{
				    begin_twopf_assignment, end_twopf_assignment,
						begin_threepf_assignment, end_threepf_assignment,
						begin_zeta_twopf_assignment, end_zeta_twopf_assignment,
						begin_zeta_threepf_assignment, end_zeta_threepf_assignment,
						begin_fNL_assignment, end_fNL_assignment,
						begin_output_assignment, end_output_assignment,
						integration_aggregation, postintegration_aggregation, derived_content_aggregation
					};

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				slave_work_event(unsigned int w, event_type t);

				slave_work_event(unsigned int w, event_type t, boost::posix_time::ptime ts, unsigned int id=0);

				virtual ~slave_work_event() = default;


				// INTERFACE

		    virtual bool is_master() const override { return(false); }

				//! return worker number
				unsigned int get_worker_number() const { return(this->worker_number); }

        //! return event type
		    event_type   get_type()          const { return(this->type); }


        // CONVERSIONS

      public:

        //! convert to JSON
        virtual void as_JSON(Json::Value& writer) const override;


		    // CLONE

		  public:

		    //! Clone
		    virtual slave_work_event* clone() const override { return new slave_work_event(dynamic_cast< const slave_work_event& >(*this)); }


				// PRIVATE DATA

		  private:

				//! cache event type
				event_type type;

		    //! cache worker number
		    unsigned int worker_number;

			};


		inline bool is_duration_delimiter(slave_work_event::event_type type)
			{
				if(type == slave_work_event::event_type::begin_twopf_assignment || type == slave_work_event::event_type::end_twopf_assignment) return(true);
		    if(type == slave_work_event::event_type::begin_threepf_assignment || type == slave_work_event::event_type::end_threepf_assignment) return(true);
		    if(type == slave_work_event::event_type::begin_zeta_twopf_assignment || type == slave_work_event::event_type::end_zeta_twopf_assignment) return(true);
		    if(type == slave_work_event::event_type::begin_zeta_threepf_assignment || type == slave_work_event::event_type::end_zeta_threepf_assignment) return(true);
		    if(type == slave_work_event::event_type::begin_fNL_assignment || type == slave_work_event::event_type::end_fNL_assignment) return(true);
		    if(type == slave_work_event::event_type::begin_output_assignment || type == slave_work_event::event_type::end_output_assignment) return(true);

				return(false);
			}


		inline bool is_milestone(slave_work_event::event_type type)
			{
				if(type == slave_work_event::event_type::integration_aggregation
					|| type == slave_work_event::event_type::postintegration_aggregation
					|| type == slave_work_event::event_type::derived_content_aggregation) return(true);

				return(false);
			}


		slave_work_event::slave_work_event(unsigned int w, typename slave_work_event::event_type t)
			: work_event(),
				type(t),
				worker_number(w)
			{
			}


    slave_work_event::slave_work_event(unsigned int w, typename slave_work_event::event_type t, boost::posix_time::ptime ts, unsigned int id)
	    : work_event(ts, id),
	      type(t),
	      worker_number(w)
	    {
	    }


    void slave_work_event::as_JSON(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_JOURNAL_JSON_TYPE]          = std::string(CPPTRANSPORT_JOURNAL_JSON_SLAVE_EVENT);
        writer[CPPTRANSPORT_JOURNAL_JSON_TIMESTAMP]     = boost::posix_time::to_simple_string(this->get_timestamp());
        writer[CPPTRANSPORT_JOURNAL_JSON_ID]            = this->get_id();
        writer[CPPTRANSPORT_JOURNAL_JSON_WORKER_NUMBER] = this->get_worker_number();

        switch(this->get_type())
          {
            case slave_work_event::event_type::begin_twopf_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_TWOPF_BEGIN);
              break;

            case slave_work_event::event_type::end_twopf_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_TWOPF_END);
              break;

            case slave_work_event::event_type::begin_threepf_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_THREEPF_BEGIN);
              break;

            case slave_work_event::event_type::end_threepf_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_THREEPF_END);
              break;

            case slave_work_event::event_type::begin_zeta_twopf_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_ZETA_TWOPF_BEGIN);
              break;

            case slave_work_event::event_type::end_zeta_twopf_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_ZETA_TWOPF_END);
              break;

            case slave_work_event::event_type::begin_zeta_threepf_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_ZETA_THREEPF_BEGIN);
              break;

            case slave_work_event::event_type::end_zeta_threepf_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_ZETA_THREEPF_END);
              break;

            case slave_work_event::event_type::begin_fNL_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_FNL_BEGIN);
              break;

            case slave_work_event::event_type::end_fNL_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_FNL_END);
              break;

            case slave_work_event::event_type::begin_output_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_OUTPUT_BEGIN);
              break;

            case slave_work_event::event_type::end_output_assignment:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_OUTPUT_END);
              break;

            case slave_work_event::event_type::integration_aggregation:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_INTEGRATION_AGG);
              break;

            case slave_work_event::event_type::postintegration_aggregation:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_POSTINTEGRATION_AGG);
              break;

            case slave_work_event::event_type::derived_content_aggregation:
              writer[CPPTRANSPORT_JOURNAL_JSON_EVENT] = std::string(CPPTRANSPORT_JOURNAL_JSON_OUTPUT_AGG);
              break;

            default:
              assert(false);
              throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_UNEXPECTED_EVENT);
          }
      }


    //! a work journal records events in the timeline of a process,
    //! at a worker-level granularity.
		class work_journal
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! set up a work journal for N_workers workers (plus the master process)
				work_journal(unsigned int N);

				//! destructor is default
				~work_journal() = default;


				// INTERFACE -- JOURNAL ENTRIES

				//! push item to the journal

		  public:

				//! add a work event to the journal
				void add_entry(const work_event& w);


				// INTERFACE -- ANALYSIS

		  public:

				//! write a Gantt chart from the journal entries
				void make_gantt_chart(const std::string& filename, local_environment& env, argument_cache& args);

        //! write a JSON-format journal
        void make_journal(const std::string& filename, local_environment& env, argument_cache& args);


		  protected:

				//! bin journal entries into Gantt bars
				void bin_bars(std::list<std::list<Gantt_bar> >& list);

				//! bin journal entries for master events into Gantt bars
				void bin_master_bars(std::list<std::list<Gantt_bar> >& list);

				//! bin journal entries for slave events into Gantt bars
				void bin_slave_bars(std::list<std::list<Gantt_bar> >& list);

				//! bin journal entries into Gantt milestones
				void bin_milestones(std::list<std::list<Gantt_milestone> >& list);

				//! bin journal entries for master events into Gantt milestones
				void bin_master_milestones(std::list<std::list<Gantt_milestone> >& list);

				//! bin journal entries for slave events into Gantt milestones
				void bin_slave_milestones(std::list<std::list<Gantt_milestone> >& list);

				// PRIVATE DATA

		  private:

				//! cache number of workers
				unsigned int N_workers;

        typedef std::list< std::shared_ptr<work_event> >        event_journal;
        typedef std::list< std::shared_ptr<master_work_event> > master_event_journal;
        typedef std::list< std::shared_ptr<slave_work_event> >  slave_event_journal;

				//! journal; use std::shared_ptr to manage lifetimes of each entry
				event_journal journal;

      };


		work_journal::work_journal(unsigned int N)
			: N_workers(N)
			{
			}


		void work_journal::add_entry(const work_event& w)
			{
				this->journal.push_back(std::shared_ptr<work_event>(w.clone()));
			}


		void work_journal::bin_bars(std::list<std::list<Gantt_bar> >& list)
			{
		    list.clear();

		    this->bin_master_bars(list);
		    this->bin_slave_bars(list);
			}


		namespace work_journal_impl
			{

		    class WorkItemSorter
			    {
		      public:
		        bool operator()(const std::shared_ptr<work_event> A, const std::shared_ptr<work_event> B) // don't take arguments by reference so that shared_ptrs can be upcast
			        {
		            return(A->get_timestamp() < B->get_timestamp());
			        }
			    };

			}


		void work_journal::bin_master_bars(std::list<std::list<Gantt_bar> >& list)
			{
				// strip out master duration-bracketing events and sort them in order
		    master_event_journal master_events;
				for(event_journal::iterator t = this->journal.begin(); t != this->journal.end(); ++t)
					{
						if((*t)->is_master())
							{
                std::shared_ptr<master_work_event> event = std::dynamic_pointer_cast<master_work_event>(*t);
								if(is_duration_delimiter(event->get_type())) master_events.push_back(event);
							}
					}
				master_events.sort(work_journal_impl::WorkItemSorter());

				list.resize(list.size()+1);
		    std::list< std::list<Gantt_bar> >::iterator current_bin = --list.end();

				// work through master events, pairing them up
				for(master_event_journal::const_iterator t = master_events.begin(); t != master_events.end(); ++t)
					{
						switch((*t)->get_type())
							{
						    case master_work_event::event_type::aggregate_begin:
							    {
						        boost::posix_time::ptime begin_time = (*t)->get_timestamp();
						        t++;
						        if(t == master_events.end()) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_AGGREGATE_TOO_FEW);
						        if((*t)->get_type() != master_work_event::event_type::aggregate_end) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_AGGREGATE_END_MISSING);
						        current_bin->push_back(Gantt_bar(begin_time, (*t)->get_timestamp(), "black"));
						        break;
							    }

						    case master_work_event::event_type::MPI_begin:
							    {
						        boost::posix_time::ptime begin_time = (*t)->get_timestamp();
								    t++;
								    if(t == master_events.end()) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_MPI_TOO_FEW);
								    if((*t)->get_type() != master_work_event::event_type::MPI_end) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_MPI_END_MISSING);
								    current_bin->push_back(Gantt_bar(begin_time, (*t)->get_timestamp(), "darkgoldenrod"));
								    break;
							    }

                case master_work_event::event_type::database_begin:
                  {
                    boost::posix_time::ptime begin_time = (*t)->get_timestamp();
                    t++;
                    if(t == master_events.end()) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_DATABASE_TOO_FEW);
                    if((*t)->get_type() != master_work_event::event_type::database_end) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_DATABASE_END_MISSING);
                    current_bin->push_back(Gantt_bar(begin_time, (*t)->get_timestamp(), "aquamarine"));
                    break;
                  }

						    default:
							    assert(false);
							    throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_UNEXPECTED_EVENT);
							};
					}
			}


		void work_journal::bin_slave_bars(std::list<std::list<Gantt_bar> >& list)
			{
				// strip out events for each worker
		    slave_event_journal worker_events;
				for(unsigned int i = 0; i < this->N_workers; ++i)
					{
				    worker_events.clear();
						for(event_journal::iterator t = this->journal.begin(); t != this->journal.end(); ++t)
							{
								if(!(*t)->is_master())
									{
                    std::shared_ptr<slave_work_event> event = std::dynamic_pointer_cast<slave_work_event>(*t);
								    if(event->get_worker_number() == i && is_duration_delimiter(event->get_type())) worker_events.push_back(event);
									}
							}
						worker_events.sort(work_journal_impl::WorkItemSorter());

				    list.resize(list.size()+1);
				    std::list< std::list<Gantt_bar> >::iterator current_bin = --list.end();

						// work through events, pairing them up
						for(slave_event_journal::const_iterator t = worker_events.begin(); t != worker_events.end(); ++t)
							{
								switch((*t)->get_type())
									{
								    case slave_work_event::event_type::begin_twopf_assignment:
									    {
								        boost::posix_time::ptime begin_time = (*t)->get_timestamp();
								        t++;
								        if(t == worker_events.end()) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_TWOPF_TOO_FEW);
								        if((*t)->get_type() != slave_work_event::event_type::end_twopf_assignment) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_TWOPF_END_MISSING);
								        current_bin->push_back(Gantt_bar(begin_time, (*t)->get_timestamp(), "red"));
								        break;
									    }

								    case slave_work_event::event_type::begin_threepf_assignment:
									    {
								        boost::posix_time::ptime begin_time = (*t)->get_timestamp();
								        t++;
								        if(t == worker_events.end()) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_THREEPF_TOO_FEW);
								        if((*t)->get_type() != slave_work_event::event_type::end_threepf_assignment) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_THREEPF_END_MISSING);
										    current_bin->push_back(Gantt_bar(begin_time, (*t)->get_timestamp(), "orangered"));
								        break;
									    }

								    case slave_work_event::event_type::begin_zeta_twopf_assignment:
									    {
								        boost::posix_time::ptime begin_time = (*t)->get_timestamp();
										    t++;
										    if(t == worker_events.end()) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_ZETA_TWOPF_TOO_FEW);
										    if((*t)->get_type() != slave_work_event::event_type::end_zeta_twopf_assignment) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_ZETA_TWOPF_END_MISSING);
										    current_bin->push_back(Gantt_bar(begin_time, (*t)->get_timestamp(), "green"));
										    break;
									    }

								    case slave_work_event::event_type::begin_zeta_threepf_assignment:
									    {
								        boost::posix_time::ptime begin_time = (*t)->get_timestamp();
										    t++;
										    if(t == worker_events.end()) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_ZETA_THREEPF_TOO_FEW);
										    if((*t)->get_type() != slave_work_event::event_type::end_zeta_threepf_assignment) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_ZETA_THREEPF_END_MISSING);
										    current_bin->push_back(Gantt_bar(begin_time, (*t)->get_timestamp(), "limegreen"));
										    break;
									    }

								    case slave_work_event::event_type::begin_fNL_assignment:
									    {
								        boost::posix_time::ptime begin_time = (*t)->get_timestamp();
										    t++;
										    if(t == worker_events.end()) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_FNL_TOO_FEW);
										    if((*t)->get_type() != slave_work_event::event_type::end_fNL_assignment) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_FNL_END_MISSING);
										    current_bin->push_back(Gantt_bar(begin_time, (*t)->get_timestamp(), "royalblue"));
										    break;
									    }

								    case slave_work_event::event_type::begin_output_assignment:
									    {
								        boost::posix_time::ptime begin_time = (*t)->get_timestamp();
										    t++;
										    if(t == worker_events.end()) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_OUTPUT_TOO_FEW);
										    if((*t)->get_type() != slave_work_event::event_type::end_output_assignment) throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_OUTPUT_END_MISSING);
										    current_bin->push_back(Gantt_bar(begin_time, (*t)->get_timestamp(), "orchid"));
										    break;
									    }

								    default:
									    throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_UNEXPECTED_EVENT);
									};
							}
					}
			}


    void work_journal::bin_milestones(std::list<std::list<Gantt_milestone> >& list)
	    {
        list.clear();

        this->bin_master_milestones(list);
        this->bin_slave_milestones(list);
	    }


		void work_journal::bin_master_milestones(std::list< std::list<Gantt_milestone> >& list)
			{
				// strip out milestones from master list
		    master_event_journal master_events;
				for(event_journal::iterator t = this->journal.begin(); t != this->journal.end(); ++t)
					{
						if((*t)->is_master())
							{
                std::shared_ptr<master_work_event> event = std::dynamic_pointer_cast<master_work_event>(*t);
						    if(is_milestone(event->get_type())) master_events.push_back(event);
							}
					}
		    master_events.sort(work_journal_impl::WorkItemSorter());

				list.resize(list.size()+1);
		    std::list< std::list<Gantt_milestone> >::iterator current_bin = --list.end();

				for(master_event_journal::const_iterator t = master_events.begin(); t != master_events.end(); ++t)
					{
						switch((*t)->get_type())
							{
						    default:
							    assert(false);
							    throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_UNEXPECTED_EVENT);
							}
					}
			}


		void work_journal::bin_slave_milestones(std::list< std::list<Gantt_milestone> >& list)
			{
				// strip out events for each worker
		    slave_event_journal worker_events;
				for(unsigned int i = 0; i < this->N_workers; ++i)
					{
						worker_events.clear();
						for(event_journal::iterator t = this->journal.begin(); t != this->journal.end(); ++t)
							{
								if(!(*t)->is_master())
									{
                    std::shared_ptr<slave_work_event> event = std::dynamic_pointer_cast<slave_work_event>(*t);
										if(event->get_worker_number() == i && is_milestone(event->get_type())) worker_events.push_back(event);
									}
							}
						worker_events.sort(work_journal_impl::WorkItemSorter());

						list.resize(list.size()+1);
				    std::list< std::list<Gantt_milestone> >::iterator current_bin = --list.end();

						for(slave_event_journal::const_iterator t = worker_events.begin(); t != worker_events.end(); ++t)
							{
								switch((*t)->get_type())
									{
								    case slave_work_event::event_type::integration_aggregation:
									    current_bin->push_back(Gantt_milestone((*t)->get_timestamp(), "green"));
                      break;

								    case slave_work_event::event_type::postintegration_aggregation:
									    current_bin->push_back(Gantt_milestone((*t)->get_timestamp(), "red"));
									    break;

                    case slave_work_event::event_type::derived_content_aggregation:
									    current_bin->push_back(Gantt_milestone((*t)->get_timestamp(), "yellow"));
									    break;

								    default:
									    assert(false);
									    throw runtime_exception(exception_type::JOURNAL_ERROR, CPPTRANSPORT_JOURNAL_UNEXPECTED_EVENT);
									};
							}
					}
			}


		void work_journal::make_gantt_chart(const std::string& filename, local_environment& local_env, argument_cache& args)
			{
		    std::list< std::list<Gantt_bar> > bars_list;
		    this->bin_bars(bars_list);

		    std::list< std::list<Gantt_milestone> > milestones_list;
				this->bin_milestones(milestones_list);

				assert(bars_list.size() == this->N_workers+1);

				// set up BOOST path to output file
		    boost::filesystem::path out_file(filename);

				// check if out_file already exists; if so, rename it uniquely
		    if(boost::filesystem::exists(out_file))
			    {
		        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

		        boost::filesystem::path out_dir = out_file;
		        out_dir.remove_filename();

		        boost::filesystem::path out_leaf = out_file.filename();
		        boost::filesystem::path out_ext  = out_file.extension();
		        out_leaf.replace_extension("");

		        std::ostringstream new_leaf;
		        new_leaf << out_leaf.string() << "-" << boost::posix_time::to_iso_string(now);

		        out_leaf = boost::filesystem::path(new_leaf.str());
		        out_leaf.replace_extension(out_ext);
		        out_file = out_dir / out_leaf;
			    }

		    boost::filesystem::path script_file = out_file;
				if(script_file.extension() != ".py")
					{
				    // obtain path for Python script output
				    script_file.replace_extension(".py");
					}

		    std::ofstream out;
				out.open(script_file.c_str(), std::ios_base::trunc | std::ios_base::out);

				if(!out.is_open() || out.fail())
					{
				    std::ostringstream msg;
						msg << CPPTRANSPORT_JOURNAL_OPEN_FAIL << " " << script_file;
						throw runtime_exception(exception_type::JOURNAL_ERROR, msg.str());
					}

				out << "import numpy as np" << '\n';
        plot_environment plot_env(local_env, args);
        plot_env.write_environment(out);

				out << "import matplotlib.dates as mdt" << '\n';
        out << "import matplotlib.patches as patches" << '\n';
        out << "from matplotlib import rcParams" << '\n';
				out << "import datetime as dt" << '\n';
				out << "import dateutil.parser" << '\n';
        out << "rcParams['legend.numpoints'] = 1" << '\n';

        // height is 3" + 0.5" for each bar
        double height = 3.0 + 0.5*bars_list.size();
				out << "fig = plt.figure(figsize=(13," << height << "))" << '\n';
				out << "ax = plt.gca()" << '\n';

				Gantt_environment gantt_env("ax", "mdt", "dt", "dateutil.parser", 1.0);

		    // output the bar-chart part of the gantt chart

		    std::list< std::list<Gantt_bar> >::const_iterator t = bars_list.begin();
				unsigned int process_count = 0;       // number of processes appearing on the chart
				unsigned int total_count = 0;         // number of individual bars appearing on the chart (each process may have several bars)

		    while(t != bars_list.end())
			    {
		        std::list<Gantt_bar>::const_iterator u = t->begin();
		        while(u != t->end())
			        {
		            out << u->format(total_count, static_cast<double>(process_count) * 1.0, gantt_env) << '\n';
		            ++u;
		            ++total_count;
			        }

		        ++process_count;
		        ++t;
			    }

				// output milestones

		    std::list< std::list<Gantt_milestone> >::const_iterator v = milestones_list.begin();
				process_count = 0;
				total_count = 0;

				while(v != milestones_list.end())
					{
				    std::list<Gantt_milestone>::const_iterator w = v->begin();
						while(w != v->end())
							{
								out << w->format(total_count, static_cast<double>(process_count) * 1.0, gantt_env) << '\n';
								++w;
								++total_count;
							}

						++process_count;
						++v;
					}

				// label y-axis by worker if there are not too many labels
				if(this->N_workers <= 20)
					{
				    out << "ylabels = []" << '\n';
				    out << "ylabels.append('Master')" << '\n';

				    for(unsigned int i = 0; i < this->N_workers; ++i)
					    {
				        out << "ylabels.append('Worker " << i+1 << "')" << '\n';
					    }

				    out << "pos = np.arange(0.5, " << 0.5 + static_cast<double>(process_count)*1.0 << ", 1.0)" << '\n';
				    out << "locsy, labelsy = plt.yticks(pos, ylabels)" << '\n';
				    out << "plt.setp(labelsy, fontsize=12)" << '\n';
					}

		    // format the x-axis
		    out << "ax.xaxis_date()" << '\n';
				out << "ax.axis('tight')" << '\n';

				out << "ax.invert_yaxis()" << '\n';
				out << "fig.autofmt_xdate()" << '\n';

        // add legend
        out << "aggregate = patches.Patch(color='black', label='Aggregation')" << '\n'
            << "MPI_master = patches.Patch(color='darkgoldenrod', label='MPI activity')" << '\n'
            << "database = patches.Patch(color='aquamarine', label='Database activity')" << '\n'
            << "twopf_task = patches.Patch(color='red', label='2pf task')" << '\n'
            << "threepf_task = patches.Patch(color='orangered', label='3pf task')" << '\n'
            << "zeta_twopf_task = patches.Patch(color='green', label='$\\zeta$ 2pf task')" << '\n'
            << "zeta_threepf_task = patches.Patch(color='limegreen', label='$\\zeta$ 3pf task')" << '\n'
            << "fNL_task = patches.Patch(color='royalblue', label='$f_{NL}$ task')" << '\n'
            << "output_task = patches.Patch(color='orchid', label='Output task')" << '\n'
            << "i_aggregation_event = plt.Line2D((0,1),(0,0), mfc='green', marker='D', mec='black', linestyle='', label='Push integration')" << '\n'
            << "p_aggregation_event = plt.Line2D((0,1),(0,0), mfc='red', marker='D', mec='black', linestyle='', label='Push postintegration')" << '\n'
            << "c_aggregation_event = plt.Line2D((0,1),(0,0), mfc='yellow', marker='D', mec='black', linestyle='', label='Push output')" << '\n'
            << "plt.legend(frameon=False,bbox_to_anchor=(1.05, 1),loc=2,borderaxespad=0.0,handles=[aggregate,MPI_master,database,twopf_task,threepf_task,zeta_twopf_task,zeta_threepf_task,fNL_task,output_task,i_aggregation_event,p_aggregation_event,c_aggregation_event])"  << '\n';

        out << "box = ax.get_position()" << '\n'
            << "ax.set_position([box.x0, box.y0, 0.8*box.width, box.height])" << '\n';

        // determine what to do based on file extension
				if(out_file.extension() != ".py")
					{
				    out << "plt.savefig(" << out_file << ")" << '\n';
					}
				else
					{
				    boost::filesystem::path temp = out_file;
						temp.replace_extension(".pdf");
						out << "plt.savefig(" << temp << ")" << '\n';
					}

				out.close();

				// if output format wasn't Python, try to execute this script
				if(out_file.extension() != ".py")
					{
            int rc = local_env.execute_python(script_file);

				    // remove python script if worked ok
				    if(rc == 0)
					    {
				        boost::filesystem::remove(script_file);
					    }
					}
			}


    void work_journal::make_journal(const std::string& filename, local_environment& env, argument_cache& args)
      {
        Json::Value entries(Json::arrayValue);

        for(event_journal::iterator t = this->journal.begin(); t != this->journal.end(); ++t)
          {
            Json::Value item(Json::objectValue);
            (*t)->as_JSON(item);
            entries.append(item);
          }

        // set up BOOST path to output file
        boost::filesystem::path out_file(filename);

        // check if out_file already exists; if so, rename it uniquely
        if(boost::filesystem::exists(out_file))
          {
            boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

            boost::filesystem::path out_dir = out_file;
            out_dir.remove_filename();

            boost::filesystem::path out_leaf = out_file.filename();
            boost::filesystem::path out_ext  = out_file.extension();
            out_leaf.replace_extension("");

            std::stringstream new_leaf;
            new_leaf << out_leaf.string() << "-" << boost::posix_time::to_iso_string(now);

            out_leaf = boost::filesystem::path(new_leaf.str());
            out_leaf.replace_extension(out_ext);
            out_file = out_dir / out_leaf;
          }

        std::ofstream out;
        out.open(out_file.c_str(), std::ios_base::trunc | std::ios_base::out);

        if(!out.is_open() || out.fail())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_JOURNAL_OPEN_FAIL << " " << out_file;
            throw runtime_exception(exception_type::JOURNAL_ERROR, msg.str());
          }

        out << entries;
        out.close();
      }


		//! journal_instrument is a class representing an instrument which automatically journals the
		//! time consumed by a block of code.
		//! It stores its creation time, and on destruction it generates a pair of journal entries
		//! recording when it was created and when it was destroyed.
		//! An optional resolution can be supplied which is the minimum timespan that is actually journalled.
    class journal_instrument
	    {

      public:

        journal_instrument(work_journal& j,
                           master_work_event::event_type b, master_work_event::event_type,
                           unsigned int i = 0, unsigned int m = CPPTRANSPORT_JOURNAL_MINIMUM_TIMESPAN);

        ~journal_instrument();


		    // INTERFACE

      public:

		    void stop() { this->stop_time = boost::posix_time::second_clock::universal_time(); this->stopped = true; }


		    // INTERNAL DATA

      private:

		    //! record start time of instrument
		    boost::posix_time::ptime start_time;

		    //! record stop time, if needed
		    boost::posix_time::ptime stop_time;

		    //! stopped?
		    bool stopped;

		    //! reference to parent scheduler
		    work_journal& journal;

		    //! label for begin journal item
		    master_work_event::event_type begin_label;

		    //! label for end journal item
		    master_work_event::event_type end_label;

		    //! minimum interval which will be journalled, measured in seconds
		    unsigned int minimum_interval;

		    //! id of events
		    unsigned int id;

	    };


    journal_instrument::journal_instrument(work_journal& j, master_work_event::event_type b, master_work_event::event_type e, unsigned int i, unsigned int m)
	    : journal(j),
	      begin_label(b),
	      end_label(e),
	      id(i),
	      minimum_interval(m),
	      stopped(false)
	    {
        this->start_time = boost::posix_time::second_clock::universal_time();
        this->stop_time  = this->start_time;
	    }


		journal_instrument::~journal_instrument()
			{
				if(!this->stopped) this->stop_time = boost::posix_time::second_clock::universal_time();

		    boost::posix_time::time_duration duration = this->stop_time - this->start_time;

				if(duration.total_seconds() > this->minimum_interval)
					{
						this->journal.add_entry(master_work_event(this->begin_label, this->start_time, this->id));
						this->journal.add_entry(master_work_event(this->end_label, this->stop_time, this->id));
					}
			}


	}   // namespace transport


#endif //CPPTRANSPORT_WORK_JOURNAL_H
