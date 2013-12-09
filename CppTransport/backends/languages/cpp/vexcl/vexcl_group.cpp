//
// Created by David Seery on 06/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "vexcl_group.h"

#include "cpp_printer.h"
#include "cpp_cse.h"


vexcl_group::vexcl_group(macro_packages::replacement_data& d, bool do_cse)
  : package_group(d), printer()
  {
    // set up cse worker instance
    // this has to happen before setting up the individual macro packages,
    // because it gets pushed to them when they join the package group
    cse_worker = new cpp::cpp_cse(0, this->printer, do_cse);

    f  = new macro_packages::fundamental       (d, this->printer);
    ft = new macro_packages::flow_tensors      (d, this->printer);
    lt = new macro_packages::lagrangian_tensors(d, this->printer);
    ut = new macro_packages::utensors          (d, this->printer);
    xf = new macro_packages::gauge_xfm         (d, this->printer);
    tp = new macro_packages::temporary_pool    (d, this->printer);
    vs = new cpp::vexcl_steppers               (d, this->printer);

    this->push_back(vs);
    this->push_back(tp);
    this->push_back(ut);
    this->push_back(xf);
    this->push_back(lt);
    this->push_back(ft);
    this->push_back(f);
  }


vexcl_group::~vexcl_group()
  {
    delete f;
    delete ft;
    delete lt;
    delete ut;
    delete xf;
    delete tp;
    delete vs;

    delete cse_worker;
  }
