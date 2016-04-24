//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_TRANSPORT_TENSOR_H
#define CPPTRANSPORT_TRANSPORT_TENSOR_H


#include <stdexcept>

#include "macro_types.h"


//! define 'transport_tensor' concept, representing an arbitrary symbolic tensor
//! (eg. U-tensor, Lagrangian tensor such as A, B, C)
//! which knows how to compute itself symbolically in a way suitable for
//! explicit index-set unrolling,
//! but also how to represent itself abstractly in a way suitable for
//! handling index-sets via a for-loop
class transport_tensor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    transport_tensor() = default;

    //! destructor is default
    virtual ~transport_tensor() = default;


    // INTERFACE

  public:

    //! determine whether this tensor can be unrolled with the current resources
    virtual enum unroll_behaviour get_unroll() = 0;

  };


class tensor_exception: std::runtime_error
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    tensor_exception(std::string x)
      : std::runtime_error(std::move(x))
      {
      }

    //! destructor is default
    ~tensor_exception() = default;

  };


#endif //CPPTRANSPORT_TRANSPORT_TENSOR_H
