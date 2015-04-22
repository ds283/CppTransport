//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __integration_abstract_task_H_
#define __integration_abstract_task_H_


#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/tasks/configuration-database/time_config_database.h"

#include "transport-runtime-api/utilities/random_string.h"


#define __CPP_TRANSPORT_NODE_FAST_FORWARD                  "fast-forward"
#define __CPP_TRANSPORT_NODE_FAST_FORWARD_EFOLDS           "ff-efolds"
#define __CPP_TRANSPORT_NODE_MESH_REFINEMENTS              "mesh-refinements"

#define __CPP_TRANSPORT_NODE_TIME_RANGE                    "integration-range"

#define __CPP_TRANSPORT_NODE_PACKAGE_NAME                  "package"


namespace transport
	{

    template <typename number> class integration_task;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const integration_task<number>& obj);

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
        integration_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t);

        //! Construct an anonymized integration task with supplied initial conditions.
        //! Anonymized tasks are used for things like constructing initial conditions,
        //! integrating the background only, finding H at horizon-crossing etc.
        //! Science output is expected to be generated using named tasks.
        integration_task(const initial_conditions<number>& i, const range<double>& t)
	        : integration_task(random_string(), i, t)
	        {
	        }

        //! deserialization constructor
        integration_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i);

        //! override copy constructor to perform a deep copy of associated range<> object
        integration_task(const integration_task<number>& obj);

        //! Destroy an integration task
        virtual ~integration_task();


        // INTERFACE - TASK COMPONENTS

      public:

        //! Get 'initial conditions' object associated with this task
        const initial_conditions<number>& get_ics() const { return(this->ics); }

        //! Get 'parameters' object associated with this task
        const parameters<number>& get_params() const { return(this->ics.get_params()); }

        //! Get model associated with this task
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


        // TIME CONFIGURATION DATABASE

      protected:

        //! build and return a database of times at which to sample a time integration
        time_config_database build_time_config_database() const;

        //! build and cache stored time database -- only includes records for times which are committed to the database
        virtual void cache_stored_time_config_database();


        // SERIALIZE - implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // WRITE TO STREAM

      public:

        //! Write to a standard output stream
        friend std::ostream& operator<< <>(std::ostream& out, const integration_task<number>& obj);


        // INTERNAL DATA

      protected:

        //! Initial conditions for this task (including parameter choices)
        initial_conditions<number>       ics;

        //! Range of times at which to sample for this task;
        //! kept for serialization purposes, and so we can reconstruct the time configuration database if the
		    //! k-range changes
        range<double>*                   times;


        // STORED TIME CONFIGURATION DATABASE

        time_config_database             stored_time_db;

	    };


    template <typename number>
    integration_task<number>::integration_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t)
	    : derivable_task<number>(nm),
	      ics(i),
	      times(t.clone())
	    {
        // validate relation between Nstar and the sampling time
        assert(times->get_steps() > 0);
        assert(times->get_min() >= ics.get_N_initial());

        if(times->get_steps() == 0)
	        {
            std::ostringstream msg;
            msg << "'" << this->get_name() << "': " << __CPP_TRANSPORT_NO_TIMES;
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	        }

        // the sampling points don't have to begin at the initial time, but they shouldn't be earlier than it
        if(times->get_min() < ics.get_N_initial())
	        {
            std::ostringstream msg;
            msg << "'" << this->get_name() << "': " << __CPP_TRANSPORT_SAMPLES_START_TOO_EARLY_A << " ("
              << __CPP_TRANSPORT_SAMPLES_START_TOO_EARLY_B << "=" << times->get_min() << ", "
              << __CPP_TRANSPORT_SAMPLES_START_TOO_EARLY_C << "=" << i.get_N_initial() << ")";
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	        }
	    }


    template <typename number>
    integration_task<number>::integration_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : derivable_task<number>(nm, reader),
	      ics(i),
	      times(range_helper::deserialize<double>(reader[__CPP_TRANSPORT_NODE_TIME_RANGE]))
	    {
	    }


    template <typename number>
    integration_task<number>::integration_task(const integration_task<number>& obj)
	    : derivable_task<number>(obj),
	      ics(obj.ics),
	      times(obj.times->clone()),
        stored_time_db(obj.stored_time_db)
	    {
	    }


    template <typename number>
    integration_task<number>::~integration_task()
	    {
        assert(this->times != nullptr);

        delete this->times;
	    }


    template <typename number>
    void integration_task<number>::serialize(Json::Value& writer) const
	    {
        // store name of package
        writer[__CPP_TRANSPORT_NODE_PACKAGE_NAME] = this->ics.get_name();

        Json::Value time_data(Json::objectValue);
        this->times->serialize(time_data);
        writer[__CPP_TRANSPORT_NODE_TIME_RANGE] = time_data;

        // note that we do not serialize the initial conditions;
        // these are handled separately by the repository layer

        // call next serializers in the queue
        this->derivable_task<number>::serialize(writer);
	    }


    template <typename number>
    time_config_database integration_task<number>::build_time_config_database() const
	    {
        // set up new time configuration database
        time_config_database time_db;

		    // push all user-specified sample times into the database

		    // get raw time sample points
		    const std::vector<double>& raw_times = this->times->get_grid();

        unsigned int serial = 0;
        bool first = true;
		    for(std::vector<double>::const_iterator t = raw_times.begin(); t != raw_times.end(); t++, serial++)
			    {
            if(*t > this->get_N_initial())
              {
                if(first)   // check that initial time is included, because this sets where the integrator begins
                  {
                    if(this->get_N_initial() < *t) time_db.add_record(this->get_N_initial(), false, 0);
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
    void integration_task<number>::cache_stored_time_config_database()
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
    std::ostream& operator<<(std::ostream& out, const integration_task<number>& obj)
	    {
        out << obj.ics << std::endl;
//        out << __CPP_TRANSPORT_TASK_TIMES << obj.times;
        return(out);
	    }

	}   // namespace transport


#endif //__integration_abstract_task_H_
