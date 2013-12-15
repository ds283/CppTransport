//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __macropackage_cuda_macros_H_
#define __macropackage_cuda_macros_H_


#include "replacement_rule_package.h"


namespace cuda
  {

    class cuda_macros: public ::macro_packages::replacement_rule_package
      {
      public:
        cuda_macros(translation_unit* u, language_printer& p, std::string l= OUTPUT_OPENCL_DEFAULT_LABEL);

        const std::vector<macro_packages::simple_rule> get_pre_rules();
        const std::vector<macro_packages::simple_rule> get_post_rules();
        const std::vector<macro_packages::index_rule>  get_index_rules();

      protected:
        std::string args_params(const std::vector<std::string> &args);

        std::string args_1index(const std::vector<std::string>& args);
        std::string args_2index(const std::vector<std::string>& args);
        std::string args_3index(const std::vector<std::string>& args);

        std::string label;
      };

  } // namespace cuda


#endif //__macropackage_cuda_macros_H_
