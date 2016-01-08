//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_HUBBLE_H
#define CPPTRANSPORT_HUBBLE_H


#include "transport_tensor.h"
#include "flattened_tensor.h"


// not Hubble does not inherit from transport_tensor
class Hubble
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    Hubble() = default;

    //! destructor is default
    virtual ~Hubble() = default;


    // INTERFACE

  public:

    //! evaluate potential
    virtual GiNaC::ex compute_V() = 0;

    //! evaluate Hubble parameter
    virtual GiNaC::ex compute_Hsq() = 0;

    //! evaluate epsilon parameter
    virtual GiNaC::ex compute_eps() = 0;

  };


#endif //CPPTRANSPORT_HUBBLE_H
