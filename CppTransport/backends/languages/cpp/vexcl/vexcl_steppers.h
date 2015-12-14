//
// Created by David Seery on 06/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __macropackage_vexcl_steppers_H_
#define __macropackage_vexcl_steppers_H_


#include "replacement_rule_package.h"
#include "stepper.h"


namespace cpp
  {

    class vexcl_steppers: public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        vexcl_steppers(u_tensor_factory& uf, flattener& f, cse& cw, translator_data& p, language_printer& prn)
          : ::macro_packages::replacement_rule_package(uf, f, cw, p, prn)
          {
          }

        //! destructor
        virtual ~vexcl_steppers() = default;


        // INTERFACE

      public:

        const std::vector<macro_packages::simple_rule> get_pre_rules();
        const std::vector<macro_packages::simple_rule> get_post_rules();
        const std::vector<macro_packages::index_rule>  get_index_rules();


        // INTERFACE

      protected:

        std::string replace_backg_stepper(const std::vector<std::string>& args);
        std::string replace_pert_stepper (const std::vector<std::string>& args);
        std::string stepper_name         (const std::vector<std::string>& args);

      };

  } // namespace cpp


#endif //__macropackage_vexcl_steppers_H_
