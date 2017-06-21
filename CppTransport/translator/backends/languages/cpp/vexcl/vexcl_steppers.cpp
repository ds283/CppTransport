//
// Created by David Seery on 06/12/2013.
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


#include <assert.h>
#include <string>
#include <sstream>

#include <functional>

#include "vexcl_steppers.h"
#include "translation_unit.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, p, prn))
#define EMPLACE(pkg, obj) try { emplace_rule(pkg, obj); } catch(std::exception& xe) { }


namespace vexcl
  {

    constexpr auto VEXCL_STEPPER = "runge_kutta_dopri5";


    vexcl_steppers::vexcl_steppers(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : ::macro_packages::replacement_rule_package(f, cw, lm, p, prn)
      {
        EMPLACE(pre_package, BIND(replace_backg_stepper, "MAKE_BACKG_STEPPER"));
        EMPLACE(pre_package, BIND(replace_pert_stepper, "MAKE_PERT_STEPPER"));
        EMPLACE(pre_package, BIND(stepper_name, "BACKG_STEPPER"));
        EMPLACE(pre_package, BIND(stepper_name, "PERT_STEPPER"));
      }


    // *******************************************************************


    std::string replace_backg_stepper::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_perturbations_stepper();
        std::string state_name = args[BACKG_STEPPER_STATE_ARGUMENT];

        if(!s)
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }

        auto& step = ***s;

        if(step.get_name() != VEXCL_STEPPER)
          {
            std::ostringstream msg;
            msg << WARNING_VEXCL_STEPPER_IGNORED_A << " '" << VEXCL_STEPPER << "' " << WARNING_VEXCL_STEPPER_IGNORED_B << " ('" << step.get_name() << "')";
    
            error_context err_ctx = this->data_payload.make_error_context();
            err_ctx.warn(msg.str());
          }

        std::ostringstream out;
        out << "make_controlled< " << VEXCL_STEPPER << "< " << state_name << ", double, " << state_name << ", double, vector_space_algebra, default_operations > >(" << step.get_abserr() << ", " << step.get_relerr() << ")";

        return(out.str());
      }


    std::string replace_pert_stepper::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_perturbations_stepper();
        std::string state_name = args[PERT_STEPPER_STATE_ARGUMENT];

        if(!s)
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }

        auto& step = ***s;

        if(step.get_name() != VEXCL_STEPPER)
          {
            std::ostringstream msg;
            msg << WARNING_VEXCL_STEPPER_IGNORED_A << " '" << VEXCL_STEPPER << "' " << WARNING_VEXCL_STEPPER_IGNORED_B << " ('" << step.get_name() << "')";
    
            error_context err_ctx = this->data_payload.make_error_context();
            err_ctx.warn(msg.str());
          }

        std::ostringstream out;
        out << "make_controlled< " << VEXCL_STEPPER << "< " << state_name << ", double, " << state_name << ", double, vector_space_algebra, default_operations > >(" << step.get_abserr() << ", " << step.get_relerr() << ")";

        return(out.str());
      }

    std::string stepper_name::evaluate(const macro_argument_list& args)
      {
        return(VEXCL_STEPPER);
      }


  } // namespace cpp
