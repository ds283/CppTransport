//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_ZETA2_H
#define CPPTRANSPORT_ZETA2_H


#include "transport_tensor.h"
#include "flattened_tensor.h"

#include "indices.h"


class zeta2: public transport_tensor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    zeta2() = default;

    //! destructor is default
    virtual ~zeta2() = default;


    // INTERFACE

  public:

    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor> compute(GiNaC::symbol& k, GiNaC::symbol& k1,
                                                      GiNaC::symbol& k2, GiNaC::symbol& a) = 0;

    //! evaluate component of tensor
    virtual GiNaC::ex compute_component(phase_index i, phase_index j,
                                        GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a) = 0;

    //! invalidate cache
    virtual void reset_cache() = 0;

  };



#endif //CPPTRANSPORT_ZETA2_H
