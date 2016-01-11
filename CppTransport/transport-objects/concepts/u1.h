//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_U1_H
#define CPPTRANSPORT_U1_H


#include "transport_tensor.h"
#include "flattened_tensor.h"

#include "indices.h"
#include "lambdas.h"


class u1: public transport_tensor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    u1() = default;

    //! destructor is default
    virtual ~u1() = default;


    // INTERFACE

  public:

    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor> compute() = 0;

    //! evaluate component of tensor
    virtual GiNaC::ex compute_component(phase_index i) = 0;

    //! evaluate lambda for tensor
    virtual std::unique_ptr<map_lambda> compute_lambda(const abstract_index& i) = 0;

    //! invalidate cache
    virtual void reset_cache() = 0;

  };



#endif //CPPTRANSPORT_U1_H
