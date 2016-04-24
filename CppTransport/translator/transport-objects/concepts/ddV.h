//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DDV_H
#define CPPTRANSPORT_DDV_H


#include "transport_tensor.h"
#include "flattened_tensor.h"

#include "indices.h"
#include "lambdas.h"


class ddV: public transport_tensor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    ddV() = default;

    //! destructor is default
    virtual ~ddV() = default;


    // INTERFACE

  public:

    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor> compute() = 0;

    //! evaluate lambda for tensor
    virtual std::unique_ptr<atomic_lambda> compute_lambda(const abstract_index& i, const abstract_index& j) = 0;

  };



#endif //CPPTRANSPORT_DDV_H
