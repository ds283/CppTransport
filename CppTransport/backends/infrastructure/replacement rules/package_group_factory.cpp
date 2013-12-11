//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "package_group_factory.h"

#include "core_group.h"
#include "vexcl_group.h"
#include "opencl_group.h"


package_group* package_group_factory(std::string backend, translation_unit* unit)
  {
    package_group* rval = nullptr;

    // would be more elegant to do this via a table, but you can't take the
    // address of a constructor (there is no instance to go with it -- we need new to do that for us)
    if(backend == "cpp")
      {
        rval = new core_group(unit);
      }
    else if(backend == "vexcl")
      {
        rval = new vexcl_group(unit);
      }
    else if(backend == "opencl")
      {
        rval = new opencl_group(unit);
      }

    return(rval);
  }