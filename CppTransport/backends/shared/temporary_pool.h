//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_temporary_pool_H_
#define __macropackage_temporary_pool_H_


#include <list>

#include "macro.h"
#include "replacement_rule_package.h"

#include "buffer.h"

namespace macro_packages
  {

    class temporary_pool: public replacement_rule_package
      {
      public:
        temporary_pool(replacement_data& d, ginac_printer p, macro_package* m,
                       unsigned int dm = DEFAULT_RECURSION_DEPTH,
                       std::string t = OUTPUT_DEFAULT_POOL_TEMPLATE)
          : pool_template(t), recursion_depth(0), recursion_max(dm), ms(m), buf(nullptr),
            replacement_rule_package(d, p)
          {
          }

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();

        void                           deposit_temporaries();

        void                           set_buffer(buffer* b);

      protected:
        buffer*        buf;
        macro_package* ms;

        std::string    pool_template;

        unsigned int   recursion_depth;
        unsigned int   recursion_max;

        std::string replace_temp_pool (const std::vector<std::string>& args);
      };

  } // namespace macro_packages


#endif //__macropackage_temporary_pool_H_
