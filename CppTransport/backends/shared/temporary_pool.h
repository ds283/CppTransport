//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_temporary_pool_H_
#define __macropackage_temporary_pool_H_


#include <list>

#include "macro.h"
#include "replacement_rule_package.h"


namespace macro_packages
  {

    class temporary_pool: public replacement_rule_package
      {
      public:
        temporary_pool(replacement_data& d, ginac_printer p,
                       std::list<std::string>::iterator cpl, unsigned int dm = DEFAULT_RECURSION_DEPTH,
                       std::string t = OUTPUT_DEFAULT_POOL_TEMPLATE)
          : pool_template(t), current_pool_location(cpl), recursion_depth(0), recursion_max(dm),
            replacement_rule_package(d, p)
          {
          }

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();

        void        deposit_temporaries    ();

      protected:
        std::string                        pool_template;
        std::list<std::string>::iterator   current_pool_location;

        unsigned int                       recursion_depth;
        unsigned int                       recursion_max;

        std::string replace_temp_pool      (const std::vector<std::string>& args);
      };

  } // namespace macro_packages


#endif //__macropackage_temporary_pool_H_
