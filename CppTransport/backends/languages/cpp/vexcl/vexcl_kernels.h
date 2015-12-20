//
// Created by David Seery on 08/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __macropackage_vexcl_opencl_kernels_H_
#define __macropackage_vexcl_opencl_kernels_H_


#include "replacement_rule_package.h"


namespace vexcl
  {

    class vexcl_kernels : public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        vexcl_kernels(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
          : ::macro_packages::replacement_rule_package(f, cw, p, prn)
          {
          }

        //! destructor is default
        virtual ~vexcl_kernels() = default;


        // INTERFACE

      public:

        const std::vector<macro_packages::simple_rule> get_pre_rules  ();
        const std::vector<macro_packages::simple_rule> get_post_rules ();
        const std::vector<macro_packages::index_rule>  get_index_rules();


        // INTERNAL API

      protected:

        std::string import_kernel(const macro_argument_list& args);

      };

  } // namespace cpp


#endif //__vexcl_opencl_kernels_H_
