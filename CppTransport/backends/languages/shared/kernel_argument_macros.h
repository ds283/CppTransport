//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MACROS_KERNEL_ARGUMENTS_H
#define CPPTRANSPORT_MACROS_KERNEL_ARGUMENTS_H


#include "replacement_rule_package.h"


namespace shared
  {

    constexpr unsigned int PARAM_ARGS_TOTAL_ARGUMENTS = 0;

    constexpr unsigned int COORD_ARGS_NAME_ARGUMENT = 0;
    constexpr unsigned int COORD_ARGS_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int U2_ARGS_NAME_ARGUMENT = 0;
    constexpr unsigned int U2_ARGS_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int TWOPF_ARGS_NAME_ARGUMENT = 0;
    constexpr unsigned int TWOPF_ARGS_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int U3_ARGS_NAME_ARGUMENT = 0;
    constexpr unsigned int U3_ARGS_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int THREEPF_ARGS_NAME_ARGUMENT = 0;
    constexpr unsigned int THREEPF_ARGS_TOTAL_ARGUMENTS = 1;


    class args_params : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        args_params(std::string n, translator_data& p, language_printer& prn, std::string q="")
          : ::macro_packages::replacement_rule_simple(std::move(n), PARAM_ARGS_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn),
            qualifier(q)
          {
          }

        //! destructor
        virtual ~args_params() = default;


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

        //! qualifier
        std::string qualifier;

      };


    class args_1index : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        args_1index(std::string n, translator_data& p, language_printer& prn, std::string q="")
          : ::macro_packages::replacement_rule_simple(std::move(n), COORD_ARGS_TOTAL_ARGUMENTS),    // pick representative number of args
            data_payload(p),
            printer(prn),
            qualifier(q)
          {
          }

        //! destructor
        virtual ~args_1index() = default;


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

        //! qualifier
        std::string qualifier;

      };


    class args_2index : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        args_2index(std::string n, translator_data& p, language_printer& prn, std::string q="")
          : ::macro_packages::replacement_rule_simple(std::move(n), TWOPF_ARGS_TOTAL_ARGUMENTS),        // pick representative number of args
            data_payload(p),
            printer(prn),
            qualifier(q)
          {
          }

        //! destructor
        virtual ~args_2index() = default;


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

        //! qualifier
        std::string qualifier;

      };


    class args_3index : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        args_3index(std::string n, translator_data& p, language_printer& prn, std::string q=OUTPUT_OPENCL_QUALIFIER)
          : ::macro_packages::replacement_rule_simple(std::move(n), THREEPF_ARGS_TOTAL_ARGUMENTS),          // pick representative number of args
            data_payload(p),
            printer(prn),
            qualifier(q)
          {
          }

        //! destructor
        virtual ~args_3index() = default;


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

        //! qualifier
        std::string qualifier;

      };


    class kernel_argument_macros : public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        kernel_argument_macros(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~kernel_argument_macros() = default;

      };

  } // namespace shared


#endif //CPPTRANSPORT_MACROS_KERNEL_ARGUMENTS_H
