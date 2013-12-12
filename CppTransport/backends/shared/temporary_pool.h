//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_temporary_pool_H_
#define __macropackage_temporary_pool_H_


#include <list>

#include "replacement_rule_package.h"
#include "buffer.h"
#include "core.h"


namespace macro_packages
  {

    class temporary_pool: public replacement_rule_package
      {
      public:
        temporary_pool(translation_unit* u, language_printer& p, std::string t = OUTPUT_DEFAULT_POOL_TEMPLATE);

        ~temporary_pool();

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();

        void                           deposit_temporaries();

      protected:
        std::string                    pool_template;       // template to use when constructing temporaries
        unsigned int                   unique;              // unique id labelling temporary pools (helpful for matching up while debugging)
        bool                           tag_set;             // keep track of whether we have set a tag yet (helpful for debugging)

        buffer*                        registered_buf;      // buffer to which we have registered a flush handler
        buffer_flush_handler           registered_handler;  // the flush handler we registered - cached so we can deregister it later

        std::string                    replace_temp_pool(const std::vector<std::string>& args);
      };

  } // namespace macro_packages


#endif //__macropackage_temporary_pool_H_
