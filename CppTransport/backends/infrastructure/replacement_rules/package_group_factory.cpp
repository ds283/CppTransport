//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "package_group_factory.h"

#include "core_group.h"
#include "vexcl_group.h"
#include "opencl_group.h"
#include "cuda_group.h"


std::shared_ptr<package_group> package_group_factory(const std::string& in, const std::string& backend, translation_unit* unit,
                                                     ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache)
  {
    if(backend == "cpp")
      {
		    return std::shared_ptr<core_group>(new core_group(unit, cache));
      }
    else if(backend == "vexcl")
      {
		    return std::shared_ptr<vexcl_group>(new vexcl_group(unit, cache));
      }
    else if(backend == "opencl")
      {
		    return std::shared_ptr<opencl_group>(new opencl_group(unit, cache));
      }
    else if(backend == "cuda")
      {
		    return std::shared_ptr<cuda_group>(new cuda_group(unit, cache));
      }

    std::ostringstream msg;
    msg << ERROR_TEMPLATE_BACKEND_A << " '" << in << "' " << ERROR_TEMPLATE_BACKEND_B << " '" << backend << "'";
    throw std::runtime_error(msg.str());
  }
