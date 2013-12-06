//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include <functional>

#include "core_macros.h"

#define BIND(X) std::bind(&core_macros::X, this, std::placeholders::_1)

namespace cpp
  {

    const std::vector<macro_packages::simple_rule> core_macros::get_pre_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
          { BIND(replace_backg_stepper), BIND(replace_pert_stepper)
          };

        const std::vector<std::string> names =
          { "MAKE_BACKG_STEPPER",        "MAKE_PERT_STEPPER"
          };

        const std::vector<unsigned int> args =
          { 1,                           1
          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); i++)
          {
            macro_packages::simple_rule rule;

            rule.rule = rules[i];
            rule.args = args[i];
            rule.name = names[i];

            package.push_back(rule);
          }

        return(package);
      }


    const std::vector<macro_packages::simple_rule> core_macros::get_post_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        return(package);
      }


    const std::vector<macro_packages::index_rule> core_macros::get_index_rules()
      {
        std::vector<macro_packages::index_rule> package;

        return(package);
      }


    // *******************************************************************


    std::string core_macros::replace_stepper(const struct stepper& s, std::string state_name)
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
            out << "make_dense_output< runge_kutta_dopri5< " << state_name << " > >(" << s.abserr << ", " << s.relerr << ")";
          }
        else if(s.name == "bulirsch_stoer_dense_out")
          {
            out << "bulirsch_stoer_dense_out< " << state_name << " >(" << s.abserr << ", " << s.relerr << ")";
          }
        else if(s.name == "runge_kutta_fehlberg78")
          {
            out << "make_controlled< runge_kutta_fehlberg78< " << state_name << " > >(" << s.abserr << ", " << s.relerr << ")";
          }
        else
          {
            std::ostringstream msg;
            msg << ERROR_UNKNOWN_STEPPER << " '" << s.name << "'";
            error(msg.str());
            out << "<UNKNOWN_STEPPER>";
          }

        return(out.str());
      }


    // ********************************************************************************


    std::string core_macros::replace_backg_stepper(const std::vector<std::string>& args)
      {
        const struct stepper s = data.parse_tree->get_background_stepper();

        assert(args.size() == 1);
        std::string state_name = (args.size() >= 1 ? args[0] : this->default_state);

        return(this->replace_stepper(s, state_name));
      }


    std::string core_macros::replace_pert_stepper(const std::vector<std::string>& args)
      {
        const struct stepper s = data.parse_tree->get_perturbations_stepper();

        assert(args.size() == 1);
        std::string state_name = (args.size() >= 1 ? args[0] : this->default_state);

        return(this->replace_stepper(s, state_name));
      }


  } // namespace cpp