//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef CPPTRANSPORT_MODEL_H_
#define CPPTRANSPORT_MODEL_H_

#include <string>
#include <vector>
#include <functional>

#include <math.h>

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/defaults.h"

#include "transport-runtime-api/concepts/flattener.h"
#include "transport-runtime-api/concepts/initial_conditions.h"
#include "transport-runtime-api/concepts/parameters.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/scheduler/scheduler.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"

#include "transport-runtime-api/manager/instance_manager.h"
#include "transport-runtime-api/data/data_manager.h"

#include "transport-runtime-api/models/advisory_classes.h"

#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"


namespace transport
  {

    // MODEL OBJECTS -- objects representing inflationary models

    // basic class from which all other model representations are derived
    template <typename number>
    class model: public abstract_flattener
      {

      public:

        typedef std::vector< std::vector<number> > backg_history;

		    typedef enum { cpu, gpu } backend_type;


        // CONSTRUCTORS, DESTRUCTORS

      public:

        model(instance_manager<number>* m, const std::string& u, unsigned int v);
		    ~model();


        // EXTRACT MODEL INFORMATION

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
        //! Return name of stepper used to do background evolution in the computation
        virtual const std::string&              get_back_stepper() const = 0;
        //! Return name of stepper used to do perturbation evolution in the computation
        virtual const std::string&              get_pert_stepper() const = 0;

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


        // COMPUTE BASIC PHYSICAL QUANTITIES

      public:

        //! Compute Hubble rate H given a phase-space configuration
        virtual number H(const parameters<number>& __params, const std::vector<number>& __coords) const = 0;
        //! Compute slow-roll parameter epsilon given a phase-space configuration
        virtual number epsilon(const parameters<number>& __params, const std::vector<number>& __coords) const = 0;


        // INITIAL CONDITIONS HANDLING

      public:

		    //! Validate initial conditions
		    virtual void validate_ics(const parameters<number>& params, const std::vector<number>& input, std::vector<number>& output) = 0;

        //! Compute initial conditions which give horizon-crossing at time Ncross,
        //! if we allow Npre e-folds before horizon-crossing.
        //! The supplied parameters should be validated.
        void offset_ics(const parameters<number>& params, const std::vector<number>& input, std::vector<number>& output,
                        double Ninit, double Ncross, double Npre,
                        double tolerance = CPPTRANSPORT_DEFAULT_ICS_GAP_TOLERANCE,
                        unsigned int time_steps = CPPTRANSPORT_DEFAULT_ICS_TIME_STEPS);


		    // WAVENUMBER NORMALIZATION

      public:

        //! Get value of H at horizon crossing, which can be used to normalize the comoving waveumbers
        double compute_kstar(const twopf_list_task<number>* tk, unsigned int time_steps=CPPTRANSPORT_DEFAULT_ICS_TIME_STEPS);

        //! Compute when the end of inflation occurs relative to the initial conditions
        virtual double compute_end_of_inflation(const integration_task<number>* tk, double search_time=CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH) = 0;

		    //! Compute aH as a function of N up to the horizon-exit time of some wavenumber
		    virtual void compute_aH(const twopf_list_task<number>* tk, std::vector<double>& N, std::vector<number>& aH, double largest_k) = 0;


        // INTERFACE - PARAMETER HANDLING

      public:

		    virtual void validate_params(const std::vector<number>& input, std::vector<number>& output) = 0;


        // CALCULATE MODEL-SPECIFIC QUANTITIES

      public:

        // calculate shape-dependent gauge transformations using full cosmological perturbation theory
        // pure virtual, so must be implemented by derived class
        virtual void compute_gauge_xfm_1(const twopf_list_task<number>* __task, const std::vector<number>& __state, std::vector<number>& __dN) = 0;

        virtual void compute_gauge_xfm_2(const twopf_list_task<number>* __task, const std::vector<number>& __state, double __k, double __k1, double __k2, double __N, std::vector< std::vector<number> >& __ddN) = 0;

		    // calculate 'deltaN' gauge transformations using separate-universe methods
		    // pure virtual, so must be implemented by derived class
		    virtual void compute_deltaN_xfm_1(const twopf_list_task<number>* __task, const std::vector<number>& __state, std::vector<number>& __dN) = 0;

		    virtual void compute_deltaN_xfm_2(const twopf_list_task<number>* __task, const std::vector<number>& __state, std::vector< std::vector<number> >& __ddN) = 0;

        // calculate tensor quantities, including the 'flow' tensors u2, u3 and the basic tensors A, B, C from which u3 is built
		    // pure virtual, so must be implemented by derived class
        virtual void u2(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __k, double __N, std::vector< std::vector<number> >& __u2) = 0;

        virtual void u3(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __u3) = 0;

        virtual void A(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __A) = 0;

        virtual void B(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __B) = 0;

        virtual void C(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __C) = 0;


        // BACKEND

      public:

        // obtain the compute context for this calculation
        // this method must be over-ridden by a derived implementation class, and should
        // supply a suitable context for whatever compute backend is in use
        virtual context backend_get_context(void) = 0;

		    // obtain backend type
		    virtual backend_type get_backend_type(void) = 0;

		    // obtain backend memory capacity
		    virtual unsigned int get_backend_memory(void) = 0;

		    // obtain backend priority
		    virtual unsigned int get_backend_priority(void) = 0;

        // process a background computation
        // unlike the twopf and threepf cases, we assume this can be done in memory
        // suitable storage is passed in soln
        virtual void backend_process_backg(const background_task<number>* tk, std::vector< std::vector<number> >& solution, bool silent=false) = 0;

        // process a work list of twopf items
        // must be over-ridden by a derived implementation class
        virtual void backend_process_queue(work_queue<twopf_kconfig_record>& work, const twopf_list_task<number>* tk,
                                           twopf_batcher<number>& batcher, bool silent = false) = 0;

        // process a work list of threepf items
        // must be over-ridden by a derived implementation class
        virtual void backend_process_queue(work_queue<threepf_kconfig_record>& work, const threepf_task<number>* tk,
                                           threepf_batcher<number>& batcher, bool silent = false) = 0;

        // return size of state vectors
        virtual unsigned int backend_twopf_state_size(void) const = 0;
        virtual unsigned int backend_threepf_state_size(void) const = 0;

        // does the backend support collection of per-configuration statistics?
        virtual bool supports_per_configuration_statistics(void) const = 0;


        // INTERNAL UTILITY FUNCTIONS

      protected:

        //! Write information about the task we are processing
        void write_task_data(const integration_task<number>* task, generic_batcher& batcher,
                             double abs_err, double rel_err, double step_size, std::string stepper_name);


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
    void model<number>::offset_ics(const parameters<number>& params, const std::vector<number>& input, std::vector<number>& output,
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
            std::vector< std::vector<number> > history;

            // set up times at which we wish to sample -- we just need a few
            stepping_range<double> times(Ninit, Ncross-Npre, time_steps);

            // set up initial conditions
            initial_conditions<double> ics(params.get_model(), params, input, Ninit, Ncross-Ninit);

            // set up a new task object for this integration
            background_task<double> tk(ics, times);

            this->backend_process_backg(&tk, history, true);

            if(history.size() > 0)
              {
                output = history.back();
              }
            else
              {
                assert(false);
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
              }
          }
      }


    template <typename number>
    void model<number>::write_task_data(const integration_task<number>* task, generic_batcher& batcher,
                                        double abs_err, double rel_err, double step_size, std::string stepper_name)
      {
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << CPPTRANSPORT_SOLVING_ICS_MESSAGE;

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << *task;

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal)
          << CPPTRANSPORT_STEPPER_MESSAGE    << " '"  << stepper_name
          << "', " << CPPTRANSPORT_ABS_ERR   << " = " << abs_err
          << ", "  << CPPTRANSPORT_REL_ERR   << " = " << rel_err
          << ", "  << CPPTRANSPORT_STEP_SIZE << " = " << step_size;
      }


    template <typename number>
    double model<number>::compute_kstar(const twopf_list_task<number>* tk, unsigned int time_steps)
      {
        // integrate for a small interval up to horizon-crossing,
        // and extract the value of H there

        std::vector< std::vector<number> > history;

        // set up times at which we wish to sample -- we just need a few scattered between the initial time and the horizon-crossing time
        stepping_range<double> times(tk->get_N_initial(), tk->get_N_horizon_crossing(), time_steps);

        initial_conditions<double> new_ics(tk->get_model(), tk->get_params(), tk->get_ics().get_vector(), tk->get_N_initial(), tk->get_N_subhorizon_efolds());

        background_task<double> new_task(new_ics, times);

        this->backend_process_backg(&new_task, history, true);

        if(history.size() > 0)
          {
            // the wavenumbers supplied to the twopf, threepf integration routines
            // use k=1 for the wavenumber which crosses the horizon at time Nstar.
            // This wavenumber should have comoving value k=aH
            // To avoid numbers becoming too large or small, and also because the integrator has
            // noticeably better performance for correlation-function amplitudes in a
            // certain range, use a fixed normalization which can be adjusted in twopf_list_task
            return( this->H(tk->get_params(), history.back()) * exp(tk->get_astar_normalization()) );
          }
        else
          {
            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_INTEGRATION_FAIL);
          }
      }


  }    // namespace transport

#endif // CPPTRANSPORT_MODEL_H_
