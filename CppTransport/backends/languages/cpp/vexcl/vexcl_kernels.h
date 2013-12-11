//
// Created by David Seery on 08/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_vexcl_opencl_kernels_H_
#define __macropackage_vexcl_opencl_kernels_H_


#include "replacement_rule_package.h"


namespace cpp
  {

    class vexcl_kernels : public ::macro_packages::replacement_rule_package
      {
      public:
        vexcl_kernels(translation_unit* u, language_printer& p)
          : ::macro_packages::replacement_rule_package(u, p)
          {
          }

        const std::vector<macro_packages::simple_rule> get_pre_rules  ();
        const std::vector<macro_packages::simple_rule> get_post_rules ();
        const std::vector<macro_packages::index_rule>  get_index_rules();

      protected:
        std::string import_kernel(const std::vector<std::string>& args);
      };

  } // namespace cpp


#endif //__vexcl_opencl_kernels_H_
