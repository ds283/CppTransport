//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "core_group.h"

#include "cpp_printer.h"
#include "cpp_cse.h"


core_group::core_group(macro_packages::replacement_data& d, bool do_cse)
  : package_group(d)
  {
    // set up cse worker instance
    // this has to happen before setting up the individual macro packages,
    // because it gets pushed to them automatically when we add this packages
    // to out list
    cse_worker = new cpp::cpp_cse(0, ginac_printer(&cpp::print), do_cse);

    f  = new macro_packages::fundamental(d, ginac_printer(&cpp::print));
    ft = new macro_packages::flow_tensors(d, ginac_printer(&cpp::print));
    lt = new macro_packages::lagrangian_tensors(d, ginac_printer(&cpp::print));
    ut = new macro_packages::utensors(d, ginac_printer(&cpp::print));
    tp = new macro_packages::temporary_pool(d, ginac_printer(&cpp::print));
    cm = new cpp::core_macros(d, ginac_printer(&cpp::print));

    this->push_back(cm);
    this->push_back(tp);
    this->push_back(ut);
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
    delete tp;
    delete cm;

    delete cse_worker;
  }
