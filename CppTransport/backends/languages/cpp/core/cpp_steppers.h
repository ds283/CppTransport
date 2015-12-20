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

    class cpp_steppers: public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        cpp_steppers(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
          : ::macro_packages::replacement_rule_package(f, cw, p, prn)
          {
          }

        //! destructor is default
        virtual ~cpp_steppers() = default;


        // INTERFACE

      public:

        const std::vector<macro_packages::simple_rule> get_pre_rules();
        const std::vector<macro_packages::simple_rule> get_post_rules();
        const std::vector<macro_packages::index_rule>  get_index_rules();


        // INTERNAL API

      protected:

        std::string replace_stepper      (const struct stepper& s, std::string state_name);

        std::string replace_backg_stepper(const macro_argument_list& args);
        std::string replace_pert_stepper (const macro_argument_list& args);

      };

  } // namespace cpp


#endif //CPPTRANSPORT_MACROS_CPP_STEPPERS_H
