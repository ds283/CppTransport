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


#define BIND(X, N) std::move(std::make_unique<X>(N, p, prn))


namespace macro_packages
  {


    fundamental::fundamental(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, p, prn)
      {
        pre_package.emplace_back(BIND(replace_tool, "TOOL"));
        pre_package.emplace_back(BIND(replace_version, "VERSION"));
        pre_package.emplace_back(BIND(replace_guard, "GUARD"));
        pre_package.emplace_back(BIND(replace_date, "DATE"));
        pre_package.emplace_back(BIND(replace_source, "SOURCE"));
        pre_package.emplace_back(BIND(replace_name, "NAME"));
        pre_package.emplace_back(BIND(replace_author, "AUTHOR"));
        pre_package.emplace_back(BIND(replace_tag, "TAG"));
        pre_package.emplace_back(BIND(replace_model, "MODEL"));
        pre_package.emplace_back(BIND(replace_uid, "UNIQUE_ID"));
        pre_package.emplace_back(BIND(replace_header, "HEADER"));
        pre_package.emplace_back(BIND(replace_core, "CORE"));
        pre_package.emplace_back(BIND(replace_number_fields, "NUMBER_FIELDS"));
        pre_package.emplace_back(BIND(replace_number_params, "NUMBER_PARAMS"));
        pre_package.emplace_back(BIND(replace_field_list, "FIELD_NAME_LIST"));
        pre_package.emplace_back(BIND(replace_latex_list, "LATEX_NAME_LIST"));
        pre_package.emplace_back(BIND(replace_param_list, "PARAM_NAME_LIST"));
        pre_package.emplace_back(BIND(replace_platx_list, "PLATX_NAME_LIST"));
        pre_package.emplace_back(BIND(replace_state_list, "STATE_NAME_LIST"));
        pre_package.emplace_back(BIND(replace_b_abs_err, "BACKG_ABS_ERR"));
        pre_package.emplace_back(BIND(replace_b_rel_err, "BACKG_REL_ERR"));
        pre_package.emplace_back(BIND(replace_b_step, "BACKG_STEP_SIZE"));
        pre_package.emplace_back(BIND(replace_b_stepper, "BACKG_STEPPER"));
        pre_package.emplace_back(BIND(replace_p_abs_err, "PERT_ABS_ERR"));
        pre_package.emplace_back(BIND(replace_p_rel_err, "PERT_REL_ERR"));
        pre_package.emplace_back(BIND(replace_p_step, "PERT_STEP_SIZE"));
        pre_package.emplace_back(BIND(replace_p_stepper, "PERT_STEPPER"));

        post_package.emplace_back(BIND(replace_unique, "UNIQUE"));
      }


    // *******************************************************************


    // REPLACEMENT RULES

    // PRE macros


    std::string replace_tool::evaluate(const macro_argument_list& args)
      {
        return(CPPTRANSPORT_NAME);
      }


    std::string replace_version::evaluate(const macro_argument_list& args)
      {
        return(CPPTRANSPORT_VERSION);
      }


    std::string replace_guard::evaluate(const macro_argument_list& args)
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


    std::string replace_date::evaluate(const macro_argument_list& args)
      {
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        return(boost::posix_time::to_simple_string(now));
      }


    std::string replace_source::evaluate(const macro_argument_list& args)
      {
        return(this->data_payload.get_model_input().string());
      }


    std::string replace_uid::evaluate(const macro_argument_list& args)
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


    std::string replace_name::evaluate(const macro_argument_list& args)
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


    std::string replace_author::evaluate(const macro_argument_list& args)
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


    std::string replace_tag::evaluate(const macro_argument_list& args)
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


    std::string replace_model::evaluate(const macro_argument_list& args)
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


    std::string replace_header::evaluate(const macro_argument_list& args)
      {
        return(this->data_payload.get_implementation_output().string());
      }


    std::string replace_core::evaluate(const macro_argument_list& args)
      {
        return(this->data_payload.get_core_output().string());
      }


    std::string replace_number_fields::evaluate(const macro_argument_list& args)
      {
        std::ostringstream out;

        out << this->data_payload.get_number_fields();

        return(out.str());
      }


    std::string replace_number_params::evaluate(const macro_argument_list& args)
      {
        std::ostringstream out;

        out << this->data_payload.get_number_parameters();

        return(out.str());
      }


    std::string replace_field_list::evaluate(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_field_list();

        return this->printer.initialization_list(list);
      }


    std::string replace_latex_list::evaluate(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_latex_list();

        return this->printer.initialization_list(list);
      }


    std::string replace_param_list::evaluate(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_param_list();

        return this->printer.initialization_list(list);
      }


    std::string replace_platx_list::evaluate(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_platx_list();

        return this->printer.initialization_list(list);
      }


    std::string replace_state_list::evaluate(const macro_argument_list& args)
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


     std::string replace_b_abs_err::evaluate(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return boost::lexical_cast<std::string>(s.abserr);
      }


    std::string replace_b_rel_err::evaluate(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return boost::lexical_cast<std::string>(s.relerr);
      }


    std::string replace_b_step::evaluate(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return boost::lexical_cast<std::string>(s.stepsize);
      }


    std::string replace_b_stepper::evaluate(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_background_stepper();

        return(s.name);
      }


    std::string replace_p_abs_err::evaluate(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return boost::lexical_cast<std::string>(s.abserr);
      }


    std::string replace_p_rel_err::evaluate(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return boost::lexical_cast<std::string>(s.relerr);
      }


    std::string replace_p_step::evaluate(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return boost::lexical_cast<std::string>(s.stepsize);
      }


    std::string replace_p_stepper::evaluate(const macro_argument_list& args)
      {
        const struct stepper s = this->data_payload.get_perturbations_stepper();

        return(s.name);
      }


    // POST macros


    std::string replace_unique::evaluate(const macro_argument_list& args)
      {
        return boost::lexical_cast<std::string>(this->unique++);
      }


  } // namespace macro_packages


