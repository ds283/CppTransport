//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __macropackage_opencl_macros_H_
#define __macropackage_opencl_macros_H_


#include "replacement_rule_package.h"


namespace opencl
  {

    class opencl_macros: public ::macro_packages::replacement_rule_package
      {
      public:
        opencl_macros(translation_unit* u, language_printer& p, std::string l= OUTPUT_OPENCL_DEFAULT_LABEL);

        const std::vector<macro_packages::simple_rule> get_pre_rules();
        const std::vector<macro_packages::simple_rule> get_post_rules();
        const std::vector<macro_packages::index_rule>  get_index_rules();

      protected:
        std::string param_args(const std::vector<std::string>& args);
        std::string coord_args(const std::vector<std::string>& args);
        std::string u2_args   (const std::vector<std::string>& args);

        std::string label;
      };

  } // namespace opencl


#endif //__macropackage_opencl_macros_H_
