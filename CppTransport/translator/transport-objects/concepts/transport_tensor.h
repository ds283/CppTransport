//
// Created by David Seery on 18/12/2015.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
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
