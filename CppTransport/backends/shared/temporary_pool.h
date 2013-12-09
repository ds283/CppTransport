//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_temporary_pool_H_
#define __macropackage_temporary_pool_H_


#include <list>

#include "replacement_rule_package.h"
#include "buffer.h"


namespace macro_packages
  {

    class temporary_pool: public replacement_rule_package
      {
      public:
        temporary_pool(replacement_data& d, ginac_printer p,
                       unsigned int dm = DEFAULT_RECURSION_DEPTH,
                       std::string t = OUTPUT_DEFAULT_POOL_TEMPLATE)
          : pool_template(t), ms(nullptr), buf(nullptr),
            replacement_rule_package(d, p)
          {
          }

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();

        void                           deposit_temporaries();

        void                           set_buffer(buffer* b);
        void                           set_macros(macro_package* m);

      protected:
        buffer*        buf;
        macro_package* ms;

        std::string    pool_template;

        std::string    replace_temp_pool (const std::vector<std::string>& args);
      };

  } // namespace macro_packages


#endif //__macropackage_temporary_pool_H_
