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

    class replace_tool : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_tool(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_tool() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_version(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_version() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_guard(translator_data& p, language_printer& prn,
                      std::string t = OUTPUT_CPPTRANSPORT_TAG,
                      std::string ge = "_H")
          : data_payload(p),
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
        replace_date(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_date() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_source(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_source() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_uid(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_uid() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_name(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_name() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_author(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_author() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_tag(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_tag() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_model(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_model() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_header(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_header() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_core(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_core() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_number_fields(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_number_fields() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_number_params(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_number_params() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_field_list(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_field_list() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_latex_list(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_latex_list() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_param_list(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_param_list() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_platx_list(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_platx_list() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_state_list(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_state_list() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_b_abs_err(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_abs_err() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_b_rel_err(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_rel_err() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_b_step(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_step() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_b_stepper(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_b_stepper() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_p_abs_err(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_abs_err() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_p_rel_err(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_rel_err() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_p_step(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_step() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_p_stepper(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_p_stepper() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        replace_unique(translator_data& p, language_printer& prn)
          : data_payload(p),
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
