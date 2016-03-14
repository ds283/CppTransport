//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_TWOPF_LIST_TASK_H
#define CPPTRANSPORT_TWOPF_LIST_TASK_H


#include <functional>
#include <limits>

#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/tasks/integration_detail/abstract.h"

#include "transport-runtime-api/tasks/configuration-database/twopf_config_database.h"

#include "transport-runtime-api/models/advisory_classes.h"

#include "transport-runtime-api/utilities/spline1d.h"

#include "transport-runtime-api/defaults.h"

#include "boost/math/tools/roots.hpp"
#include "boost/log/utility/formatting_ostream.hpp"



namespace transport
	{

    constexpr auto CPPTRANSPORT_NODE_TWOPF_LIST_KSTAR = "kstar";
    constexpr auto CPPTRANSPORT_NODE_TWOPF_LIST_NORMALIZATION = "normalization";
    constexpr auto CPPTRANSPORT_NODE_TWOPF_LIST_COLLECT_ICS = "collect-ics";

    namespace task_impl
      {

        class TolerancePredicate
          {
          public:
            TolerancePredicate(double t)
              : tol(t)
              {
              }

            bool operator()(const double& a, const double& b)
              {
                return(this->fractional_diff(a,b) - this->tol < 0);
              }

            double fractional_diff(const double& a, const double& b)
              {
                double frac = 2.0*(a-b)/(std::abs(a)+std::abs(b));

                assert(!std::isinf(frac));
                assert(!std::isnan(frac));
                if(std::isinf(frac)) throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, CPPTRANSPORT_TASK_SEARCH_ROOT_INF);
                if(std::isnan(frac)) throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, CPPTRANSPORT_TASK_SEARCH_ROOT_NAN);

                return(std::abs(frac));
              }

          private:
            double tol;
          };


        template <typename SplineObject, typename TolerancePolicy>
        double find_zero_of_spline(SplineObject& sp, TolerancePolicy& tol)
          {
            // find root; note use of std::ref, because root finder would normally would take a copy of
            // its system function and this is slow -- we have to copy the whole spline
            assert(sp(sp.get_min_x()) * sp(sp.get_max_x()) < 0.0);
            if(sp(sp.get_min_x()) * sp(sp.get_max_x()) >= 0.0)
              {
                throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, CPPTRANSPORT_TASK_SEARCH_ROOT_BRACKET);
              }

            boost::uintmax_t max_iter = CPPTRANSPORT_MAX_ITERATIONS;
            std::pair< double, double > result = boost::math::tools::bisect(std::ref(sp), sp.get_min_x(), sp.get_max_x(), tol, max_iter);

            double res = (result.first + result.second)/2.0;

            assert(max_iter < CPPTRANSPORT_MAX_ITERATIONS);

            // could check that tol(result.first, result.second) is strictly satisfied, but boost::bisect occasionally
            // seems to return pairs which don't satisfy this.
            // This may be a bug in Boost, or it may indicate that the returned interval is slightly different from
            // the interval used to check for termination.
            // We can instead rely on the check that |k-aH| is sufficiently small, made below
//            assert(tol(result.first, result.second));

            assert(!std::isinf(result.first) && !std::isinf(result.second));
            assert(!std::isnan(result.first) && !std::isnan(result.second));

            if(max_iter >= CPPTRANSPORT_MAX_ITERATIONS)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_SEARCH_ROOT_ACCURACY << " [" << CPPTRANSPORT_TASK_SEARCH_ROOT_ITERATIONS << "=" << max_iter << "]";
                throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, msg.str());
              }

            if(std::isinf(result.first) || std::isinf(result.second)) throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, CPPTRANSPORT_TASK_SEARCH_ROOT_INF);
            if(std::isnan(result.first) || std::isnan(result.second)) throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, CPPTRANSPORT_TASK_SEARCH_ROOT_NAN);

            assert(std::abs(sp(res)) < CPPTRANSPORT_ROOT_FIND_ACCURACY);
            if(std::abs(sp(res)) >= CPPTRANSPORT_ROOT_FIND_ACCURACY)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_SEARCH_ROOT_ACCURACY << " [" << CPPTRANSPORT_TASK_SEARCH_ROOT_ZERO_EQ << "=" << sp(res) << "]";
                throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, msg.str());
              }

            return(res);
          };


      }   // namespace task_impl


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
        twopf_list_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t, bool ff, double ast=CPPTRANSPORT_DEFAULT_ASTAR_NORMALIZATION);

        //! deserialization constructor
        twopf_list_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i);

        virtual ~twopf_list_task() = default;


        // INTERFACE - TWOPF K-CONFIGURATIONS

      public:

        //! Provide access to twopf k-configuration database
        const twopf_kconfig_database& get_twopf_database() const { return(*(this->twopf_db)); }

		    //! Compute horizon-exit times for each mode in the database,
        //! and also values of t_massless, the point where each mode becomes massless

		    //! Should be called once the database has been populated.
		    //! Horizon exit times and massless times are stored when the database is serialized,
        //! so does not need to be called again.
		    virtual void compute_horizon_exit_times();

        //! Write k-configuration database to disk
        virtual void write_kconfig_database(sqlite3* handle) override;

		    //! Check whether twopf database has been modified
		    virtual bool is_kconfig_database_modified() const override { return(this->twopf_db->is_modified()); }

      protected:

		    //! Compute horizon-exit times for each mode in the database -- use supplied spline;
        //! also compute t_massless
		    template <typename SplineObject, typename TolerancePolicy>
		    void twopf_compute_horizon_exit_times(SplineObject& log_aH, SplineObject& log_a2H2M, TolerancePolicy tol);

        //! Compute t_massless for a k-mode
        //! Before t_massless the mode can safely be approximated as massless because
        // (k/a)^2 >> the largest eigenvalue of the mass matrix
        //! Uses supplied spline which gives time dependence of largest eigenvalue of the mass matrix
        template <typename SplineObject, typename TolerancePolicy>
        double compute_t_massless(const twopf_kconfig& config, double t_exit, SplineObject& log_a2H2M,
                                  TolerancePolicy tol);

        //! Issue advisory messages if failed to compute horizon exit times
        void compute_horizon_exit_times_fail(failed_to_compute_horizon_exit& xe);


		    // INTERFACE - INITIAL CONDITIONS AND INTEGRATION DETAILS

      public:

        //! Get std::vector of initial conditions, offset using fast forwarding if enabled
        std::vector<number> get_ics_vector(const twopf_kconfig& kconfig) const;

		    //! Get time of horizon exit for a k-configuration
		    double get_ics_exit_time(const twopf_kconfig& config) const;

		    //! Get std::vector of initial conditions at horizon exit time for a k-configuration
        std::vector<number> get_ics_exit_vector(const twopf_kconfig& config) const;

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
        virtual void set_fast_forward(bool g) { this->fast_forward = g; this->validate_subhorizon_efolds(); this->cache_stored_time_config_database(this->twopf_db->get_kmax_conventional()); }

        //! Get number of fast-forward e-folds
        double get_fast_forward_efolds() const { return(this->ff_efolds); }

        //! Set number of fast-forward e-folds
        virtual void set_fast_forward_efolds(double N) { this->fast_forward = true; this->ff_efolds = (N >= 0.0 ? N : this->ff_efolds); this->validate_subhorizon_efolds(); this->cache_stored_time_config_database(this->twopf_db->get_kmax_conventional()); }

        //! Get start time for a twopf configuration
        double get_initial_time(const twopf_kconfig& config) const;


        // INTERFACE- TIME STEP REFINEMENT

      public:

        //! Get number of allowed time step refinements
        unsigned int get_max_refinements() const { return(this->max_refinements); }

        //! Set number of allowed time step refinements
        void set_max_refinements(unsigned int max) { this->max_refinements = (max > 0 ? max : this->max_refinements); }


		    // INTERFACE - COLLECTION OF INITIAL CONDITIONS

      public:

		    //! Get current collection status
		    bool get_collect_initial_conditions() const { return(this->collect_initial_conditions); }

		    //! Set current collection status
		    void set_collect_initial_conditions(bool g) { this->collect_initial_conditions = g; }


        // TIME CONFIGURATION DATABASE

      protected:

        //! override cache_stored_time_config_database()
        //! supplied by integration_task<> in order to account for fast-forward integration
        //! if it is being used
        virtual void cache_stored_time_config_database(double largest_conventional_k) override;


        // INTERNAL API

      protected:

        //! output advisory information about horizon crossing times and number of subhorizon efolds
        void write_time_details();

        //! validate intended number of subhorizon efolds (if fast-forward integration is being used),
        //! or check that initial conditions allow all modes to be subhorizon at the initial time otherwise
        void validate_subhorizon_efolds();

        //! Populate list of time configurations to be stored, beginning from the earliest time Nbegin
        time_config_database build_time_config_database(double Nbegin, double largest_conventional_k) const;

        //! Get earliest time which can safely be recorded for all configurations
        double get_earliest_recordable_time(double largest_conventional_k) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // WRITE TO STREAM

      public:

        //! write self-details to stream
        template <typename Stream> void write(Stream& obj) const;


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


		    // COLLECTION OF INITIAL CONDITIONS

		    //! Write initial conditions into the container during integration?
		    bool collect_initial_conditions;


		    // K-CONFIGURATION DATABASE
		    // (note this has to be declared *after* astar_normalization, so that astar_normalization will be set
		    // when trying to compute k*)

		    //! comoving wavenumber normalization constant
		    double kstar;

        //! database of twopf k-configurations

        //! Use std::shared_ptr<> to manage the lifetime of the database.
        //! This avoids costly copy operations when the database is large.
        //! When a task is cloned, it simply inherits a reference to the
        //! shared database
        std::shared_ptr<twopf_kconfig_database> twopf_db;

	    };


    template <typename number>
    twopf_list_task<number>::twopf_list_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t,
                                             bool ff, double ast)
	    : integration_task<number>(nm, i, t),
        fast_forward(ff),
        ff_efolds(CPPTRANSPORT_DEFAULT_FAST_FORWARD_EFOLDS),
        max_refinements(CPPTRANSPORT_DEFAULT_MESH_REFINEMENTS),
        astar_normalization(ast),
        collect_initial_conditions(CPPTRANSPORT_DEFAULT_COLLECT_INITIAL_CONDITIONS),
        kstar(i.get_model()->compute_kstar(this))
      {
		    twopf_db = std::make_shared<twopf_kconfig_database>(kstar);
	    }


    template <typename number>
    twopf_list_task<number>::twopf_list_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i)
	    : integration_task<number>(nm, reader, i)
	    {
        kstar    = reader[CPPTRANSPORT_NODE_TWOPF_LIST_KSTAR].asDouble();
        twopf_db = std::make_shared<twopf_kconfig_database>(kstar, handle);

        fast_forward               = reader[CPPTRANSPORT_NODE_FAST_FORWARD].asBool();
        ff_efolds                  = reader[CPPTRANSPORT_NODE_FAST_FORWARD_EFOLDS].asDouble();
        max_refinements            = reader[CPPTRANSPORT_NODE_MESH_REFINEMENTS].asUInt();
        astar_normalization        = reader[CPPTRANSPORT_NODE_TWOPF_LIST_NORMALIZATION].asDouble();
        collect_initial_conditions = reader[CPPTRANSPORT_NODE_TWOPF_LIST_COLLECT_ICS].asBool();
	    }


    template <typename number>
    void twopf_list_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_FAST_FORWARD]             = this->fast_forward;
        writer[CPPTRANSPORT_NODE_FAST_FORWARD_EFOLDS]      = this->ff_efolds;
        writer[CPPTRANSPORT_NODE_MESH_REFINEMENTS]         = this->max_refinements;
        writer[CPPTRANSPORT_NODE_TWOPF_LIST_NORMALIZATION] = this->astar_normalization;
        writer[CPPTRANSPORT_NODE_TWOPF_LIST_COLLECT_ICS]   = this->collect_initial_conditions;
		    writer[CPPTRANSPORT_NODE_TWOPF_LIST_KSTAR]         = this->kstar;

		    // twopf database is serialized separately into an SQLite database
        // this serialization is handled by the repository layer via write_kconfig_database() below

        this->integration_task<number>::serialize(writer);
	    }


    template <typename number>
    void twopf_list_task<number>::write_kconfig_database(sqlite3* handle)
	    {
        this->twopf_db->write(handle);
	    }


    template <typename number>
    const time_config_database twopf_list_task<number>::get_time_config_database(const twopf_kconfig& config) const
      {
        return this->build_time_config_database(this->get_initial_time(config), this->twopf_db->get_kmax_conventional());
      }


    template <typename number>
    double twopf_list_task<number>::get_initial_time(const twopf_kconfig& config) const
      {
        if(this->fast_forward)
          {
            return(config.t_massless - this->ff_efolds);
          }
        else
          {
            return(this->ics.get_N_initial());
          }
      }


    template <typename number>
    std::vector<number> twopf_list_task<number>::get_ics_vector(const twopf_kconfig& config) const
	    {
        if(this->fast_forward)
          {
            return this->integration_task<number>::get_ics_vector(this->get_initial_time(config));
          }
        else
          {
            return this->ics.get_vector();
          }
	    }


		template <typename number>
		double twopf_list_task<number>::get_ics_exit_time(const twopf_kconfig& config) const
			{
				return(config.t_exit);
			}


		template <typename number>
		std::vector<number> twopf_list_task<number>::get_ics_exit_vector(const twopf_kconfig& config) const
			{
				return this->integration_task<number>::get_ics_vector(this->get_ics_exit_time(config));
			}


    template <typename number>
    void twopf_list_task<number>::write_time_details()
      {
        // compute horizon-crossing time for earliest mode (the one with the smallest wavenumber) and the latest one (the one with the largest wavenumber)

		    double earliest_crossing = std::numeric_limits<double>::max();
		    double latest_crossing   = -std::numeric_limits<double>::max();

        for(twopf_kconfig_database::const_config_iterator t = this->twopf_db->config_begin(); t != this->twopf_db->config_end(); ++t)
	        {
		        if(t->t_exit < earliest_crossing) earliest_crossing = t->t_exit;
		        if(t->t_exit > latest_crossing)   latest_crossing = t->t_exit;
	        }

		    // these are measured relative to horizon crossing
		    earliest_crossing -= this->get_N_horizon_crossing();
		    latest_crossing   -= this->get_N_horizon_crossing();

        std::cout << "'" << this->get_name() << "': ";
        std::cout << CPPTRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_A << this->twopf_db->get_kmin_conventional()
          << " " << CPPTRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_B;

        std::ostringstream early_time;
		    early_time << std::setprecision(3);
        if(earliest_crossing > 0) early_time << "+";
		    early_time << earliest_crossing;

        std::cout << early_time.str() << ", ";

        std::cout << CPPTRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_C << this->twopf_db->get_kmax_conventional()
          << " " << CPPTRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_D;

        std::ostringstream late_time;
		    late_time << std::setprecision(3);
        if(latest_crossing > 0) late_time << "+";
        late_time << latest_crossing;

        std::cout << late_time.str() << '\n';

        this->validate_subhorizon_efolds();

        try
          {
            double end_of_inflation = this->get_N_end_of_inflation();
            std::cout << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_END_OF_INFLATION  << end_of_inflation << '\n';

            // check if end time is after the end of inflation
            double end_time = this->times->get_grid().back();
            if(end_time > end_of_inflation)
              {
                std::cout << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_WARN_LATE_END << '\n';
              }
          }
        catch(end_of_inflation_not_found& xe)
          {
            std::cout << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_NO_END_INFLATION << '\n';
          }
      }


    template <typename number>
    void twopf_list_task<number>::validate_subhorizon_efolds()
      {
        double earliest_required = std::numeric_limits<double>::max();
        double earliest_tmassless = std::numeric_limits<double>::max();

        for(twopf_kconfig_database::const_config_iterator t = this->twopf_db->config_begin(); t != this->twopf_db->config_end(); ++t)
	        {
            double ics_time = this->get_initial_time(*t);
            if(ics_time < earliest_required) earliest_required = ics_time;
            if(t->t_massless < earliest_tmassless) earliest_tmassless = t->t_massless;
	        }

        if(earliest_required < this->get_N_initial())
          {
            std::ostringstream msg;
            msg << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_A << earliest_required << " "
              << CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_B << this->get_N_initial();
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        if(!this->fast_forward && earliest_tmassless - this->get_N_initial() < CPPTRANSPORT_DEFAULT_RECOMMENDED_EFOLDS)
          {
            std::cout << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_A << this->get_N_initial() << " "
              << CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_B << " " << earliest_required-this->get_N_initial() << " "
              << CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_C << '\n';
          }
      }


    template <typename number>
    double twopf_list_task<number>::get_earliest_recordable_time(double largest_conventional_k) const
      {
        twopf_kconfig_database::record_iterator rec;

        if(!this->twopf_db->find(largest_conventional_k, rec))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_TWOPF_LIST_DATABASE_MISS << " " << largest_conventional_k;
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        return this->get_initial_time(*(*rec));
      }


		template <typename number>
		time_config_database twopf_list_task<number>::build_time_config_database(double Nbegin, double largest_conventional_k) const
			{
        // set up new time configuration database
        time_config_database time_db;

				// find earliest recordable time, which is the initial time of the mode with largest wavenumber
        double earliest_recordable = this->get_earliest_recordable_time(largest_conventional_k);

        // get raw time sample points
		    const std::vector<double> raw_times = this->times->get_grid();

        if(raw_times.size() == 0)
          {
            std::ostringstream msg;
            msg << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_NO_TIMES;
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        bool first = true;
        unsigned int serial = CPPTRANSPORT_TIME_DATABASE_LOWEST_SERIAL;
        for(std::vector<double>::const_iterator t = raw_times.begin(); t != raw_times.end(); ++t, ++serial)
          {
            if(*t >= Nbegin)
              {
                if(*t >= earliest_recordable)   // don't record any times before the start
                  {
                    if(first)
                      {
                        if(Nbegin < *t) time_db.add_record(Nbegin, false, CPPTRANSPORT_TIME_DATABASE_SPECIAL_SERIAL);
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
    void twopf_list_task<number>::cache_stored_time_config_database(double largest_conventional_k)
      {
        this->stored_time_db.clear();

        // check for fast-forward integration, and push only those sample times which are
        // guaranteed to be available for all k-configurations into the database
        double earliest_recordable = this->get_earliest_recordable_time(largest_conventional_k);

        // get raw time sample points
        const std::vector<double> raw_times = this->times->get_grid();

        unsigned int serial = CPPTRANSPORT_TIME_DATABASE_LOWEST_SERIAL;
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
				double largest_k = this->twopf_db->get_kmax_comoving();

		    std::vector<double> N;
		    std::vector<number> log_aH;
        std::vector<number> log_a2H2M;

        try
          {
            this->get_model()->compute_aH(this, N, log_aH, log_a2H2M, largest_k);
            assert(N.size() == log_aH.size());
            assert(N.size() == log_a2H2M.size());

            spline1d<number> log_aH_sp(N, log_aH);
            spline1d<number> log_a2H2M_sp(N, log_a2H2M);

            this->twopf_compute_horizon_exit_times(log_aH_sp, log_a2H2M_sp, task_impl::TolerancePredicate(CPPTRANSPORT_ROOT_FIND_TOLERANCE));
          }
        catch(failed_to_compute_horizon_exit& xe)
          {
            this->compute_horizon_exit_times_fail(xe);
            exit(EXIT_FAILURE);
          }
			};


		template <typename number>
		template <typename SplineObject, typename TolerancePolicy>
		void twopf_list_task<number>::twopf_compute_horizon_exit_times(SplineObject& log_aH, SplineObject& log_a2H2M, TolerancePolicy tol)
			{
		    for(twopf_kconfig_database::config_iterator t = this->twopf_db->config_begin(); t != this->twopf_db->config_end(); ++t)
			    {
		        // set spline to evaluate log(aH)-log(k) and then solve for N which makes this
            // zero, hence k = aH
		        log_aH.set_offset(log(t->k_comoving));

            // update database with computed horizon exit time
		        t->t_exit = task_impl::find_zero_of_spline(log_aH, tol);
            t->t_massless = this->compute_t_massless(*t, t->t_exit, log_a2H2M, tol);
			    }
			}


    template <typename number>
    template <typename SplineObject, typename TolerancePolicy>
    double twopf_list_task<number>::compute_t_massless(const twopf_kconfig& config, double t_exit,
                                                       SplineObject& log_a2H2M, TolerancePolicy tol)
      {
        double log_k = std::log(config.k_comoving);

        // test whether this k-mode is heavy at the initial time
        double kM_ratio_init = 2.0*log_k - log_a2H2M(this->ics.get_N_initial());
        bool light_at_init = kM_ratio_init > 0.0;

        // test whether this k-mode is heavy at horizon exit
        double kM_ratio_exit = 2.0*log_k - log_a2H2M(t_exit);
        bool light_at_exit = kM_ratio_exit > 0.0;

        // sensible default for initial time; to be updated later
        double t_massless = t_exit;

        if(light_at_init && light_at_exit)
          {
            // mass matrix is not relevant any time before horizon exit

            t_massless = t_exit;
          }
        else if(light_at_init && !light_at_exit)
          {
            // mass matrix becomes relevant some time before horizon exit

            log_a2H2M.set_offset(2.0*log_k);
            t_massless = task_impl::find_zero_of_spline(log_a2H2M, tol);
            log_a2H2M.set_offset(0.0);
          }
        else if(!light_at_init && light_at_exit)
          {
            // mode is heavy at initial time but becomes light by horizon exit;
            // does this ever happen in practice?

            throw no_massless_time(kM_ratio_init, kM_ratio_exit, t_exit, config.k_conventional);
          }
        else if(!light_at_init && !light_at_exit)
          {
            // mode is never light

            throw no_massless_time(kM_ratio_init, kM_ratio_exit, t_exit, config.k_conventional);
          }

        return(t_massless);
      }


    template <typename number>
    void twopf_list_task<number>::compute_horizon_exit_times_fail(failed_to_compute_horizon_exit& xe)
      {
        std::cout << "'" << this->get_name() << "': ";
        std::cout << CPPTRANSPORT_TASK_FAIL_COMPUTE_HEXIT << '\n';

        std::cout << CPPTRANSPORT_TASK_SEARCH_FROM << xe.get_search_begin() << " " << CPPTRANSPORT_TASK_SEARCH_TO << xe.get_search_end() << " ";

        if(xe.get_found_end())
          {
            std::cout << CPPTRANSPORT_TASK_SEARCH_FOUND_END;
          }
        else
          {
            std::cout << CPPTRANSPORT_TASK_SEARCH_NO_FOUND_END;
          }
        std::cout << '\n';

        std::cout << CPPTRANSPORT_TASK_SEARCH_RECORDED << " " << xe.get_N_samples() << " " << CPPTRANSPORT_TASK_SEARCH_SAMPLES;
        std::cout << ", " << CPPTRANSPORT_TASK_SEARCH_LAST_SAMPLE << xe.get_last_log_aH() << " ";
        std::cout << CPPTRANSPORT_TASK_SEARCH_LAST_SAMPLE_TIME << xe.get_last_N();
        std::cout << ", " << CPPTRANSPORT_TASK_SEARCH_LARGEST_K << std::log(xe.get_largest_k());
        std::cout << ", " << CPPTRANSPORT_TASK_SEARCH_KAH << xe.get_largest_k()/std::exp(xe.get_last_log_aH()) << '\n';

        if(xe.get_found_end() && xe.get_N_samples() > 1 && xe.get_last_log_aH() < std::log(xe.get_largest_k()))
          {
            std::cout << CPPTRANSPORT_TASK_SEARCH_GUESS_FAIL << '\n';
          }
        else if(xe.get_found_end() && xe.get_N_samples() > 1 && xe.get_last_log_aH() > std::log(xe.get_largest_k()))
          {
            std::cout << CPPTRANSPORT_TASK_SEARCH_TOO_CLOSE_FAIL << '\n';
          }
      }


    template <typename number>
    template <typename Stream>
    void twopf_list_task<number>::write(Stream& out) const
      {
        out << CPPTRANSPORT_FAST_FORWARD     << ": " << (this->get_fast_forward() ? CPPTRANSPORT_YES : CPPTRANSPORT_NO) << ", ";
        out << CPPTRANSPORT_MESH_REFINEMENTS << ": " << this->get_max_refinements() << '\n';
        out << static_cast< const integration_task<number>& >(*this);
      }


		template <typename number, typename Char, typename Traits>
		std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const twopf_list_task<number>& obj)
	    {
        obj.write(out);
        return(out);
	    };


    template <typename number, typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const twopf_list_task<number>& obj)
      {
        obj.write(out);
        return(out);
      };


	}   // namespace transport


#endif //CPPTRANSPORT_TWOPF_LIST_TASK_H
