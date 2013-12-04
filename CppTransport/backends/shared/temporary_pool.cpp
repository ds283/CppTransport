//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <string>

#include "temporary_pool.h"

namespace macro_packages
  {

    void temporary_pool::deposit_temporaries()
      {
        // deposit any temporaries generated in the current temporary pool,
        // and then reset
        //
        // the insertion happens before the element pointed
        // to by data.pool, so there should be no need
        // to update its location
        std::string temps = this->data.temp_factory.temporaries(data.pool_template);
        this->data.ms->apply(temps);
        this->data.buffer.insert(data.pool, temps);

        this->data.temp_factory.clear();
      }

    std::string temporary_pool::replace_temp_pool(const std::vector<std::string>& args)
      {
        assert(args.size() == 1);
        std::string t = (args.size() >= 1 ? args[0] : this->default_template);

        // deposit any temporaries generated up to this point the current temporary pool
        //
        // the insertion happens before the element pointed
        // to by data.pool, so there should be no need
        // to update its location
        this->deposit_temporaries(data);

        // mark current endpoint in the buffer as the new insertion point

        this->data.pool_template = t;
        this->data.pool          = --this->data.buffer.end();

        return(""); // replace with a blank
      }

  } // namespace macro_packages