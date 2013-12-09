//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "package_group_factory.h"

#include "core_group.h"
#include "vexcl_group.h"


package_group* package_group_factory(std::string backend, macro_packages::replacement_data& data, buffer* buf, bool do_cse)
  {
    package_group* rval = nullptr;

    assert(buf != nullptr);

    // would be more elegant to do this via a table, but you can't take the
    // address of a constructor (there is no instance to go with it -- we need new to do that for us)
    if(backend == "cpp")
      {
        rval = new core_group(data, do_cse);
      }
    else if(backend == "vexcl")
      {
        rval = new vexcl_group(data, do_cse);
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_TEMPLATE_BACKEND_A << " '" << data.template_in << "' " << ERROR_TEMPLATE_BACKEND_B << " '" << backend << "'";
        error(msg.str());
      }

    // push information about the output buffer to the selected backend
    if(rval != nullptr) rval->set_buffer(buf);

    return(rval);
  }
