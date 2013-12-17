//
// Created by David Seery on 17/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __macropackage_summation_H_
#define __macropackage_summation_H_


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class summation: public ::macro_packages::replacement_rule_package
      {
      public:
        summation(translation_unit* u, language_printer& p)
          : ::macro_packages::replacement_rule_package(u, p)
          {
          }

        const std::vector<macro_packages::simple_rule> get_pre_rules();
        const std::vector<macro_packages::simple_rule> get_post_rules();
        const std::vector<macro_packages::index_rule>  get_index_rules();

      protected:
        std::string null(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state);
      };

  }



#endif //__macropackage_summation_H_
