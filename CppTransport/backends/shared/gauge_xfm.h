//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_gauge_xfm_H_
#define __macropackage_gauge_xfm_H_


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class gauge_xfm: public replacement_rule_package
      {
      public:
        gauge_xfm(replacement_data& d, ginac_printer p)
          : replacement_rule_package(d, p)
          {
          }

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();

      protected:
        void* pre_zeta_xfm_1(const std::vector<std::string>& args);
        void* pre_zeta_xfm_2(const std::vector<std::string>& args);

      };

  } // namespace macro_packages


#endif //__gauge_xfm_H_
