//
// Created by David Seery on 04/12/2013.
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


#include <functional>
#include <time.h>

#include "fundamental.h"
#include "to_printable.h"
#include "translation_unit.h"
#include "formatter.h"
#include "core.h"

#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/string_generator.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "openssl/md5.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, p, prn))


namespace macro_packages
  {


    fundamental::fundamental(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, lm, p, prn)
      {
        pre_package.emplace_back(BIND(replace_tool, "TOOL"));
        pre_package.emplace_back(BIND(replace_version, "VERSION"));
        pre_package.emplace_back(BIND(replace_numeric_version, "NUMERIC_VERSION"));
        pre_package.emplace_back(BIND(replace_guard, "GUARD"));
        pre_package.emplace_back(BIND(replace_date, "DATE"));
        pre_package.emplace_back(BIND(replace_source, "SOURCE"));
        pre_package.emplace_back(BIND(replace_model, "MODEL"));
        pre_package.emplace_back(BIND(replace_name, "NAME"));
        pre_package.emplace_back(BIND(replace_author, "AUTHORS"));
        pre_package.emplace_back(BIND(replace_citeguide, "CITEGUIDE"));
        pre_package.emplace_back(BIND(replace_description, "DESCRIPTION"));
        pre_package.emplace_back(BIND(replace_license, "LICENSE"));
        pre_package.emplace_back(BIND(replace_revision, "REVISION"));
        pre_package.emplace_back(BIND(replace_references, "REFERENCES"));
        pre_package.emplace_back(BIND(replace_urls, "URLS"));
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
        return format_version(CPPTRANSPORT_NUMERIC_VERSION);
      }


      std::string replace_numeric_version::evaluate(const macro_argument_list& args)
        {
          return boost::lexical_cast<std::string>(CPPTRANSPORT_NUMERIC_VERSION);
        }


    std::string replace_guard::evaluate(const macro_argument_list& args)
      {
        std::string guard;
        process_type type = this->data_payload.get_stack().top_process_type();

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
        return(this->data_payload.get_source_filename().string());
      }


    std::string replace_uid::evaluate(const macro_argument_list& args)
      {
        std::ostringstream uid_str;

        auto md_value = this->data_payload.templates.get_model();
        if(md_value) uid_str << static_cast<std::string>(*md_value);

        auto nm_value = this->data_payload.meta.get_name();
        if(nm_value) uid_str << static_cast<std::string>(*nm_value);

        auto lc_value = this->data_payload.meta.get_license();
        if(lc_value) uid_str << static_cast<std::string>(*lc_value);
        
        auto rv_value = this->data_payload.meta.get_revision();
        if(rv_value) uid_str << static_cast<unsigned int>(*rv_value);

        // hash using MD5 so we are guaranteed to get the same result on all platforms
        unsigned char result[MD5_DIGEST_LENGTH];
        MD5(reinterpret_cast<const unsigned char*>(uid_str.str().c_str()), uid_str.str().length(), result);

        std::ostringstream id_str;
        for(unsigned int i = 0; i < MD5_DIGEST_LENGTH; ++i)
          {
            id_str << std::setw(2) << static_cast<int>(result[i]);
          }

        // convert hash to UUID-like format for convenience
        boost::uuids::string_generator gen;
        boost::uuids::uuid id = gen(id_str.str());

        return(boost::uuids::to_string(id));
      }


    std::string replace_name::evaluate(const macro_argument_list& args)
      {
        auto value = this->data_payload.meta.get_name();
        if(value)
          {
            return to_printable(*value);
          }
        else
          {
            return(std::string("\"\""));
          }
      }


    std::string replace_author::evaluate(const macro_argument_list& args)
      {
        std::string record = args[AUTHOR_RECORD_CLASS_ARGUMENT];

        // convert author table into a suitable initialization list
        const author_table& table = this->data_payload.meta.get_author();
        std::vector<std::string> init_list;
        for(const author_table::value_type& item : table)
          {
            const author_declaration& decl = *item.second;
            std::string ctor = record;

            size_t pos;
            std::string name = to_printable(decl.get_name());
            while((pos = ctor.find("$NAME")) != std::string::npos)
              {
                ctor.replace(pos, 5, name);
              }

            std::string email = to_printable(decl.get_email());
            while((pos = ctor.find("$EMAIL")) != std::string::npos)
              {
                ctor.replace(pos, 6, email);
              }

            std::string institute = to_printable(decl.get_institute());
            while((pos = ctor.find("$INSTITUTE")) != std::string::npos)
              {
                ctor.replace(pos, 10, institute);
              }

            init_list.push_back(ctor);
          }

        return this->printer.initialization_list(init_list, false);
      }


    std::string replace_citeguide::evaluate(const macro_argument_list& args)
      {
        auto value = this->data_payload.meta.get_citeguide();
        if(value)
          {
            return to_printable(*value);
          }
        else
          {
            return(std::string("\"\""));
          }
      }


    std::string replace_description::evaluate(const macro_argument_list& args)
      {
        auto value = this->data_payload.meta.get_description();
        if(value)
          {
            return to_printable(*value);
          }
        else
          {
            return(std::string("\"\""));
          }
      }


    std::string replace_license::evaluate(const macro_argument_list& args)
      {
        auto value = this->data_payload.meta.get_license();
        if(value)
          {
            return to_printable(*value);
          }
        else
          {
            return(std::string("\"\""));
          }
      }


    std::string replace_revision::evaluate(const macro_argument_list& args)
      {
        auto value = this->data_payload.meta.get_revision();
        if(value)
          {
            return boost::lexical_cast<std::string>(*value);
          }
        else
          {
            return(std::string("1"));
          }
      }


    std::string replace_references::evaluate(const macro_argument_list& args)
      {
        auto value = this->data_payload.meta.get_references();
        if(value)
          {
            std::vector<std::string> list;
            for(const contexted_value<std::string>& item : *value)
              {
                list.push_back(item);
              }
            return this->printer.initialization_list(list, true);
          }
        else
          {
            return(std::string("{}"));
          }
      }


    std::string replace_urls::evaluate(const macro_argument_list& args)
      {
        auto value = this->data_payload.meta.get_urls();
        if(value)
          {
            std::vector<std::string> list;
            for(const contexted_value<std::string>& item : *value)
              {
                list.push_back(item);
              }
            return this->printer.initialization_list(list, true);
          }
        else
          {
            return(std::string("{}"));
          }
      }


    std::string replace_model::evaluate(const macro_argument_list& args)
      {
        auto value = this->data_payload.templates.get_model();
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
        return(this->data_payload.get_implementation_filename().string());
      }


    std::string replace_core::evaluate(const macro_argument_list& args)
      {
        return(this->data_payload.get_core_filename().string());
      }


    std::string replace_number_fields::evaluate(const macro_argument_list& args)
      {
        std::ostringstream out;

        out << this->data_payload.model.get_number_fields();

        return(out.str());
      }


    std::string replace_number_params::evaluate(const macro_argument_list& args)
      {
        std::ostringstream out;

        out << this->data_payload.model.get_number_params();

        return(out.str());
      }


    std::string replace_field_list::evaluate(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.model.get_field_name_list();

        return this->printer.initialization_list(list, true);
      }


    std::string replace_latex_list::evaluate(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.model.get_field_latex_list();

        return this->printer.initialization_list(list, true);
      }


    std::string replace_param_list::evaluate(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.model.get_param_name_list();

        return this->printer.initialization_list(list, true);
      }


    std::string replace_platx_list::evaluate(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.model.get_param_latex_list();

        return this->printer.initialization_list(list, true);
      }


    std::string replace_state_list::evaluate(const macro_argument_list& args)
      {
        symbol_list f_list = this->data_payload.model.get_field_symbols();
        symbol_list d_list = this->data_payload.model.get_deriv_symbols();

        std::vector<std::string> list;

        for(int i = 0; i < f_list.size(); ++i)
          {
            list.push_back(f_list[i].get_name());
          }
        for(int i = 0; i < d_list.size(); ++i)
          {
            list.push_back(d_list[i].get_name());
          }

        return this->printer.initialization_list(list, true);
      }


    std::string replace_b_abs_err::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_background_stepper();

        if(s)
          {
            auto& step = **s;
            return boost::lexical_cast<std::string>(step.get_abserr());
          }
        else
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }
      }


    std::string replace_b_rel_err::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_background_stepper();

        if(s)
          {
            auto& step = **s;
            return boost::lexical_cast<std::string>(step.get_relerr());
          }
        else
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }
      }


    std::string replace_b_step::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_background_stepper();

        if(s)
          {
            auto& step = **s;
            return boost::lexical_cast<std::string>(step.get_stepsize());
          }
        else
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }
      }


    std::string replace_b_stepper::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_background_stepper();

        if(s)
          {
            auto& step = **s;
            return step.get_name();
          }
        else
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }
      }


    std::string replace_p_abs_err::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_perturbations_stepper();

        if(s)
          {
            auto& step = **s;
            return boost::lexical_cast<std::string>(step.get_abserr());
          }
        else
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }
      }


    std::string replace_p_rel_err::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_perturbations_stepper();

        if(s)
          {
            auto& step = **s;
            return boost::lexical_cast<std::string>(step.get_relerr());
          }
        else
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }
      }


    std::string replace_p_step::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_perturbations_stepper();

        if(s)
          {
            auto& step = **s;
            return boost::lexical_cast<std::string>(step.get_stepsize());
          }
        else
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }
      }


    std::string replace_p_stepper::evaluate(const macro_argument_list& args)
      {
        auto s = this->data_payload.templates.get_perturbations_stepper();

        if(s)
          {
            auto& step = **s;
            return step.get_name();
          }
        else
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }
      }


    // POST macros


    std::string replace_unique::evaluate(const macro_argument_list& args)
      {
        return std::to_string(this->unique++);
      }


  } // namespace macro_packages


