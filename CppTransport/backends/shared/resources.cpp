//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>

#include "resources.h"


#define BIND1(X) std::bind(&resources::X, this, std::placeholders::_1)


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


    const std::vector<simple_rule> resources::get_pre_rules()
      {
        std::vector<simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
          { BIND1(set_params),                BIND1(set_coordinates),
            BIND1(set_phase_flatten),         BIND1(set_field_flatten),           BIND1(release_flatteners),
            BIND1(set_dV),                    BIND1(set_ddV),                     BIND1(set_dddV),
            BIND1(set_connexion),             BIND1(set_Riemann),
            BIND1(release)
          };

        const std::vector<std::string> names =
          { "RESOURCE_PARAMETERS",            "RESOURCE_COORDINATES",
            "PHASE_FLATTEN",                  "FIELD_FLATTEN",                    "RELEASE_FLATTENERS",
            "RESOURCE_DV",                    "RESOURCE_DDV",                     "RESOURCE_DDDV",
            "RESOURCE_CONNEXION",             "RESOURCE_RIEMANN",
            "RESOURCE_RELEASE"
          };

        const std::vector<unsigned int> args =
          { PARAMETERS_TOTAL_ARGUMENTS,       COORDINATES_TOTAL_ARGUMENTS,
            PHASE_FLATTEN_TOTAL_ARGUMENTS,    FIELD_FLATTEN_TOTAL_ARGUMENTS,      RELEASE_FLATTENERS_TOTAL_ARGUMENTS,
            DV_TOTAL_ARGUMENTS,               DDV_TOTAL_ARGUMENTS,                DDDV_TOTAL_ARGUMENTS,
            CONNEXION_TOTAL_ARGUMENTS,        RIEMANN_TOTAL_ARGUMENTS,
            RELEASE_TOTAL_ARGUMENTS
          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); ++i)
          {
            package.emplace_back(names[i], rules[i], args[i]);
          }

        return(package);
      }


    const std::vector<simple_rule> resources::get_post_rules()
      {
        std::vector<simple_rule> package;

        return(package);
      }


    const std::vector<index_rule> resources::get_index_rules()
      {
        std::vector<index_rule> package;

        return(package);
      }


    std::string resources::set_params(const macro_argument_list& args)
      {
        this->mgr.assign_parameters(args[PARAMETERS_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PARAMETERS << " '" << static_cast<std::string>(args[PARAMETERS_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string resources::set_coordinates(const macro_argument_list& args)
      {
        this->mgr.assign_coordinates(args[COORDINATES_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_COORDINATES << " '" << static_cast<std::string>(args[COORDINATES_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string resources::set_phase_flatten(const macro_argument_list& args)
      {
        this->mgr.assign_phase_flatten(args[PHASE_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PHASE_FLATTEN << " '" << static_cast<std::string>(args[PHASE_FLATTEN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string resources::set_field_flatten(const macro_argument_list& args)
      {
        this->mgr.assign_field_flatten(args[FIELD_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_FIELD_FLATTEN << " '" << static_cast<std::string>(args[FIELD_FLATTEN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string resources::release_flatteners(const macro_argument_list& args)
      {
        this->mgr.release_flatteners();

        return this->printer.comment(RESOURCE_RELEASE_FLATTENERS);
      }


    std::string resources::set_dV(const macro_argument_list& args)
      {
        this->mgr.assign_dV(args[DV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DV << " '" << static_cast<std::string>(args[DV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string resources::set_ddV(const macro_argument_list& args)
      {
        this->mgr.assign_ddV(args[DDV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDV << " '" << static_cast<std::string>(args[DDV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string resources::set_dddV(const macro_argument_list& args)
      {
        this->mgr.assign_dddV(args[DDDV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDDV << " '" << static_cast<std::string>(args[DDDV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string resources::set_connexion(const macro_argument_list& args)
      {
        this->mgr.assign_connexion(args[CONNEXION_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_CONNEXION << " '" << static_cast<std::string>(args[CONNEXION_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string resources::set_Riemann(const macro_argument_list& args)
      {
        this->mgr.assign_Riemann(args[RIEMANN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_RIEMANN << " '" << static_cast<std::string>(args[RIEMANN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string resources::release(const macro_argument_list& args)
      {
        this->mgr.release();

        return this->printer.comment(RESOURCE_RELEASE);
      }

  }
