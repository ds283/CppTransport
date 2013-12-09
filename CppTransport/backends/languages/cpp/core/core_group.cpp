//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "core_group.h"

#include "cpp_printer.h"
#include "cpp_cse.h"


core_group::core_group(macro_packages::replacement_data& d, bool do_cse)
  : package_group(d), printer()
  {
    // set up cse worker instance
    // this has to happen before setting up the individual macro packages,
    // because it gets pushed to them automatically when we add this packages
    // to our list
    cse_worker = new cpp::cpp_cse(0, this->printer, do_cse);

    f  = new macro_packages::fundamental       (d, this->printer);
    ft = new macro_packages::flow_tensors      (d, this->printer);
    lt = new macro_packages::lagrangian_tensors(d, this->printer);
    ut = new macro_packages::utensors          (d, this->printer);
    xf = new macro_packages::gauge_xfm         (d, this->printer);
    tp = new macro_packages::temporary_pool    (d, this->printer);
    cm = new cpp::core_macros                  (d, this->printer);

    this->push_back(cm);
    this->push_back(tp);
    this->push_back(ut);
    this->push_back(xf);
    this->push_back(lt);
    this->push_back(ft);
    this->push_back(f);
  }


core_group::~core_group()
  {
    delete f;
    delete ft;
    delete lt;
    delete ut;
    delete xf;
    delete tp;
    delete cm;

    delete cse_worker;
  }
