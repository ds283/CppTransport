//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <sstream>
#include <string>

#include "package_group_factory.h"

#include "core_group.h"
#include "vexcl_group.h"


package_group* package_group_factory(std::string backend, macro_packages::replacement_data& data, buffer* buf, bool do_cse)
  {
    package_group* rval = nullptr;

    assert(buf != nullptr);

    if(backend == "core")
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

    // inform the selected backend of the output buffer
    if(rval != nullptr) rval->set_buffer(buf);

    return(rval);
  }
