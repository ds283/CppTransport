//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __macropackage_shared_kernelargs_macros_H_
#define __macropackage_shared_kernelargs_macros_H_


#include "replacement_rule_package.h"


namespace shared
  {

    class kernel_argument_macros : public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        kernel_argument_macros(translator_data& p, language_printer& prn, std::string q="", std::string l=OUTPUT_OPENCL_DEFAULT_LABEL);

        //! destructor is default
        virtual ~kernel_argument_macros() = default;


        // INTERFACE

      public:

        const std::vector<macro_packages::simple_rule> get_pre_rules();
        const std::vector<macro_packages::simple_rule> get_post_rules();
        const std::vector<macro_packages::index_rule>  get_index_rules();


        // INTERNAL API

      protected:

        std::string args_params(const macro_argument_list& args);

        std::string args_1index(const macro_argument_list& args);
        std::string args_2index(const macro_argument_list& args);
        std::string args_3index(const macro_argument_list& args);


        // INTERNAL DATA

      protected:

        std::string qualifier;
        std::string label;

      };

  } // namespace shared


#endif //__macropackage_shared_kernelargs_macros_H_
