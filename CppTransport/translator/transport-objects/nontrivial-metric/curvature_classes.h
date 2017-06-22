//
// Created by Sean Butchers on 20/06/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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
// @contributor: Sean Butchers <smlb20@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_NONCANONICAL_CURVATURE_CLASSES_H
#define CPPTRANSPORT_NONCANONICAL_CURVATURE_CLASSES_H


#include <iostream>

#include "symbol_list.h"
#include "concepts/flattened_tensor.h"

#include "ginac/ginac.h"


class Christoffel
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor accepts a GiNaC matrix and its inverse, and a list of symbols representing the fields of
    //! the model
    Christoffel(const GiNaC::matrix& G_, const GiNaC::matrix& Ginv_, const symbol_list& c_);

    //! destructor is default
    ~Christoffel() = default;


    // EXTRACT COMPONENTS

  public:

    //! extract a component; i = top index, (j,k) = symmetric lower indices
    const GiNaC::ex& operator()(unsigned int i, unsigned int j, unsigned int k) const;

    //! get number of fields
    unsigned int get_number_fields() const { return this->N; }

    //! get metric
    const GiNaC::matrix& get_G() const { return this->G; }

    //! get inverse metric
    const GiNaC::matrix& get_Ginv() const { return this->Ginv; }

    //! get list of field labels
    const symbol_list& get_coords() const { return this->coords; }

    //! get size
    size_t size() const;


    // INTERNAL DATA

  private:

    //! cache number of fields
    const unsigned int N;

    //! flattened tensor representing the components of the connexion
    flattened_tensor gamma;

    //! reference to matrix used to construct connexion
    const GiNaC::matrix& G;

    //! reference to inverse matrix used to construct connexion
    const GiNaC::matrix& Ginv;

    //! copy of list of field labels
    const symbol_list& coords;

  };


class Riemann_T
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor takes a reference to a Christoffel object and uses it to compute the corresponding
    //! components of the Riemann tensor
    Riemann_T(const Christoffel& Gamma_);

    //! destructor is default
    ~Riemann_T() = default;


    // EXTRACT COMPONENTS

  public:

    //! extract a component in the fully-covariant index position R_{ijkl}
    GiNaC::ex operator()(unsigned int i, unsigned int j, unsigned int k, unsigned int l) const;

    //! get size
    size_t size() const;


    // INTERNAL DATA

  private:

    //! cache number of fields
    const unsigned int N;

    //! flattened tensor representing the components of the Riemann tenosr
    flattened_tensor rie_t;

    //! reference to Christoffel object from which the Riemann tensor is built
    const Christoffel& Gamma;

  };

#endif //CPPTRANSPORT_NONCANONICAL_CURVATURE_CLASSES_H
