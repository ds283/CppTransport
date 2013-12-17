//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __opencl_group_H_
#define __opencl_group_H_


#include "package_group.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "summation.h"
#include "kernel_argument_macros.h"
#include "opencl_printer.h"


class opencl_group: public package_group
  {
  public:
    opencl_group(translation_unit* u);
    ~opencl_group();

  protected:
    opencl::opencl_printer              printer;

    macro_packages::fundamental*        f;
    macro_packages::flow_tensors*       ft;
    macro_packages::lagrangian_tensors* lt;
    macro_packages::utensors*           ut;
    macro_packages::gauge_xfm*          xf;
    macro_packages::temporary_pool*     tp;
    macro_packages::summation*          su;
    shared::kernel_argument_macros*     ka;
  };


#endif //__opencl_group_H_
