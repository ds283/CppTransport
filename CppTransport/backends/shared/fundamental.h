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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_tool() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(TOOL_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_version() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(VERSION_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn),
            tag(t),
            guard_terminator(ge)
          {
          }

        //! destructor
        virtual ~replace_guard() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(GUARD_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_date() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(DATE_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_source() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(SOURCE_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_uid() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(UUID_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_name() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(NAME_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_author() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(AUTHOR_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_tag() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(TAG_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_model() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(MODEL_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_header() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(HEADER_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_core() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(CORE_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_number_fields() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(NUMBER_FIELDS_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_number_params() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(NUMBER_PARAMS_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_field_list() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(FIELD_LIST_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_latex_list() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(LATEX_LIST_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_param_list() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(PARAM_LIST_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_platx_list() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(PLATX_LIST_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_state_list() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(STATE_LIST_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_abs_err() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(BACKG_ABS_ERR_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_rel_err() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(BACKG_REL_ERR_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_step() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(BACKG_STEP_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_stepper() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(BACKG_NAME_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_abs_err() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(PERT_ABS_ERR_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_rel_err() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(PERT_REL_ERR_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_step() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(PERT_STEP_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_stepper() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(PERT_NAME_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            data_payload(p),
            printer(prn),
            unique(0)
          {
          }

        //! destructor
        virtual ~replace_unique() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(UNIQUE_TOTAL_ARGUMENTS); }


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
