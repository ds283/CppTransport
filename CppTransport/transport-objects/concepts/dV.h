//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DV_H
#define CPPTRANSPORT_DV_H


#include "transport_tensor.h"
#include "flattened_tensor.h"

#include "indices.h"


class dV
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    dV() = default;

    //! destructor is default
    virtual ~dV() = default;


    // INTERFACE

  public:

    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor> compute() = 0;

  };



#endif //CPPTRANSPORT_DV_H
