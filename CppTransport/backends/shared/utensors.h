//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MACROS_UTENSORS_H
#define CPPTRANSPORT_MACROS_UTENSORS_H


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class utensors: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        utensors(u_tensor_factory& uf, cse& cw, translator_data& p, language_printer& prn)
          : replacement_rule_package(uf, cw, p, prn)
        {
        }

        //! destructor
        virtual ~utensors() = default;


        // INTERFACE

      public:

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();


        // INTERNAL API

      protected:
    
        std::unique_ptr<cse_map> pre_u1_tensor(const macro_argument_list& args);
    
        std::unique_ptr<cse_map> pre_u2_tensor(const macro_argument_list& args);
    
        std::unique_ptr<cse_map> pre_u3_tensor(const macro_argument_list& args);
    
        std::unique_ptr<cse_map> pre_u1_predef(const macro_argument_list& args);
    
        std::unique_ptr<cse_map> pre_u2_predef(const macro_argument_list& args);
    
        std::unique_ptr<cse_map> pre_u3_predef(const macro_argument_list& args);

      };
  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROS_UTENSORS_H
