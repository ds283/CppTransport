//
// Created by David Seery on 18/12/2015.
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
#include <functional>

#include "resources.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, m, p))


namespace macro_packages
  {

    resources::resources(translator_data& p, resource_manager& m)
      : directive_package(p)
      {
        simple_package.emplace_back(BIND(set_params, "RESOURCE_PARAMETERS"));
        simple_package.emplace_back(BIND(set_coordinates, "RESOURCE_COORDINATES"));
        simple_package.emplace_back(BIND(set_phase_flatten, "PHASE_FLATTEN"));
        simple_package.emplace_back(BIND(set_field_flatten, "FIELD_FLATTEN"));
        simple_package.emplace_back(BIND(release_flatteners, "RELEASE_FLATTENERS"));
        simple_package.emplace_back(BIND(set_dV, "RESOURCE_DV"));
        simple_package.emplace_back(BIND(set_ddV, "RESOURCE_DDV"));
        simple_package.emplace_back(BIND(set_dddV, "RESOURCE_DDDV"));
        simple_package.emplace_back(BIND(release, "RESOURCE_RELEASE"));
        simple_package.emplace_back(BIND(set_working_type, "WORKING_TYPE"));
        simple_package.emplace_back(BIND(release_working_type, "RELEASE_WORKING_TYPE"));
      }


    std::string set_params::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_parameters(args[RESOURCES::PARAMETERS_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PARAMETERS << " '" << static_cast<std::string>(args[RESOURCES::PARAMETERS_KERNEL_ARGUMENT]) << "'";

        return msg.str();
      }


    std::string set_coordinates::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_coordinates(args[RESOURCES::COORDINATES_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_COORDINATES << " '" << static_cast<std::string>(args[RESOURCES::COORDINATES_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string set_phase_flatten::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_phase_flatten(args[RESOURCES::PHASE_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PHASE_FLATTEN << " '" << static_cast<std::string>(args[RESOURCES::PHASE_FLATTEN_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string set_field_flatten::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_field_flatten(args[RESOURCES::FIELD_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_FIELD_FLATTEN << " '" << static_cast<std::string>(args[RESOURCES::FIELD_FLATTEN_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string release_flatteners::evaluate(const macro_argument_list& args)
      {
        this->mgr.release_flatteners();

        return RESOURCE_RELEASE_FLATTENERS;
      }


    std::string set_dV::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_dV(args[RESOURCES::DV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DV << " '" << static_cast<std::string>(args[RESOURCES::DV_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string set_ddV::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_ddV(args[RESOURCES::DDV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDV << " '" << static_cast<std::string>(args[RESOURCES::DDV_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string set_dddV::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_dddV(args[RESOURCES::DDDV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDDV << " '" << static_cast<std::string>(args[RESOURCES::DDDV_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string set_connexion::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_connexion(args[RESOURCES::CONNEXION_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_CONNEXION << " '" << static_cast<std::string>(args[RESOURCES::CONNEXION_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string set_Riemann::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_Riemann(args[RESOURCES::RIEMANN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_RIEMANN << " '" << static_cast<std::string>(args[RESOURCES::RIEMANN_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string release::evaluate(const macro_argument_list& args)
      {
        this->mgr.release();

        return RESOURCE_RELEASE;
      }


    std::string set_working_type::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_working_type(args[RESOURCES::WORKING_TYPE_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_WORKING_TYPE << " '" << static_cast<std::string>(args[RESOURCES::WORKING_TYPE_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string release_working_type::evaluate(const macro_argument_list& args)
      {
        this->mgr.release_working_type();

        return RESOURCE_RELEASE_WORKING_TYPE;
      }
  }
