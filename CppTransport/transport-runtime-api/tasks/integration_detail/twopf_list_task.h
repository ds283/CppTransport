//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __twopf_list_task_H_
#define __twopf_list_task_H_


#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/tasks/integration_detail/abstract.h"

#include "transport-runtime-api/tasks/configuration-database/twopf_config_database.h"

#define __CPP_TRANSPORT_NODE_TWOPF_LIST_DATABASE "twopf-database"


namespace transport
	{

    template <typename number> class twopf_list_task;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const twopf_list_task<number>& obj);

    //! Base type for a task which can represent a set of two-point functions evaluated at different wavenumbers.
    //! Ultimately, all n-point-function integrations are of this type because they all solve for the two-point function
    //! even if the goal is to compute a higher n-point function.
    //! The key concept associated with a twopf_list_task is a database of wavenumbers
    //! which describe the points at which we sample the twopf.
    template <typename number>
    class twopf_list_task: public integration_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a twopf-list-task object
        twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t);

        //! deserialization constructor
        twopf_list_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i);

        virtual ~twopf_list_task() = default;


        // INTERFACE - TWOPF K-CONFIGURATIONS

      public:

        //! Provide access to twopf k-configuration database
        const twopf_kconfig_database& get_twopf_database() const { return(this->twopf_db); }


		    // INTERFACE - INITIAL CONDITIONS AND INTEGRATION DETAILS

      public:

        //! Get std::vector of initial conditions, offset using fast forwarding if enabled
        std::vector<number> get_ics_vector(const twopf_kconfig& kconfig) const;

        //! Build sample-time database
        const time_config_database get_time_config_database(const twopf_kconfig& config) const;


        // INTERFACE - FAST-FORWARD MANAGEMENT

      public:

        //! Get fast-forward integration setting
        bool get_fast_forward() const { return(this->fast_forward); }

        //! Set fast-forward integration setting
        void set_fast_forward(bool g) { this->fast_forward = g; this->cache_stored_time_config_database(); }

        //! Get number of fast-forward e-folds
        double get_fast_forward_efolds() const { return(this->ff_efolds); }

        //! Set number of fast-forward e-folds
        void set_fast_forward_efolds(double N) { this->fast_forward = true; this->ff_efolds = (N >= 0.0 ? N : this->ff_efolds); this->cache_stored_time_config_database(); }

        //! Get start time for a twopf configuration
        double get_fast_forward_start(const twopf_kconfig& config) const;


        // INTERFACE- TIME STEP REFINEMENT

      public:

        //! Get number of allowed time step refinements
        unsigned int get_max_refinements() const { return(this->max_refinements); }

        //! Set number of allowed time step refinements
        void set_max_refinements(unsigned int max) { this->max_refinements = (max > 0 ? max : this->max_refinements); }


        // TIME CONFIGURATION DATABASE

      protected:

        //! override cache_stored_time_config_database() supplied by integration_task<> to account for fast-forward integration if used
        virtual void cache_stored_time_config_database() override;


        // INTERNAL API

      protected:

        //! output information about timings
        void write_time_details();

        //! Populate list of time configurations to be sorted, given that the earliest allowed time is tmin.
        //! If we are using fast-forward integration, tmin will be Nstar + ln(kmax) - ff_efolds,
        //! where kmax is the largest k number we are evolving for.
        //! Since this is the start time for that mode, we can't guarantee that results will be available
        //! be at earlier times
        time_config_database build_time_config_database(double Nbegin) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // WRITE TO STREAM

      public:

        //! Write to a standard output stream
        friend std::ostream& operator<< <>(std::ostream& out, const twopf_list_task<number>& obj);


        // INTERNAL DATA

      protected:

        //! database of twopf k-configurations
        twopf_kconfig_database twopf_db;


        // FAST FORWARD INTEGRATION

        //! Whether to use fast-forward integration
        bool fast_forward;

        //! Number of e-folds to use in fast-forward integration
        double ff_efolds;


        // TIME STEP REFINEMENT

        //! How many time step refinements to allow per triangle
        unsigned int max_refinements;

	    };


    template <typename number>
    twopf_list_task<number>::twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t)
	    : integration_task<number>(nm, i, t),
        twopf_db(i.get_model()->compute_kstar(this)),
        fast_forward(true),
        ff_efolds(__CPP_TRANSPORT_DEFAULT_FAST_FORWARD_EFOLDS),
        max_refinements(__CPP_TRANSPORT_DEFAULT_MESH_REFINEMENTS)

      {
	    }


    template <typename number>
    twopf_list_task<number>::twopf_list_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : integration_task<number>(nm, reader, i),
        twopf_db(reader[__CPP_TRANSPORT_NODE_TWOPF_LIST_DATABASE])
	    {
        // deserialize fast-forward integration setting
        fast_forward = reader[__CPP_TRANSPORT_NODE_FAST_FORWARD].asBool();

        // deserialize number of fast-forward efolds
        ff_efolds = reader[__CPP_TRANSPORT_NODE_FAST_FORWARD_EFOLDS].asDouble();

        // deserialize max number of mesh refinements
        max_refinements = reader[__CPP_TRANSPORT_NODE_MESH_REFINEMENTS].asUInt();
	    }


    template <typename number>
    void twopf_list_task<number>::serialize(Json::Value& writer) const
	    {
        // store fast-forward integration setting
        writer[__CPP_TRANSPORT_NODE_FAST_FORWARD] = this->fast_forward;

        // store number of fast-forward efolds
        writer[__CPP_TRANSPORT_NODE_FAST_FORWARD_EFOLDS] = this->ff_efolds;

        // store max number of mesh refinements
        writer[__CPP_TRANSPORT_NODE_MESH_REFINEMENTS] = this->max_refinements;

		    // serialize twopf configuration database
        Json::Value db;
        this->twopf_db.serialize(db);
        writer[__CPP_TRANSPORT_NODE_TWOPF_LIST_DATABASE] = db;

        this->integration_task<number>::serialize(writer);
	    }


    template <typename number>
    const time_config_database twopf_list_task<number>::get_time_config_database(const twopf_kconfig& config) const
      {
        return(this->build_time_config_database(this->get_fast_forward_start(config)));
      }


    template <typename number>
    double twopf_list_task<number>::get_fast_forward_start(const twopf_kconfig& config) const
      {
        return(this->ics.get_Nstar() + log(config.k_conventional) - this->ff_efolds);
      }


    template <typename number>
    std::vector<number> twopf_list_task<number>::get_ics_vector(const twopf_kconfig& config) const
	    {
        return this->integration_task<number>::get_ics_vector(this->get_fast_forward_start(config));
	    }


    template <typename number>
    void twopf_list_task<number>::write_time_details()
      {
        // compute horizon-crossing time for earliest and latest modes (the one with the smallest wavenumber)
        double earliest_crossing = log(this->twopf_db.get_kmin_conventional());
        double latest_crossing   = log(this->twopf_db.get_kmax_conventional());

        std::cout << "'" << this->get_name() << "': ";
        std::cout << __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_A << this->twopf_db.get_kmin_conventional()
          << " " << __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_B;
        if(earliest_crossing > 0) std::cout << "+";
        std::cout << earliest_crossing << ", ";
        std::cout << __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_C << this->twopf_db.get_kmax_conventional()
          << " " << __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_D;
        if(latest_crossing > 0) std::cout << "+";
        std::cout << latest_crossing;
        std::cout << std::endl;

        double earliest_required = this->get_Nstar() + earliest_crossing;
        if(this->fast_forward) earliest_required -= this->ff_efolds;

        if(earliest_required < this->get_Ninit())
          {
            std::ostringstream msg;
            msg << "'" << this->get_name() << "': " << __CPP_TRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_A << earliest_required << " "
              << __CPP_TRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_B << this->get_Ninit();
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
          }

        if(!this->ff_efolds && earliest_required - this->get_Ninit() < 2.0)
          {
            std::cout << "'" << this->get_name() << "': " << __CPP_TRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_A << this->get_Ninit() << " "
              << __CPP_TRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_B << " " << earliest_required-this->get_Ninit() << " "
              << __CPP_TRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_C << std::endl;
          }

        assert(earliest_required >= this->get_Ninit());
      }


		template <typename number>
		time_config_database twopf_list_task<number>::build_time_config_database(double Nbegin) const
			{
        // set up new time configuration database
        time_config_database time_db;

				// check for fast-forward integration, and push only those sample times which are
				// guaranteed to be available for all k-configurations into the database
        double latest_crossing = log(this->twopf_db.get_kmax_conventional());

        double earliest_recordable;
        if(this->fast_forward) earliest_recordable = this->get_Nstar() + latest_crossing - this->ff_efolds;
        else                   earliest_recordable = this->get_Ninit();

        // get raw time sample points
		    const std::vector<double> raw_times = this->times->get_grid();

        if(raw_times.size() == 0)
          {
            std::ostringstream msg;
            msg << "'" << this->get_name() << "': " << __CPP_TRANSPORT_TASK_TWOPF_LIST_NO_TIMES;
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
          }

        bool first = true;
        unsigned int serial = 0;
        for(std::vector<double>::const_iterator t = raw_times.begin(); t != raw_times.end(); t++, serial++)
          {
            if(*t >= Nbegin)
              {
                if(*t >= earliest_recordable)   // don't record any times before the start
                  {
                    if(first)
                      {
                        if(Nbegin < *t) time_db.add_record(Nbegin, false, 0);
                      }
                    time_db.add_record(*t, true, serial);
                  }
                else
                  {
                    time_db.add_record(*t, false, serial);
                  }
              }
          }

        return(time_db);
			}


    template <typename number>
    void twopf_list_task<number>::cache_stored_time_config_database()
      {
        this->stored_time_db.clear();

        // check for fast-forward integration, and push only those sample times which are
        // guaranteed to be available for all k-configurations into the database
        double latest_crossing = log(this->twopf_db.get_kmax_conventional());

        double earliest_recordable;
        if(this->fast_forward) earliest_recordable = this->get_Nstar() + latest_crossing - this->ff_efolds;
        else                   earliest_recordable = this->get_Ninit();

        // get raw time sample points
        const std::vector<double> raw_times = this->times->get_grid();

        unsigned int serial = 0;
        for(std::vector<double>::const_iterator t = raw_times.begin(); t != raw_times.end(); t++, serial++)
          {
            if(!fast_forward || *t >= earliest_recordable)
              {
                this->stored_time_db.add_record(*t, true, serial);
              }
          }
      }


		template <typename number>
		std::ostream& operator<<(std::ostream& out, const twopf_list_task<number>& obj)
	    {
        out << __CPP_TRANSPORT_FAST_FORWARD     << ": " << (obj.fast_forward ? __CPP_TRANSPORT_YES : __CPP_TRANSPORT_NO) << ", ";
        out << __CPP_TRANSPORT_MESH_REFINEMENTS << ": " << obj.max_refinements << std::endl;
				out << static_cast< const integration_task<number>& >(obj);
        return(out);
	    };


	}   // namespace transport


#endif //__twopf_list_task_H_
