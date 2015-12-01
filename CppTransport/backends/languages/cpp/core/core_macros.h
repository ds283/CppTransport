//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __macropackage_cpp_macros_H_
#define __macropackage_cpp_macros_H_


#include "replacement_rule_package.h"
#include "stepper.h"


namespace cpp
  {

    class core_macros: public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        core_macros(translator_data& p, language_printer& prn,
                   std::string ds=OUTPUT_DEFAULT_STEPPER_STATE_NAME)
          : ::macro_packages::replacement_rule_package(p, prn),
            default_state(ds)
          {
          }

        //! destructor is default
        virtual ~core_macros() = default;


        // INTERFACE

      public:

        const std::vector<macro_packages::simple_rule> get_pre_rules();
        const std::vector<macro_packages::simple_rule> get_post_rules();
        const std::vector<macro_packages::index_rule>  get_index_rules();


        // INTERNAL API

      protected:

        std::string replace_stepper      (const struct stepper& s, std::string state_name);

        std::string replace_backg_stepper(const std::vector<std::string>& args);
        std::string replace_pert_stepper (const std::vector<std::string>& args);


        // INTERNAL DATA

      protected:

        std::string default_state;

      };

  } // namespace cpp



#endif //__macropackage_cpp_macros_H_
