//
// Created by David Seery on 06/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <string>
#include <sstream>

#include <functional>

#include "vexcl_steppers.h"
#include "translation_unit.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, p, prn))


namespace vexcl
  {

    constexpr auto VEXCL_STEPPER = "runge_kutta_dopri5";


    vexcl_steppers::vexcl_steppers(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : ::macro_packages::replacement_rule_package(f, cw, lm, p, prn)
      {
        pre_package.emplace_back(BIND(replace_backg_stepper, "MAKE_BACKG_STEPPER"));
        pre_package.emplace_back(BIND(replace_pert_stepper, "MAKE_PERT_STEPPER"));
        pre_package.emplace_back(BIND(stepper_name, "BACKG_STEPPER"));
        pre_package.emplace_back(BIND(stepper_name, "PERT_STEPPER"));
      }


    // *******************************************************************


    std::string replace_backg_stepper::evaluate(const macro_argument_list& args)
      {
        const struct stepper& s = this->data_payload.get_background_stepper();
        std::string state_name = args[BACKG_STEPPER_STATE_ARGUMENT];

        if(s.get_name() != VEXCL_STEPPER)
          {
            std::ostringstream msg;
            msg << WARNING_VEXCL_STEPPER_IGNORED_A << " '" << VEXCL_STEPPER << "' " << WARNING_VEXCL_STEPPER_IGNORED_B << " ('" << s.get_name() << "')";

            error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            err_context.warn(msg.str());
          }

        std::ostringstream out;
        out << "make_controlled< " << VEXCL_STEPPER << "< " << state_name << ", double, " << state_name << ", double, vector_space_algebra, default_operations > >(" << s.get_abserr() << ", " << s.get_relerr() << ")";

        return(out.str());
      }


    std::string replace_pert_stepper::evaluate(const macro_argument_list& args)
      {
        const struct stepper& s = this->data_payload.get_perturbations_stepper();
        std::string state_name = args[PERT_STEPPER_STATE_ARGUMENT];

        if(s.get_name() != VEXCL_STEPPER)
          {
            std::ostringstream msg;
            msg << WARNING_VEXCL_STEPPER_IGNORED_A << " '" << VEXCL_STEPPER << "' " << WARNING_VEXCL_STEPPER_IGNORED_B << " ('" << s.get_name() << "')";

            error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            err_context.warn(msg.str());
          }

        std::ostringstream out;
        out << "make_controlled< " << VEXCL_STEPPER << "< " << state_name << ", double, " << state_name << ", double, vector_space_algebra, default_operations > >(" << s.get_abserr() << ", " << s.get_relerr() << ")";

        return(out.str());
      }

    std::string stepper_name::evaluate(const macro_argument_list& args)
      {
        return(VEXCL_STEPPER);
      }


  } // namespace cpp
