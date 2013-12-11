//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <string>
#include <functional>

#include "temporary_pool.h"
#include "translation_unit.h"
#include "macro.h"
#include "msg_en.h"


#define BIND(X) std::bind(&temporary_pool::X, this, std::placeholders::_1)


namespace macro_packages
  {

    temporary_pool::temporary_pool(translation_unit* u, language_printer& p, std::string t)
     : pool_template(t), replacement_rule_package(u, p)
      {
        // bind ourselves to the buffer on top of the stack, and remember which buffer that was
        // so we can deregister later
        registered_buf     = unit->get_stack()->top_buffer();
        registered_handler = std::bind(&temporary_pool::deposit_temporaries, this);

        if(registered_buf != nullptr)
          {
            registered_buf->register_closure_handler(registered_handler, this);
          }
      }


    temporary_pool::~temporary_pool()
      {
        if(this->registered_buf != nullptr)
          {
            this->registered_buf->deregister_closure_handler(registered_handler, this);
          }
      }


    const std::vector<simple_rule> temporary_pool::get_pre_rules()
      {
        std::vector<simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
          { BIND(replace_temp_pool)
          };

        const std::vector<std::string> names =
          { "TEMP_POOL"
          };

        const std::vector<unsigned int> args =
          { 1
          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); i++)
          {
            simple_rule rule;

            rule.rule = rules[i];
            rule.args = args[i];
            rule.name = names[i];

            package.push_back(rule);
          }

        return(package);
      }


    const std::vector<simple_rule> temporary_pool::get_post_rules()
      {
        std::vector<simple_rule> package;

        return(package);
      }


    const std::vector<index_rule> temporary_pool::get_index_rules()
      {
        std::vector<index_rule> package;

        return(package);
      }


    // *******************************************************************


    void temporary_pool::deposit_temporaries()
      {
        // deposit any temporaries generated in the current temporary pool,
        // and then reset
        //
        // the insertion happens before the element pointed
        // to by data.pool, so there should be no need
        // to update its location

        // get buffer and macro package from the top of the stack
        buffer*        buf = this->unit->get_stack()->top_buffer();
        macro_package* ms  = this->unit->get_stack()->top_macro_package();

        if(buf == nullptr)
          {
            error(ERROR_NO_BUFFER_REGISTERED);
          }
        else if(ms == nullptr)
          {
            error(ERROR_NO_MACROS_REGISTERED);
          }
        else
          {
            std::string temps = this->cse_worker->temporaries(this->pool_template);
            ms->apply(temps);

            // write to current tagged position, but don't move it - we might need to write again later
            buf->write_to_tag(this->printer.comment(OUTPUT_TEMPORARY_POOL_START));
            if(temps != "") buf->write_to_tag(temps);

            // clear worker object; if we don't we might duplicate temporaries we've already written out
            this->cse_worker->clear();
          }

       }

    std::string temporary_pool::replace_temp_pool(const std::vector<std::string>& args)
      {
        assert(args.size() == 1);
        std::string t = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_POOL_TEMPLATE);

        // deposit any temporaries generated up to this point the current temporary pool
        //
        // the insertion happens before the element pointed
        // to by data.pool, so there should be no need
        // to update its location

        // get buffer and macro package from the top of the stack
        buffer*        buf = this->unit->get_stack()->top_buffer();
        macro_package* ms  = this->unit->get_stack()->top_macro_package();

        if(buf == nullptr)
          {
            error(ERROR_NO_BUFFER_REGISTERED);
          }
        else if(ms == nullptr)
          {
            error(ERROR_NO_MACROS_REGISTERED);
          }
        else
          {
            this->deposit_temporaries();

            // remember new template
            this->pool_template = t;

            // mark current endpoint in the buffer as the new insertion point
            buf->set_tag_to_end();
          }

        // temporary pool will be inserted *before* the line corresponding to this macro
        return(this->printer.comment(OUTPUT_TEMPORARY_POOL_END));
      }

  } // namespace macro_packages
