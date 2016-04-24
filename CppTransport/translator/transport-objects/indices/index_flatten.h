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

#ifndef CPPTRANSPORT_INDEX_FLATTEN_H
#define CPPTRANSPORT_INDEX_FLATTEN_H


#include "indices.h"


class index_flatten
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    index_flatten(unsigned int p, unsigned int f)
      : num_params(p),
        num_fields(f)
      {
      }


    // INTERFACE

  public:

    //! get size of a flattened d-dimensional array
    template <typename IndexType>
    unsigned int get_flattened_size(unsigned int d) const;


    // INTERFACE -- FLATTEN FUNCTIONS

  public:

    //! flatten 1-index object
    template <typename IndexType>
    unsigned int flatten(const IndexType& a) const;

    //! flatten 2-index object
    template <typename IndexType>
    unsigned int flatten(const IndexType& a, const IndexType& b) const;

    //! flatten 3-index object
    template <typename IndexType>
    unsigned int flatten(const IndexType& a, const IndexType& b, const IndexType& c) const;


    // INTERNAL DATA

  private:

    //! number of parameters
    unsigned int num_params;

    //! number of fields
    unsigned int num_fields;

  };


#endif //CPPTRANSPORT_INDEX_FLATTEN_H
