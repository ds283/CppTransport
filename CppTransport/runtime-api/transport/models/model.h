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

#include "transport/messages_en.h"

#include "transport/concepts/flattener.h"
#include "transport/concepts/initial_conditions.h"
#include "transport/concepts/parameters.h"
#include "transport/tasks/task.h"
#include "transport/scheduler/scheduler.h"

#include "transport/manager/instance_manager.h"


#define DEFAULT_ICS_GAP_TOLERANCE (1E-8)
#define DEFAULT_ICS_TIME_STEPS    (20)


namespace transport
  {
    // MODEL OBJECTS -- objects representing inflationary models

    // avoid circularity in inclusions
    template <typename number, unsigned int N> class background;
    template <typename number, unsigned int N> class twopf;
    template <typename number, unsigned int N> class threepf;

    // basic class from which all other model representations are derived
    template <typename number, unsigned int Nf, unsigned int Np>
    class model: public flattener<Nf>
      {
      public:
        typedef std::vector< std::vector<number> > backg_history;

        // CONSTRUCTORS, DESTRUCTORS

      public:
        model(instance_manager<number>* m, const std::string& uid,
              const std::string& n, const std::string& a, const std::string& t,
              const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
              const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names,
              const std::vector<std::string>& s_names);

        virtual ~model();

        // EXTRACT MODEL INFORMATION

      public:
        const std::string&              get_identity_string();      // return unique string identifying this model

        const std::string&              get_name();                 // return name of model implemented by this object
        const std::string&              get_author();               // return authors of model implemented by this object
        const std::string&              get_tag();                  // return tagline for model implemented by this object

        const std::vector<std::string>& get_field_names();          // return vector of field names
        const std::vector<std::string>& get_f_latex_names();        // return vector of LaTeX field names
        const std::vector<std::string>& get_param_names();          // return vector of parameter names
        const std::vector<std::string>& get_p_latex_names();        // return vector of LaTeX parameter names
        const std::vector<std::string>& get_state_names();          // return vector of state variable names

        // BASIC PHYSICAL QUANTITIES

      public:
        virtual number                  H(const parameters<number, Np>& __params, const std::vector<number>& __coords) = 0;       // compute Hubble parameter
        virtual number                  epsilon(const parameters<number, Np>& __params, const std::vector<number>& __coords) = 0; // compute epsilon

        // INITIAL CONDITIONS HANDLING

      protected:
        // validate initial conditions (optionally adding initial conditions for momenta)
        virtual void validate_initial_conditions(const std::vector<number>& input, std::vector<number>& output) = 0;

        // compute initial conditions which give horizon-crossing at Nstar, if we allow Npre e-folds before horizon-crossing
        void find_ics(const std::vector<number>& input, std::vector<number>& output, double Ninit, double Ncross, double Npre,
                      double tolerance=DEFAULT_ICS_GAP_TOLERANCE, double time_steps=DEFAULT_ICS_TIME_STEPS);

        // get value of H at horizon crossing, which can be used to normalize the comoving ks
        double get_kstar(const parameters<number, Np>& params, const task<number>* tk, double time_steps=DEFAULT_ICS_TIME_STEPS);

      public:
        // make an ics_validator for this model
        virtual typename initial_conditions<number, Nf>::ics_validator ics_validator_factory() = 0;

        // make an ics_finder for this model
        typename initial_conditions<number, Nf>::ics_finder ics_finder_factory()
          {
            return(std::bind(&model<number>::find_ics, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                             std::placeholders::_4, std::placeholders::_5, std::placeholders::_6,
                             DEFAULT_ICS_GAP_TOLERANCE, DEFAULT_ICS_TIME_STEPS));
          }

        // make a kconfig_kstar for this model
        typename task<number>::kconfig_kstar kconfig_kstar_factory()
          {
            return(std::bind(&model<number>::get_kstar, this, std::placeholders::_1, std::placeholders::_2, DEFAULT_ICS_TIME_STEPS));
          }

      protected:
        // write information about the task we are processing
        void write_task_data(const task<number>* task, std::ostream& stream,
                             double abs_err, double rel_err, double step_size, std::string stepper_name);

        // PARAMETER HANDLING

      protected:
        // validate parameter values
        virtual void validate_parameters(const std::vector<number>& input, std::vector<number>& output) = 0;

      public:
        // make a params_validator for this model
        virtual typename parameters<number, Np>::params_validator params_validator_factory() = 0;

        // BASIC BACKGROUND, TWOPF AND THREEPF INTEGRATIONS

      public:
        background<number>              background(const task<number>* tk, bool silent=false);

        twopf<number>                   twopf     (const twopf_task<number>& tk, bool silent=false);

        threepf<number>                 threepf   (const threepf_task<number>& tk, bool silent=false);


        // CALCULATE MODEL-SPECIFIC QUANTITIES

      public:
        // calculate gauge transformations; pure virtual, so must be overridden by derived class
        virtual void compute_gauge_xfm_1(const parameters<number, Np>& __params, const std::vector<number>& __state, std::vector<number>& __dN) = 0;

        virtual void compute_gauge_xfm_2(const parameters<number, Np>& __params, const std::vector<number>& __state, std::vector< std::vector<number> >& __ddN) = 0;

        // calculate tensor quantities, including the 'flow' tensors u2, u3 and the basic tensors A, B, C from which u3 is built
        virtual void u2(const parameters<number, Np>& __params, const std::vector<number>& __fields, double __k, double __N, std::vector< std::vector<number> >& __u2) = 0;

        virtual void u3(const parameters<number, Np>& __params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __u3) = 0;

        virtual void A(const parameters<number, Np>& __params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __A) = 0;

        virtual void B(const parameters<number, Np>& __params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __B) = 0;

        virtual void C(const parameters<number, Np>& __params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __C) = 0;

        // BACKEND INTERFACE

      protected:
        // obtain the compute context for this calculation
        // this method must be over-ridden by a derived implementation class, and should
        // supply a suitable context for whatever compute backend is in use
        virtual context                 backend_get_context() = 0;

        // process a background computation
        // unlike the twopf and threepf cases, we assume this can be done in memory
        // suitable storage is passed in soln
        virtual void                    backend_process_backg(const task<number>* tk, std::vector< std::vector<number> >& solution, bool silent=false) = 0;

        // 'taskless' background integration,
        // just raw integration with supplied initial conditions and sampling times
        // this entry point is needed by the routines to find horizon-crossing values of k*, or to
        // offset a supplied set of initial conditions
        virtual void                    backend_raw_backg(const std::vector<number>& ics, const std::vector<double>& times, std::vector< std::vector<number> >& solution) = 0;

        // process a work list of twopf items
        // must be over-ridden by a derived implementation class
        // TODO: remove in-memory calculation
        virtual void                    backend_process_twopf(work_queue<twopf_kconfig>& work, const task<number>* tk,
                                                              std::vector< std::vector<number> >& backg,
                                                              std::vector< std::vector< std::vector<number> > >& twopf,
                                                              bool silent=false) = 0;

        // process a work list of threepf items
        // must be over-ridden by a derived implementation class
        // TODO: remove in-memory calculation
        virtual void                    backend_process_threepf(work_queue<threepf_kconfig>& work, const task<number>* tk,
                                                                std::vector< std::vector<number> >& backg,
                                                                std::vector< std::vector< std::vector<number> > >& twopf_re,
                                                                std::vector< std::vector< std::vector<number> > >& twopf_im,
                                                                std::vector< std::vector< std::vector<number> > >& threepf,
                                                                bool silent=false) = 0;

        // return size of state vectors
        virtual unsigned int            backend_twopf_state_size(void) = 0;
        virtual unsigned int            backend_threepf_state_size(void) = 0;

        // INTERNAL DATA

      protected:
        instance_manager<number>*       mgr;                  // manager instance

        const std::string               unique_id;            // unique string identifying this model (+CppTransport version info)

        const std::string               name;                 // name of model
        const std::string               author;               // authors
        const std::string               tag;                  // tagline, perhaps used to indicate citations

        std::vector<std::string>        field_names;          // vector of field names
        std::vector<std::string>        f_latex_names;        // vector of LaTeX names for fields

        std::vector<std::string>        param_names;          // vector of parameter names
        std::vector<std::string>        p_latex_names;        // vector of parameter LaTeX names

        std::vector<std::string>        state_names;          // vector of state variable names
      };


    //  IMPLEMENTATION -- CLASS MODEL


    // EXTRACT MODEL INFORMATION

    template <typename number, unsigned int Nf, unsigned int Np>
    model<number, Nf, Np>::model(instance_manager<number>* m, const std::string& uid,
                                 const std::string& n, const std::string& a, const std::string& t,
                                 const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                                 const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names,
                                 const std::vector<std::string>& s_names)
    : mgr(m), unique_id(uid),
      name(n), author(a), tag(t),
      field_names(f_names), f_latex_names(fl_names),
      param_names(p_names), p_latex_names(pl_names),
      state_names(s_names)
      {
        // Register ourselves with the instance manager
        mgr->register_model(this, unique_id);

        // Perform basic validation of initial data

        if(field_names.size() != Nf)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_WRONG_FIELD_NAMES_A << field_names.size() << "]"
              << __CPP_TRANSPORT_WRONG_FIELD_NAMES_B << Nf << "]";

            throw std::out_of_range(msg.str());
          }

        if(f_latex_names.size() != Nf)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_WRONG_F_LATEX_NAMES_A << f_latex_names.size() << "]"
              << __CPP_TRANSPORT_WRONG_F_LATEX_NAMES_B << Nf << "]";

            throw std::out_of_range(msg.str());
          }

        if(param_names.size() != Np)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_WRONG_PARAM_NAMES_A << param_names.size() << "]"
              << __CPP_TRANSPORT_WRONG_PARAM_NAMES_B << Np << "]";

            throw std::out_of_range(msg.str());
          }

        if(p_latex_names.size() != Np)
          {
            std::ostringstream msg;

            msg << __CPP_TRANSPORT_WRONG_P_LATEX_NAMES_A << p_latex_names.size() << "]"
              << __CPP_TRANSPORT_WRONG_P_LATEX_NAMES_B << Np << "]";

            throw std::out_of_range(msg.str());
          }

        if(parameters.size() != Np)
          {
            std::ostringstream msg;

            msg << __CPP_TRANSPORT_WRONG_PARAMS_A << parameters.size() << "]"
              << __CPP_TRANSPORT_WRONG_PARAMS_B << Np << "]";

            throw std::out_of_range(msg.str());
          }
      }


    template <typename number, unsigned int Nf, unsigned int Np>
    model<number, Nf, Np>::~model()
      {
        assert(this->mgr != nullptr);
        mgr->deregister_model(this, this->unique_id);
      }


    template <typename number, unsigned int Nf, unsigned int Np>
    const std::string& model<number, Nf, Np>::get_identity_string()
      {
        return(this->unique_id);
      }


    template <typename number, unsigned int Nf, unsigned int Np>
    const std::string& model<number, Nf, Np>::get_name()
      {
        return(this->name);
      }


    template <typename number, unsigned int Nf, unsigned int Np>
    const std::string& model<number, Nf, Np>::get_author()
      {
        return(this->author);
      }


    template <typename number, unsigned int Nf, unsigned int Np>
    const std::string& model<number, Nf, Np>::get_tag()
      {
        return(this->tag);
      }


    template <typename number, unsigned int Nf, unsigned int Np>
    const std::vector<std::string>& model<number, Nf, Np>::get_field_names()
      {
        return(this->field_names);
      }


    template <typename number, unsigned int Nf, unsigned int Np>
    const std::vector<std::string>& model<number, Nf, Np>::get_f_latex_names()
      {
        return(this->f_latex_names);
      }


    template <typename number, unsigned int Nf, unsigned int Np>
    const std::vector<std::string>& model<number, Nf, Np>::get_p_latex_names()
      {
        return(this->p_latex_names);
      }

    template <typename number, unsigned int Nf, unsigned int Np>
    const std::vector<number>& model<number, Nf, Np>::get_parameters()
      {
        return(this->parameters);
      }

    template <typename number, unsigned int Nf, unsigned int Np>
    const std::vector<std::string>& model<number, Nf, Mp>::get_state_names()
      {
        return(this->state_names);
      }


    // INITIAL CONDITIONS HANDLING


    template <typename number, unsigned int Nf, unsigned int Np>
    void model<number, Nf, Np>::find_ics(const parameters<number, Np>& params, const std::vector<number>& input, std::vector<number>& output,
                                         double Ninit, double Ncross, double Npre,
                                         double tolerance, double time_steps)
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
            std::vector<double> times;
            for(unsigned int i = 0; i <= time_steps; i++)
              {
                times.push_back(Ninit + (Ncross-Npre)*(static_cast<double>(i)/time_steps));
              }

            this->backend_raw_backg(input, times, history);

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


    template <typename number, unsigned int Nf, unsigned int Np>
    void model<number, Nf, Np>::write_task_data(const task<number>* task, std::ostream& stream,
                                                double abs_err, double rel_err, double step_size, std::string stepper_name)
      {
        stream << __CPP_TRANSPORT_SOLVING_ICS_MESSAGE << std::endl;

        stream << task.get_ics() << std::endl;

        stream << __CPP_TRANSPORT_STEPPER_MESSAGE    << " '"  << stepper_name
               << "', " << __CPP_TRANSPORT_ABS_ERR   << " = " << abs_err
               << ", "  << __CPP_TRANSPORT_REL_ERR   << " = " << rel_err
               << ", "  << __CPP_TRANSPORT_STEP_SIZE << " = " << step_size << std::endl;

        stream << std::endl;
      }


    template <typename number, unsigned int Nf, unsigned int Np>
    double model<number, Nf, Np>::get_kstar(const parameters<number, Np>& params, const task<number>* tk, double time_steps)
      {
        // integrate for a small interval up to horizon-crossing,
        // and extract the value of H there

        std::vector< std::vector<number> > history;

        // set up times at which we wish to sample -- we just need a few
        std::vector<double> times;
        for(unsigned int i = 0; i <= time_steps; i++)
          {
            times.push_back(tk->get_Ninit() + tk->get_Nstar()*(static_cast<double>(i)/time_steps));
          }

        this->backend_raw_backg(tk->get_ics(), times, history);

        if(history.size() > 0)
          {
            // the wavenumbers supplied to the twopf, threepf integration routines
            // use k=1 for the wavenumber which crosses the horizon at time Nstar.
            // This wavenumber should have comoving value k=aH
            // Here, we return the normalization constant aH
            return this->H(params, history.back()) * exp(tk->get_Nstar());
          }
        else
          {
            throw std::logic_error(__CPP_TRANSPORT_INTEGRATION_FAIL);
          }
      }


    // BASIC BACKGROUND, TWOPF AND THREEPF INTEGRATIONS


    // Integrate the background
    template <typename number, unsigned int Nf, unsigned int Np>
    transport::background<number> model<number>::background(const task<number>* tk, bool silent)
      {
        assert(tk != nullptr);

        std::vector< std::vector<number> > history;

        this->backend_process_backg(tk, history, silent);

        transport::background<number> backg(tk, history, this);

        return(backg);
      }


    // Integrate the twopf
    template <typename number, unsigned int Nf, unsigned int Np>
    transport::twopf<number> model<number>::twopf(const twopf_task<number>& tk, bool silent)
      {
        context                   ctx  = this->backend_get_context();
        scheduler                 sch  = scheduler(ctx);
        work_queue<twopf_kconfig> work = sch.make_queue(this->backend_twopf_state_size(), tk);

        std::cerr << "Work queue:" << std::endl;
        std::cerr << work;

        std::vector< std::vector<number> >                backg;
        std::vector< std::vector< std::vector<number> > > twopf;

        backend_process_twopf(work, &tk, backg, twopf, silent);

        transport::twopf<number> tpf(&tk, backg, twopf, this);
        return(tpf);
      }


    // Integrate the threepf
    template <typename number, unsigned int Nf, unsigned int Np>
    transport::threepf<number> model<number>::threepf(const threepf_task<number>& tk, bool silent)
      {
        context                     ctx  = this->backend_get_context();
        scheduler                   sch  = scheduler(ctx);
        work_queue<threepf_kconfig> work = sch.make_queue(this->backend_threepf_state_size(), tk);

        std::cerr << "Work queue:" << std::endl;
        std::cerr << work;

        std::vector< std::vector<number> >                backg;
        std::vector< std::vector< std::vector<number> > > twopf_re;
        std::vector< std::vector< std::vector<number> > > twopf_im;
        std::vector< std::vector< std::vector<number> > > threepf;

        backend_process_threepf(work, &tk, backg, twopf_re, twopf_im, threepf, silent);

        transport::threepf<number> tpf(&tk, backg, twopf_re, twopf_im, threepf, this);
        return(tpf);
      }


  }    // namespace transport

#endif // __CPP_TRANSPORT_MODEL_H_
