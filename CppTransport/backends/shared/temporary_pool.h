//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_temporary_pool_H_
#define __macropackage_temporary_pool_H_


#include "macro.h"
#include "replacement_rule_package.h"


namespace macro_packages
  {

    class temporary_pool: public replacement_rule_package
      {
      public:
        temporary_pool(replacement_data& d, std::string t = OUTPUT_DEFAULT_POOL_TEMPLATE)
          : default_template(t), replacement_rule_package(d)
          {
          }

        const rule_package<replacement_rule_simple>& get_simple_rules();
        const rule_package<replacement_rule_index>&  get_index_rules();

        void        deposit_temporaries    ();

      protected:
        std::string default_template;

        std::string replace_temp_pool      (const std::vector<std::string>& args);
      };

  } // namespace macro_packages


#endif //__macropackage_temporary_pool_H_
