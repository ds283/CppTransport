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
#include "vexcl_steppers.h"
#include "vexcl_kernels.h"
#include "cpp_printer.h"


class vexcl_group: public package_group
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    vexcl_group(translator_data& p, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache);

    //! destructor is default
    ~vexcl_group() = default;


    // INTERNAL DATA

  protected:

    cpp::cpp_printer printer;

  };


#endif //__vexcl_group_H_
