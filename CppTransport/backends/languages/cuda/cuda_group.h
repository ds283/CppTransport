//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_CUDA_GROUP_H
#define CPPTRANSPORT_CUDA_GROUP_H


#include "package_group.h"
#include "u_tensor_factory.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "kernel_argument_macros.h"
#include "cuda_printer.h"


class cuda_group : public package_group
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    cuda_group(translator_data& p, u_tensor_factory& factory);

    //! destructor is default
    ~cuda_group() = default;

  };


#endif //CPPTRANSPORT_CUDA_GROUP_H
