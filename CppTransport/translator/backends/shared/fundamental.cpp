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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#include <functional>
#include <time.h>

#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/string_generator.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "fundamental.h"
#include "to_printable.h"
#include "translation_unit.h"
#include "formatter.h"
#include "core.h"

#include "openssl/md5.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, p, prn))
#define EMPLACE(pkg, obj) try { emplace_rule(pkg, obj); } catch(std::exception& xe) { }


namespace macro_packages
  {


    fundamental::fundamental(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, lm, p, prn)
      {
        EMPLACE(pre_package, BIND(replace_tool, "TOOL"));
        EMPLACE(pre_package, BIND(replace_version, "VERSION"));
        EMPLACE(pre_package, BIND(replace_numeric_version, "NUMERIC_VERSION"));
        EMPLACE(pre_package, BIND(replace_guard, "GUARD"));
        EMPLACE(pre_package, BIND(replace_date, "DATE"));
        EMPLACE(pre_package, BIND(replace_source, "SOURCE"));
        EMPLACE(pre_package, BIND(replace_model, "MODEL"));
        EMPLACE(pre_package, BIND(replace_name, "NAME"));
        EMPLACE(pre_package, BIND(replace_author, "AUTHORS"));
        EMPLACE(pre_package, BIND(replace_citeguide, "CITEGUIDE"));
        EMPLACE(pre_package, BIND(replace_description, "DESCRIPTION"));
        EMPLACE(pre_package, BIND(replace_license, "LICENSE"));
        EMPLACE(pre_package, BIND(replace_revision, "REVISION"));
        EMPLACE(pre_package, BIND(replace_references, "REFERENCES"));
        EMPLACE(pre_package, BIND(replace_urls, "URLS"));
        EMPLACE(pre_package, BIND(replace_uid, "UNIQUE_ID"));
        EMPLACE(pre_package, BIND(replace_header, "HEADER"));
        EMPLACE(pre_package, BIND(replace_core, "CORE"));
        EMPLACE(pre_package, BIND(replace_sample, "SAMPLE"));
        EMPLACE(pre_package, BIND(replace_values, "VALUES"));
        EMPLACE(pre_package, BIND(replace_priors, "PRIORS"));
        EMPLACE(pre_package, BIND(replace_mcmc, "MCMC"));
        EMPLACE(pre_package, BIND(replace_number_fields, "NUMBER_FIELDS"));
        EMPLACE(pre_package, BIND(replace_number_params, "NUMBER_PARAMS"));
        EMPLACE(pre_package, BIND(replace_field_list, "FIELD_NAME_LIST"));
        EMPLACE(pre_package, BIND(replace_fields, "FIELD_LIST"));
        EMPLACE(pre_package, BIND(replace_fields_init, "FIELD_LIST_INIT"));
        EMPLACE(pre_package, BIND(replace_params, "PARAM_LIST"));
        EMPLACE(pre_package, BIND(replace_latex_list, "LATEX_NAME_LIST"));
        EMPLACE(pre_package, BIND(replace_param_list, "PARAM_NAME_LIST"));
        EMPLACE(pre_package, BIND(replace_platx_list, "PLATX_NAME_LIST"));
        EMPLACE(pre_package, BIND(replace_state_list, "STATE_NAME_LIST"));
        EMPLACE(pre_package, BIND(replace_b_abs_err, "BACKG_ABS_ERR"));
        EMPLACE(pre_package, BIND(replace_b_rel_err, "BACKG_REL_ERR"));
        EMPLACE(pre_package, BIND(replace_b_step, "BACKG_STEP_SIZE"));
        EMPLACE(pre_package, BIND(replace_b_stepper, "BACKG_STEPPER"));
        EMPLACE(pre_package, BIND(replace_p_abs_err, "PERT_ABS_ERR"));
        EMPLACE(pre_package, BIND(replace_p_rel_err, "PERT_REL_ERR"));
        EMPLACE(pre_package, BIND(replace_p_step, "PERT_STEP_SIZE"));
        EMPLACE(pre_package, BIND(replace_p_stepper, "PERT_STEPPER"));
        EMPLACE(pre_package, BIND(replace_cppt_build_location, "CPPT_BUILD_LOCATION"));

        EMPLACE(post_package, BIND(replace_unique, "UNIQUE"));
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
        else if(type == process_type::process_sampling)
        {
          guard = this->data_payload.get_sampling_guard();
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
            // POLICY: POLICY_HEX_UUID corrects omission of std::hex in versions prior to 201801
            if(this->policy_hex_uuid)
              {
                id_str << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(result[i]);
              }
            else
              {
                id_str << std::setw(2) << static_cast<int>(result[i]);
              }
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


    std::string replace_sample::evaluate(const macro_argument_list& args)
    {
      return(this->data_payload.get_sampling_filename().string());
    }

    std::string replace_values::evaluate(const macro_argument_list& args)
    {
      return(this->data_payload.get_sampling_values_filename().string());
    }

    std::string replace_priors::evaluate(const macro_argument_list& args)
    {
      return(this->data_payload.get_sampling_priors_filename().string());
    }

    std::string replace_mcmc::evaluate(const macro_argument_list& args)
    {
      return(this->data_payload.get_sampling_mcmc_filename().string());
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

    std::string replace_fields::evaluate(const macro_argument_list& args)
    {
      symbol_list f_list = this->data_payload.model.get_field_symbols();
      //symbol_list d_list = this->data_payload.model.get_deriv_symbols();
      symbol_list d_list = this->data_payload.model.get_field_deriv();

      std::vector<std::string> list;

      for(int i = 0; i < f_list.size(); ++i)
      {
        list.push_back(f_list[i].get_name());
      }
      for(int i = 0; i < d_list.size(); ++i)
      {
        list.push_back(d_list[i].get_name());
      }

      return this->printer.cpptsample_fields_list(list, false);
      //return this->printer.initialization_list(list, true);
    }

    std::string replace_fields_init::evaluate(const macro_argument_list& args)
    {
      symbol_list f_list = this->data_payload.model.get_field_symbols();
      //symbol_list d_list = this->data_payload.model.get_deriv_symbols();
      symbol_list d_list = this->data_payload.model.get_field_deriv();

      std::vector<std::string> list;

      for(int i = 0; i < f_list.size(); ++i)
      {
        list.push_back(f_list[i].get_name());
      }
      for(int i = 0; i < d_list.size(); ++i)
      {
        list.push_back(d_list[i].get_name());
      }

      return this->printer.cpptsample_fields_list(list, true);
      //return this->printer.initialization_list(list, true);
    }

    std::string replace_params::evaluate(const macro_argument_list& args)
    {
      std::vector<std::string> list = this->data_payload.model.get_param_name_list();

      return this->printer.cpptsample_fields_list(list, false);
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
            auto& step = ***s;
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
            auto& step = ***s;
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
            auto& step = ***s;
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
            auto& step = ***s;
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
            auto& step = ***s;
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
            auto& step = ***s;
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
            auto& step = ***s;
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
            auto& step = ***s;
            return step.get_name();
          }
        else
          {
            throw macro_packages::rule_apply_fail(ERROR_UNDEFINED_STEPPER);
          }
      }

    std::string replace_cppt_build_location::evaluate(const macro_argument_list& args)
    {
      // Try to find where we are reading in the CMakeLists.txt template file from and then go up to the source build directory
      finder PathFinder = this->data_payload.get_finder();
      boost::optional<boost::filesystem::path> TemplateInstallPath;
      TemplateInstallPath = PathFinder.find("cpptsample_CMakeLists_template.txt");
      if (TemplateInstallPath)
      {
        boost::filesystem::path CppTBuildPath =  (*TemplateInstallPath).parent_path().parent_path().parent_path().parent_path();
        return(CppTBuildPath.string());
      }
      else
      {
        // Could not find, return default location
        return(std::string("~/.cpptransport"));
      }
    }


    // POST macros


    std::string replace_unique::evaluate(const macro_argument_list& args)
      {
        return std::to_string(this->unique++);
      }


  } // namespace macro_packages


