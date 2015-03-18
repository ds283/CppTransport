//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#include "opencl_group.h"

#include "opencl_printer.h"
#include "cpp_cse.h"


opencl_group::opencl_group(translation_unit* u, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache)
  : package_group(u, OPENCL_COMMENT_SEPARATOR, cache), printer()
  {
    // set up cse worker instance
    // this has to happen before setting up the individual macro packages,
    // because it gets pushed to them when they join the package group
    cse_worker = new cpp::cpp_cse(0, this->printer, this->unit->get_do_cse());

    f  = new macro_packages::fundamental       (u, this->printer);
    ft = new macro_packages::flow_tensors      (u, this->printer);
    lt = new macro_packages::lagrangian_tensors(u, this->printer);
    ut = new macro_packages::utensors          (u, this->printer);
    xf = new macro_packages::gauge_xfm         (u, this->printer);
    tp = new macro_packages::temporary_pool    (u, this->printer);
    su = new macro_packages::summation         (u, this->printer);
    ka = new shared::kernel_argument_macros    (u, this->printer, "global");

    this->push_back(ka);
    this->push_back(su);
    this->push_back(tp);
    this->push_back(ut);
    this->push_back(xf);
    this->push_back(lt);
    this->push_back(ft);
    this->push_back(f);
  }


opencl_group::~opencl_group()
  {
    delete f;
    delete ft;
    delete lt;
    delete ut;
    delete xf;
    delete tp;
    delete su;
    delete ka;

    // cse_worker gets deleted by base
  }


void opencl_group::report_end_of_input()
	{
    assert(this->tp != nullptr);

    this->tp->report_end_of_input();
	}
