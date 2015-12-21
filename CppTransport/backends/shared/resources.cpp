//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>

#include "resources.h"


#define BIND1(X) std::move(std::make_shared<X>(this->mgr, this->printer))


namespace macro_packages
  {

    constexpr unsigned int PARAMETERS_KERNEL_ARGUMENT = 0;
    constexpr unsigned int PARAMETERS_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int COORDINATES_KERNEL_ARGUMENT = 0;
    constexpr unsigned int COORDINATES_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int PHASE_FLATTEN_KERNEL_ARGUMENT = 0;
    constexpr unsigned int PHASE_FLATTEN_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int FIELD_FLATTEN_KERNEL_ARGUMENT = 0;
    constexpr unsigned int FIELD_FLATTEN_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int RELEASE_FLATTENERS_TOTAL_ARGUMENTS = 0;

    constexpr unsigned int DV_KERNEL_ARGUMENT = 0;
    constexpr unsigned int DV_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int DDV_KERNEL_ARGUMENT = 0;
    constexpr unsigned int DDV_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int DDDV_KERNEL_ARGUMENT = 0;
    constexpr unsigned int DDDV_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int CONNEXION_KERNEL_ARGUMENT = 0;
    constexpr unsigned int CONNEXION_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int RIEMANN_KERNEL_ARGUMENT = 0;
    constexpr unsigned int RIEMANN_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int RELEASE_TOTAL_ARGUMENTS = 0;


    resources::resources(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, p, prn),
        mgr(f.get_resource_manager())
      {
        pre_package.emplace_back("RESOURCE_PARAMETERS", BIND1(set_params), PARAMETERS_TOTAL_ARGUMENTS);
        pre_package.emplace_back("RESOURCE_COORDINATES", BIND1(set_coordinates), COORDINATES_TOTAL_ARGUMENTS);
        pre_package.emplace_back("PHASE_FLATTEN", BIND1(set_phase_flatten), PHASE_FLATTEN_TOTAL_ARGUMENTS);
        pre_package.emplace_back("FIELD_FLATTEN", BIND1(set_field_flatten), FIELD_FLATTEN_TOTAL_ARGUMENTS);
        pre_package.emplace_back("RELEASE_FLATTENERS", BIND1(release_flatteners), RELEASE_FLATTENERS_TOTAL_ARGUMENTS);
        pre_package.emplace_back("RESOURCE_DV", BIND1(set_dV), DV_TOTAL_ARGUMENTS);
        pre_package.emplace_back("RESOURCE_DDV", BIND1(set_ddV), DDV_TOTAL_ARGUMENTS);
        pre_package.emplace_back("RESOURCE_DDDV", BIND1(set_dddV), DDDV_TOTAL_ARGUMENTS);
        pre_package.emplace_back("RESOURCE_RELEASE", BIND1(release), RELEASE_TOTAL_ARGUMENTS);
      }


    const std::vector<index_rule> resources::get_index_rules()
      {
        std::vector<index_rule> package;

        return(package);
      }


    std::string set_params::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_parameters(args[PARAMETERS_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PARAMETERS << " '" << static_cast<std::string>(args[PARAMETERS_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_coordinates::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_coordinates(args[COORDINATES_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_COORDINATES << " '" << static_cast<std::string>(args[COORDINATES_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_phase_flatten::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_phase_flatten(args[PHASE_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PHASE_FLATTEN << " '" << static_cast<std::string>(args[PHASE_FLATTEN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_field_flatten::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_field_flatten(args[FIELD_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_FIELD_FLATTEN << " '" << static_cast<std::string>(args[FIELD_FLATTEN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string release_flatteners::operator()(const macro_argument_list& args)
      {
        this->mgr.release_flatteners();

        return this->printer.comment(RESOURCE_RELEASE_FLATTENERS);
      }


    std::string set_dV::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_dV(args[DV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DV << " '" << static_cast<std::string>(args[DV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_ddV::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_ddV(args[DDV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDV << " '" << static_cast<std::string>(args[DDV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_dddV::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_dddV(args[DDDV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDDV << " '" << static_cast<std::string>(args[DDDV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_connexion::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_connexion(args[CONNEXION_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_CONNEXION << " '" << static_cast<std::string>(args[CONNEXION_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_Riemann::operator()(const macro_argument_list& args)
      {
        this->mgr.assign_Riemann(args[RIEMANN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_RIEMANN << " '" << static_cast<std::string>(args[RIEMANN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string release::operator()(const macro_argument_list& args)
      {
        this->mgr.release();

        return this->printer.comment(RESOURCE_RELEASE);
      }

  }
