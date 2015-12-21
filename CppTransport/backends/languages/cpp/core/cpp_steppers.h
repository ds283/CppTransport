//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MACROS_CPP_STEPPERS_H
#define CPPTRANSPORT_MACROS_CPP_STEPPERS_H


#include "replacement_rule_package.h"
#include "stepper.h"


namespace cpp
  {

    class replace_backg_stepper : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_backg_stepper(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_backg_stepper() = default;


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


    class replace_pert_stepper : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_pert_stepper(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_pert_stepper() = default;


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


    class cpp_steppers: public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        cpp_steppers(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~cpp_steppers() = default;


        // INTERFACE

      public:

        const std::vector<macro_packages::index_rule>  get_index_rules();


        // INTERNAL API

      protected:

      };

  } // namespace cpp


#endif //CPPTRANSPORT_MACROS_CPP_STEPPERS_H
