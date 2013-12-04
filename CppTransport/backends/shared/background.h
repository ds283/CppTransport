//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_background_H_
#define __macropackage_background_H_


#include <string>

#include "backends.h"
#include "macro.h"

#include "replacement_rule_package.h"

namespace macro_packages
  {

    class background: public replacement_rule_package
      {
      public:
        background(replacement_data& d)
          : replacement_rule_package(d);
          {
          }

        const rule_package<replacement_rule_simple>& get_simple_rules();
        const rule_package<replacement_rule_index>&  get_index_rules();

      protected:

        std::string replace_V  (const std::vector<std::string> &args);
        std::string replace_Hsq(const std::vector<std::string> &args);
        std::string replace_eps(const std::vector<std::string> &args);
      };

  } // namespace macro_packages


#endif //__macropackage_background_H_
