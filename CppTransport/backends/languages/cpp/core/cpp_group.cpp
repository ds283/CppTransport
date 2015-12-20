//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include "cpp_group.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "cpp_steppers.h"
#include "resources.h"

#include "cpp_cse.h"
#include "cpp_printer.h"


cpp_group::cpp_group(translator_data& p, tensor_factory& fctry)
  : package_group(p, fctry)
  {
    // set up cse worker instance
    // this has to happen before setting up the individual macro packages,
    // because it gets pushed to them automatically when we add this packages
    // to our list
    l_printer = std::make_unique<cpp::cpp_printer>();
    cse_worker = std::make_unique<cpp::cpp_cse>(0, *l_printer, this->data_payload);

    // construct replacement rule packages
    this->add_package<macro_packages::fundamental>       (p, *l_printer);
    this->add_package<macro_packages::flow_tensors>      (p, *l_printer);
    this->add_package<macro_packages::lagrangian_tensors>(p, *l_printer);
    this->add_package<macro_packages::utensors>          (p, *l_printer);
    this->add_package<macro_packages::gauge_xfm>         (p, *l_printer);
    this->add_package<macro_packages::resources>         (p, *l_printer);
    this->add_package<macro_packages::temporary_pool>    (p, *l_printer);
    this->add_package<cpp::cpp_steppers>                 (p, *l_printer);
  }
