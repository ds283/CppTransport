//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SR_VELOCITY_H
#define CPPTRANSPORT_SR_VELOCITY_H


#include "transport_tensor.h"
#include "flattened_tensor.h"

#include "indices.h"


class SR_velocity: public transport_tensor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    SR_velocity() = default;

    //! destructor is default
    virtual ~SR_velocity() = default;


    // INTERFACE

  public:

    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor> compute() = 0;

    //! evaluate component of tensor
    virtual GiNaC::ex compute_component(field_index i) = 0;

    //! invalidate cache
    virtual void reset_cache() = 0;

  };



#endif //CPPTRANSPORT_SR_VELOCITY_H
