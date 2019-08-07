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



#ifndef CPPTRANSPORT_MACROS_FUNDAMENTAL_H
#define CPPTRANSPORT_MACROS_FUNDAMENTAL_H


#include <string>

#include "msg_en.h"
#include "replacement_rule_package.h"


namespace macro_packages
  {

    constexpr unsigned int TOOL_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int VERSION_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int NUMERIC_VERSION_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int GUARD_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DATE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int SOURCE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int NAME_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int AUTHOR_RECORD_CLASS_ARGUMENT = 0;
    constexpr unsigned int AUTHOR_TOTAL_ARGUMENTS = 1;
    constexpr unsigned int CITEGUIDE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DESCRIPTION_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int LICENSE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int REVISION_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int REFERENCES_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int URLS_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int MODEL_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int UUID_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int HEADER_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int CORE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int SAMPLE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int VALUES_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PRIORS_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int MCMC_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int NUMBER_FIELDS_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int NUMBER_PARAMS_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int FIELD_LIST_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int CPPTSAMPLE_FIELD_LIST_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int CPPTSAMPLE_FIELD_LIST_INIT_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PARAMS_LIST_TOTAL_ARGUMENTS = 0;
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
    constexpr unsigned int CPPT_BUILD_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int UNIQUE_TOTAL_ARGUMENTS = 0;


    class replace_tool : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_tool(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), TOOL_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_tool() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_version : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_version(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), VERSION_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_version() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


      class replace_numeric_version : public replacement_rule_simple
        {

          // CONSTRUCTOR, DESTRUCTOR

        public:

          //! constructor
          replace_numeric_version(std::string n, translator_data& p, language_printer& prn)
            : replacement_rule_simple(std::move(n), NUMERIC_VERSION_TOTAL_ARGUMENTS),
              data_payload(p),
              printer(prn)
            {
            }

          //! destructor
          virtual ~replace_numeric_version() = default;


          // INTERNAL API

        protected:

          //! evaluate
          virtual std::string evaluate(const macro_argument_list& args) override;


          // INTERNAL DATA

        private:

          //! data payload
          translator_data& data_payload;

          //! language printer
          language_printer& printer;

        };


    class replace_guard : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_guard(std::string n, translator_data& p, language_printer& prn,
                      std::string t = OUTPUT_CPPTRANSPORT_TAG,
                      std::string ge = "_H")
          : replacement_rule_simple(std::move(n), GUARD_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn),
            tag(t),
            guard_terminator(ge)
          {
          }

        //! destructor
        virtual ~replace_guard() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

        //! tag
        std::string tag;

        //! guard terminator
        std::string guard_terminator;

      };


    class replace_date : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_date(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), DATE_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_date() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_source : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_source(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), SOURCE_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_source() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_uid : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_uid(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), UUID_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
            policy_hex_uuid = this->data_payload.get_version_policy().adjudicate_policy(POLICY_HEX_UUID);
          }

        //! destructor
        virtual ~replace_uid() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;
        
        //! apply HEX_UUID policy?
        bool policy_hex_uuid;

      };


    class replace_name : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_name(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), NAME_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_name() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_author : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_author(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), AUTHOR_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_author() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_citeguide : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_citeguide(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), CITEGUIDE_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_citeguide() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_description : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_description(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), DESCRIPTION_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_description() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_license : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_license(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), LICENSE_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_license() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_revision : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_revision(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), REVISION_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_revision() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_references : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_references(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), REFERENCES_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_references() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_urls : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_urls(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), URLS_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_urls() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_model : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_model(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), MODEL_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_model() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_header : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_header(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), HEADER_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_header() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_core : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_core(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), CORE_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_core() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };

    class replace_sample : public replacement_rule_simple
    {
        // CONSTRUCTOR, DESTRUCTOR
    public:
        //! constructor
        replace_sample(std::string n, translator_data& p, language_printer& prn)
                : replacement_rule_simple(std::move(n), SAMPLE_TOTAL_ARGUMENTS),
                  data_payload(p),
                  printer(prn)
        {
        }
        //! destructor
        virtual ~replace_sample() = default;

        // INTERNAL API
    protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;

        // INTERNAL DATA
    private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;
    };

    class replace_values : public replacement_rule_simple
    {
        // CONSTRUCTOR, DESTRUCTOR
    public:
        //! constructor
        replace_values(std::string n, translator_data& p, language_printer& prn)
                : replacement_rule_simple(std::move(n), VALUES_TOTAL_ARGUMENTS),
                  data_payload(p),
                  printer(prn)
        {
        }
        //! destructor
        virtual ~replace_values() = default;

        // INTERNAL API
    protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;

        // INTERNAL DATA
    private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;
    };

    class replace_priors : public replacement_rule_simple
    {
        // CONSTRUCTOR, DESTRUCTOR
    public:
        //! constructor
        replace_priors(std::string n, translator_data& p, language_printer& prn)
                : replacement_rule_simple(std::move(n), PRIORS_TOTAL_ARGUMENTS),
                  data_payload(p),
                  printer(prn)
        {
        }
        //! destructor
        virtual ~replace_priors() = default;

        // INTERNAL API
    protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;

        // INTERNAL DATA
    private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;
    };

    class replace_mcmc : public replacement_rule_simple
    {
        // CONSTRUCTOR, DESTRUCTOR
    public:
        //! constructor
        replace_mcmc(std::string n, translator_data& p, language_printer& prn)
                : replacement_rule_simple(std::move(n), MCMC_TOTAL_ARGUMENTS),
                  data_payload(p),
                  printer(prn)
        {
        }
        //! destructor
        virtual ~replace_mcmc() = default;

        // INTERNAL API
    protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;

        // INTERNAL DATA
    private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;
    };


    class replace_number_fields : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_number_fields(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), NUMBER_FIELDS_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_number_fields() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_number_params : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_number_params(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), NUMBER_PARAMS_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_number_params() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_field_list : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_field_list(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), FIELD_LIST_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_field_list() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };

    class replace_fields : public replacement_rule_simple
    {

        // CONSTRUCTOR, DESTRUCTOR

    public:

        //! constructor
        replace_fields(std::string n, translator_data& p, language_printer& prn)
                : replacement_rule_simple(std::move(n), CPPTSAMPLE_FIELD_LIST_TOTAL_ARGUMENTS),
                  data_payload(p),
                  printer(prn)
        {
        }

        //! destructor
        virtual ~replace_fields() = default;


        // INTERNAL API

    protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

    private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

    };

    class replace_fields_init : public replacement_rule_simple
    {

        // CONSTRUCTOR, DESTRUCTOR

    public:

        //! constructor
        replace_fields_init(std::string n, translator_data& p, language_printer& prn)
                : replacement_rule_simple(std::move(n), CPPTSAMPLE_FIELD_LIST_INIT_TOTAL_ARGUMENTS),
                  data_payload(p),
                  printer(prn)
        {
        }

        //! destructor
        virtual ~replace_fields_init() = default;


        // INTERNAL API

    protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

    private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

    };

    class replace_params : public replacement_rule_simple
    {

        // CONSTRUCTOR, DESTRUCTOR

    public:

        //! constructor
        replace_params(std::string n, translator_data& p, language_printer& prn)
                : replacement_rule_simple(std::move(n), PARAMS_LIST_TOTAL_ARGUMENTS),
                  data_payload(p),
                  printer(prn)
        {
        }

        //! destructor
        virtual ~replace_params() = default;


        // INTERNAL API

    protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

    private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

    };


    class replace_latex_list : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_latex_list(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), LATEX_LIST_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_latex_list() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_param_list : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_param_list(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), PARAM_LIST_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_param_list() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_platx_list : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_platx_list(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), PLATX_LIST_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_platx_list() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_state_list : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_state_list(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), STATE_LIST_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_state_list() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_b_abs_err : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_b_abs_err(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), BACKG_ABS_ERR_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_abs_err() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_b_rel_err : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_b_rel_err(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), BACKG_REL_ERR_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_rel_err() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_b_step : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_b_step(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), BACKG_STEP_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_step() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_b_stepper : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_b_stepper(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), BACKG_NAME_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_stepper() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_p_abs_err : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_p_abs_err(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), PERT_ABS_ERR_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_abs_err() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_p_rel_err : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_p_rel_err(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), PERT_REL_ERR_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_rel_err() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_p_step : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_p_step(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), PERT_STEP_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_step() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class replace_p_stepper : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_p_stepper(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), PERT_NAME_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_stepper() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };

    class replace_cppt_build_location : public replacement_rule_simple
    {

        // CONSTRUCTOR, DESTRUCTOR

    public:

        //! constructor
        replace_cppt_build_location(std::string n, translator_data& p, language_printer& prn)
                : replacement_rule_simple(std::move(n), CPPT_BUILD_TOTAL_ARGUMENTS),
                  data_payload(p),
                  printer(prn)
        {
        }

        //! destructor
        virtual ~replace_cppt_build_location() = default;


        // INTERNAL API

    protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

    private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

    };


    class replace_unique : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_unique(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), UNIQUE_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn),
            unique(0)
          {
          }

        //! destructor
        virtual ~replace_unique() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

        //! unique number
        unsigned int unique;

      };


    class fundamental: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        fundamental(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~fundamental() = default;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROS_FUNDAMENTAL_H
