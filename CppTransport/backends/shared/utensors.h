//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __macropackage_utensors_H_
#define __macropackage_utensors_H_


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class utensors: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        utensors(translator_data& p, language_printer& prn,
                 std::string da=OUTPUT_DEFAULT_A_NAME, std::string dHsq=OUTPUT_DEFAULT_HSQ_NAME,
                 std::string deps=OUTPUT_DEFAULT_EPS_NAME,
                 std::string dk=OUTPUT_DEFAULT_K_NAME,
                 std::string dk1=OUTPUT_DEFAULT_K1_NAME, std::string dk2=OUTPUT_DEFAULT_K2_NAME, std::string dk3=OUTPUT_DEFAULT_K3_NAME)
          : default_a(da),
            default_Hsq(dHsq),
            default_eps(deps),
            default_k(dk),
            default_k1(dk1),
            default_k2(dk2),
            default_k3(dk3),
            replacement_rule_package(p, prn)
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

        void* pre_u1_tensor(const macro_argument_list& args);
        void *pre_u2_tensor(const macro_argument_list& args);
        void *pre_u3_tensor(const macro_argument_list& args);
        void *pre_u1_predef(const macro_argument_list& args);
        void *pre_u2_predef(const macro_argument_list& args);
        void *pre_u3_predef(const macro_argument_list& args);


        // INTERNAL DATA

      protected:

        std::string default_a;
        std::string default_Hsq;
        std::string default_eps;
        std::string default_k;
        std::string default_k1;
        std::string default_k2;
        std::string default_k3;

      };
  } // namespace macro_packages


#endif //__macropackage_utensors_H_
