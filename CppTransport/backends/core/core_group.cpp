//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "core_group.h"

#include "cpp_printer.h"
#include "cpp_cse.h"


core_group::core_group(macro_packages::replacement_data& d)
  : package_group(d)
  {
    f  = new macro_packages::fundamental(d, ginac_printer(&cpp::print));
    ft = new macro_packages::flow_tensors(d, ginac_printer(&cpp::print));
    lt = new macro_packages::lagrangian_tensors(d, ginac_printer(&cpp::print));
    ut = new macro_packages::utensors(d, ginac_printer(&cpp::print));
    tp = new macro_packages::temporary_pool(d, ginac_printer(&cpp::print));
    cm = new cpp::core_macros(d, ginac_printer(&cpp::print));

    packages.push_back(cm);
    packages.push_back(tp);
    packages.push_back(ut);
    packages.push_back(lt);
    packages.push_back(ft);
    packages.push_back(f);

    cse_worker = new cpp::cpp_cse(0, ginac_printer(&cpp::print));
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
