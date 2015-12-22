//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DDDV_H
#define CPPTRANSPORT_DDDV_H


#include "transport_tensor.h"
#include "flattened_tensor.h"

#include "indices.h"


class dddV: public transport_tensor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    dddV() = default;

    //! destructor is default
    virtual ~dddV() = default;


    // INTERFACE

  public:

    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor> compute() = 0;

  };



#endif //CPPTRANSPORT_DDDV_H
