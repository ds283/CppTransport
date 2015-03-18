//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __cuda_group_H_
#define __cuda_group_H_


#include "package_group.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "summation.h"
#include "kernel_argument_macros.h"
#include "cuda_printer.h"


class cuda_group : public package_group
  {
  public:
    cuda_group(translation_unit* u, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache);
    ~cuda_group();

  protected:
    cuda::cuda_printer                  printer;

    macro_packages::fundamental*        f;
    macro_packages::flow_tensors*       ft;
    macro_packages::lagrangian_tensors* lt;
    macro_packages::utensors*           ut;
    macro_packages::gauge_xfm*          xf;
    macro_packages::temporary_pool*     tp;
    macro_packages::summation*          su;
    shared::kernel_argument_macros*     ka;
  };


#endif //__cuda_group_H_
