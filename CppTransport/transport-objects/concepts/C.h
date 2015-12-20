//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_C_H
#define CPPTRANSPORT_C_H


#include "transport_tensor.h"
#include "flattened_tensor.h"

#include "indices.h"


class C: public transport_tensor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    C() = default;

    //! destructor is default
    virtual ~C() = default;


    // INTERFACE

  public:

    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor> compute(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) = 0;

    //! evaluate component of tensor
    virtual GiNaC::ex compute_component(field_index i, field_index j, field_index k,
                                        GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) = 0;

    //! invalidate cache
    virtual void reset_cache() = 0;

  };



#endif //CPPTRANSPORT_C_H
