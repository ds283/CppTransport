//
// Created by David Seery on 06/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef CPPTRANSPORT_VEXCL_GROUP_H
#define CPPTRANSPORT_VEXCL_GROUP_H


#include "package_group.h"
#include "u_tensor_factory.h"

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
    vexcl_group(translator_data& p, u_tensor_factory& factory);

    //! destructor is default
    ~vexcl_group() = default;

  };


#endif //CPPTRANSPORT_VEXCL_GROUP_H
