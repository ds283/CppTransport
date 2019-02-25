//
// Created by David Seery on 05/12/2013.
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


#include <string>
#include <sstream>

#include <assert.h>
#include <functional>

#include "cpp_steppers.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, p, prn))
#define EMPLACE(pkg, obj) try { emplace_rule(pkg, obj); } catch(std::exception& xe) { }


namespace cpp
  {

    static std::string replace_stepper(const struct stepper& s, std::string state_name);


    cpp_steppers::cpp_steppers(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : ::macro_packages::replacement_rule_package(f, cw, lm, p, prn)
      {
        EMPLACE(pre_package, BIND(replace_backg_stepper, "MAKE_BACKG_STEPPER"));
        EMPLACE(pre_package, BIND(replace_pert_stepper, "MAKE_PERT_STEPPER"));
      }


    // *******************************************************************
    
    
    static std::string
    replace_stepper(boost::optional<contexted_value<std::shared_ptr<stepper> > > s, const std::string& state_name,
                    const std::string& value_type, const std::string& time_type, const std::string& algebra_name,
                    const std::string& operations_name)
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
        //     reduced to obtain the state x(t) exactly at the time point. [runge_kutta_cash_karp45, runge_kutta_fehlberg78]
        //  ** If stepper is a Dense Output Stepper then dt is the initial step size. The actual step
        //     size is adjusted during integration according to error control. Dense output is used
        //     to obtain the states x(t) at the time points from the sequence. [runge_kutta_dopri5, bulirsch_stoer]

        std::string name;

        if(s)
          {
            auto& step = ***s;
            name = step.get_name();

            if(name == "runge_kutta_dopri5")
              {
                out << "boost::numeric::odeint::make_dense_output< boost::numeric::odeint::runge_kutta_dopri5< "
                    << state_name << ", " << value_type << ", " << state_name << ", " << time_type << ", "
                    << algebra_name << ", " << operations_name
                    << " > >(" << step.get_abserr() << ", " << step.get_relerr() << ")";
              }
            else if(name == "bulirsch_stoer_dense_out")
              {
                out << "boost::numeric::odeint::bulirsch_stoer_dense_out< "
                    << state_name << ", " << value_type << ", " << state_name << ", " << time_type << ", "
                    << algebra_name << ", " << operations_name
                    << " >(" << step.get_abserr() << ", " << step.get_relerr() << ")";
              }
            else if(name == "bulirsch_stoer")
              {
                out << "boost::numeric::odeint::bulirsch_stoer< "
                    << state_name << ", " << value_type << ", " << state_name << ", " << time_type << ", "
                    << algebra_name << ", " << operations_name
                    << " >(" << step.get_abserr() << ", " << step.get_relerr() << ")";
              }
            else if(name == "runge_kutta_fehlberg78")
              {
                out << "boost::numeric::odeint::make_controlled< boost::numeric::odeint::runge_kutta_fehlberg78< "
                    << state_name << ", " << value_type << ", " << state_name << ", " << time_type << ", "
                    << algebra_name << ", " << operations_name
                    << " > >(" << step.get_abserr() << ", " << step.get_relerr() << ")";
              }
            else if(name == "runge_kutta_cash_karp45")
              {
                out << "boost::numeric::odeint::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp45< "
                    << state_name << ", " << value_type << ", " << state_name << ", " << time_type << ", "
                    << algebra_name << ", " << operations_name
                    << " > >(" << step.get_abserr() << ", " << step.get_relerr() << ")";
              }
            else if(name == "adams_bashforth_moulton")
              {
                out << "boost::numeric::odeint::make_controlled< boost::numeric::odeint::adaptive_adams_bashforth_moulton< 5, "
                    << state_name << ", " << value_type << ", " << state_name << ", " << time_type << ", "
                    << algebra_name << ", " << operations_name
                    << " > >(" << step.get_abserr() << ", " << step.get_relerr() << ")";
              }
            else
              {
                std::ostringstream msg;
                msg << ERROR_UNKNOWN_STEPPER << " '" << name << "'";
                throw macro_packages::rule_apply_fail(msg.str());
              }
          }
        else
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }


        return(out.str());
      }


    // ********************************************************************************


    std::string replace_backg_stepper::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_background_stepper();
        std::string state_name = args[BACKG_STEPPER_STATE_ARGUMENT];
        std::string value_type = args[BACKG_STEPPER_VALUE_TYPE_ARGUMENT];
        std::string time_type = args[BACKG_STEPPER_TIME_TYPE_ARGUMENT];
        std::string algebra_name = args[BACKG_STEPPER_ALGEBRA_ARGUMENT];
        std::string operations_name = args[BACKG_STEPPER_OPERATIONS_ARGUMENT];

        return(replace_stepper(s, state_name, value_type, time_type, algebra_name, operations_name));
      }


    std::string replace_pert_stepper::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_perturbations_stepper();
        std::string state_name = args[PERT_STEPPER_STATE_ARGUMENT];
        std::string value_type = args[PERT_STEPPER_VALUE_TYPE_ARGUMENT];
        std::string time_type = args[PERT_STEPPER_TIME_TYPE_ARGUMENT];
        std::string algebra_name = args[BACKG_STEPPER_ALGEBRA_ARGUMENT];
        std::string operations_name = args[BACKG_STEPPER_OPERATIONS_ARGUMENT];

        return(replace_stepper(s, state_name, value_type, time_type, algebra_name, operations_name));
      }


  } // namespace cpp
