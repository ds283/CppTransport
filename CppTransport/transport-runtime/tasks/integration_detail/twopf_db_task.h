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


#ifndef CPPTRANSPORT_TWOPF_DB_TASK_H
#define CPPTRANSPORT_TWOPF_DB_TASK_H


#include <functional>
#include <limits>

#include "transport-runtime/tasks/integration_detail/common.h"
#include "transport-runtime/tasks/integration_detail/abstract.h"

#include "transport-runtime/tasks/configuration-database/twopf_config_database.h"

#include "transport-runtime/models/advisory_classes.h"

#include "transport-runtime/utilities/spline1d.h"

#include "transport-runtime/reporting/key_value.h"

#include "transport-runtime/defaults.h"

#include "boost/math/tools/roots.hpp"
#include "boost/log/utility/formatting_ostream.hpp"


namespace transport
	{

    constexpr auto CPPTRANSPORT_NODE_TWOPF_LIST_KSTAR                 = "kstar";
    constexpr auto CPPTRANSPORT_NODE_TWOPF_LIST_NORMALIZATION         = "normalization";
    constexpr auto CPPTRANSPORT_NODE_TWOPF_LIST_COLLECT_ICS           = "collect-ics";
    constexpr auto CPPTRANSPORT_NODE_TWOPF_LIST_COLLECT_SPECTRAL_DATA = "collect-spectral-data";

    namespace task_impl
      {

        template <typename number>
        class HorizonExitTolerancePredicate
          {

            // CONSTRUCTOR, DESTRUCTOR
            
          public:

            //! constructor
            HorizonExitTolerancePredicate(std::string n, number t)
              : name(std::move(n)),
                tol(t)
              {
              }
            
            //! destructor is default
            ~HorizonExitTolerancePredicate() = default;

            
            // INTERFACE
            
          public:
            
            bool operator()(const number& a, const number& b)
              {
                // the termination condition is based on the absolute difference between the two extreme values,
                // because our final accuracy condition log(aH/k) < 1E-3 is based on the absolute difference
                // in the same way.
                // if we checked for relative tolerances, as previous versions of CppTransport did, then it becomes
                // easy to satisfy the relative tolerance condition when the e-fold number N is large,
                // but the final accuracy condition is still violated.
                auto unsigned_diff = std::abs(a - b);

                assert(!std::isinf(unsigned_diff));
                assert(!std::isnan(unsigned_diff));

                // check for invalid results
                if(std::isinf(unsigned_diff))
                  {
                    std::ostringstream msg;
                    msg << "'" << name << "': " << CPPTRANSPORT_TASK_SEARCH_ROOT_INF;
                    throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, msg.str());
                  }
                if(std::isnan(unsigned_diff))
                  {
                    std::ostringstream msg;
                    msg << "'" << name << "': " << CPPTRANSPORT_TASK_SEARCH_ROOT_NAN;
                    throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, msg.str());
                  }

                return(unsigned_diff < this->tol);
              }

          private:

            //! cache required tolerance
            number tol;

            //! cache job name, for use in emitting error messages
            std::string name;

          };
    
    
        template <typename SplineObject, typename TolerancePolicy>
        double find_zero_of_spline(const std::string& task_name, std::string bracket_error, SplineObject& sp,
                                   TolerancePolicy& tol)
          {
            // boost::math::tools::bisect assumes that value type and domain type are the same, so we need to
            // promote our domain variables to the value tpye
            using number = typename SplineObject::value_type;
            
            // check extreme values bracket the root
            assert(sp(sp.get_min_x()) * sp(sp.get_max_x()) < 0.0);
            if(sp(sp.get_min_x()) * sp(sp.get_max_x()) >= 0.0)
              {
                std::ostringstream msg;
                msg << "'" << task_name << "': " << bracket_error;
                throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, msg.str());
              }
            
            // boost::math::tools::bisect expects domain and value types to agree, so we need an adapter
            // class to interface the SplineObject
            class spline_adapter
              {
              
              public:
                
                spline_adapter(SplineObject& sp_)
                  : sp(sp_)
                  {
                  }
                
                ~spline_adapter() = default;
                
                typename SplineObject::value_type operator()(typename SplineObject::value_type x)
                  {
                    return this->sp(static_cast<typename SplineObject::domain_type>(x));
                  }
                
              private:
                
                SplineObject& sp;
                
              };
    
            // find root; note use of std::ref, because root finder would normally would take a copy of
            // its system function and this is slow -- we have to copy the whole spline
            boost::uintmax_t max_iter = CPPTRANSPORT_MAX_ITERATIONS;
            spline_adapter adapted_sp{sp};
            std::pair< number, number > result =
              boost::math::tools::bisect(std::ref(adapted_sp), static_cast<number>(sp.get_min_x()),
                                         static_cast<number>(sp.get_max_x()), tol, max_iter);

            // may require explicit cast down to double
            double res = static_cast<double>((result.first + result.second)/2.0);

            assert(max_iter < CPPTRANSPORT_MAX_ITERATIONS);

            // could check that tol(result.first, result.second) is strictly satisfied, but boost::bisect occasionally
            // seems to return pairs which don't satisfy this.
            // This may be a bug in Boost, or it may indicate that the returned interval is slightly different from
            // the interval used to check for termination.
            // We can instead rely on the check that |k-aH| is sufficiently small, made below
//            assert(tol(result.first, result.second));

            assert(!std::isinf(result.first) && !std::isinf(result.second));
            assert(!std::isnan(result.first) && !std::isnan(result.second));

            // check for invalid return conditions

            // maximum number of iterations exceeded? if so, result need not be accurate
            if(max_iter >= CPPTRANSPORT_MAX_ITERATIONS)
              {
                std::ostringstream msg;
                msg << "'" << task_name << "': " << CPPTRANSPORT_TASK_SEARCH_ROOT_ACCURACY << " [" << CPPTRANSPORT_TASK_SEARCH_ROOT_ITERATIONS << "=" << max_iter << "]";
                throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, msg.str());
              }

            // infinity returned?
            if(std::isinf(result.first) || std::isinf(result.second))
              {
                std::ostringstream msg;
                msg << "'" << task_name << "': " << CPPTRANSPORT_TASK_SEARCH_ROOT_INF;
                throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, msg.str());
              }

            // NaN returned?
            if(std::isnan(result.first) || std::isnan(result.second))
              {
                std::ostringstream msg;
                msg << "'" << task_name << "': " << CPPTRANSPORT_TASK_SEARCH_ROOT_NAN;
                throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, msg.str());
              }

            // check root is accurate enough
            assert(std::abs(sp(res)) < CPPTRANSPORT_ROOT_FIND_ACCURACY);
            if(std::abs(sp(res)) >= CPPTRANSPORT_ROOT_FIND_ACCURACY)
              {
                std::ostringstream msg;
                msg << "'" << task_name << "': " << CPPTRANSPORT_TASK_SEARCH_ROOT_ACCURACY << " [" << CPPTRANSPORT_TASK_SEARCH_ROOT_ZERO_EQ << "=" << sp(res) << "]";
                throw runtime_exception(exception_type::TASK_STRUCTURE_ERROR, msg.str());
              }

            return res;
          };


      }   // namespace task_impl


    //! Base type for a task which can represent a set of two-point functions evaluated at different wavenumbers.
    //! Ultimately, all n-point-function integrations are of this type because they all solve for the two-point function
    //! even if the goal is to compute a higher n-point function.
    //! The key concept associated with a twopf_db_task is a database of wavenumbers
    //! which describe the points at which we sample the twopf.
    template <typename number>
    class twopf_db_task: public integration_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a twopf-list-task object
        twopf_db_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t, bool adpt_ics,
                      double ast=CPPTRANSPORT_DEFAULT_ASTAR_NORMALIZATION);

        //! deserialization constructor
        twopf_db_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i);

        //! destructor is default
        virtual ~twopf_db_task() = default;


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
        void write_kconfig_database(sqlite3* handle) override;

		    //! Check whether twopf database has been modified
		    bool is_kconfig_database_modified() const override { return(this->twopf_db->is_modified()); }

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
        //! This determines the value of the scale factor a(t) at the distinguished scale k*.
        //! The value supplied is ln(a*), so it can be positive or negative
        double get_astar_normalization() const { return(this->astar_normalization); }

        //! Set current a* normalization
        //! The value supplied is ln(a*), not a* itself
        twopf_db_task<number>& set_astar_normalization(double astar) const { this->astar_normalization = astar; }


        // INTERFACE - ADAPTIVE INITIAL CONDITIONS

      public:

        //! Get adaptive ics setting
        bool get_adaptive_ics() const { return(this->adaptive_ics); }

        //! Set adaptive ics setting
        virtual twopf_db_task<number>& set_adaptive_ics(bool g)
          {
            this->adaptive_ics = g;
            this->validate_subhorizon_efolds();
            this->cache_stored_time_config_database(this->twopf_db->get_kmax_conventional());
            return *this;
          }

        //! Get number of adaptive e-folds
        double get_adaptive_ics_efolds() const { return(this->adaptive_efolds); }

        //! Set adaptive e-folds
        virtual twopf_db_task<number>& set_adaptive_ics_efolds(double N)
          {
            this->adaptive_ics = true;
            this->adaptive_efolds = (N >= 0.0 ? N : this->adaptive_efolds);
            this->validate_subhorizon_efolds();
            this->cache_stored_time_config_database(this->twopf_db->get_kmax_conventional());
            return *this;
          }

        //! Get start time for a twopf configuration
        double get_initial_time(const twopf_kconfig& config) const;


        // INTERFACE- TIME STEP REFINEMENT

      public:

        //! Get number of allowed time step refinements
        unsigned int get_max_refinements() const { return(this->max_refinements); }

        //! Set number of allowed time step refinements
        twopf_db_task<number>& set_max_refinements(unsigned int max) { this->max_refinements = (max > 0 ? max : this->max_refinements); return *this; }


		    // INTERFACE - OPTIONAL COLLECTION BEHAVIOUR

      public:

		    //! Get current collection status for initial conditions
		    bool get_collect_initial_conditions() const { return(this->collect_initial_conditions); }

		    //! Set current collection status for initial conditions
		    twopf_db_task<number>& set_collect_initial_conditions(bool g) { this->collect_initial_conditions = g; return *this; }

		    //! Get current collection status for spectral data
		    bool get_collect_spectral_data() const { return(this->collect_spectral_data); }

		    //! Set current collection status for spectral data
		    twopf_db_task<number>& set_collect_spectral_data(bool g) { this->collect_spectral_data = g; this *this; }


        // TIME CONFIGURATION DATABASE

      protected:

        //! override cache_stored_time_config_database()
        //! supplied by integration_task<> in order to account for adaptive ics
        //! if they are being used
        void cache_stored_time_config_database(double largest_conventional_k) override;


        // INTERNAL API

      protected:

        //! output advisory information about horizon crossing times and number of subhorizon efolds
        void write_time_details(reporting::key_value& kv);

        //! validate intended number of subhorizon efolds (if adaptive ics are being used),
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


        // ADAPTIVE INITIAL CONDITIONS

        //! Whether to use adaptive ics
        bool adaptive_ics;

        //! Number of e-folds to use in adaptive initial conditions
        double adaptive_efolds;


        // TIME STEP REFINEMENT

        //! How many time step refinements to allow per triangle
        unsigned int max_refinements;


		    // OPTIONAL DATA COLLECTION

		    //! Write initial conditions into the container during integration?
		    bool collect_initial_conditions;

		    //! Collect spectral data?
		    bool collect_spectral_data;


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
    twopf_db_task<number>::twopf_db_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t,
                                         bool adpt_ics, double ast)
	    : integration_task<number>(nm, i, t),
        adaptive_ics(adpt_ics),
        adaptive_efolds(CPPTRANSPORT_DEFAULT_ADAPTIVE_ICS_EFOLDS),
        max_refinements(CPPTRANSPORT_DEFAULT_MESH_REFINEMENTS),
        astar_normalization(ast),
        collect_initial_conditions(CPPTRANSPORT_DEFAULT_COLLECT_INITIAL_CONDITIONS),
        collect_spectral_data(CPPTRANSPORT_DEFAULT_COLLECT_SPECTRAL_DATA),
        kstar(i.get_model()->compute_kstar(this))     // compute k* for our choice of horizon-crossing time
      {
		    twopf_db = std::make_shared<twopf_kconfig_database>(kstar);
	    }


    template <typename number>
    twopf_db_task<number>::twopf_db_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i)
	    : integration_task<number>(nm, reader, i)
	    {
        kstar    = reader[CPPTRANSPORT_NODE_TWOPF_LIST_KSTAR].asDouble();
        twopf_db = std::make_shared<twopf_kconfig_database>(kstar, handle);

        adaptive_ics               = reader[CPPTRANSPORT_NODE_ADAPTIVE_ICS].asBool();
        adaptive_efolds            = reader[CPPTRANSPORT_NODE_ADAPTIVE_ICS_EFOLDS].asDouble();
        max_refinements            = reader[CPPTRANSPORT_NODE_MESH_REFINEMENTS].asUInt();
        astar_normalization        = reader[CPPTRANSPORT_NODE_TWOPF_LIST_NORMALIZATION].asDouble();
        collect_initial_conditions = reader[CPPTRANSPORT_NODE_TWOPF_LIST_COLLECT_ICS].asBool();

        // use .get() with a default argument; this field was introduced in 2021.1, so may not be present in older repositories
        collect_spectral_data      = reader.get(CPPTRANSPORT_NODE_TWOPF_LIST_COLLECT_SPECTRAL_DATA, Json::Value(false)).asBool();
	    }


    template <typename number>
    void twopf_db_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_ADAPTIVE_ICS]                     = this->adaptive_ics;
        writer[CPPTRANSPORT_NODE_ADAPTIVE_ICS_EFOLDS]              = this->adaptive_efolds;
        writer[CPPTRANSPORT_NODE_MESH_REFINEMENTS]                 = this->max_refinements;
        writer[CPPTRANSPORT_NODE_TWOPF_LIST_NORMALIZATION]         = this->astar_normalization;
        writer[CPPTRANSPORT_NODE_TWOPF_LIST_COLLECT_ICS]           = this->collect_initial_conditions;
        writer[CPPTRANSPORT_NODE_TWOPF_LIST_COLLECT_SPECTRAL_DATA] = this->collect_spectral_data;
		    writer[CPPTRANSPORT_NODE_TWOPF_LIST_KSTAR]                 = this->kstar;

		    // twopf database is serialized separately into an SQLite database
        // this serialization is handled by the repository layer via write_kconfig_database() below

        this->integration_task<number>::serialize(writer);
	    }


    template <typename number>
    void twopf_db_task<number>::write_kconfig_database(sqlite3* handle)
	    {
        this->twopf_db->write(handle);
	    }


    template <typename number>
    const time_config_database twopf_db_task<number>::get_time_config_database(const twopf_kconfig& config) const
      {
        return this->build_time_config_database(this->get_initial_time(config), this->twopf_db->get_kmax_conventional());
      }


    template <typename number>
    double twopf_db_task<number>::get_initial_time(const twopf_kconfig& config) const
      {
        if(this->adaptive_ics)
          {
            return(config.t_massless - this->adaptive_efolds);
          }
        else
          {
            return(this->ics.get_N_initial());
          }
      }


    template <typename number>
    std::vector<number> twopf_db_task<number>::get_ics_vector(const twopf_kconfig& config) const
	    {
        if(this->adaptive_ics)
          {
            return this->integration_task<number>::get_ics_vector(this->get_initial_time(config));
          }
        else
          {
            return this->ics.get_vector();
          }
	    }


		template <typename number>
		double twopf_db_task<number>::get_ics_exit_time(const twopf_kconfig& config) const
			{
				return(config.t_exit);
			}


		template <typename number>
		std::vector<number> twopf_db_task<number>::get_ics_exit_vector(const twopf_kconfig& config) const
			{
				return this->integration_task<number>::get_ics_vector(this->get_ics_exit_time(config));
			}


    template <typename number>
    void twopf_db_task<number>::write_time_details(reporting::key_value& kv)
      {
        // compute horizon-crossing time for earliest mode (the one with the smallest wavenumber) and the latest one (the one with the largest wavenumber)

		    double earliest_crossing = std::numeric_limits<double>::max();
		    double latest_crossing   = -std::numeric_limits<double>::max();

        for(auto t = this->twopf_db->config_begin(); t != this->twopf_db->config_end(); ++t)
	        {
		        if(t->t_exit < earliest_crossing) earliest_crossing = t->t_exit;
		        if(t->t_exit > latest_crossing)   latest_crossing = t->t_exit;
	        }

		    // these are measured relative to horizon crossing
		    earliest_crossing -= this->get_N_horizon_crossing();
		    latest_crossing   -= this->get_N_horizon_crossing();

        kv.insert_back(CPPTRANSPORT_TASK_DATA_SMALLEST, format_number(this->twopf_db->get_kmin_conventional()));
        kv.insert_back(CPPTRANSPORT_TASK_DATA_LARGEST, format_number(this->twopf_db->get_kmax_conventional()));
        kv.insert_back(CPPTRANSPORT_TASK_DATA_EARLIEST,
                       std::string(CPPTRANSPORT_TASK_DATA_NSTAR) + (earliest_crossing > 0 ? "+" : "") +
                       format_number(earliest_crossing, 4));
        kv.insert_back(CPPTRANSPORT_TASK_DATA_LATEST,
                       std::string(CPPTRANSPORT_TASK_DATA_NSTAR) + (latest_crossing > 0 ? "+" : "") +
                       format_number(latest_crossing, 4));

        this->validate_subhorizon_efolds();

        try
          {
            double end_of_inflation = this->get_N_end_of_inflation();

            kv.insert_back(CPPTRANSPORT_TASK_DATA_END_INFLATION, std::string(CPPTRANSPORT_TASK_DATA_N) + format_number(end_of_inflation, 4));

            // check if end time is after the end of inflation
            double end_time = this->times->get_grid().back();
            if(end_time > end_of_inflation)
              {
                std::ostringstream message;
                message << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_WARN_LATE_END;
                this->get_model()->warn(message.str());
              }
          }
        catch(end_of_inflation_not_found& xe)
          {
            std::ostringstream message;
            message << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_NO_END_INFLATION;
            this->get_model()->warn(message.str());
          }
      }


    template <typename number>
    void twopf_db_task<number>::validate_subhorizon_efolds()
      {
        double earliest_required = std::numeric_limits<double>::max();
        double earliest_tmassless = std::numeric_limits<double>::max();

        for(auto t = this->twopf_db->config_begin(); t != this->twopf_db->config_end(); ++t)
	        {
            double ics_time = this->get_initial_time(*t);
            if(ics_time < earliest_required) earliest_required = ics_time;
            if(t->t_massless < earliest_tmassless) earliest_tmassless = t->t_massless;
	        }

        // check for inconsistent configurations or warn about potentially dangerous scenarios, as needed
        // first, earliest t_massless should be > time of initial conditions
        // this has to be true otherwise we would have failed to compute t_massless for some configurations
        // and therefore wouldn't have reached this point
        if(earliest_tmassless < this->get_N_initial())
          {
            std::ostringstream msg;
            msg << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_VALIDATE_INCONSISTENT;
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        if(adaptive_ics)
          {
            // is earliest required time earlier than time of initial conditions?
            // this can only occur with adaptive_ics; in other cases, the earliest required
            // time is always the initial time anyway
            if(earliest_required < this->get_N_initial())
              {
                throw adaptive_ics_before_Ninit(this->get_name(), this-> adaptive_efolds, earliest_required, this->get_N_initial());
              }

            // does the initial time allow for 'settling'?
            // if unsure, issue warning
            if(earliest_required - this->get_N_initial() < CPPTRANSPORT_DEFAULT_RECOMMENDED_SETTLE_EFOLDS)
              {
                std::ostringstream msg;
                msg << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_A << format_number(this->adaptive_efolds, 4) << " "
                  << CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_B << format_number(earliest_required, 4) << " "
                  << CPPTRANSPORT_TASK_TWOPF_LIST_SETTLING_A << " " << format_number(earliest_required - this->get_N_initial(), 4) << " "
                  << CPPTRANSPORT_TASK_TWOPF_LIST_SETTLING_B;
                this->get_model()->warn(msg.str());
              }
          }
        else
          {
            // if no adaptive initial conditions, then need only check that earliest t_massless gives sufficient
            // number of massless e-folds

            // note concept of 'settling' doesn't really exist in this case; we have to assume that the initial conditions
            // are already 'settled', perhaps by using initial_conditions<> to offset them when they were set up
            if(earliest_tmassless - this->get_N_initial() < CPPTRANSPORT_DEFAULT_RECOMMENDED_EFOLDS)
              {
                std::ostringstream msg;
                msg << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_A << format_number(earliest_tmassless, 4) << " "
                  << CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_B << " " << format_number(earliest_tmassless - this->get_N_initial(), 4) << " "
                  << CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_C;
                this->get_model()->warn(msg.str());
              }
          }
      }


    template <typename number>
    double twopf_db_task<number>::get_earliest_recordable_time(double largest_conventional_k) const
      {
        twopf_kconfig_database::const_record_iterator rec = this->twopf_db->find(largest_conventional_k);

        if(rec == this->twopf_db->record_cend())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_TWOPF_LIST_DATABASE_MISS << " " << largest_conventional_k;
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        return this->get_initial_time(*(*rec));
      }


		template <typename number>
		time_config_database twopf_db_task<number>::build_time_config_database(double Nbegin, double largest_conventional_k) const
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
    void twopf_db_task<number>::cache_stored_time_config_database(double largest_conventional_k)
      {
        this->stored_time_db.clear();

        // check for adaptive initial conditions, and push only those sample times which are
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
		void twopf_db_task<number>::compute_horizon_exit_times()
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

            this->twopf_compute_horizon_exit_times
              (log_aH_sp, log_a2H2M_sp, task_impl::HorizonExitTolerancePredicate<number>{this->name, CPPTRANSPORT_ROOT_FIND_TOLERANCE});
          }
        catch(failed_to_compute_horizon_exit& xe)
          {
            this->compute_horizon_exit_times_fail(xe);
            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
        catch(Hsq_is_negative& xe)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_HSQ_IS_NEGATIVE << " " << xe.what();
            this->get_model()->error(msg.str());
    
            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
        catch(integration_produced_nan& xe)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_INTEGRATION_PRODUCED_NAN << " " << xe.what();
            this->get_model()->error(msg.str());
    
            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
        catch(eps_is_negative& xe)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_EPS_IS_NEGATIVE << " " << xe.what();
            this->get_model()->error(msg.str());

            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
        catch(eps_too_large& xe)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_EPS_TOO_LARGE << " " << xe.what();
            this->get_model()->error(msg.str());

            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
        catch(V_is_negative& xe)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_V_IS_NEGATIVE << " " << xe.what();
            this->get_model()->error(msg.str());

            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
			};


		template <typename number>
		template <typename SplineObject, typename TolerancePolicy>
		void twopf_db_task<number>::twopf_compute_horizon_exit_times(SplineObject& log_aH, SplineObject& log_a2H2M, TolerancePolicy tol)
			{
		    for(auto t = this->twopf_db->config_begin(); t != this->twopf_db->config_end(); ++t)
			    {
		        // set spline to evaluate log(aH)-log(k) and then solve for N which makes this
            // zero, hence k = aH
		        log_aH.set_offset(log(t->k_comoving));

            // update database with computed horizon exit time
		        t->t_exit = task_impl::find_zero_of_spline(this->name, CPPTRANSPORT_TASK_SEARCH_ROOT_BRACKET_EXIT, log_aH, tol);
            t->t_massless = this->compute_t_massless(*t, t->t_exit, log_a2H2M, tol);
			    }
			}


    template <typename number>
    template <typename SplineObject, typename TolerancePolicy>
    double twopf_db_task<number>::compute_t_massless(const twopf_kconfig& config, double t_exit,
                                                     SplineObject& log_a2H2M, TolerancePolicy tol)
      {
        // k-modes are conceptually 'double's, but the values returned from model functions such as
        // log_a2H2M are 'number'. So here, we promote log_k to 'number'
        number log_k = std::log(config.k_comoving);

        // test whether this k-mode is heavy at the initial time
        number kM_ratio_init = 2.0*log_k - log_a2H2M(this->ics.get_N_initial());
        bool light_at_init = kM_ratio_init > 0.0;

        // test whether this k-mode is heavy at horizon exit
        number kM_ratio_exit = 2.0*log_k - log_a2H2M(t_exit);
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

            t_massless = task_impl::find_zero_of_spline(this->name, CPPTRANSPORT_TASK_SEARCH_ROOT_BRACKET_MASSLESS, log_a2H2M, tol);

            log_a2H2M.set_offset(0.0);
          }
        else if(!light_at_init && light_at_exit)
          {
            // mode is heavy at initial time but becomes light by horizon exit;
            // does this ever happen in practice?

            throw no_massless_time(static_cast<double>(kM_ratio_init), static_cast<double>(kM_ratio_exit),
                                   t_exit, config.k_conventional);
          }
        else if(!light_at_init && !light_at_exit)
          {
            // mode is never light

            throw no_massless_time(static_cast<double>(kM_ratio_init), static_cast<double>(kM_ratio_exit),
                                   t_exit, config.k_conventional);
          }

        return(t_massless);
      }


    template <typename number>
    void twopf_db_task<number>::compute_horizon_exit_times_fail(failed_to_compute_horizon_exit& xe)
      {
        std::ostringstream msg1;
        msg1 << "'" << this->get_name() << "': ";
        msg1 << CPPTRANSPORT_TASK_FAIL_COMPUTE_HEXIT;
        this->get_model()->message(msg1.str());

        std::ostringstream msg2;
        msg2 << CPPTRANSPORT_TASK_SEARCH_FROM << xe.get_search_begin() << " " << CPPTRANSPORT_TASK_SEARCH_TO << xe.get_search_end() << " ";

        if(xe.get_found_end())
          {
            msg2 << CPPTRANSPORT_TASK_SEARCH_FOUND_END;
          }
        else
          {
            msg2 << CPPTRANSPORT_TASK_SEARCH_NO_FOUND_END;
          }
        this->get_model()->message(msg2.str());

        std::ostringstream msg3;
        msg3 << CPPTRANSPORT_TASK_SEARCH_RECORDED << " " << xe.get_N_samples() << " " << CPPTRANSPORT_TASK_SEARCH_SAMPLES;
        msg3 << ", " << CPPTRANSPORT_TASK_SEARCH_LAST_SAMPLE << xe.get_last_log_aH() << " ";
        msg3 << CPPTRANSPORT_TASK_SEARCH_LAST_SAMPLE_TIME << xe.get_last_N();
        msg3 << ", " << CPPTRANSPORT_TASK_SEARCH_LARGEST_K << std::log(xe.get_largest_k());
        msg3 << ", " << CPPTRANSPORT_TASK_SEARCH_KAH << xe.get_largest_k()/std::exp(xe.get_last_log_aH());
        this->get_model()->message(msg3.str());

        if(xe.get_found_end() && xe.get_N_samples() > 1 && xe.get_last_log_aH() < std::log(xe.get_largest_k()))
          {
            this->get_model()->warn(CPPTRANSPORT_TASK_SEARCH_GUESS_FAIL);
          }
        else if(xe.get_found_end() && xe.get_N_samples() > 1 && xe.get_last_log_aH() > std::log(xe.get_largest_k()))
          {
            this->get_model()->warn(CPPTRANSPORT_TASK_SEARCH_TOO_CLOSE_FAIL);
          }
      }


    template <typename number>
    template <typename Stream>
    void twopf_db_task<number>::write(Stream& out) const
      {
        out << CPPTRANSPORT_ADAPTIVE_ICS << ": " << (this->get_adaptive_ics() ? CPPTRANSPORT_YES : CPPTRANSPORT_NO);
        out << " " << CPPTRANSPORT_WITH << " " << this->adaptive_efolds << " " << CPPTRANSPORT_EFOLDS << ", ";
        out << CPPTRANSPORT_MESH_REFINEMENTS << ": " << this->get_max_refinements() << '\n';
        out << CPPTRANSPORT_COLLECTING_ICS << ": " << (this->collect_initial_conditions ? CPPTRANSPORT_YES : CPPTRANSPORT_NO) << "\n";
        out << CPPTRANSPORT_COLLECTING_SPECTRAL_DATA << ": " << (this->collect_spectral_data ? CPPTRANSPORT_YES : CPPTRANSPORT_NO) << "\n";

        // forward to underlying write method of integration_task<>
        out << static_cast< const integration_task<number>& >(*this);
      }


		template <typename number, typename Char, typename Traits>
		std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const twopf_db_task<number>& obj)
	    {
        obj.write(out);
        return(out);
	    };


    template <typename number, typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const twopf_db_task<number>& obj)
      {
        obj.write(out);
        return(out);
      };


	}   // namespace transport


#endif //CPPTRANSPORT_TWOPF_DB_TASK_H
