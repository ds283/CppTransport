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


#define BIND(X) std::move(std::make_shared<X>(this->data_payload, this->printer))


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
    constexpr unsigned int UUID_TOTAL_ARGUMENTS = 0;
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
    constexpr unsigned int UNIQUE_TOTAL_ARGUMENTS = 0;


    fundamental::fundamental(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, p, prn)
      {
        pre_package.emplace_back("TOOL", BIND(replace_tool), TOOL_TOTAL_ARGUMENTS);
        pre_package.emplace_back("VERSION", BIND(replace_version), VERSION_TOTAL_ARGUMENTS);
        pre_package.emplace_back("GUARD", BIND(replace_guard), GUARD_TOTAL_ARGUMENTS);
        pre_package.emplace_back("DATE", BIND(replace_date), DATE_TOTAL_ARGUMENTS);
        pre_package.emplace_back("SOURCE", BIND(replace_source), SOURCE_TOTAL_ARGUMENTS);
        pre_package.emplace_back("NAME", BIND(replace_name), NAME_TOTAL_ARGUMENTS);
        pre_package.emplace_back("AUTHOR", BIND(replace_author), AUTHOR_TOTAL_ARGUMENTS);
        pre_package.emplace_back("TAG", BIND(replace_tag), TAG_TOTAL_ARGUMENTS);
        pre_package.emplace_back("MODEL", BIND(replace_model), MODEL_TOTAL_ARGUMENTS);
        pre_package.emplace_back("UNIQUE_ID", BIND(replace_uid), UUID_TOTAL_ARGUMENTS);
        pre_package.emplace_back("HEADER", BIND(replace_header), HEADER_TOTAL_ARGUMENTS);
        pre_package.emplace_back("CORE", BIND(replace_core), CORE_TOTAL_ARGUMENTS);
        pre_package.emplace_back("NUMBER_FIELDS", BIND(replace_number_fields), NUMBER_FIELDS_TOTAL_ARGUMENTS);
        pre_package.emplace_back("NUMBER_PARAMS", BIND(replace_number_params), NUMBER_PARAMS_TOTAL_ARGUMENTS);
        pre_package.emplace_back("FIELD_NAME_LIST", BIND(replace_field_list), FIELD_LIST_TOTAL_ARGUMENTS);
        pre_package.emplace_back("LATEX_NAME_LIST", BIND(replace_latex_list), LATEX_LIST_TOTAL_ARGUMENTS);
        pre_package.emplace_back("PARAM_NAME_LIST", BIND(replace_param_list), PARAM_LIST_TOTAL_ARGUMENTS);
        pre_package.emplace_back("PLATX_NAME_LIST", BIND(replace_platx_list), PLATX_LIST_TOTAL_ARGUMENTS);
        pre_package.emplace_back("STATE_NAME_LIST", BIND(replace_state_list), STATE_LIST_TOTAL_ARGUMENTS);
        pre_package.emplace_back("BACKG_ABS_ERR", BIND(replace_b_abs_err), BACKG_ABS_ERR_TOTAL_ARGUMENTS);
        pre_package.emplace_back("BACKG_REL_ERR", BIND(replace_b_rel_err), BACKG_REL_ERR_TOTAL_ARGUMENTS);
        pre_package.emplace_back("BACKG_STEP_SIZE", BIND(replace_b_step), BACKG_STEP_TOTAL_ARGUMENTS);
        pre_package.emplace_back("BACKG_STEPPER", BIND(replace_b_stepper), BACKG_NAME_TOTAL_ARGUMENTS);
        pre_package.emplace_back("PERT_ABS_ERR", BIND(replace_p_abs_err), PERT_ABS_ERR_TOTAL_ARGUMENTS);
        pre_package.emplace_back("PERT_REL_ERR", BIND(replace_p_rel_err), PERT_REL_ERR_TOTAL_ARGUMENTS);
        pre_package.emplace_back("PERT_STEP_SIZE", BIND(replace_p_step), PERT_STEP_TOTAL_ARGUMENTS);
        pre_package.emplace_back("PERT_STEPPER", BIND(replace_p_stepper), PERT_NAME_TOTAL_ARGUMENTS);

        post_package.emplace_back("UNIQUE", BIND(replace_unique), UNIQUE_TOTAL_ARGUMENTS);
      }


    const std::vector<index_rule> fundamental::get_index_rules()
      {
        std::vector<index_rule> package;

        return(package);
      }


    // *******************************************************************


    // REPLACEMENT RULES

    // PRE macros


    std::string replace_tool::operator()(const macro_argument_list& args)
      {
        return(CPPTRANSPORT_NAME);
      }


    std::string replace_version::operator()(const macro_argument_list& args)
      {
        return(CPPTRANSPORT_VERSION);
      }


    std::string replace_guard::operator()(const macro_argument_list& args)
      {
        std::string guard;
        enum process_type type = this->data_payload.get_stack().top_process_type();

        if(type == process_type::process_core)
          {
            guard = this->data_payload.get_core_guard();
          }
        else if(type == process_type::process_implementation)
          {
            guard = this->data_payload.get_implementation_guard();
          }

        return(this->tag + "_" + guard + this->guard_terminator);
      }


    std::string replace_date::operator()(const macro_argument_list& args)
      {
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        return(boost::posix_time::to_simple_string(now));
      }


    std::string replace_source::operator()(const macro_argument_list& args)
      {
        return(this->data_payload.get_model_input().string());
      }


    std::string replace_uid::operator()(const macro_argument_list& args)
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


    std::string replace_name::operator()(const macro_argument_list& args)
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


    std::string replace_author::operator()(const macro_argument_list& args)
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


    std::string replace_tag::operator()(const macro_argument_list& args)
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


    std::string replace_model::operator()(const macro_argument_list& args)
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


    std::string replace_header::operator()(const macro_argument_list& args)
      {
        return(this->data_payload.get_implementation_output().string());
      }


    std::string replace_core::operator()(const macro_argument_list& args)
      {
        return(this->data_payload.get_core_output().string());
      }


    std::string replace_number_fields::operator()(const macro_argument_list& args)
      {
        std::ostringstream out;

        out << this->data_payload.get_number_fields();

        return(out.str());
      }


    std::string replace_number_params::operator()(const macro_argument_list& args)
      {
        std::ostringstream out;

        out << this->data_payload.get_number_parameters();

        return(out.str());
      }


    std::string replace_field_list::operator()(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_field_list();

        return this->printer.initialization_list(list);
      }


    std::string replace_latex_list::operator()(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_latex_list();

        return this->printer.initialization_list(list);
      }


    std::string replace_param_list::operator()(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_param_list();

        return this->printer.initialization_list(list);
      }


    std::string replace_platx_list::operator()(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_platx_list();

        return this->printer.initialization_list(list);
      }


    std::string replace_state_list::operator()(const macro_argument_list& args)
      {
        symbol_list f_list = this->data_payload.get_field_symbols();
        symbol_list d_list = this->data_payload.get_deriv_symbols();

        std::vector<std::string> list;

        for(int i = 0; i < f_list.size(); ++i)
          {
            list.push_back(f_list[i].get_name());
          }
        for(int i = 0; i < d_list.size(); ++i)
          {
            list.push_back(d_list[i].get_name());
          }

        return this->printer.initialization_list(list);
      }


     std::string replace_b_abs_err::operator()(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return boost::lexical_cast<std::string>(s.abserr);
      }


    std::string replace_b_rel_err::operator()(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return boost::lexical_cast<std::string>(s.relerr);
      }


    std::string replace_b_step::operator()(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return boost::lexical_cast<std::string>(s.stepsize);
      }


    std::string replace_b_stepper::operator()(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return(s.name);
      }


    std::string replace_p_abs_err::operator()(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return boost::lexical_cast<std::string>(s.abserr);
      }


    std::string replace_p_rel_err::operator()(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return boost::lexical_cast<std::string>(s.relerr);
      }


    std::string replace_p_step::operator()(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return boost::lexical_cast<std::string>(s.stepsize);
      }


    std::string replace_p_stepper::operator()(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return(s.name);
      }


    // POST macros


    std::string replace_unique::operator()(const macro_argument_list& args)
      {
        return boost::lexical_cast<std::string>(this->unique++);
      }


  } // namespace macro_packages


