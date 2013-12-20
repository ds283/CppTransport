//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_utensors_H_
#define __macropackage_utensors_H_


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class utensors: public replacement_rule_package
      {
      public:
        utensors(translation_unit* u, language_printer& p,
                 std::string da=OUTPUT_DEFAULT_A_NAME, std::string dHsq=OUTPUT_DEFAULT_HSQ_NAME,
                 std::string deps=OUTPUT_DEFAULT_EPS_NAME,
                 std::string dk=OUTPUT_DEFAULT_K_NAME,
                 std::string dk1=OUTPUT_DEFAULT_K1_NAME, std::string dk2=OUTPUT_DEFAULT_K2_NAME, std::string dk3=OUTPUT_DEFAULT_K3_NAME)
          : default_a(da), default_Hsq(dHsq), default_eps(deps),
            default_k(dk), default_k1(dk1), default_k2(dk2), default_k3(dk3),
            replacement_rule_package(u, p)
        {
        }

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();

      protected:
        std::string default_a;
        std::string default_Hsq;
        std::string default_eps;
        std::string default_k;
        std::string default_k1;
        std::string default_k2;
        std::string default_k3;

        void* pre_u1_tensor(const std::vector<std::string> &args);
        void *pre_u2_tensor(const std::vector<std::string> &args);
        void *pre_u3_tensor(const std::vector<std::string> &args);
        void *pre_u1_predef(const std::vector<std::string> &args);
        void *pre_u2_predef(const std::vector<std::string> &args);
        void *pre_u3_predef(const std::vector<std::string> &args);

      };
  } // namespace macro_packages


#endif //__macropackage_utensors_H_
