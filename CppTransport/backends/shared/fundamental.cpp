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

    constexpr unsigned int TOOL_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int VERSION_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int GUARD_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DATE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int SOURCE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int NAME_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int AUTHOR_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int TAG_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int MODEL_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int UNIQUE_ID_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int HEADER_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int CORE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int NUMBER_FIELDS_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int NUMBER_PARAMS_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int FIELD_LIST_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int LATEX_LIST_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PARAM_LIST_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PLATX_LIST_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int STATE_LIST_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int BACKG_ABS_ERR_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int BACKG_REL_ERR_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int BACKG_STEP_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int BACKG_NAME_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PERT_ABS_ERR_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PERT_REL_ERR_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PERT_STEP_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PERT_NAME_TOTAL_ARGUMENTS = 0;


    const std::vector<simple_rule> fundamental::get_pre_rules()
      {
        std::vector<simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
          { BIND(replace_tool) ,            BIND(replace_version),          BIND(replace_guard),            BIND(replace_date),             BIND(replace_source),
            BIND(replace_name),             BIND(replace_author),           BIND(replace_tag),              BIND(replace_model),            BIND(replace_uid),
            BIND(replace_header),           BIND(replace_core),             BIND(replace_number_fields),    BIND(replace_number_params),
            BIND(replace_field_list),       BIND(replace_latex_list),       BIND(replace_param_list),       BIND(replace_platx_list),       BIND(replace_state_list),
            BIND(replace_b_abs_err),        BIND(replace_b_rel_err),        BIND(replace_b_step),           BIND(replace_b_stepper),
            BIND(replace_p_abs_err),        BIND(replace_p_rel_err),        BIND(replace_p_step),           BIND(replace_p_stepper)
          };

        const macro_argument_list names =
          { "TOOL",                         "VERSION",                      "GUARD",                        "DATE",                         "SOURCE",
            "NAME",                         "AUTHOR",                       "TAG",                          "MODEL",                        "UNIQUE_ID",
            "HEADER",                       "CORE",                         "NUMBER_FIELDS",                "NUMBER_PARAMS",
            "FIELD_NAME_LIST",              "LATEX_NAME_LIST",              "PARAM_NAME_LIST",              "PLATX_NAME_LIST",              "STATE_NAME_LIST",
            "BACKG_ABS_ERR",                "BACKG_REL_ERR",                "BACKG_STEP_SIZE",              "BACKG_STEPPER",
            "PERT_ABS_ERR",                 "PERT_REL_ERR",                 "PERT_STEP_SIZE",               "PERT_STEPPER"
          };

        const std::vector<unsigned int> args =
          { TOOL_TOTAL_ARGUMENTS,           VERSION_TOTAL_ARGUMENTS,        GUARD_TOTAL_ARGUMENTS,          DATE_TOTAL_ARGUMENTS,           SOURCE_TOTAL_ARGUMENTS,
            NAME_TOTAL_ARGUMENTS,           AUTHOR_TOTAL_ARGUMENTS,         TAG_TOTAL_ARGUMENTS,            MODEL_TOTAL_ARGUMENTS,          UNIQUE_ID_TOTAL_ARGUMENTS,
            HEADER_TOTAL_ARGUMENTS,         CORE_TOTAL_ARGUMENTS,           NUMBER_FIELDS_TOTAL_ARGUMENTS,  NUMBER_PARAMS_TOTAL_ARGUMENTS,
            FIELD_LIST_TOTAL_ARGUMENTS,     LATEX_LIST_TOTAL_ARGUMENTS,     PARAM_LIST_TOTAL_ARGUMENTS,     PLATX_LIST_TOTAL_ARGUMENTS,     STATE_LIST_TOTAL_ARGUMENTS,
            BACKG_ABS_ERR_TOTAL_ARGUMENTS,  BACKG_REL_ERR_TOTAL_ARGUMENTS,  BACKG_STEP_TOTAL_ARGUMENTS,     BACKG_NAME_TOTAL_ARGUMENTS,
            PERT_ABS_ERR_TOTAL_ARGUMENTS,   PERT_REL_ERR_TOTAL_ARGUMENTS,   PERT_STEP_TOTAL_ARGUMENTS,      PERT_NAME_TOTAL_ARGUMENTS
          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); ++i)
          {
            package.emplace_back(names[i], rules[i], args[i]);
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
            package.emplace_back(names[i], rules[i], args[i]);
          }

        return(package);
      }


    const std::vector<index_rule> fundamental::get_index_rules()
      {
        std::vector<index_rule> package;

        return(package);
      }


    // *******************************************************************


    template <typename ListType>
    std::string fundamental::stringize_list(const ListType& list)
      {
        std::ostringstream out;

        out << this->list_start + this->list_pad;

        unsigned int i = 0;
        for(const std::string& item : list)
          {
            if(i > 0)
              {
                out << this->list_separator + this->list_pad;
              }

            out << to_printable(item);
            ++i;
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

    std::string fundamental::replace_tool(const macro_argument_list& args)
      {
        return(CPPTRANSPORT_NAME);
      }


    std::string fundamental::replace_version(const macro_argument_list& args)
      {
        return(CPPTRANSPORT_VERSION);
      }


    std::string fundamental::replace_guard(const macro_argument_list& args)
      {
        std::string guard;
        enum process_type type = this->data_payload.get_stack().top_process_type();

        if(type == process_core)
          {
            guard = this->data_payload.get_core_guard();
          }
        else if(type == process_implementation)
          {
            guard = this->data_payload.get_implementation_guard();
          }

        return(this->tag + "_" + guard + this->guard_terminator);
      }


    std::string fundamental::replace_date(const macro_argument_list& args)
      {
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        return(boost::posix_time::to_simple_string(now));
      }


    std::string fundamental::replace_source(const macro_argument_list& args)
      {
        return(this->data_payload.get_model_input().string());
      }


    std::string fundamental::replace_uid(const macro_argument_list& args)
      {
        boost::optional< contexted_value<std::string>& > nm_value = this->data_payload.get_name();
        std::string name = nm_value ? *nm_value : std::string();

        boost::optional< contexted_value<std::string>& > au_value = this->data_payload.get_author();
        std::string author = au_value ? *au_value : std::string();

        boost::optional< contexted_value<std::string>& > tg_value = this->data_payload.get_tag();
        std::string tag = tg_value ? *tg_value : std::string();

        boost::optional< contexted_value<std::string>& > md_value = this->data_payload.get_model();
        std::string model = md_value ? *md_value : std::string();

        std::string unique_string = name + author + tag + model + CPPTRANSPORT_VERSION;
        boost::uuids::string_generator gen;
        boost::uuids::uuid id = gen(unique_string);

        return(boost::uuids::to_string(id));
      }


    std::string fundamental::replace_name(const macro_argument_list& args)
      {
        boost::optional< contexted_value<std::string>& > value = this->data_payload.get_name();
        if(value)
          {
            return *value;
          }
        else
          {
            return(std::string(DEFAULT_MODEL_NAME));
          }
      }


    std::string fundamental::replace_author(const macro_argument_list& args)
      {
        boost::optional< contexted_value<std::string>& > value = this->data_payload.get_author();
        if(value)
          {
            return *value;
          }
        else
          {
            return(std::string(DEFAULT_AUTHOR));
          }
      }


    std::string fundamental::replace_tag(const macro_argument_list& args)
      {
        boost::optional< contexted_value<std::string>& > value = this->data_payload.get_tag();
        if(value)
          {
            return *value;
          }
        else
          {
            return(std::string());
          }
      }


    std::string fundamental::replace_model(const macro_argument_list& args)
      {
        boost::optional< contexted_value<std::string>& > value = this->data_payload.get_model();
        if(value)
          {
            return *value;
          }
        else
          {
            return(std::string(DEFAULT_MODEL_NAME));
          }
      }


    std::string fundamental::replace_header(const macro_argument_list& args)
      {
        return(this->data_payload.get_implementation_output().string());
      }


    std::string fundamental::replace_core(const macro_argument_list& args)
      {
        return(this->data_payload.get_core_output().string());
      }


    std::string fundamental::replace_number_fields(const macro_argument_list& args)
      {
        std::ostringstream out;

        out << this->data_payload.get_number_fields();

        return(out.str());
      }


    std::string fundamental::replace_number_params(const macro_argument_list& args)
      {
        std::ostringstream out;

        out << this->data_payload.get_number_parameters();

        return(out.str());
      }


    std::string fundamental::replace_field_list(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_field_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_latex_list(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_latex_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_param_list(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_param_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_platx_list(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_platx_list();

        return(this->stringize_list(list));
      }


    std::string fundamental::replace_state_list(const macro_argument_list& args)
      {
        std::vector<GiNaC::symbol> f_list = this->data_payload.get_field_symbols();
        std::vector<GiNaC::symbol> d_list = this->data_payload.get_deriv_symbols();

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


     std::string fundamental::replace_b_abs_err(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return(this->stringize_number(s.abserr));
      }


    std::string fundamental::replace_b_rel_err(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return(this->stringize_number(s.relerr));
      }


    std::string fundamental::replace_b_step(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return(this->stringize_number(s.stepsize));
      }


    std::string fundamental::replace_b_stepper(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return(s.name);
      }


    std::string fundamental::replace_p_abs_err(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return(this->stringize_number(s.abserr));
      }


    std::string fundamental::replace_p_rel_err(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return(this->stringize_number(s.relerr));
      }


    std::string fundamental::replace_p_step(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return(this->stringize_number(s.stepsize));
      }


    std::string fundamental::replace_p_stepper(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return(s.name);
      }


    // POST macros


    std::string fundamental::replace_unique(const macro_argument_list& args)
      {
        return(this->stringize_number(this->unique++));
      }


  } // namespace macro_packages


