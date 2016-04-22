//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DV_H
#define CPPTRANSPORT_DV_H


#include "transport_tensor.h"
#include "flattened_tensor.h"

#include "indices.h"
#include "lambdas.h"


class dV: public transport_tensor
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

    //! evaluate lambda for tensor
    virtual std::unique_ptr<atomic_lambda> compute_lambda(const abstract_index& i) = 0;

  };



#endif //CPPTRANSPORT_DV_H
