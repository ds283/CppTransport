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


#define BIND(X) std::move(std::make_shared<X>(this->data_payload, this->printer))


namespace vexcl
  {

    constexpr auto VEXCL_STEPPER = "runge_kutta_dopri5";

    constexpr unsigned int BACKG_STEPPER_STATE_ARGUMENT = 0;
    constexpr unsigned int BACKG_STEPPER_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int PERT_STEPPER_STATE_ARGUMENT = 0;
    constexpr unsigned int PERT_STEPPER_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int BACKG_NAME_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PERT_NAME_TOTAL_ARGUMENTS = 0;


    vexcl_steppers::vexcl_steppers(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : ::macro_packages::replacement_rule_package(f, cw, p, prn)
      {
        pre_package.emplace_back("MAKE_BACKG_STEPPER", BIND(replace_backg_stepper), BACKG_STEPPER_TOTAL_ARGUMENTS);
        pre_package.emplace_back("MAKE_PERT_STEPPER", BIND(replace_pert_stepper), PERT_STEPPER_TOTAL_ARGUMENTS);
        pre_package.emplace_back("BACKG_STEPPER", BIND(stepper_name), BACKG_NAME_TOTAL_ARGUMENTS);
        pre_package.emplace_back("PERT_STEPPER", BIND(stepper_name), PERT_NAME_TOTAL_ARGUMENTS);
      }


    const std::vector<macro_packages::index_rule> vexcl_steppers::get_index_rules()
      {
        std::vector<macro_packages::index_rule> package;

        return(package);
      }


    // *******************************************************************


    std::string replace_backg_stepper::operator()(const macro_argument_list& args)
      {
        const struct stepper& s = this->data_payload.get_background_stepper();
        std::string state_name = args[BACKG_STEPPER_STATE_ARGUMENT];

        if(s.name != VEXCL_STEPPER)
          {
            std::ostringstream msg;
            msg << WARNING_VEXCL_STEPPER_IGNORED_A << " '" << VEXCL_STEPPER << "' " << WARNING_VEXCL_STEPPER_IGNORED_B << " ('" << s.name << "')";

            error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            err_context.warn(msg.str());
          }

        std::ostringstream out;
        out << "make_controlled< " << VEXCL_STEPPER << "< " << state_name << ", double, " << state_name << ", double, vector_space_algebra, default_operations > >(" << s.abserr << ", " << s.relerr << ")";

        return(out.str());
      }


    std::string replace_pert_stepper::operator()(const macro_argument_list& args)
      {
        const struct stepper& s = this->data_payload.get_perturbations_stepper();
        std::string state_name = args[PERT_STEPPER_STATE_ARGUMENT];

        if(s.name != VEXCL_STEPPER)
          {
            std::ostringstream msg;
            msg << WARNING_VEXCL_STEPPER_IGNORED_A << " '" << VEXCL_STEPPER << "' " << WARNING_VEXCL_STEPPER_IGNORED_B << " ('" << s.name << "')";

            error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            err_context.warn(msg.str());
          }

        std::ostringstream out;
        out << "make_controlled< " << VEXCL_STEPPER << "< " << state_name << ", double, " << state_name << ", double, vector_space_algebra, default_operations > >(" << s.abserr << ", " << s.relerr << ")";

        return(out.str());
      }

    std::string stepper_name::operator()(const macro_argument_list& args)
      {
        return(VEXCL_STEPPER);
      }


  } // namespace cpp
