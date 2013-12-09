//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "fundamental.h"
#include "to_printable.h"

#include <functional>
#include <time.h>

#include "boost/lexical_cast.hpp"


#define BIND(X) std::bind(&fundamental::X, this, std::placeholders::_1)

namespace macro_packages
  {

    const std::vector<simple_rule> fundamental::get_pre_rules()
      {
        std::vector<simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
          { BIND(replace_tool) ,          BIND(replace_version),        BIND(replace_guard),       BIND(replace_date),        BIND(replace_source),
            BIND(replace_name),           BIND(replace_author),         BIND(replace_tag),         BIND(replace_model),
            BIND(replace_header),         BIND(replace_core),
            BIND(replace_number_fields),  BIND(replace_number_params),
            BIND(replace_field_list),     BIND(replace_latex_list),     BIND(replace_param_list),  BIND(replace_platx_list),  BIND(replace_state_list),
            BIND(replace_b_abs_err),      BIND(replace_b_rel_err),      BIND(replace_b_step),      BIND(replace_b_stepper),
            BIND(replace_p_abs_err),      BIND(replace_p_rel_err),      BIND(replace_p_step),      BIND(replace_p_stepper)
          };

        const std::vector<std::string> names =
          { "TOOL",                       "VERSION",                    "GUARD",                   "DATE",                    "SOURCE",
            "NAME",                       "AUTHOR",                     "TAG",                     "MODEL",
            "HEADER",                     "CORE",
            "NUMBER_FIELDS",              "NUMBER_PARAMS",
            "FIELD_NAME_LIST",            "LATEX_NAME_LIST",            "PARAM_NAME_LIST",         "PLATX_NAME_LIST",         "STATE_NAME_LIST",
            "BACKG_ABS_ERR",              "BACKG_REL_ERR",              "BACKG_STEP_SIZE",         "BACKG_STEPPER",
            "PERT_ABS_ERR",               "PERT_REL_ERR",               "PERT_STEP_SIZE",          "PERT_STEPPER"
          };

        const std::vector<unsigned int> args =
          { 0,                            0,                            0,                         0,                         0,
            0,                            0,                            0,                         0,
            0,                            0,
            0,                            0,
            0,                            0,                            0,                         0,                         0,
            0,                            0,                            0,                         0,
            0,                            0,                            0,                         0
          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); i++)
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

        for(int i = 0; i < rules.size(); i++)
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

        for(int i = 0; i < list.size(); i++)
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
        return CPPTRANSPORT_NAME;
      }


    std::string fundamental::replace_version(const std::vector<std::string> &args)
      {
        return CPPTRANSPORT_VERSION;
      }


    std::string fundamental::replace_guard(const std::vector<std::string> &args)
      {
        return this->tag + "_" + this->data.guard + this->guard_terminator;
      }


    std::string fundamental::replace_date(const std::vector<std::string> &args)
      {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);

        strftime(buf, sizeof(buf), this->time_format.c_str(), &tstruct);

        std::string rval(buf);

        return(rval);
      }


    std::string fundamental::replace_source(const std::vector<std::string> &args)
      {
        return(this->data.script_in);
      }


    std::string fundamental::replace_name(const std::vector<std::string> &args)
      {
        return(this->data.parse_tree->get_name());
      }


    std::string fundamental::replace_author(const std::vector<std::string> &args)
      {
        return(this->data.parse_tree->get_author());
      }


    std::string fundamental::replace_tag(const std::vector<std::string> &args)
      {
        return(this->data.parse_tree->get_tag());
      }


    std::string fundamental::replace_model(const std::vector<std::string>& args)
      {
        return(this->data.parse_tree->get_model());
      }


    std::string fundamental::replace_header(const std::vector<std::string> &args)
      {
        return(this->data.file_out);
      }


    std::string fundamental::replace_core(const std::vector<std::string> &args)
      {
        return(this->data.core_out);
      }


    std::string fundamental::replace_number_fields(const std::vector<std::string> &args)
      {
        std::ostringstream out;

        out << this->data.parse_tree->get_number_fields();

        return(out.str());
      }


    std::string fundamental::replace_number_params(const std::vector<std::string> &args)
      {
        std::ostringstream out;

        out << this->data.parse_tree->get_number_params();

        return(out.str());
      }


    std::string fundamental::replace_field_list(const std::vector<std::string> &args)
      {
        std::vector<std::string> list = this->data.parse_tree->get_field_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_latex_list(const std::vector<std::string> &args)
      {
        std::vector<std::string> list = this->data.parse_tree->get_latex_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_param_list(const std::vector<std::string> &args)
      {
        std::vector<std::string> list = this->data.parse_tree->get_param_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_platx_list(const std::vector<std::string> &args)
      {
        std::vector<std::string> list = this->data.parse_tree->get_platx_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_state_list(const std::vector<std::string> &args)
      {
        std::vector<GiNaC::symbol> f_list = this->data.parse_tree->get_field_symbols();
        std::vector<GiNaC::symbol> d_list = this->data.parse_tree->get_deriv_symbols();

        std::vector<std::string> list;
        for(int i = 0; i < f_list.size(); i++)
          {
            list.push_back(f_list[i].get_name());
          }
        for(int i = 0; i < d_list.size(); i++)
          {
            list.push_back(d_list[i].get_name());
          }

        return(this->stringize_list(list));
      }


     std::string fundamental::replace_b_abs_err(const std::vector<std::string> &args)
      {
        const struct stepper s = this->data.parse_tree->get_background_stepper();

        return(this->stringize_number(s.abserr));
      }


    std::string fundamental::replace_b_rel_err(const std::vector<std::string> &args)
      {
        const struct stepper s = this->data.parse_tree->get_background_stepper();

        return(this->stringize_number(s.relerr));
      }


    std::string fundamental::replace_b_step(const std::vector<std::string> &args)
      {
        const struct stepper s = this->data.parse_tree->get_background_stepper();

        return(this->stringize_number(s.stepsize));
      }


    std::string fundamental::replace_b_stepper(const std::vector<std::string> &args)
      {
        const struct stepper s = this->data.parse_tree->get_background_stepper();

        return(s.name);
      }


    std::string fundamental::replace_p_abs_err(const std::vector<std::string> &args)
      {
        const struct stepper s = this->data.parse_tree->get_perturbations_stepper();

        return(this->stringize_number(s.abserr));
      }


    std::string fundamental::replace_p_rel_err(const std::vector<std::string> &args)
      {
        const struct stepper s = this->data.parse_tree->get_perturbations_stepper();

        return(this->stringize_number(s.relerr));
      }


    std::string fundamental::replace_p_step(const std::vector<std::string> &args)
      {
        const struct stepper s = this->data.parse_tree->get_perturbations_stepper();

        return(this->stringize_number(s.stepsize));
      }


    std::string fundamental::replace_p_stepper(const std::vector<std::string>& args)
      {
        const struct stepper s = this->data.parse_tree->get_perturbations_stepper();

        return(s.name);
      }


    // POST macros


    std::string fundamental::replace_unique(const std::vector<std::string> &args)
      {
        return(this->stringize_number(this->unique++));
      }


  } // namespace macro_packages


