//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef CPPTRANSPORT_CORE_GROUP_H
#define CPPTRANSPORT_CORE_GROUP_H


#include "package_group.h"
#include "u_tensor_factory.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "cpp_steppers.h"
#include "cpp_printer.h"


class cpp_group : public package_group
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    cpp_group(translator_data& p, u_tensor_factory& factory);

    //! destructor is default
    virtual ~cpp_group() = default;

  };


#endif //CPPTRANSPORT_CORE_GROUP_H
