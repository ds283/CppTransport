//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_M_H
#define CPPTRANSPORT_M_H


#include "transport_tensor.h"
#include "flattened_tensor.h"

#include "indices.h"
#include "lambdas.h"


class M: public transport_tensor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    M() = default;

    //! destructor is default
    virtual ~M() = default;


    // INTERFACE

  public:

    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor> compute() = 0;

    //! evaluate component of tensor
    virtual GiNaC::ex compute_component(field_index i, field_index j) = 0;

    //! evaluate lambda for tensor
    virtual std::unique_ptr<atomic_lambda> compute_lambda(const abstract_index& i, const abstract_index& j) = 0;

    //! invalidate cache
    virtual void reset_cache() = 0;

  };



#endif //CPPTRANSPORT_M_H
