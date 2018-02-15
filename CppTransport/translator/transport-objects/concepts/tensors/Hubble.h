//
// Created by David Seery on 19/12/2015.
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

#ifndef CPPTRANSPORT_HUBBLE_H
#define CPPTRANSPORT_HUBBLE_H


#include "concepts/transport_tensor.h"
#include "concepts/flattened_tensor.h"


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

    //! evaluate eta parameter
    virtual GiNaC::ex compute_eta() = 0;

  };


#endif //CPPTRANSPORT_HUBBLE_H
