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
#include "index_literal.h"

#include "tensor_exception.h"


// forward-declare TensorJanitor class
class TensorJanitor;

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
    virtual unroll_state get_unroll(const index_literal_list& idx_list) = 0;

    // typical transport tensors will also define compute(), compute_component() and compute_lambda()
    // methods, but since these carry a variable number of arguments depending on the requirements of
    // the transport tensor being computed they are declared on a tensor-by-tensor basis


    // JANITORIAL FUNCTIONS -- API USED BY JANITOR OBJECT

  protected:

    //! cache resources required for evaluation on an explicit index assignment
    virtual void pre_explicit(const index_literal_list& indices) = 0;

    //! release resources
    virtual void post() = 0;

    //! declare TensorJanitor a friend, so it has access to the pre() and post() methods
    friend class TensorJanitor;

  };



//! RAII janitor object for a transport tensor;
//! must be used to wrap calls to the compute_component() (or similar)
//! methods of the tensor. Handles resource acquisition and release.
class TensorJanitor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor accepts a tensor object to manage and an index_literal_list
    //! that determines what variances the tensor should cache for.
    //! It then calls the pre() method on t
    TensorJanitor(transport_tensor& t, const index_literal_list& l)
      : tensor(t)
      {
        tensor.pre_explicit(l);
      }


    //! destructor calls post() method on t
    ~TensorJanitor()
      {
        this->tensor.post();
      }


    // INTERNAL DATA

  private:

    //! tensor object we are managing
    transport_tensor& tensor;

  };


#endif //CPPTRANSPORT_TRANSPORT_TENSOR_H
