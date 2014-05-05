//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_MODEL_H_
#define __CPP_TRANSPORT_MODEL_H_

#include <string>
#include <vector>
#include <functional>

#include <math.h>

#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/concepts/flattener.h"
#include "transport-runtime-api/concepts/initial_conditions.h"
#include "transport-runtime-api/concepts/parameters.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/scheduler/scheduler.h"

#include "transport-runtime-api/manager/instance_manager.h"
#include "transport-runtime-api/manager/data_manager.h"

#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"


#define __CPP_TRANSPORT_DEFAULT_ICS_GAP_TOLERANCE (1E-8)
#define __CPP_TRANSPORT_DEFAULT_ICS_TIME_STEPS    (20)


namespace transport
  {
    // MODEL OBJECTS -- objects representing inflationary models

    // basic class from which all other model representations are derived
    template <typename number>
    class model: public abstract_flattener
      {
      public:
        typedef std::vector< std::vector<number> > backg_history;


        // CONSTRUCTORS, DESTRUCTORS

      public:
        model(instance_manager<number>* m, const std::string& u, unsigned int v);

        virtual ~model();


        // INTERFACE: EXTRACT MODEL INFORMATION

      public:
        //! Return unique string identifying the model (and CppTransport version)
        const std::string&                      get_identity_string() const { return(this->uid); }

        unsigned int                            get_translator_version() const { return(this->tver); }

        //! Return name of the model implemented by this object
        virtual const std::string&              get_name() const = 0;
        //! Return authors of the model implemented by this object
        virtual const std::string&              get_author() const = 0;
        //! Return tagline for the model implemented by this object
        virtual const std::string&              get_tag() const = 0;
        //! Return name of backend used to do the computation
        virtual const std::string&              get_backend() const = 0;

        //! Return number of fields belonging to the model implemented by this object
        virtual unsigned int                    get_N_fields() const = 0;
        //! Return number of parameters required by the model implemented by this object
        virtual unsigned int                    get_N_params() const = 0;

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

        // INTERFACE: COMPUTE BASIC PHYSICAL QUANTITIES

      public:
        //! Compute Hubble rate H given a phase-space configuration
        virtual number                  H(const parameters<number>& __params, const std::vector<number>& __coords) const = 0;
        //! Compute slow-roll parameter epsilon given a phase-space configuration
        virtual number                  epsilon(const parameters<number>& __params, const std::vector<number>& __coords) const = 0;


        // INTERFACE - INITIAL CONDITIONS HANDLING

      protected:
        //! Validate initial conditions (optionally adding initial conditions for momenta)
        virtual void validate_initial_conditions(const parameters<number>& params, const std::vector<number>& input, std::vector<number>& output) = 0;

        //! Compute initial conditions which give horizon-crossing at Nstar, if we allow Npre e-folds before horizon-crossing
        void find_ics(const parameters<number>& params, const std::vector<number>& input, std::vector<number>& output,
                      double Ninit, double Ncross, double Npre,
                      double tolerance= __CPP_TRANSPORT_DEFAULT_ICS_GAP_TOLERANCE,
                      unsigned int time_steps= __CPP_TRANSPORT_DEFAULT_ICS_TIME_STEPS);

        //! Get value of H at horizon crossing, which can be used to normalize the comoving waveumbers
        double get_kstar(const task<number>* tk, unsigned int time_steps= __CPP_TRANSPORT_DEFAULT_ICS_TIME_STEPS);

      public:
        //! Make an 'ics_validator' object for this model
        virtual typename initial_conditions<number>::ics_validator ics_validator_factory() = 0;

        //! Make an 'ics_finder' object for this model
        typename initial_conditions<number>::ics_finder ics_finder_factory()
          {
            return(std::bind(&model<number>::find_ics, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                             std::placeholders::_4, std::placeholders::_5, std::placeholders::_6,
                             __CPP_TRANSPORT_DEFAULT_ICS_GAP_TOLERANCE, __CPP_TRANSPORT_DEFAULT_ICS_TIME_STEPS));
          }

        //! Make a 'kconfig_kstar' object for this model
        typename task<number>::kconfig_kstar kconfig_kstar_factory()
          {
            return(std::bind(&model<number>::get_kstar, this, std::placeholders::_1, __CPP_TRANSPORT_DEFAULT_ICS_TIME_STEPS));
          }

      protected:
        //! Write information about the task we are processing
        void write_task_data(const task<number>* task, typename data_manager<number>::generic_batcher& batcher,
                             double abs_err, double rel_err, double step_size, std::string stepper_name);


        // INTERFACE - PARAMETER HANDLING

      protected:
        //! Validate parameter values
        virtual void validate_parameters(const std::vector<number>& input, std::vector<number>& output) = 0;

      public:
        //! Make a 'params_validator' objcet for this model
        virtual typename parameters<number>::params_validator params_validator_factory() = 0;


        // CALCULATE MODEL-SPECIFIC QUANTITIES

      public:
        // calculate gauge transformations; pure virtual, so must be overridden by derived class
        virtual void compute_gauge_xfm_1(const parameters<number>& __params, const std::vector<number>& __state, std::vector<number>& __dN) = 0;

        virtual void compute_gauge_xfm_2(const parameters<number>& __params, const std::vector<number>& __state, std::vector< std::vector<number> >& __ddN) = 0;

        // calculate tensor quantities, including the 'flow' tensors u2, u3 and the basic tensors A, B, C from which u3 is built
        virtual void u2(const parameters<number>& __params, const std::vector<number>& __fields, double __k, double __N, std::vector< std::vector<number> >& __u2) = 0;

        virtual void u3(const parameters<number>& __params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __u3) = 0;

        virtual void A(const parameters<number>& __params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __A) = 0;

        virtual void B(const parameters<number>& __params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __B) = 0;

        virtual void C(const parameters<number>& __params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __C) = 0;

        // BACKEND INTERFACE

      public:
        // obtain the compute context for this calculation
        // this method must be over-ridden by a derived implementation class, and should
        // supply a suitable context for whatever compute backend is in use
        virtual context      backend_get_context() = 0;

        // process a background computation
        // unlike the twopf and threepf cases, we assume this can be done in memory
        // suitable storage is passed in soln
        virtual void         backend_process_backg(const task<number>* tk, std::vector< std::vector<number> >& solution, bool silent=false) = 0;

        // process a work list of twopf items
        // must be over-ridden by a derived implementation class
        virtual void         backend_process_twopf(work_queue<twopf_kconfig>& work, const task<number>* tk,
                                                   typename data_manager<number>::twopf_batcher& batcher,
                                                   bool silent=false) = 0;

        // process a work list of threepf items
        // must be over-ridden by a derived implementation class
        virtual void         backend_process_threepf(work_queue<threepf_kconfig>& work, const task<number>* tk,
                                                     typename data_manager<number>::threepf_batcher& batcher,
                                                     bool silent=false) = 0;

        // return size of state vectors
        virtual unsigned int backend_twopf_state_size(void) = 0;
        virtual unsigned int backend_threepf_state_size(void) = 0;


        // INTERNAL DATA

      private:
        //! copy of instance manager, used for deregistration
        instance_manager<number>* mgr;
        //! copy of unique id, used for deregistration
        const std::string uid;
        //! copy of translator version used to produce this model, used for registration
        const unsigned int tver;
      };


    //  IMPLEMENTATION -- CLASS MODEL


    // EXTRACT MODEL INFORMATION

    template <typename number>
    model<number>::model(instance_manager<number>* m, const std::string& u, unsigned int v)
    : mgr(m), uid(u), tver(v)
      {
        // Register ourselves with the instance manager
        mgr->register_model(this, uid, tver);
      }


    template <typename number>
    model<number>::~model()
      {
        assert(this->mgr != nullptr);
        mgr->deregister_model(this, this->uid, tver);
      }


    // INITIAL CONDITIONS HANDLING


    template <typename number>
    void model<number>::find_ics(const parameters<number>& params, const std::vector<number>& input, std::vector<number>& output,
                                 double Ninit, double Ncross, double Npre,
                                 double tolerance, unsigned int time_steps)
      {
        assert(Ncross >= Npre);

        // we are guaranteed that the input ics 'input' are validated

        if(fabs(Ncross-Npre) < tolerance)
          {
            output = input;
          }
        else
          {
            // to offset the initial conditions,
            // integrate for a small interval between the initial time Ninit
            // and the 'offset time' Ncross-Npre

            // allocate space for the solution on this interval
            std::vector< std::vector<number> > history;

            // set up times at which we wish to sample -- we just need a few
            range<double> times(Ninit, Ncross-Npre, time_steps);

            // set up initial conditions
            // Npre is irrelevant, provided it falls between the beginning and end times
            double temp_Nstar = (Ninit + Ncross - Npre)/2.0;
            initial_conditions<double> ics(params, input, this->get_state_names(), temp_Nstar, this->ics_validator_factory());

            // set up a new task object for this integration
            task<double> tk(ics, times);

            this->backend_process_backg(&tk, history, true);

            if(history.size() > 0)
              {
                output = history.back();
              }
            else
              {
                throw std::logic_error(__CPP_TRANSPORT_INTEGRATION_FAIL);
              }
          }
      }


    template <typename number>
    void model<number>::write_task_data(const task<number>* task, typename data_manager<number>::generic_batcher& batcher,
                                        double abs_err, double rel_err, double step_size, std::string stepper_name)
      {
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << __CPP_TRANSPORT_SOLVING_ICS_MESSAGE;

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << *task;

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal)
          << __CPP_TRANSPORT_STEPPER_MESSAGE    << " '"  << stepper_name
          << "', " << __CPP_TRANSPORT_ABS_ERR   << " = " << abs_err
          << ", "  << __CPP_TRANSPORT_REL_ERR   << " = " << rel_err
          << ", "  << __CPP_TRANSPORT_STEP_SIZE << " = " << step_size;
      }


    template <typename number>
    double model<number>::get_kstar(const task<number>* tk, unsigned int time_steps)
      {
        // integrate for a small interval up to horizon-crossing,
        // and extract the value of H there

        std::vector< std::vector<number> > history;

        // set up times at which we wish to sample -- we just need a few
        range<double> times(tk->get_Ninit(), tk->get_Nstar(), time_steps);

        double new_Npre = (tk->get_Ninit() + tk->get_Nstar()) / 2.0;
        initial_conditions<double> new_ics(tk->get_params(), tk->get_ics().get_vector(), this->get_state_names(),
                                           new_Npre, this->ics_validator_factory());

        task<double> new_task(new_ics, times);

        this->backend_process_backg(&new_task, history, true);

        if(history.size() > 0)
          {
            // the wavenumbers supplied to the twopf, threepf integration routines
            // use k=1 for the wavenumber which crosses the horizon at time Nstar.
            // This wavenumber should have comoving value k=aH
            // Here, we return the normalization constant aH
            return this->H(tk->get_params(), history.back()) * exp(tk->get_Nstar());
          }
        else
          {
            throw std::logic_error(__CPP_TRANSPORT_INTEGRATION_FAIL);
          }
      }


  }    // namespace transport

#endif // __CPP_TRANSPORT_MODEL_H_
