//
// Created by David Seery on 06/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef CPPTRANSPORT_MACROS_VEXCL_STEPPERS_H
#define CPPTRANSPORT_MACROS_VEXCL_STEPPERS_H


#include "replacement_rule_package.h"
#include "stepper.h"


namespace vexcl
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


    class stepper_name : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        stepper_name(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~stepper_name() = default;


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


    class vexcl_steppers: public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        vexcl_steppers(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor
        virtual ~vexcl_steppers() = default;


        // INTERFACE

      public:

        const std::vector<macro_packages::index_rule>  get_index_rules();

      };

  } // namespace cpp


#endif //CPPTRANSPORT_MACROS_VEXCL_STEPPERS_H
