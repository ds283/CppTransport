//
// Created by David Seery on 06/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __vexcl_group_H_
#define __vexcl_group_H_


#include "package_group.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "summation.h"
#include "vexcl_steppers.h"
#include "vexcl_kernels.h"
#include "cpp_printer.h"


class vexcl_group: public package_group
  {
  public:
    vexcl_group(translation_unit* u, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache);
    ~vexcl_group();

    virtual void report_end_of_input() override;

  protected:
    cpp::cpp_printer                    printer;

    macro_packages::fundamental*        f;
    macro_packages::flow_tensors*       ft;
    macro_packages::lagrangian_tensors* lt;
    macro_packages::utensors*           ut;
    macro_packages::gauge_xfm*          xf;
    macro_packages::temporary_pool*     tp;
    macro_packages::summation*          su;
    cpp::vexcl_steppers*                vs;
    cpp::vexcl_kernels*                 vk;
  };


#endif //__vexcl_group_H_
