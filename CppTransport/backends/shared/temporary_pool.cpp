//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <string>

#include "temporary_pool.h"

namespace macro_packages
  {
    const std::vector<simple_rule> temporary_pool::get_pre_rules()
      {
        std::vector<simple_rule> package;

        const std::vector<replacement_rule_simple> rules
          { &this->replace_temp_pool
          };

        const std::vector<std::string> names
          { "TEMP_POOL"
          }

        const std::vector<unsigned int> args
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
        std::string temps = this->data.temp_factory.temporaries(this->pool_template);

        if(++this->recursion_depth < this->recursion_max)
          {
            this->data.ms->apply(temps);
            --this->recursion_depth;
          }
        else
          {
            std::ostringstream msg;
            msg << WARNING_RECURSION_DEPTH << " " << this->recursion_max << ")";
            warn(msg.str(), this->data.current_line, this->data->path);
          }

        this->data.buffer.insert(this->current_pool_location, temps);

        this->data.temp_factory.clear();
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
        this->deposit_temporaries();

        // mark current endpoint in the buffer as the new insertion point

        this->pool_template         = t;
        this->current_pool_location = --this->data.buffer.end();

        return(""); // replace with a blank
      }

  } // namespace macro_packages
