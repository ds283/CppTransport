//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <functional>
#include <time.h>

#include "fundamental.h"
#include "to_printable.h"
#include "translation_unit.h"
#include "core.h"

#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/string_generator.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#define BIND(X) std::bind(&fundamental::X, this, std::placeholders::_1)

namespace macro_packages
  {

    const std::vector<simple_rule> fundamental::get_pre_rules()
      {
        std::vector<simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
          { BIND(replace_tool) ,          BIND(replace_version),        BIND(replace_guard),       BIND(replace_date),        BIND(replace_source),
            BIND(replace_name),           BIND(replace_author),         BIND(replace_tag),         BIND(replace_model),       BIND(replace_uid),
            BIND(replace_header),         BIND(replace_core),
            BIND(replace_number_fields),  BIND(replace_number_params),
            BIND(replace_field_list),     BIND(replace_latex_list),     BIND(replace_param_list),  BIND(replace_platx_list),  BIND(replace_state_list),
            BIND(replace_b_abs_err),      BIND(replace_b_rel_err),      BIND(replace_b_step),      BIND(replace_b_stepper),
            BIND(replace_p_abs_err),      BIND(replace_p_rel_err),      BIND(replace_p_step),      BIND(replace_p_stepper)
          };

        const std::vector<std::string> names =
          { "TOOL",                       "VERSION",                    "GUARD",                   "DATE",                    "SOURCE",
            "NAME",                       "AUTHOR",                     "TAG",                     "MODEL",                   "UNIQUE_ID",
            "HEADER",                     "CORE",
            "NUMBER_FIELDS",              "NUMBER_PARAMS",
            "FIELD_NAME_LIST",            "LATEX_NAME_LIST",            "PARAM_NAME_LIST",         "PLATX_NAME_LIST",         "STATE_NAME_LIST",
            "BACKG_ABS_ERR",              "BACKG_REL_ERR",              "BACKG_STEP_SIZE",         "BACKG_STEPPER",
            "PERT_ABS_ERR",               "PERT_REL_ERR",               "PERT_STEP_SIZE",          "PERT_STEPPER"
          };

        const std::vector<unsigned int> args =
          { 0,                            0,                            0,                         0,                         0,
            0,                            0,                            0,                         0,                         0,
            0,                            0,
            0,                            0,
            0,                            0,                            0,                         0,                         0,
            0,                            0,                            0,                         0,
            0,                            0,                            0,                         0
          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); ++i)
          {
            simple_rule rule;

            rule.rule = rules[i];
            rule.args = args[i];
            rule.name = names[i];

            package.push_back(rule);
          }

        return(package);
      }


    const std::vector<simple_rule> fundamental::get_post_rules()
      {
        std::vector<simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
          { BIND(replace_unique)
          };

        const std::vector<std::string> names =
          { "UNIQUE"
          };

        const std::vector<unsigned int> args =
          { 0
          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); ++i)
          {
            simple_rule rule;

            rule.rule = rules[i];
            rule.args = args[i];
            rule.name = names[i];

            package.push_back(rule);
          }

        return(package);
      }


    const std::vector<index_rule> fundamental::get_index_rules()
      {
        std::vector<index_rule> package;

        return(package);
      }


    // *******************************************************************


    std::string fundamental::stringize_list(std::vector<std::string> list)
      {
        std::ostringstream out;

        out << this->list_start + this->list_pad;

        for(int i = 0; i < list.size(); ++i)
          {
            if(i > 0)
              {
                out << this->list_separator + this->list_pad;
              }
            out << to_printable(list[i]);
          }
        out << this->list_pad + this->list_end;

        return(out.str());
      }


    std::string fundamental::stringize_number(double number)
      {
        return(boost::lexical_cast<std::string>(number));
      }


    // *******************************************************************


    // REPLACEMENT RULES

    // PRE macros

    std::string fundamental::replace_tool(const std::vector<std::string> &args)
      {
        return(CPPTRANSPORT_NAME);
      }


    std::string fundamental::replace_version(const std::vector<std::string> &args)
      {
        return(CPPTRANSPORT_VERSION);
      }


    std::string fundamental::replace_guard(const std::vector<std::string> &args)
      {
        std::string guard;
        enum process_type type = this->unit->get_stack().top_process_type();

        if(type == process_core)
          {
            guard = this->unit->get_core_guard();
          }
        else if(type == process_implementation)
          {
            guard = this->unit->get_implementation_guard();
          }

        return(this->tag + "_" + guard + this->guard_terminator);
      }


    std::string fundamental::replace_date(const std::vector<std::string> &args)
      {
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        return(boost::posix_time::to_simple_string(now));
      }


    std::string fundamental::replace_source(const std::vector<std::string>& args)
      {
        return(this->unit->get_model_input());
      }


    std::string fundamental::replace_uid(const std::vector<std::string>& args)
      {
        std::string unique_string = this->unit->get_name() + this->unit->get_author() + this->unit->get_tag() + this->unit->get_model() + CPPTRANSPORT_VERSION;
        boost::uuids::string_generator gen;
        boost::uuids::uuid id = gen(unique_string);

        return(boost::uuids::to_string(id));
      }


    std::string fundamental::replace_name(const std::vector<std::string> &args)
      {
        return(this->unit->get_name());
      }


    std::string fundamental::replace_author(const std::vector<std::string> &args)
      {
        return(this->unit->get_author());
      }


    std::string fundamental::replace_tag(const std::vector<std::string> &args)
      {
        return(this->unit->get_tag());
      }


    std::string fundamental::replace_model(const std::vector<std::string>& args)
      {
        return(this->unit->get_model());
      }


    std::string fundamental::replace_header(const std::vector<std::string> &args)
      {
        return(this->unit->get_implementation_output());
      }


    std::string fundamental::replace_core(const std::vector<std::string> &args)
      {
        return(this->unit->get_core_output());
      }


    std::string fundamental::replace_number_fields(const std::vector<std::string> &args)
      {
        std::ostringstream out;

        out << this->unit->get_number_fields();

        return(out.str());
      }


    std::string fundamental::replace_number_params(const std::vector<std::string> &args)
      {
        std::ostringstream out;

        out << this->unit->get_number_parameters();

        return(out.str());
      }


    std::string fundamental::replace_field_list(const std::vector<std::string> &args)
      {
        std::vector<std::string> list = this->unit->get_field_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_latex_list(const std::vector<std::string> &args)
      {
        std::vector<std::string> list = this->unit->get_latex_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_param_list(const std::vector<std::string> &args)
      {
        std::vector<std::string> list = this->unit->get_param_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_platx_list(const std::vector<std::string> &args)
      {
        std::vector<std::string> list = this->unit->get_platx_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_state_list(const std::vector<std::string> &args)
      {
        std::vector<GiNaC::symbol> f_list = this->unit->get_field_symbols();
        std::vector<GiNaC::symbol> d_list = this->unit->get_deriv_symbols();

        std::vector<std::string> list;

        for(int i = 0; i < f_list.size(); ++i)
          {
            list.push_back(f_list[i].get_name());
          }
        for(int i = 0; i < d_list.size(); ++i)
          {
            list.push_back(d_list[i].get_name());
          }

        return(this->stringize_list(list));
      }


     std::string fundamental::replace_b_abs_err(const std::vector<std::string> &args)
      {
        const struct stepper s = this->unit->get_background_stepper();

        return(this->stringize_number(s.abserr));
      }


    std::string fundamental::replace_b_rel_err(const std::vector<std::string> &args)
      {
        const struct stepper s = this->unit->get_background_stepper();

        return(this->stringize_number(s.relerr));
      }


    std::string fundamental::replace_b_step(const std::vector<std::string> &args)
      {
        const struct stepper s = this->unit->get_background_stepper();

        return(this->stringize_number(s.stepsize));
      }


    std::string fundamental::replace_b_stepper(const std::vector<std::string> &args)
      {
        const struct stepper s = this->unit->get_background_stepper();

        return(s.name);
      }


    std::string fundamental::replace_p_abs_err(const std::vector<std::string> &args)
      {
        const struct stepper s = this->unit->get_perturbations_stepper();

        return(this->stringize_number(s.abserr));
      }


    std::string fundamental::replace_p_rel_err(const std::vector<std::string> &args)
      {
        const struct stepper s = this->unit->get_perturbations_stepper();

        return(this->stringize_number(s.relerr));
      }


    std::string fundamental::replace_p_step(const std::vector<std::string> &args)
      {
        const struct stepper s = this->unit->get_perturbations_stepper();

        return(this->stringize_number(s.stepsize));
      }


    std::string fundamental::replace_p_stepper(const std::vector<std::string>& args)
      {
        const struct stepper s = this->unit->get_perturbations_stepper();

        return(s.name);
      }


    // POST macros


    std::string fundamental::replace_unique(const std::vector<std::string> &args)
      {
        return(this->stringize_number(this->unique++));
      }


  } // namespace macro_packages


