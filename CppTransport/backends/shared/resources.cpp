//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>

#include "resources.h"


#define BIND1(X, N) std::move(std::make_unique<X>(N, this->mgr, this->printer))


namespace macro_packages
  {

    resources::resources(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, p, prn),
        mgr(f.get_resource_manager())
      {
        pre_package.emplace_back(BIND1(set_params, "RESOURCE_PARAMETERS"));
        pre_package.emplace_back(BIND1(set_coordinates, "RESOURCE_COORDINATES"));
        pre_package.emplace_back(BIND1(set_phase_flatten, "PHASE_FLATTEN"));
        pre_package.emplace_back(BIND1(set_field_flatten, "FIELD_FLATTEN"));
        pre_package.emplace_back(BIND1(release_flatteners, "RELEASE_FLATTENERS"));
        pre_package.emplace_back(BIND1(set_dV, "RESOURCE_DV"));
        pre_package.emplace_back(BIND1(set_ddV, "RESOURCE_DDV"));
        pre_package.emplace_back(BIND1(set_dddV, "RESOURCE_DDDV"));
        pre_package.emplace_back(BIND1(release, "RESOURCE_RELEASE"));
      }


    const std::vector<index_rule> resources::get_index_rules()
      {
        std::vector<index_rule> package;

        return(package);
      }


    std::string set_params::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_parameters(args[RESOURCES::PARAMETERS_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PARAMETERS << " '" << static_cast<std::string>(args[RESOURCES::PARAMETERS_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_coordinates::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_coordinates(args[RESOURCES::COORDINATES_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_COORDINATES << " '" << static_cast<std::string>(args[RESOURCES::COORDINATES_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_phase_flatten::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_phase_flatten(args[RESOURCES::PHASE_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PHASE_FLATTEN << " '" << static_cast<std::string>(args[RESOURCES::PHASE_FLATTEN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_field_flatten::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_field_flatten(args[RESOURCES::FIELD_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_FIELD_FLATTEN << " '" << static_cast<std::string>(args[RESOURCES::FIELD_FLATTEN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string release_flatteners::operator()(const macro_argument_list& args)
      {
        this->mgr.release_flatteners();

        return this->printer.comment(RESOURCE_RELEASE_FLATTENERS);
      }


    std::string set_dV::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_dV(args[RESOURCES::DV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DV << " '" << static_cast<std::string>(args[RESOURCES::DV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_ddV::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_ddV(args[RESOURCES::DDV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDV << " '" << static_cast<std::string>(args[RESOURCES::DDV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_dddV::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_dddV(args[RESOURCES::DDDV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDDV << " '" << static_cast<std::string>(args[RESOURCES::DDDV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_connexion::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_connexion(args[RESOURCES::CONNEXION_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_CONNEXION << " '" << static_cast<std::string>(args[RESOURCES::CONNEXION_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_Riemann::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_Riemann(args[RESOURCES::RIEMANN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_RIEMANN << " '" << static_cast<std::string>(args[RESOURCES::RIEMANN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string release::operator()(const macro_argument_list& args)
      {
        this->mgr.release();

        return this->printer.comment(RESOURCE_RELEASE);
      }

  }
