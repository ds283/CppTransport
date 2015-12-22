//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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


    class replace_tag : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_tag(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_simple(std::move(n), TAG_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_tag() = default;


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
        fundamental(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~fundamental() = default;


        // INTERFACE

      public:

        //! return index- macros package
        const std::vector<index_rule>  get_index_rules() override;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROS_FUNDAMENTAL_H
