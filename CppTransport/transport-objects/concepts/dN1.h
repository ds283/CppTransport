//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DN1_H
#define CPPTRANSPORT_DN1_H


#include "transport_tensor.h"
#include "flattened_tensor.h"

#include "indices.h"


class dN1: public transport_tensor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    dN1() = default;

    //! destructor is default
    virtual ~dN1() = default;


    // INTERFACE

  public:

    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor> compute() = 0;

    //! evaluate component of tensor
    virtual GiNaC::ex compute_component(phase_index i) = 0;

    //! invalidate cache
    virtual void reset_cache() = 0;

  };



#endif //CPPTRANSPORT_DN1_H
