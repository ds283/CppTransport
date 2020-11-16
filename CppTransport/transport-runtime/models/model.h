//
// Created by David Seery on 30/06/2013.
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

#ifndef CPPTRANSPORT_MODEL_H
#define CPPTRANSPORT_MODEL_H

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include <math.h>

#include "transport-runtime/messages.h"
#include "transport-runtime/defaults.h"
#include "transport-runtime/enumerations.h"

#include "transport-runtime/concepts/flattener.h"
#include "transport-runtime/concepts/initial_conditions.h"
#include "transport-runtime/concepts/parameters.h"
#include "transport-runtime/tasks/task.h"
#include "transport-runtime/tasks/integration_tasks.h"
#include "transport-runtime/scheduler/scheduler.h"

#include "transport-runtime/derived-products/utilities/index_selector.h"

#include "transport-runtime/manager/model_manager.h"
#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/argument_cache.h"
#include "transport-runtime/manager/message_handlers.h"
#include "transport-runtime/data/data_manager.h"
#include "transport-runtime/reporting/key_value.h"

#include "transport-runtime/models/advisory_classes.h"

#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"

#include "boost/optional.hpp"


namespace transport
  {

    // MODEL OBJECTS -- objects representing inflationary models

    //! the author_record type handles author metadata as supplied in the model description file
    class author_record
      {

        // CONSTRUCTOR

      public:

        //! constructor
        author_record(const std::string n, const std::string e, const std::string i)
          : name(n),
            email(e),
            institute(i)
          {
          }

        //! destructor is default
        ~author_record() = default;


        // INTERFACE

      public:

        //! query name of author
        const std::string& get_name() const { return(this->name); }

        //! query email address of author
        const std::string& get_email() const { return(this->email); }

        //! format email address as a link
        const std::string format_email() const { return("mailto:" + this->email); }

        //! query institute of author
        const std::string& get_institute() const { return(this->institute); }


        // INTERNAL DATA

      private:

        //! name of author
        const std::string name;

        //! email address of author
        const std::string email;

        //! institute of author
        const std::string institute;

      };


    //! set up a typedef for the author database -- basically an
    //! indexed list of author_record records
    using author_db = std::map< std::string, std::unique_ptr<author_record> >;

    
    // TEMPLATED ALIASES FOR TENSOR TYPES
    
    template <typename number>
    using flattened_tensor = std::vector<number>;
    
    template <typename number>
    using backg_state = flattened_tensor<number>;
    
    template <typename number>
    using backg_history = std::vector< backg_state<number> >;


    enum class massmatrix_type
      {
        include_mixing, hessian_approx
      };
    
    
    // ABSTRACT MODEL CLASS
    
    //! abstract base class from which all other model representations are derived;
    //! defines a common interface for all models
    template <typename number>
    class model: public abstract_flattener
      {
        
        // CONSTRUCTORS, DESTRUCTORS

      public:

        //! constructor
        model(const std::string& u, unsigned int v, local_environment& e, argument_cache& a);

        //! destructor is default
		    ~model() = default;


        // EXTRACT MODEL INFORMATION

      public:

        //! Return UUID identifying the model
        const std::string& get_identity_string() const { return (this->uid); }

        //! Return version of translator used to produce the header
        unsigned int get_translator_version() const { return (this->tver); }

        //! Return name of the model implemented by this object
        virtual const std::string& get_name() const = 0;

        //! Return authors of the model implemented by this object
        virtual const author_db& get_authors() const = 0;

        //! Return citation guideance for the model implemented by this object
        virtual const std::string& get_citeguide() const = 0;

        //! Return description for the model implemented by this object
        virtual const std::string& get_description() const = 0;

        //! Return license for the model implemented by this object
        virtual const std::string& get_license() const = 0;

        //! Return revision for the model implemented by this object
        virtual unsigned int get_revision() const = 0;

        //! Return reference list for the model implemented by this object
        virtual const std::vector<std::string>& get_references() const = 0;

        //! Return URL list for the model implemented by this object
        virtual const std::vector<std::string>& get_urls() const = 0;

        //! Return name of backend used to do the computation
        virtual const std::string& get_backend() const = 0;

        //! Return name of stepper used to do background evolution in the computation
        virtual const std::string& get_back_stepper() const = 0;

        //! Return name of stepper used to do perturbation evolution in the computation
        virtual const std::string& get_pert_stepper() const = 0;

        //! Return (abs, rel) tolerance of stepper used to do background evolution
        virtual std::pair<double, double> get_back_tol() const = 0;

        //! Return (abs, rel) tolerance of stepper used to do perturbation evolution
        virtual std::pair<double, double> get_pert_tol() const = 0;

        //! Return number of fields belonging to the model implemented by this object
        virtual unsigned int get_N_fields() const = 0;

        //! Return number of parameters required by the model implemented by this object
        virtual unsigned int get_N_params() const = 0;

        //! Return vector of field names for the model implemented by this object
        virtual const std::vector<std::string>& get_field_names() const = 0;

        //! Return vector of LaTeX names for the fields of the model implemented by this object
        virtual const std::vector<std::string>& get_f_latex_names() const = 0;

        //! Return vector of parameter names for the model implemented by this object
        virtual const std::vector<std::string>& get_param_names() const = 0;

        //! Return vector of LaTeX names for the parameters of the model implemented by this object
        virtual const std::vector<std::string>& get_p_latex_names() const = 0;

        //! Return vector of names for the phase-space coordinates (fields+momenta) of the model implemented by this object
        virtual const std::vector<std::string>& get_state_names() const = 0;


        // COMPUTE BASIC PHYSICAL QUANTITIES

      public:

        //! Compute Hubble rate H given a phase-space configuration
        virtual number H(const parameters<number>& __params, const flattened_tensor<number>& __coords) const = 0;

        //! Compute slow-roll parameter epsilon given a phase-space configuration
        virtual number epsilon(const parameters<number>& __params, const flattened_tensor<number>& __coords) const = 0;

        //! Compute slow-roll parameter eta given a phase-space configuration
        virtual number eta(const parameters<number>& __params, const flattened_tensor<number>& __coords) const = 0;


        // INITIAL CONDITIONS HANDLING

      public:

		    //! Validate initial conditions
        virtual void validate_ics(const parameters<number>& params, const flattened_tensor<number>& input,
                                  flattened_tensor<number>& output) = 0;

        //! Compute initial conditions which give horizon-crossing at time Ncross,
        //! if we allow Npre e-folds before horizon-crossing.
        //! The supplied parameters should have been validated.
        void offset_ics(const parameters<number>& params, const flattened_tensor<number>& input,
                        flattened_tensor<number>& output, double Ninit, double Ncross, double Npre,
                        double tolerance = CPPTRANSPORT_DEFAULT_ICS_GAP_TOLERANCE,
                        unsigned int time_steps = CPPTRANSPORT_DEFAULT_ICS_TIME_STEPS);


		    // WAVENUMBER NORMALIZATION

      public:

        //! Get value of H at horizon crossing, which can be used to normalize the comoving waveumbers
        double compute_kstar(const twopf_db_task<number>* tk,
                             unsigned int time_steps = CPPTRANSPORT_DEFAULT_ICS_TIME_STEPS);

        //! Compute when the end of inflation occurs relative to the initial conditions
        virtual double compute_end_of_inflation(const integration_task<number>* tk,
                                                double search_time = CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH) = 0;

		    //! Compute aH as a function of N up to the horizon-exit time of some wavenumber.
        //! Samples of log(aH) and the fields are returned in the supplied vector, with samples taken
        //! at internally-chosen values of N -- also returned in the corresponding vector
        //! Also computes log(a^2 * H^2 largest eigenvalue of the mass matrix) and returns this in
        //! log_a2H2M. Note that the mass matrix used in the code is M^2/H^2, ie. is dimensionless
		    virtual void compute_aH(const integration_task<number>* tk, std::vector<double>& N,
		                            flattened_tensor<number>& log_aH, flattened_tensor<number>& log_a2H2M,
		                            boost::optional<double> largest_k) = 0;

		    //! Compute H as a function of N similarly to the compute_aH function above. Samples of log(H) are returned
		    //! in the supplied log_H vector with samples taken from the N vector. This will be useful for computing
		    //! physical wavenumbers using the matching equation as H is the important dynamical quantity.
        virtual void compute_H(const integration_task<number>* tk, std::vector<double>& N,
                       flattened_tensor<number>& log_H, boost::optional<double> largest_k) = 0;


        // INTERFACE - PARAMETER HANDLING

      public:

        //! validate a set of parameters
		    virtual void validate_params(const flattened_tensor<number>& input, flattened_tensor<number>& output) = 0;


        // CALCULATE MODEL-SPECIFIC QUANTITIES

      public:

        // GAUGE TRANSFORMATIONS
        
        // these calculate configuration-dependent gauge transformations using full cosmological perturbation theory

        //! compute first-order gauge transformation
        virtual void
        compute_gauge_xfm_1(const integration_task<number>* __task, const flattened_tensor<number>& __state,
                            flattened_tensor<number>& __dN) = 0;

        //! compute second-order gauge transformation
        virtual void
        compute_gauge_xfm_2(const integration_task<number>* __task, const flattened_tensor<number>& __state, double __k,
                            double __k1, double __k2, double __N, flattened_tensor<number>& __ddN) = 0;


        // TENSORS

        // calculate tensor quantities, including the 'flow' tensors u2, u3 and the basic tensors A, B, C from which u3 is built

        //! compute u2 tensor in 'standard' index configuration (first index up, remaining indices down)
        virtual void
        u2(const integration_task<number>* __task, const flattened_tensor<number>& __fields, double __k, double __N,
           flattened_tensor<number>& __u2) = 0;

        //! compute u3 tensor in 'standard' index configuration (first index up, remaining indices down)
        virtual void
        u3(const integration_task<number>* __task, const flattened_tensor<number>& __fields, double __km, double __kn,
           double __kr, double __N, flattened_tensor<number>& __u3) = 0;

        //! compute A tensor in 'standard' index configuration (all indices down)
        //! currently A isn't used by the platform, so the precise index arrangement is arbitrary
        virtual void
        A(const integration_task<number>* __task, const flattened_tensor<number>& __fields, double __km, double __kn,
          double __kr, double __N, flattened_tensor<number>& __A) = 0;

        //! compute B tensor in 'standard' index configuration (last index up, first two indices down)
        //! this is the index configuration needed for shifting a correlation function from momenta
        //! to time derivatives
        virtual void
        B(const integration_task<number>* __task, const flattened_tensor<number>& __fields, double __km, double __kn,
          double __kr, double __N, flattened_tensor<number>& __B) = 0;

        //! compute C tensor in 'standard' index configuration (first index up, last two indices down)
        //! this is the index configuration needed for shifting a correlation function from momenta
        //! to time derivatives
        virtual void
        C(const integration_task<number>* __task, const flattened_tensor<number>& __fields, double __km, double __kn,
          double __kr, double __N, flattened_tensor<number>& __C) = 0;

        //! compute M tensor in 'standard' index configuration (first index up, second index down)
        //! this is the arrangement needed to compute the mass spectrum
        virtual void
        M(const integration_task<number>* __task, const flattened_tensor<number>& __fields,
          flattened_tensor<number>& __M, massmatrix_type __type = massmatrix_type::include_mixing) = 0;


        // MASS SPECTRUM

        //! compute the raw mass spectrum
        virtual void
        mass_spectrum(const integration_task<number>* __task, const flattened_tensor<number>& __fields, bool __norm,
                      flattened_tensor<number>& __M, flattened_tensor<number>& __E,
                      massmatrix_type __type = massmatrix_type::include_mixing) = 0;

        //! obtain the sorted mass spectrum, normalized to the Hubble rate^2 if desired
        virtual void
        sorted_mass_spectrum(const integration_task<number>* __task, const flattened_tensor<number>& __fields,
                             bool __norm, flattened_tensor<number>& __M, flattened_tensor<number>& __E,
                             massmatrix_type __type = massmatrix_type::include_mixing) = 0;


        // BACKEND

      public:

        // obtain the compute context for this calculation
        // this method must be over-ridden by a derived implementation class, and should
        // supply a suitable context for whatever compute backend is in use
        virtual compute_context backend_get_context(void) = 0;

		    // obtain backend type
		    virtual worker_type get_backend_type(void) = 0;

		    // obtain backend memory capacity
		    virtual unsigned int get_backend_memory(void) = 0;

		    // obtain backend priority
		    virtual unsigned int get_backend_priority(void) = 0;

        // process a background computation
        // unlike the twopf and threepf cases, we assume this can be done in memory
        // suitable storage is passed in soln
        virtual void
        backend_process_backg(const background_task<number>* tk, std::vector<flattened_tensor<number> >& solution,
                              bool silent = false) = 0;

        // process a work list of twopf items
        // must be over-ridden by a derived implementation class
        virtual void backend_process_queue(device_queue_manager<twopf_kconfig_record>& work, const twopf_db_task<number>* tk,
                                           twopf_batcher<number>& batcher, bool silent = false) = 0;

        // process a work list of threepf items
        // must be over-ridden by a derived implementation class
        virtual void backend_process_queue(device_queue_manager<threepf_kconfig_record>& work, const threepf_task<number>* tk,
                                           threepf_batcher<number>& batcher, bool silent = false) = 0;

        // return size of state vectors
        virtual unsigned int backend_twopf_state_size(void) const = 0;
        virtual unsigned int backend_threepf_state_size(void) const = 0;

        // does the backend support collection of per-configuration statistics?
        virtual bool supports_per_configuration_statistics(void) const = 0;


        // SERVICES

      public:

        //! report erorr
        void error(const std::string& msg) const { this->error_h(msg); }

        //! report warning
        void warn(const std::string& msg) const { this->warn_h(msg); }

        //! report message
        void message(const std::string& msg) const { this->message_h(msg); }

        //! generate key-value reporting object
        std::unique_ptr<reporting::key_value> make_key_value() { return std::make_unique<reporting::key_value>(this->env, this->args); }

        //! expose verbose option setting
        bool is_verbose() const { return this->args.get_verbose(); }


        // INTERNAL UTILITY FUNCTIONS

      protected:

        //! Write information about the task we are processing
        void write_task_data(const integration_task<number>* task, generic_batcher& batcher,
                             double abs_err, double rel_err, double step_size, std::string stepper_name);


        // INTERNAL DATA

      private:

        //! copy of unique id, used for deregistration
        const std::string uid;

        //! copy of translator version used to produce this model, used for registration
        const unsigned int tver;


        // POLICY OBJECTS

      protected:

        //! reference to local environment
        local_environment& env;

        //! reference to argument cache
        argument_cache& args;


        // ERROR, WARNING AND MESSAGE HANDLERS

      protected:

        //! error handler
        error_handler error_h;

        //! warning handler
        warning_handler warn_h;

        //! message handler
        message_handler message_h;

      };


    //  IMPLEMENTATION -- CLASS MODEL


    // CONSTRUCTOR, DESTRUCTOR

    template <typename number>
    model<number>::model(const std::string& u, unsigned int v, local_environment& e, argument_cache& a)
      : uid(u),
        tver(v),
        env(e),
        args(a),
        error_h(e, a),
        warn_h(e, a),
        message_h(e, a)
      {
      }


    // INITIAL CONDITIONS HANDLING


    template <typename number>
    void model<number>::offset_ics(const parameters<number>& params, const flattened_tensor<number>& input, flattened_tensor<number>& output,
                                   double Ninit, double Ncross, double Npre, double tolerance, unsigned int time_steps)
      {
				assert(Ncross - Npre >= Ninit);

        // we are guaranteed that the input ics 'input' are validated

        if(std::abs(Ncross-Npre-Ninit) < tolerance)
          {
            output = input;
          }
        else
          {
            // to offset the initial conditions,
            // integrate for a small interval between the initial time Ninit
            // and the 'offset time' Ncross-Npre

            // allocate space for the solution on this interval
            backg_history<number> history;

            // set up times at which we wish to sample -- we just need a few
            basic_range<double> times(Ninit, Ncross-Npre, time_steps);

            // set up initial conditions
            initial_conditions<number> ics(params, input, Ninit, Ncross-Ninit);

            // set up a new task object for this integration
            background_task<number> tk(ics, times);

            try
              {
                this->backend_process_backg(&tk, history, true);
              }
            catch(Hsq_is_negative& xe)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_HSQ_IS_NEGATIVE << " " << xe.what();
                this->error_h(msg.str());

                throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
              }
            catch(integration_produced_nan& xe)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_INTEGRATION_PRODUCED_NAN << " " << xe.what();
                this->error_h(msg.str());
    
                throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
              }
            catch(eps_is_negative& xe)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EPS_IS_NEGATIVE << " " << xe.what();
                this->error_h(msg.str());

                throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
              }
            catch(eps_too_large& xe)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EPS_TOO_LARGE << " " << xe.what();
                this->error_h(msg.str());

                throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
              }
            catch(V_is_negative& xe)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_V_IS_NEGATIVE << " " << xe.what();
                this->error_h(msg.str());

                throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
              }

            if(history.size() > 0)
              {
                output = history.back();
              }
            else
              {
                assert(false);
                throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
              }
          }
      }


    template <typename number>
    void model<number>::write_task_data(const integration_task<number>* task, generic_batcher& batcher,
                                        double abs_err, double rel_err, double step_size, std::string stepper_name)
      {
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << CPPTRANSPORT_SOLVING_ICS_MESSAGE;

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << *task;

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal)
          << CPPTRANSPORT_STEPPER_MESSAGE    << " '"  << stepper_name
          << "', " << CPPTRANSPORT_ABS_ERR   << " = " << abs_err
          << ", "  << CPPTRANSPORT_REL_ERR   << " = " << rel_err
          << ", "  << CPPTRANSPORT_STEP_SIZE << " = " << step_size;
      }


    template <typename number>
    double model<number>::compute_kstar(const twopf_db_task<number>* tk, unsigned int time_steps)
      {
        // integrate for a small interval up to horizon-crossing,
        // and extract the value of H there

        backg_history<number> history;

        // set up times at which we wish to sample -- we just need a few scattered between the initial time and the horizon-crossing time
        basic_range<double> times(tk->get_N_initial(), tk->get_N_horizon_crossing(), time_steps);

        initial_conditions<number> new_ics(tk->get_params(), tk->get_ics().get_vector(), tk->get_N_initial(), tk->get_N_subhorizon_efolds());

        background_task<number> new_task(new_ics, times);
    
        try
          {
            this->backend_process_backg(&new_task, history, true);
          }
        catch(Hsq_is_negative& xe)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_HSQ_IS_NEGATIVE << " " << xe.what();
            this->error_h(msg.str());
    
            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
        catch(integration_produced_nan& xe)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_INTEGRATION_PRODUCED_NAN << " " << xe.what();
            this->error_h(msg.str());
    
            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
        catch(eps_is_negative& xe)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_EPS_IS_NEGATIVE << " " << xe.what();
            this->error_h(msg.str());

            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
        catch(eps_too_large& xe)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_EPS_TOO_LARGE << " " << xe.what();
            this->error_h(msg.str());

            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
        catch(V_is_negative& xe)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_V_IS_NEGATIVE << " " << xe.what();
            this->error_h(msg.str());

            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }

        if(history.size() > 0)
          {
            // H() is defined to return a number, which must be downcast to double if needed
            double H = static_cast<double>(this->H(tk->get_params(), history.back()));

            // the wavenumbers supplied to the twopf, threepf integration routines
            // use k=1 for the wavenumber which crosses the horizon at time Nstar.
            // This wavenumber should have comoving value k=aH
            // To avoid numbers becoming too large or small, and also because the integrator has
            // noticeably better performance for correlation-function amplitudes in a
            // certain range, use a fixed normalization which can be adjusted in twopf_db_task
            double a = exp(tk->get_astar_normalization());

            return(a*H);
          }
        else
          {
            assert(false);
            throw runtime_exception(exception_type::FATAL_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
      }


  }    // namespace transport

#endif // CPPTRANSPORT_MODEL_H
