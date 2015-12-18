//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include "opencl_group.h"

#include "opencl_printer.h"
#include "cpp_cse.h"


opencl_group::opencl_group(translator_data& p, u_tensor_factory& factory)
  : package_group(p, factory)
  {
    // set up cse worker instance
    // this has to happen before setting up the individual macro packages,
    // because it gets pushed to them when they join the package group
    l_printer = std::make_unique<opencl::opencl_printer>();
    cse_worker = std::make_unique<cpp::cpp_cse>(0, *l_printer, this->data_payload);

    // construct replacement rule packages
    this->add_package<macro_packages::fundamental>       (p, *l_printer);
    this->add_package<macro_packages::flow_tensors>      (p, *l_printer);
    this->add_package<macro_packages::lagrangian_tensors>(p, *l_printer);
    this->add_package<macro_packages::utensors>          (p, *l_printer);
    this->add_package<macro_packages::gauge_xfm>         (p, *l_printer);
    this->add_package<macro_packages::temporary_pool>    (p, *l_printer);
    this->add_package<shared::kernel_argument_macros>    (p, *l_printer, "global");
  }
