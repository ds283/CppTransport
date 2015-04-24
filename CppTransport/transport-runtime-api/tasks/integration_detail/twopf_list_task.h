//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __twopf_list_task_H_
#define __twopf_list_task_H_


#include <functional>

#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/tasks/integration_detail/abstract.h"

#include "transport-runtime-api/tasks/configuration-database/twopf_config_database.h"

#include "transport-runtime-api/models/advisory_classes.h"

#include "transport-runtime-api/utilities/spline1d.h"

#include <boost/math/tools/roots.hpp>

#define __CPP_TRANSPORT_NODE_TWOPF_LIST_DATABASE      "twopf-database"
#define __CPP_TRANSPORT_NODE_TWOPF_LIST_NORMALIZATION "normalization"


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
        twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t, bool ff, double ast=__CPP_TRANSPORT_DEFAULT_ASTAR_NORMALIZATION);

        //! deserialization constructor
        twopf_list_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i);

        virtual ~twopf_list_task() = default;


        // INTERFACE - TWOPF K-CONFIGURATIONS

      public:

        //! Provide access to twopf k-configuration database
        const twopf_kconfig_database& get_twopf_database() const { return(this->twopf_db); }

		    //! Compute horizon-exit times for each mode in the database
		    void compute_horizon_exit_times();


		    // INTERFACE - INITIAL CONDITIONS AND INTEGRATION DETAILS

      public:

        //! Get std::vector of initial conditions, offset using fast forwarding if enabled
        std::vector<number> get_ics_vector(const twopf_kconfig& kconfig) const;

        //! Build sample-time database
        const time_config_database get_time_config_database(const twopf_kconfig& config) const;

        //! Get number of subhorizon e-folds of evolution
        double get_N_subhorizon_efolds() const { return(this->ics.get_N_subhorion_efolds()); }

        //! Get horizon-crossing time
        double get_N_horizon_crossing() const { return(this->ics.get_N_horizon_crossing()); }

        //! Get current a* normalization
        double get_astar_normalization() const { return(this->astar_normalization); }


        // INTERFACE - FAST-FORWARD MANAGEMENT

      public:

        //! Get fast-forward integration setting
        bool get_fast_forward() const { return(this->fast_forward); }

        //! Set fast-forward integration setting
        void set_fast_forward(bool g) { this->fast_forward = g; this->validate_subhorizon_efolds(); this->cache_stored_time_config_database(); }

        //! Get number of fast-forward e-folds
        double get_fast_forward_efolds() const { return(this->ff_efolds); }

        //! Set number of fast-forward e-folds
        void set_fast_forward_efolds(double N) { this->fast_forward = true; this->ff_efolds = (N >= 0.0 ? N : this->ff_efolds); this->validate_subhorizon_efolds(); this->cache_stored_time_config_database(); }

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

        //! output advisory information about horizon crossing times and number of subhorizon efolds
        void write_time_details();

        //! validate intended number of subhorizon efolds (if fast-forward integration is being used),
        //! or check that initial conditions allow all modes to be subhorizon at the initial time otherwise
        void validate_subhorizon_efolds();

        //! Populate list of time configurations to be stored.
        //! The parameter N_config_begin specifies the initial time for whatever configuration is being integrated.
        //! If fast-forward integration is being used then this parameter is used to set the initial
        //! time of the integration.
        //! Otherwise, integration begins at the initial conditions time.
        time_config_database build_time_config_database(double N_config_begin) const;


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


        // NORMALIZATION DURING INTEGRATION

        //! during integration, we need to have a normalization for a* = a(t*).
        //! In principle this can be anything (and could be set by the user's time choice),
        //! but in practice the integrator performs best in a fairly narrow range of choices.
        double astar_normalization;


        // FAST FORWARD INTEGRATION

        //! Whether to use fast-forward integration
        bool fast_forward;

        //! Number of e-folds to use in fast-forward integration
        double ff_efolds;


        // TIME STEP REFINEMENT

        //! How many time step refinements to allow per triangle
        unsigned int max_refinements;


		    // K-CONFIGURATION DATABASE
		    // (note this has to be declared *after* astar_normalization, so that astar_normalization will be set
		    // when trying to compute k*)

        //! database of twopf k-configurations
        twopf_kconfig_database twopf_db;

	    };


    template <typename number>
    twopf_list_task<number>::twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                             bool ff, double ast)
	    : integration_task<number>(nm, i, t),
        fast_forward(ff),
        ff_efolds(__CPP_TRANSPORT_DEFAULT_FAST_FORWARD_EFOLDS),
        max_refinements(__CPP_TRANSPORT_DEFAULT_MESH_REFINEMENTS),
        astar_normalization(ast),
		    twopf_db(i.get_model()->compute_kstar(this))
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

        // deserialize max number of stepsize refinements
        max_refinements = reader[__CPP_TRANSPORT_NODE_MESH_REFINEMENTS].asUInt();

        // deserialize a* normalization
        astar_normalization = reader[__CPP_TRANSPORT_NODE_TWOPF_LIST_NORMALIZATION].asDouble();

		    // reconstruct horizon-exit times; these aren't stored in the repository record to save space
		    this->compute_horizon_exit_times();
	    }


    template <typename number>
    void twopf_list_task<number>::serialize(Json::Value& writer) const
	    {
        // store fast-forward integration setting
        writer[__CPP_TRANSPORT_NODE_FAST_FORWARD] = this->fast_forward;

        // store number of fast-forward efolds
        writer[__CPP_TRANSPORT_NODE_FAST_FORWARD_EFOLDS] = this->ff_efolds;

        // store max number of stepsize refinements
        writer[__CPP_TRANSPORT_NODE_MESH_REFINEMENTS] = this->max_refinements;

        // serialize a* normalization
        writer[__CPP_TRANSPORT_NODE_TWOPF_LIST_NORMALIZATION] = this->astar_normalization;

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
        return(config.t_exit - this->ff_efolds);
      }


    template <typename number>
    std::vector<number> twopf_list_task<number>::get_ics_vector(const twopf_kconfig& config) const
	    {
        if(this->fast_forward)
          {
            return this->integration_task<number>::get_ics_vector(this->get_fast_forward_start(config));
          }
        else
          {
            return this->ics.get_vector();
          }
	    }


    template <typename number>
    void twopf_list_task<number>::write_time_details()
      {
        // compute horizon-crossing time for earliest mode (the one with the smallest wavenumber) and the latest one (the one with the largest wavenumber)

		    double earliest_crossing = std::numeric_limits<double>::max();
		    double latest_crossing   = -std::numeric_limits<double>::max();

        for(twopf_kconfig_database::const_config_iterator t = this->twopf_db.config_begin(); t != this->twopf_db.config_end(); ++t)
	        {
		        if(t->t_exit < earliest_crossing) earliest_crossing = t->t_exit;
		        if(t->t_exit > latest_crossing)   latest_crossing = t->t_exit;
	        }

		    // these are measured relative to horizon crossing
		    earliest_crossing -= this->get_N_horizon_crossing();
		    latest_crossing   -= this->get_N_horizon_crossing();

        std::cout << "'" << this->get_name() << "': ";
        std::cout << __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_A << this->twopf_db.get_kmin_conventional()
          << " " << __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_B;

        std::ostringstream early_time;
		    early_time << std::setprecision(3);
        if(earliest_crossing > 0) early_time << "+";
		    early_time << earliest_crossing;

        std::cout << early_time.str() << ", ";

        std::cout << __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_C << this->twopf_db.get_kmax_conventional()
          << " " << __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_D;

        std::ostringstream late_time;
		    late_time << std::setprecision(3);
        if(latest_crossing > 0) late_time << "+";
        late_time << latest_crossing;

        std::cout << late_time.str() << std::endl;

        this->validate_subhorizon_efolds();

        try
          {
            double end_of_inflation = this->get_N_end_of_inflation();
            std::cout << "'" << this->get_name() << "': " << __CPP_TRANSPORT_TASK_TWOPF_LIST_END_OF_INFLATION  << end_of_inflation << std::endl;

            // check if end time is after the end of inflation
            double end_time = this->times->get_grid().back();
            if(end_time > end_of_inflation)
              {
                std::cout << "'" << this->get_name() << "': " << __CPP_TRANSPORT_TASK_TWOPF_LIST_WARN_LATE_END << std::endl;
              }
          }
        catch(end_of_inflation_not_found& xe)
          {
            std::cout << "'" << this->get_name() << "': " << __CPP_TRANSPORT_TASK_TWOPF_LIST_NO_END_INFLATION << std::endl;
          }
      }


    template <typename number>
    void twopf_list_task<number>::validate_subhorizon_efolds()
      {
        double earliest_required = std::numeric_limits<double>::max();

        for(twopf_kconfig_database::const_config_iterator t = this->twopf_db.config_begin(); t != this->twopf_db.config_end(); ++t)
	        {
            if(t->t_exit < earliest_required) earliest_required = t->t_exit;
	        }

        if(this->fast_forward) earliest_required -= this->ff_efolds;

        if(earliest_required < this->get_N_initial())
          {
            std::ostringstream msg;
            msg << "'" << this->get_name() << "': " << __CPP_TRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_A << earliest_required << " "
              << __CPP_TRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_B << this->get_N_initial();
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
          }

        if(!this->fast_forward && earliest_required - this->get_N_initial() < __CPP_TRANSPORT_DEFAULT_RECOMMENDED_EFOLDS)
          {
            std::cout << "'" << this->get_name() << "': " << __CPP_TRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_A << this->get_N_initial() << " "
              << __CPP_TRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_B << " " << earliest_required-this->get_N_initial() << " "
              << __CPP_TRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_C << std::endl;
          }
      }


		template <typename number>
		time_config_database twopf_list_task<number>::build_time_config_database(double N_config_begin) const
			{
        // set up new time configuration database
        time_config_database time_db;

				// check for fast-forward integration, and push only those sample times which are
				// guaranteed to be available for all k-configurations into the database
        double latest_crossing = log(this->twopf_db.get_kmax_conventional());

        double earliest_recordable;
        if(this->fast_forward) earliest_recordable = this->get_N_horizon_crossing() + latest_crossing - this->ff_efolds;
        else                   earliest_recordable = this->get_N_initial();

        double Nbegin = 0.0;
        if(this->fast_forward) Nbegin = N_config_begin;
        else                   Nbegin = this->ics.get_N_initial();

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
        for(std::vector<double>::const_iterator t = raw_times.begin(); t != raw_times.end(); ++t, ++serial)
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
                    first = false;
                  }
                else
                  {
                    time_db.add_record(*t, false, serial);
                  }
              }
          }

		    // should always produce some output
		    assert(time_db.size() > 0);

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
        if(this->fast_forward) earliest_recordable = this->get_N_horizon_crossing() + latest_crossing - this->ff_efolds;
        else                   earliest_recordable = this->get_N_initial();

        // get raw time sample points
        const std::vector<double> raw_times = this->times->get_grid();

        unsigned int serial = 0;
        for(std::vector<double>::const_iterator t = raw_times.begin(); t != raw_times.end(); ++t, ++serial)
          {
            if(*t >= earliest_recordable)
              {
                this->stored_time_db.add_record(*t, true, serial);
              }
          }
      }


		template <typename number>
		void twopf_list_task<number>::compute_horizon_exit_times()
			{
				double largest_k = this->twopf_db.get_kmax_comoving();

		    std::vector<double> N;
		    std::vector<number> aH;
				this->get_model()->compute_aH(this, N, aH, largest_k);

				spline1d<number> sp(N, aH);
		    boost::uintmax_t max_iter = 500;

				class TolerancePredicate
					{
				  public:
						TolerancePredicate(double t)
							: tol(t)
							{
							}

						bool operator()(const double& a, const double& b)
							{
								return(fabs(a-b) < this->tol);
							}

				  private:
						double tol;
					};

		    for(twopf_kconfig_database::config_iterator t = this->twopf_db.config_begin(); t != this->twopf_db.config_end(); ++t)
			    {
				    // set spline to evaluate aH-k and then solve for N
				    sp.set_offset(t->k_comoving);

				    // find root; note use of std::ref, because toms748_solve normally would take a copy of
				    // its system function and this is slow -- we have to copy the whole spline
		        std::pair< double, double > result = boost::math::tools::toms748_solve(std::ref(sp), sp.get_min_x(), sp.get_max_x(), TolerancePredicate(1E-5), max_iter);

						t->t_exit = (result.first + result.second)/2.0;
			    }

			};


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
