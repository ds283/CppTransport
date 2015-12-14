//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#include "opencl_group.h"

#include "opencl_printer.h"
#include "cpp_cse.h"


opencl_group::opencl_group(translator_data& p, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache)
  : package_group(p, OPENCL_COMMENT_SEPARATOR, OPENCL_OPEN_BRACE, OPENCL_CLOSE_BRACE, OPENCL_BRACE_INDENT, OPENCL_BLOCK_INDENT, cache),
    printer()
  {
    // set up cse worker instance
    // this has to happen before setting up the individual macro packages,
    // because it gets pushed to them when they join the package group
    cse_worker = std::make_unique<cpp::cpp_cse>(0, this->printer, this->data_payload);

    // construct replacement rule packages
    this->add_package<macro_packages::fundamental>       (p, this->printer);
    this->add_package<macro_packages::flow_tensors>      (p, this->printer);
    this->add_package<macro_packages::lagrangian_tensors>(p, this->printer);
    this->add_package<macro_packages::utensors>          (p, this->printer);
    this->add_package<macro_packages::gauge_xfm>         (p, this->printer);
    this->add_package<macro_packages::temporary_pool>    (p, this->printer);
    this->add_package<shared::kernel_argument_macros>    (p, this->printer, "global");
  }


std::string opencl_group::plant_for_loop(const std::string& loop_variable, unsigned int min, unsigned int max) const
  {
    std::ostringstream stmt;
    stmt << "for(unsigned int " << loop_variable << " = " << min << "; " << loop_variable << " < " << max << "; ++" << loop_variable << ")";
    return(stmt.str());
  }
