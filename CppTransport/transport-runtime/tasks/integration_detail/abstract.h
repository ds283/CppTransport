//
// Created by David Seery on 15/04/15.
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


#ifndef CPPTRANSPORT_INTEGRATION_ABSTRACT_TASK_H
#define CPPTRANSPORT_INTEGRATION_ABSTRACT_TASK_H


#include "transport-runtime/tasks/integration_detail/common.h"
#include "transport-runtime/tasks/configuration-database/time_config_database.h"
#include "transport-runtime/models/advisory_classes.h"

#include "transport-runtime/utilities/random_string.h"

#include "boost/log/utility/formatting_ostream.hpp"

#include "sqlite3.h"


namespace transport
	{

    constexpr auto CPPTRANSPORT_NODE_ADAPTIVE_ICS           = "adaptive-ics";
    constexpr auto CPPTRANSPORT_NODE_ADAPTIVE_ICS_EFOLDS    = "adaptive-ics-efolds";
    constexpr auto CPPTRANSPORT_NODE_MESH_REFINEMENTS       = "mesh-refinements";
    constexpr auto CPPTRANSPORT_NODE_END_OF_INFLATION       = "end-of-inflation";
    constexpr auto CPPTRANSPORT_NODE_DEFAULT_CHECKPOINT     = "default-checkpoint";
    constexpr auto CPPTRANSPORT_NODE_DEFAULT_CHECKPOINT_SET = "default-checkpoint-set";

    constexpr auto CPPTRANSPORT_NODE_TIME_RANGE             = "integration-range";

    constexpr auto CPPTRANSPORT_NODE_PACKAGE_NAME           = "package";

    //! An 'integration_task' is a specialization of 'task'. It contains the basic information
    //! needed to carry out an integration. The more specialized two- and three-pf integration
    //! tasks are derived from it, as is a concrete 'background' task object which is used
    //! internally for integrating the background only.

    //! An 'integration_task' contains information on the initial conditions, horizon-crossing
    //! time and sampling times.
    template <typename number>
    class integration_task: public derivable_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named integration task with supplied initial conditions and sample times
        integration_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t);

        //! Construct an anonymized integration task with supplied initial conditions.
        //! Anonymized tasks are used for things like constructing initial conditions,
        //! integrating the background only, finding H at horizon-crossing etc.
        //! Science output is expected to be generated using named tasks.
        integration_task(const initial_conditions<number>& i, range<double>& t)
	        : integration_task(random_string(), i, t)
	        {
            // mark this task as not serializable
            this->task<number>::serializable = false;
	        }

        //! deserialization constructor
        integration_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i);

        //! override copy constructor to perform a deep copy of associated range<> object
        integration_task(const integration_task<number>& obj);

        //! destructor is default
        virtual ~integration_task() = default;


        // INTERFACE

      public:

        //! identify integration task type
        virtual integration_task_type get_task_type() const = 0;


        // INTERFACE - TASK COMPONENTS

      public:

        //! Get 'initial conditions' object associated with this task
        const initial_conditions<number>& get_ics() const { return(this->ics); }

        //! Get 'parameters' object associated with this task
        const parameters<number>& get_params() const { return(this->ics.get_params()); }

        //! Get model associated with this task; passed through to initial_conditions<> object
        //! which owns the model pointer
        model<number>* get_model() const { return(this->ics.get_model()); }


        // INTERFACE - implements a 'derivable task' interface

      public:

        //! Get database of stored time configurations
        virtual const time_config_database& get_stored_time_config_database() const override { return(this->stored_time_db); }


        // INTERFACE - INTEGRATION DETAILS - TIMES AND INITIAL CONDITIONS

      public:

		    //! Build sample-time database
		    const time_config_database get_time_config_database() const { return(this->build_time_config_database()); }

        //! Get time at which initial conditions are set
        double get_N_initial() const { return(this->ics.get_N_initial()); }

        //! Get std::vector of initial conditions at a specified absolute time Nstart, which should be > Ninit
        std::vector<number> get_ics_vector(double Nstart) const;

		    //! Get std::vector of initial conditions at time Ninit
		    std::vector<number> get_ics_vector(void) const;

		    //! Get time of end of inflation -- non-const version which caches its result
		    double get_N_end_of_inflation();

        //! Get time of end of inflation -- const version; cannot cache result
        double get_N_end_of_inflation() const;


        // INTERFACE - INTEGRATION MANAGEMENT

      public:

        //! has a default checkpoint been set?
        bool has_default_checkpoint() const { return(this->default_checkpoint_set); }

        //! get default checkpoint
        boost::optional<unsigned int> get_default_checkpoint() const { if(this->default_checkpoint_set) return this->default_checkpoint; else return boost::none; }

        //! set default checkpoint
        void set_default_checkpoint(unsigned int t) { this->default_checkpoint = t; this->default_checkpoint_set = (t > 0); }

        //! unset default checkpoint
        void unset_default_checkpoint() { this->default_checkpoint = 0; this->default_checkpoint_set = false; }


        // TIME CONFIGURATION DATABASE

      protected:

        //! build and return a database of times at which to sample a time integration
        time_config_database build_time_config_database() const;

        //! build and cache stored time database -- only includes records for times which are committed to the database
        virtual void cache_stored_time_config_database(double largest_conventional_k);


		    // K-CONFIGURATION DATABASE

      public:

		    //! write k-configuration database to disk
		    virtual void write_kconfig_database(sqlite3* handle) = 0;

		    //! check whether databases have been modified
		    virtual bool is_kconfig_database_modified() const = 0;


        // SERIALIZE - implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // WRITE TO STREAM

      public:

        //! write self-details to stream
        template <typename Stream> void write(Stream& obj) const;


        // INTERNAL DATA

      protected:

        //! Initial conditions for this task (including parameter choices)
        initial_conditions<number> ics;

        //! Range of times at which to sample for this task;
        //! kept for serialization purposes, and so we can reconstruct the time configuration database if the
		    //! k-range changes
        std::unique_ptr< range<double> > times;

		    //! time of end-of-inflation
        double end_of_inflation;

		    //! flag to indicate whether end-of-inflation time has been cached
        bool cached_end_of_inflation;

        //! flag to indicate whether a default checkpoint has been set
        bool default_checkpoint_set;

        //! default checkpoint interval in minutes, if used
        unsigned int default_checkpoint;


        // STORED TIME CONFIGURATION DATABASE

        //! time database
        time_config_database stored_time_db;

	    };


    template <typename number>
    integration_task<number>::integration_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t)
	    : derivable_task<number>(nm),
	      ics(i),
	      times(t.clone()),
        end_of_inflation(0.0),
        cached_end_of_inflation(false),
        default_checkpoint(0),
        default_checkpoint_set(false)
	    {
        // validate relation between Nstar and the sampling time
        assert(times->size() > 0);
        assert(times->get_min() >= ics.get_N_initial());

        if(times->size() == 0)
	        {
            std::ostringstream msg;
            msg << "'" << this->get_name() << "': " << CPPTRANSPORT_NO_TIMES;
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
	        }

        // the sampling points don't have to begin at the initial time, but they shouldn't be earlier than it
        if(times->get_min() < ics.get_N_initial())
	        {
            std::ostringstream msg;
            msg << "'" << this->get_name() << "': " << CPPTRANSPORT_SAMPLES_START_TOO_EARLY_A << " ("
              << CPPTRANSPORT_SAMPLES_START_TOO_EARLY_B << "=" << times->get_min() << ", "
              << CPPTRANSPORT_SAMPLES_START_TOO_EARLY_C << "=" << i.get_N_initial() << ")";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
	        }
	    }


    template <typename number>
    integration_task<number>::integration_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : derivable_task<number>(nm, reader),
	      ics(i),
	      times(range_helper::deserialize<double>(reader[CPPTRANSPORT_NODE_TIME_RANGE])),
        cached_end_of_inflation(true)
	    {
		    if(reader.isMember(CPPTRANSPORT_NODE_END_OF_INFLATION))
			    {
		        end_of_inflation = reader[CPPTRANSPORT_NODE_END_OF_INFLATION].asDouble();
						cached_end_of_inflation = true;
			    }
		    else
			    {
				    cached_end_of_inflation = false;
			    }

        default_checkpoint_set = reader[CPPTRANSPORT_NODE_DEFAULT_CHECKPOINT_SET].asBool();
        default_checkpoint = reader[CPPTRANSPORT_NODE_DEFAULT_CHECKPOINT].asUInt();
	    }


    template <typename number>
    integration_task<number>::integration_task(const integration_task<number>& obj)
	    : derivable_task<number>(obj),
	      ics(obj.ics),
	      times(obj.times->clone()),
        stored_time_db(obj.stored_time_db),
        end_of_inflation(obj.end_of_inflation),
        cached_end_of_inflation(obj.cached_end_of_inflation),
        default_checkpoint_set(obj.default_checkpoint_set),
        default_checkpoint(obj.default_checkpoint)
	    {
	    }


    template <typename number>
    void integration_task<number>::serialize(Json::Value& writer) const
	    {
        // store name of package
        writer[CPPTRANSPORT_NODE_PACKAGE_NAME] = this->ics.get_name();

		    if(this->cached_end_of_inflation)
			    {
		        writer[CPPTRANSPORT_NODE_END_OF_INFLATION] = this->end_of_inflation;
			    }
		    else
			    {
				    try
					    {
				        // can't use get_N_end_of_inflation() because it is not const
				        writer[CPPTRANSPORT_NODE_END_OF_INFLATION] = this->ics.get_model()->compute_end_of_inflation(this);
					    }
						catch (end_of_inflation_not_found& xe)
							{
								// need do nothing
							}
			    }

        Json::Value time_data(Json::objectValue);
        this->times->serialize(time_data);
        writer[CPPTRANSPORT_NODE_TIME_RANGE] = time_data;

        writer[CPPTRANSPORT_NODE_DEFAULT_CHECKPOINT_SET] = this->default_checkpoint_set;
        writer[CPPTRANSPORT_NODE_DEFAULT_CHECKPOINT] = this->default_checkpoint;

        // note that we do not serialize the initial conditions;
        // these are handled separately by the repository layer

        // call next serializers in the queue
        this->derivable_task<number>::serialize(writer);
	    }


    template <typename number>
    double integration_task<number>::get_N_end_of_inflation()
	    {
        if(this->cached_end_of_inflation) return(this->end_of_inflation);

		    this->end_of_inflation = this->ics.get_model()->compute_end_of_inflation(this);
		    this->cached_end_of_inflation = true;

		    return(this->end_of_inflation);
	    }


    template <typename number>
    double integration_task<number>::get_N_end_of_inflation() const
	    {
        if(this->cached_end_of_inflation) return(this->end_of_inflation);

        double end_of_inflation = this->ics.get_model()->compute_end_of_inflation(this);
        return(end_of_inflation);
	    }


    template <typename number>
    time_config_database integration_task<number>::build_time_config_database() const
	    {
        // set up new time configuration database
        time_config_database time_db;

		    // push all user-specified sample times into the database

		    // get raw time sample points
		    const std::vector<double>& raw_times = this->times->get_grid();

        unsigned int serial = CPPTRANSPORT_TIME_DATABASE_LOWEST_SERIAL;
        bool first = true;
		    for(std::vector<double>::const_iterator t = raw_times.begin(); t != raw_times.end(); ++t, ++serial)
			    {
            if(*t > this->get_N_initial())
              {
                if(first)   // check that initial time is included, because this sets where the integrator begins
                  {
                    if(this->get_N_initial() < *t) time_db.add_record(this->get_N_initial(), false, CPPTRANSPORT_TIME_DATABASE_SPECIAL_SERIAL);
                  }
                time_db.add_record(*t, true, serial);
                first = false;
              }
			    }

		    // should always produce some output
		    assert(time_db.size() > 0);

        return(time_db);
	    }


    template <typename number>
    void integration_task<number>::cache_stored_time_config_database(double largest_conventional_k)
      {
        // empty database and start again
        this->stored_time_db = build_time_config_database();
      }


    template <typename number>
    std::vector<number> integration_task<number>::get_ics_vector(double Nstart) const
	    {
        assert(Nstart >= this->ics.get_N_initial());

        if(Nstart > this->ics.get_N_initial()) return this->ics.get_offset_vector(Nstart);
        else                                   return this->ics.get_vector();
	    }


		template <typename number>
		std::vector<number> integration_task<number>::get_ics_vector(void) const
			{
				return this->ics.get_vector();
			}


    template <typename number>
    template <typename Stream>
    void integration_task<number>::write(Stream& out) const
      {
        out << this->get_ics() << '\n';
      }


    template <typename number, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const integration_task<number>& obj)
	    {
        obj.write(out);
        return(out);
	    }


    template <typename number, typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const integration_task<number>& obj)
      {
        obj.write(out);
        return(out);
      }

	}   // namespace transport


#endif //CPPTRANSPORT_INTEGRATION_ABSTRACT_TASK_H
