//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MACROS_KERNEL_ARGUMENTS_H
#define CPPTRANSPORT_MACROS_KERNEL_ARGUMENTS_H


#include "replacement_rule_package.h"


namespace shared
  {

    class args_params : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        args_params(translator_data& p, language_printer& prn, std::string q="")
          : data_payload(p),
            printer(prn),
            qualifier(q)
          {
          }

        //! destructor
        virtual ~args_params() = default;


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

        //! qualifier
        std::string qualifier;

      };


    class args_1index : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        args_1index(translator_data& p, language_printer& prn, std::string q="")
          : data_payload(p),
            printer(prn),
            qualifier(q)
          {
          }

        //! destructor
        virtual ~args_1index() = default;


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

        //! qualifier
        std::string qualifier;

      };


    class args_2index : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        args_2index(translator_data& p, language_printer& prn, std::string q="")
          : data_payload(p),
            printer(prn),
            qualifier(q)
          {
          }

        //! destructor
        virtual ~args_2index() = default;


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

        //! qualifier
        std::string qualifier;

      };


    class args_3index : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        args_3index(translator_data& p, language_printer& prn, std::string q=OUTPUT_OPENCL_QUALIFIER)
          : data_payload(p),
            printer(prn),
            qualifier(q)
          {
          }

        //! destructor
        virtual ~args_3index() = default;


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

        //! qualifier
        std::string qualifier;

      };


    class kernel_argument_macros : public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        kernel_argument_macros(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~kernel_argument_macros() = default;


        // INTERFACE

      public:

        const std::vector<macro_packages::index_rule>  get_index_rules();

      };

  } // namespace shared


#endif //CPPTRANSPORT_MACROS_KERNEL_ARGUMENTS_H
