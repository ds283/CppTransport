//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_TRANSPORT_TENSOR_H
#define CPPTRANSPORT_TRANSPORT_TENSOR_H


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

  };


#endif //CPPTRANSPORT_TRANSPORT_TENSOR_H
