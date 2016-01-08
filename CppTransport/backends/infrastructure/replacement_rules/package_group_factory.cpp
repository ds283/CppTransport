//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "package_group_factory.h"

#include "cpp_group.h"
#include "vexcl_group.h"
#include "opencl_group.h"
#include "cuda_group.h"


std::unique_ptr<package_group> package_group_factory(const boost::filesystem::path& in, const backend_data& backend,
                                                     translator_data& payload, tensor_factory& fctry)
  {
    std::string backend_name = backend.get_backend_name();

    if(backend_name == "cpp")
      {
		    return std::make_unique<cpp_group>(payload, fctry);
      }
    else if(backend_name == "vexcl")
      {
		    return std::make_unique<vexcl_group>(payload, fctry);
      }
    else if(backend_name == "opencl")
      {
		    return std::make_unique<opencl_group>(payload, fctry);
      }
    else if(backend_name == "cuda")
      {
		    return std::make_unique<cuda_group>(payload, fctry);
      }

    std::ostringstream msg;
    msg << ERROR_TEMPLATE_BACKEND_A << " " << in << " " << ERROR_TEMPLATE_BACKEND_B << " '" << backend_name << "'";
    throw std::runtime_error(msg.str());
  }
