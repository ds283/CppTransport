//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include <assert.h>
#include <functional>

#include "cpp_steppers.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, p, prn))


namespace cpp
  {

    static std::string replace_stepper(const struct stepper& s, std::string state_name);


    cpp_steppers::cpp_steppers(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : ::macro_packages::replacement_rule_package(f, cw, p, prn)
      {
        pre_package.emplace_back(BIND(replace_backg_stepper, "MAKE_BACKG_STEPPER"));
        pre_package.emplace_back(BIND(replace_pert_stepper, "MAKE_PERT_STEPPER"));
      }


    // *******************************************************************


    static std::string replace_stepper(const struct stepper& s, std::string state_name)
      {
        std::ostringstream out;

        // note that we need a generic stepper which works with an arbitrary state type; see
        // http://headmyshoulder.github.io/odeint-v2/doc/boost_numeric_odeint/concepts/system.html
        // we can't use things like rosenbrock4 or the implicit euler methods which work only with boost matrices

        // exactly when the steppers call the observer functor depends which stepper is in use; see
        // http://headmyshoulder.github.io/odeint-v2/doc/boost_numeric_odeint/odeint_in_detail/integrate_functions.html

        // to summarize the discussion there:
        //  ** If stepper is a Stepper or Error Stepper dt is the step size used for integration.
        //     However, whenever a time point from the sequence is approached the step size dt will
        //     be reduced to obtain the state x(t) exactly at the time point.
        //  ** If stepper is a Controlled Stepper then dt is the initial step size. The actual step
        //     size is adjusted during integration according to error control.
        //     However, if a time point from the sequence is approached the step size is
        //     reduced to obtain the state x(t) exactly at the time point. [runge_kutta_fehlberg78]
        //  ** If stepper is a Dense Output Stepper then dt is the initial step size. The actual step
        //     size is adjusted during integration according to error control. Dense output is used
        //     to obtain the states x(t) at the time points from the sequence. [runge_kutta_dopri5, bulirsch_stoer]

        if(s.name == "runge_kutta_dopri5")
          {
            out << "boost::numeric::odeint::make_dense_output< boost::numeric::odeint::runge_kutta_dopri5< " << state_name << " > >(" << s.abserr << ", " << s.relerr << ")";
          }
        else if(s.name == "bulirsch_stoer_dense_out")
          {
            out << "boost::numeric::odeint::bulirsch_stoer_dense_out< " << state_name << " >(" << s.abserr << ", " << s.relerr << ")";
          }
        else if(s.name == "bulirsch_stoer")
	        {
						out << "boost::numeric::odeint::bulirsch_stoer< " << state_name << " >(" << s.abserr << ", " << s.relerr << ")";
	        }
        else if(s.name == "runge_kutta_fehlberg78")
          {
            out << "boost::numeric::odeint::make_controlled< boost::numeric::odeint::runge_kutta_fehlberg78< " << state_name << " > >(" << s.abserr << ", " << s.relerr << ")";
          }
        else
          {
            std::ostringstream msg;
            msg << ERROR_UNKNOWN_STEPPER << " '" << s.name << "'";
            throw macro_packages::rule_apply_fail(msg.str());
          }

        return(out.str());
      }


    // ********************************************************************************


    std::string replace_backg_stepper::evaluate(const macro_argument_list& args)
      {
        const struct stepper& s = this->data_payload.get_background_stepper();
        std::string state_name = args[BACKG_STEPPER_STATE_ARGUMENT];

        return(replace_stepper(s, state_name));
      }


    std::string replace_pert_stepper::evaluate(const macro_argument_list& args)
      {
        const struct stepper& s = this->data_payload.get_perturbations_stepper();
        std::string state_name = args[PERT_STEPPER_STATE_ARGUMENT];

        return(replace_stepper(s, state_name));
      }


  } // namespace cpp
