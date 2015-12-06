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


std::unique_ptr<package_group> package_group_factory(const boost::filesystem::path& in, const std::string& backend, translator_data& payload,
                                                     ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache)
  {
    if(backend == "cpp")
      {
		    return std::make_unique<core_group>(payload, cache);
      }
    else if(backend == "vexcl")
      {
		    return std::make_unique<vexcl_group>(payload, cache);
      }
    else if(backend == "opencl")
      {
		    return std::make_unique<opencl_group>(payload, cache);
      }
    else if(backend == "cuda")
      {
		    return std::make_unique<cuda_group>(payload, cache);
      }

    std::ostringstream msg;
    msg << ERROR_TEMPLATE_BACKEND_A << " " << in << " " << ERROR_TEMPLATE_BACKEND_B << " '" << backend << "'";
    throw std::runtime_error(msg.str());
  }
