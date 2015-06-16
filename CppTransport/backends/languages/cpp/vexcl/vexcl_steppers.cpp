//
// Created by David Seery on 06/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <string>
#include <sstream>

#include <functional>

#include "vexcl_steppers.h"
#include "translation_unit.h"


#define VEXCL_STEPPER "runge_kutta_fehlberg78"
//#define VEXCL_STEPPER "runge_kutta_dopri5"

#define BIND(X) std::bind(&vexcl_steppers::X, this, std::placeholders::_1)


namespace cpp
  {

    const std::vector<macro_packages::simple_rule> vexcl_steppers::get_pre_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
          { BIND(replace_backg_stepper), BIND(replace_pert_stepper), BIND(stepper_name), BIND(stepper_name)
          };

        const std::vector<std::string> names =
          { "MAKE_BACKG_STEPPER",        "MAKE_PERT_STEPPER",        "BACKG_STEPPER",    "BACKG_STEPPER"
          };

        const std::vector<unsigned int> args =
          { 1,                           1,                          1,                  1
          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); ++i)
          {
            macro_packages::simple_rule rule;

            rule.rule = rules[i];
            rule.args = args[i];
            rule.name = names[i];

            package.push_back(rule);
          }

        return(package);
      }


    const std::vector<macro_packages::simple_rule> vexcl_steppers::get_post_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        return(package);
      }


    const std::vector<macro_packages::index_rule> vexcl_steppers::get_index_rules()
      {
        std::vector<macro_packages::index_rule> package;

        return(package);
      }


    // *******************************************************************


    std::string vexcl_steppers::replace_backg_stepper(const std::vector<std::string>& args)
      {
        const struct stepper& s = this->unit->get_background_stepper();

        assert(args.size() == 1);
        if(args.size() < 1)
          {
            this->error(ERROR_VEXCL_NO_STEPPER_STATE);
            exit(EXIT_FAILURE);
          }
        std::string state_name = args[0];

        if(s.name != VEXCL_STEPPER)
          {
            std::ostringstream msg;
            msg << WARNING_VEXCL_STEPPER_IGNORED_A << " '" << VEXCL_STEPPER << "' " << WARNING_VEXCL_STEPPER_IGNORED_B << " ('" << s.name << "')";
            this->warn(msg.str());
          }

        std::ostringstream out;
        out << "make_controlled< " << VEXCL_STEPPER << "< " << state_name << ", double, " << state_name << ", double, vector_space_algebra, default_operations > >(" << s.abserr << ", " << s.relerr << ")";

        return(out.str());
      }


    std::string vexcl_steppers::replace_pert_stepper(const std::vector<std::string>& args)
      {
        const struct stepper& s = this->unit->get_perturbations_stepper();

        assert(args.size() == 1);
        if(args.size() < 1)
          {
            this->error(ERROR_VEXCL_NO_STEPPER_STATE);
            exit(EXIT_FAILURE);
          }
        std::string state_name = args[0];

        if(s.name != VEXCL_STEPPER)
          {
            std::ostringstream msg;
            msg << WARNING_VEXCL_STEPPER_IGNORED_A << " '" << VEXCL_STEPPER << "' " << WARNING_VEXCL_STEPPER_IGNORED_B << " ('" << s.name << "')";
            this->warn(msg.str());
          }

        std::ostringstream out;
        out << "make_controlled< " << VEXCL_STEPPER << "< " << state_name << ", double, " << state_name << ", double, vector_space_algebra, default_operations > >(" << s.abserr << ", " << s.relerr << ")";

        return(out.str());
      }

    std::string vexcl_steppers::stepper_name(const std::vector<std::string>& args)
      {
        return(VEXCL_STEPPER);
      }


  } // namespace cpp
