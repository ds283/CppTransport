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


#ifndef CPPTRANSPORT_THREEPF_TASK_H
#define CPPTRANSPORT_THREEPF_TASK_H


#include <memory>

#include "transport-runtime/tasks/integration_detail/common.h"
#include "transport-runtime/tasks/integration_detail/abstract.h"
#include "transport-runtime/tasks/integration_detail/twopf_db_task.h"
#include "transport-runtime/tasks/integration_detail/default_policies.h"
#include "transport-runtime/tasks/configuration-database/threepf_config_database.h"

#include "transport-runtime/utilities/spline1d.h"

#include "transport-runtime/reporting/key_value.h"

#include "transport-runtime/defaults.h"

#include "boost/math/tools/roots.hpp"

#include "sqlite3.h"




namespace transport
	{

    constexpr auto CPPTRANSPORT_NODE_THREEPF_INTEGRABLE = "integrable";

    constexpr auto CPPTRANSPORT_NODE_THREEPF_CUBIC_SPACING = "k-spacing";
    constexpr auto CPPTRANSPORT_NODE_THREEPF_ALPHABETA_KT_SPACING = "kt-spacing";
    constexpr auto CPPTRANSPORT_NODE_THREEPF_ALPHABETA_ALPHA_SPACING = "alpha-spacing";
    constexpr auto CPPTRANSPORT_NODE_THREEPF_ALPHABETA_BETA_SPACING = "beta-spacing";

		enum class threepf_ics_exit_type { smallest_wavenumber_exit, kt_wavenumber_exit};

    // three-point function task
    template <typename number=default_number_type>
    class threepf_task: public twopf_db_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a threepf-task
        threepf_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t, bool adpt_ics);

        //! deserialization constructor
        threepf_task(const std::string& n, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i);

        //! Destroy a three-point function task
        virtual ~threepf_task() = default;


        // INTERFACE

      public:

        //! supply 'derivable_task' interface
        task_type get_type() const override final { return task_type::integration; }

        //! respond to task type query
        integration_task_type get_task_type() const override final { return integration_task_type::threepf; }


        // INTERFACE - THREEPF K-CONFIGURATIONS

      public:

        //! Provide access to threepf k-configuration database
        const threepf_kconfig_database& get_threepf_database() const { return(*(this->threepf_db)); }

        //! Compute horizon-exit times for each mode in the database

        //! Should be called once the database has been populated.
        //! Horizon exit times are stored when the database is serialized, so does not need to be called again.
        virtual void compute_horizon_exit_times() override;

        //! Determine whether this task is integrable
        bool is_integrable() const { return(this->integrable); }

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const = 0;

        //! get measure at a particular k-configuration
        virtual number measure(const threepf_kconfig& config) const = 0;

      protected:

        //! Compute horizon-exit times for each mode in the database -- using a supplied spline
        template <typename SplineObject, typename TolerancePolicy>
        void threepf_compute_horizon_exit_times(SplineObject& sp, TolerancePolicy tol);



        // INTERFACE - INITIAL CONDITIONS AND INTEGRATION DETAILS

      public:

        //! Get std::vector of initial conditions, offset using fast forwarding if enabled
        std::vector<number> get_ics_vector(const threepf_kconfig& kconfig) const;

        //! Get time of horizon exit for a k-configuration
        double get_ics_exit_time(const threepf_kconfig& kconfig, threepf_ics_exit_type type=threepf_ics_exit_type::smallest_wavenumber_exit) const;

		    //! Get std::vector of initial conditions at horizon exit time for a k-configuration
		    std::vector<number> get_ics_exit_vector(const threepf_kconfig& kconfig, threepf_ics_exit_type type=threepf_ics_exit_type::smallest_wavenumber_exit) const;

        //! Build time-sample database
        const time_config_database get_time_config_database(const threepf_kconfig& config) const;


        // INTERFACE - FAST FORWARD MANAGEMENT

      public:

        //! Get start time for a threepf configuration
        double get_initial_time(const threepf_kconfig& config) const;

        //! Set adaptics ics setting
        virtual threepf_task<number>& set_adaptive_ics(bool g) override
          {
            this->adaptive_ics = g;
            this->validate_subhorizon_efolds();
            this->cache_stored_time_config_database(this->threepf_db->get_kmax_2pf_conventional());
            return *this;
          }

        //! Set number of adaptive e-folds
        virtual threepf_task<number>& set_adaptive_ics_efolds(double N) override
          {
            this->adaptive_ics = true;
            this->adaptive_efolds = (N >= 0.0 ? N : this->adaptive_efolds);
            this->validate_subhorizon_efolds();
            this->cache_stored_time_config_database(this->threepf_db->get_kmax_2pf_conventional());
            return *this;
          }


        // SERIALIZATION -- implements a 'serialiazble' interface

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // WRITE K-CONFIGURATION DATABASE

      public:

        //! Write k-configuration database to disk
        virtual void write_kconfig_database(sqlite3* handle) override;

		    //! Check whether k-configuration databases have been modified
		    virtual bool is_kconfig_database_modified() const override { return(this->threepf_db->is_modified() || this->twopf_db_task<number>::is_kconfig_database_modified()); }


        // INTERNAL DATA

      protected:

        //! database of threepf k-configurations

		    //! Use std::shared_ptr<> to manage the lifetime of the database.
		    //! This avoids costly copy operations when the database is large.
		    //! When a task is cloned, it simply inherits a reference to the
		    //! shared database
        std::shared_ptr<threepf_kconfig_database> threepf_db;

        //! Is this threepf task integrable? ie., have we dropped any configurations, and is the spacing linear?
        bool integrable;

	    };


    template <typename number>
    struct integration_task_traits<number, integration_task_type::threepf>
      {
        typedef threepf_task<number> task_type;
      };


    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t, bool adpt_ics)
	    : twopf_db_task<number>(nm, i, t, adpt_ics),
	      integrable(true)
	    {
        threepf_db = std::make_shared<threepf_kconfig_database>(this->twopf_db_task<number>::kstar);
	    }


    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i)
	    : twopf_db_task<number>(nm, reader, handle, i)
	    {
		    threepf_db = std::make_shared<threepf_kconfig_database>(this->twopf_db_task<number>::kstar, handle, *this->twopf_db_task<number>::twopf_db);

        //! deserialize integrable status
        integrable = reader[CPPTRANSPORT_NODE_THREEPF_INTEGRABLE].asBool();

        // rebuild database of stored times; this isn't serialized but recomputed on-the-fly
        this->cache_stored_time_config_database(threepf_db->get_kmax_2pf_conventional());
	    }


    template <typename number>
    void threepf_task<number>::serialize(Json::Value& writer) const
	    {
        // serialize integrable status
        writer[CPPTRANSPORT_NODE_THREEPF_INTEGRABLE] = this->integrable;

		    // threepf database is serialized separately to a SQLite database
        // this serialization is handled by the repository layer via write_kconfig_database() below

        this->twopf_db_task<number>::serialize(writer);
	    }


		template <typename number>
		void threepf_task<number>::write_kconfig_database(sqlite3* handle)
			{
		    this->twopf_db_task<number>::write_kconfig_database(handle);
				this->threepf_db->write(handle);
			}


    template <typename number>
    const time_config_database threepf_task<number>::get_time_config_database(const threepf_kconfig& config) const
      {
        return this->build_time_config_database(this->get_initial_time(config), this->threepf_db->get_kmax_2pf_conventional());
      }


    template <typename number>
    double threepf_task<number>::get_initial_time(const threepf_kconfig& config) const
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
    std::vector<number> threepf_task<number>::get_ics_vector(const threepf_kconfig& config) const
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
		double threepf_task<number>::get_ics_exit_time(const threepf_kconfig& config, threepf_ics_exit_type type) const
			{
		    double time = config.t_exit;

		    switch(type)
			    {
		        case threepf_ics_exit_type::smallest_wavenumber_exit:
			        {
		            double kmin = std::min(std::min(config.k1_conventional, config.k2_conventional), config.k3_conventional);

		            twopf_kconfig_database::const_record_iterator rec = this->twopf_db->find(kmin);
                if(rec == this->twopf_db->record_cend())
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_TASK_THREEPF_DATABASE_MISS << " " << kmin;
                    throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
                  }

		            time = (*rec)->t_exit;
		            break;
			        }

		        case threepf_ics_exit_type::kt_wavenumber_exit:
			        {
		            time = config.t_exit;
		            break;
			        };
			    }

				return(time);
			}


		template <typename number>
		std::vector<number> threepf_task<number>::get_ics_exit_vector(const threepf_kconfig& config, threepf_ics_exit_type type) const
			{
				return this->integration_task<number>::get_ics_vector(this->get_ics_exit_time(config, type));
			}


    template <typename number>
    void threepf_task<number>::compute_horizon_exit_times()
	    {
		    double largest_kt = this->threepf_db->get_kmax_comoving()/3.0;
        double largest_k1 = this->threepf_db->get_kmax_2pf_comoving();
        double largest_k2 = this->twopf_db->get_kmax_comoving();

        std::vector<double> N;
        std::vector<number> log_aH;
        std::vector<number> log_a2H2M;

        try
          {
            double maxk = std::max(std::max(largest_kt, largest_k1), largest_k2);
            this->get_model()->compute_aH(this, N, log_aH, log_a2H2M, maxk);
            assert(N.size() == log_aH.size());
            assert(N.size() == log_a2H2M.size());

            spline1d<number> log_aH_sp(N, log_aH);
            spline1d<number> log_a2H2M_sp(N, log_a2H2M);

            // forward to underlying twopf_db_task to update its database;
            // should be done *before* computing horizon exit times for threepfs, so that horizon exit & massless times
            // for the corresponding twopfs are known
            this->twopf_db_task<number>::twopf_compute_horizon_exit_times(log_aH_sp, log_a2H2M_sp, task_impl::HorizonExitTolerancePredicate<number>{this->name, CPPTRANSPORT_ROOT_FIND_TOLERANCE});

            // compute horizon exit times & massless times for threepf configuraitons
            this->threepf_compute_horizon_exit_times(log_aH_sp, task_impl::HorizonExitTolerancePredicate<number>{this->name, CPPTRANSPORT_ROOT_FIND_TOLERANCE});
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
		void threepf_task<number>::threepf_compute_horizon_exit_times(SplineObject& sp, TolerancePolicy tol)
			{
		    for(auto t = this->threepf_db->config_begin(); t != this->threepf_db->config_end(); ++t)
			    {
		        // set spline to evaluate aH-k and then solve for N
		        sp.set_offset(std::log(t->kt_comoving/3.0));

            // update database record with computed exit time
            t->t_exit = task_impl::find_zero_of_spline(this->name, CPPTRANSPORT_TASK_SEARCH_ROOT_BRACKET_EXIT, sp, tol);

            // determine massless time from pre-computed massless times of corresponding twopf database
            twopf_kconfig_database::const_record_iterator rec1 = this->twopf_db->find(t->k1_conventional);
            if(rec1 == this->twopf_db->record_cend())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_THREEPF_DATABASE_MISS << " " << t->k1_conventional;
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
              }

            twopf_kconfig_database::const_record_iterator rec2 = this->twopf_db->find(t->k2_conventional);
            if(rec2 == this->twopf_db->record_cend())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_THREEPF_DATABASE_MISS << " " << t->k2_conventional;
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
              }

            twopf_kconfig_database::const_record_iterator rec3 = this->twopf_db->find(t->k3_conventional);
            if(rec3 == this->twopf_db->record_cend())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_THREEPF_DATABASE_MISS << " " << t->k3_conventional;
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
              }

            t->t_massless = std::min(std::min((*rec1)->t_massless, (*rec2)->t_massless), (*rec3)->t_massless);
			    }
			}


    template <typename number=default_number_type>
    class threepf_cubic_task: public threepf_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named three-point function task based on sampling from a cubic lattice of ks,
        //! with specified policies
        template <typename StoragePolicy = task_policy_impl::DefaultStoragePolicy, typename TrianglePolicy = task_policy_impl::DefaultCubicTrianglePolicy>
        threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i,
                           range<double>& t, range<double>& ks, bool adpt_ics=false,
                           StoragePolicy policy = StoragePolicy(), TrianglePolicy triangle = TrianglePolicy());

        //! Deserialization constructor
        threepf_cubic_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i);


        // INTERFACE

      public:

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const override { return(this->spacing*this->spacing*this->spacing); }

        //! get measure; here, just dk1 dk2 dk3 so there is nothing to do
        virtual number measure(const threepf_kconfig& config) const override { return(1.0); }


        // SERIALIZATION (implements a 'serialiazble' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual threepf_cubic_task<number>* clone() const override { return new threepf_cubic_task<number>(static_cast<const threepf_cubic_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        double spacing;

	    };


    // build a 3pf task from a cubic lattice of k-modes
    template <typename number>
    template <typename StoragePolicy, typename TrianglePolicy>
    threepf_cubic_task<number>::threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i,
                                                   range<double>& t, range<double>& ks, bool adpt_ics,
                                                   StoragePolicy policy, TrianglePolicy triangle)
	    : threepf_task<number>(nm, i, t, adpt_ics)
	    {
        // step through the lattice of k-modes, recording which are viable triangular configurations
        // we insist on ordering, so i <= j <= k
        for(unsigned int j = 0; j < ks.size(); ++j)
	        {
            for(unsigned int k = 0; k < ks.size(); ++k)
	            {
                for(unsigned int l = 0; l < ks.size(); ++l)
	                {
                    if(triangle(j, k, l, ks[j], ks[k], ks[l]))      // ask policy object whether this is a triangle
	                    {
                        boost::optional<threepf_kconfig_database::record_iterator> record = this->threepf_task<number>::threepf_db->add_k1k2k3_record(*this->twopf_db_task<number>::twopf_db, ks[j], ks[k], ks[l], policy);

                        if(!record)  // storage policy declined to store this configuration
                          {
                            this->threepf_task<number>::integrable = false;    // can't integrate any task which has dropped configurations, because the points may be scattered over the integration region
                          }
	                    }
	                }
	            }
	        }

        // need linear spacing to be integrable
        if(!ks.is_simple_linear()) this->threepf_task<number>::integrable = false;
        spacing = (ks.get_max() - ks.get_min())/ks.get_steps();

        std::unique_ptr<reporting::key_value> kv = this->get_model()->make_key_value();
        kv->set_tiling(true);
        kv->set_title(this->get_name());

        kv->insert_back(CPPTRANSPORT_TASK_DATA_TWOPF, boost::lexical_cast<std::string>(this->twopf_db->size()));
        kv->insert_back(CPPTRANSPORT_TASK_DATA_THREEPF, boost::lexical_cast<std::string>(this->threepf_db->size()));

        this->compute_horizon_exit_times();

		    // write_time_details() should come *after* compute_horizon_exit_times();
        this->write_time_details(*kv);
        this->cache_stored_time_config_database(this->threepf_db->get_kmax_2pf_conventional());

        if(this->get_model()->is_verbose()) kv->write(std::cout);
	    }


    template <typename number>
    threepf_cubic_task<number>::threepf_cubic_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i)
	    : threepf_task<number>(nm, reader, handle, i)
	    {
        spacing = reader[CPPTRANSPORT_NODE_THREEPF_CUBIC_SPACING].asDouble();
	    }


    template <typename number>
    void threepf_cubic_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_TASK_TYPE]             = std::string(CPPTRANSPORT_NODE_TASK_TYPE_THREEPF_CUBIC);
        writer[CPPTRANSPORT_NODE_THREEPF_CUBIC_SPACING] = this->spacing;

        this->threepf_task<number>::serialize(writer);
	    }


    template <typename number=default_number_type>
    class threepf_alphabeta_task: public threepf_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named three-point function task based on sampling at specified values of
        //! the Fergusson-Shellard parameters k_t, alpha and beta,
        //! with specified storage policies
        template <typename StoragePolicy = task_policy_impl::DefaultStoragePolicy, typename TrianglePolicy = task_policy_impl::DefaultAlphaBetaTrianglePolicy>
        threepf_alphabeta_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t,
                               range<double>& kts, range<double>& alphas, range<double>& betas, bool adpt_ics = false,
                               StoragePolicy policy = StoragePolicy(), TrianglePolicy triangle = TrianglePolicy());

        //! Deserialization constructor
        threepf_alphabeta_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i);


        // INTERFACE

      public:

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const override { return(this->kt_spacing*this->alpha_spacing*this->beta_spacing); }

        //! get measure; here k_t^2 dk_t dalpha dbeta
        virtual number measure(const threepf_kconfig& config) const override { return(static_cast<number>(config.kt_conventional *config.kt_conventional)); }


        // SERIALIZATION (implements a 'serialiazble' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual threepf_alphabeta_task<number>* clone() const override { return new threepf_alphabeta_task<number>(static_cast<const threepf_alphabeta_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        //! k_t spacing
        double kt_spacing;

        //! alpha spacing
        double alpha_spacing;

        //! beta spacing
        double beta_spacing;

	    };


    // build a threepf task from sampling at specific values of the Fergusson-Shellard parameters k_t, alpha, beta
    template <typename number>
    template <typename StoragePolicy, typename TrianglePolicy>
    threepf_alphabeta_task<number>::threepf_alphabeta_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t,
                                                           range<double>& kts, range<double>& alphas,
                                                           range<double>& betas, bool adpt_ics,
                                                           StoragePolicy policy, TrianglePolicy triangle)
	    : threepf_task<number>(nm, i, t, adpt_ics)
	    {
        for(unsigned int j = 0; j < kts.size(); ++j)
	        {
            for(unsigned int k = 0; k < alphas.size(); ++k)
	            {
                for(unsigned int l = 0; l < betas.size(); ++l)
	                {
                    if(triangle(alphas[k], betas[l]))     // ask policy object to decide whether this is a triangle
	                    {
                        boost::optional<threepf_kconfig_database::record_iterator> record
													= this->threepf_task<number>::threepf_db->add_alphabeta_record(*this->threepf_task<number>::twopf_db, kts[j], alphas[k], betas[l], policy);

                        if(!record)   // storage policy declined to store this configuration
                          {
                            this->threepf_task<number>::integrable = false;    // can't integrate any task which has dropped configurations, because the points may be scattered over the integration region
                          }
	                    }
	                }
	            }
	        }

        // need linear spacing to be integrable
        if(!kts.is_simple_linear() || !alphas.is_simple_linear() || !betas.is_simple_linear()) this->threepf_task<number>::integrable = false;
        kt_spacing    = (kts.get_max() - kts.get_min()) / kts.get_steps();
        alpha_spacing = (alphas.get_max() - alphas.get_min()) / alphas.get_steps();
        beta_spacing  = (betas.get_max() - betas.get_min()) / betas.get_steps();

        std::unique_ptr<reporting::key_value> kv = this->get_model()->make_key_value();
        kv->set_tiling(true);
        kv->set_title(this->get_name());

        kv->insert_back(CPPTRANSPORT_TASK_DATA_TWOPF, boost::lexical_cast<std::string>(this->twopf_db->size()));
        kv->insert_back(CPPTRANSPORT_TASK_DATA_THREEPF, boost::lexical_cast<std::string>(this->threepf_db->size()));

        this->compute_horizon_exit_times();

		    // write_time_details() should come *after* compute_horizon_exit_times();
        this->write_time_details(*kv);
        this->cache_stored_time_config_database(this->threepf_db->get_kmax_2pf_conventional());

        if(this->get_model()->is_verbose()) kv->write(std::cout);
	    }


    template <typename number>
    threepf_alphabeta_task<number>::threepf_alphabeta_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i)
	    : threepf_task<number>(nm, reader, handle, i)
	    {
        kt_spacing    = reader[CPPTRANSPORT_NODE_THREEPF_ALPHABETA_KT_SPACING].asDouble();
        alpha_spacing = reader[CPPTRANSPORT_NODE_THREEPF_ALPHABETA_ALPHA_SPACING].asDouble();
        beta_spacing  = reader[CPPTRANSPORT_NODE_THREEPF_ALPHABETA_BETA_SPACING].asDouble();
	    }


    template <typename number>
    void threepf_alphabeta_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_TASK_TYPE_THREEPF_ALPHABETA);

        writer[CPPTRANSPORT_NODE_THREEPF_ALPHABETA_KT_SPACING]    = this->kt_spacing;
        writer[CPPTRANSPORT_NODE_THREEPF_ALPHABETA_ALPHA_SPACING] = this->alpha_spacing;
        writer[CPPTRANSPORT_NODE_THREEPF_ALPHABETA_BETA_SPACING]  = this->beta_spacing;

        this->threepf_task<number>::serialize(writer);
	    }

	}   // namespace transport


#endif //CPPTRANSPORT_THREEPF_TASK_H
