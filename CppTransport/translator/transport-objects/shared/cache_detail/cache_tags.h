//
// Created by David Seery on 23/06/2017.
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
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_CACHE_TAGS_H
#define CPPTRANSPORT_CACHE_TAGS_H


#include <iostream>
#include <vector>
#include <initializer_list>

#include "symbol_factory.h"

#include "ginac/ginac.h"


// forward-declare operator overloads
class cache_tags;

//! comparison operator
bool operator==(const cache_tags& A, const cache_tags& B);

//! inequality operator
bool operator!=(const cache_tags& A, const cache_tags& B);

//! stream insertion operator
std::ostream& operator<<(std::ostream& out, const cache_tags& tags);


class cache_tags
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    cache_tags() = default;

    //! destructor
    ~cache_tags() = default;


    // INTERFACE

  public:

    //! assignment - add GiNaC symbol
    cache_tags& operator+=(symbol_wrapper sym);

    //! assignment - list of GiNaC symbols
    cache_tags& operator+=(std::initializer_list<symbol_wrapper> syms);

    //! assignment - add GiNaC index with variance
    cache_tags& operator+=(GiNaC::varidx idx);

    //! assignment - list of GiNaC indices with variance
    cache_tags& operator+=(std::initializer_list<GiNaC::varidx> idxs);

    //! assignment - add GiNaC index
    cache_tags& operator+=(GiNaC::idx idx);

    //! assignment - list of GiNaC indices
    cache_tags& operator+=(std::initializer_list<GiNaC::idx> idxs);


    friend bool operator==(const cache_tags& A, const cache_tags& B);

    friend bool operator!=(const cache_tags& A, const cache_tags& B);
    
    friend std::ostream& operator<<(std::ostream& out, const cache_tags& tags);
    
    
    // SERVICES

  public:

    //! get number of items
    size_t size() const { return this->symbols.size() + this->indices.size() + this->var_indices.size(); }

    //! determine whether empty
    bool empty() const { return this->symbols.empty() && this->indices.empty() && this->var_indices.empty(); }


    // INTERNAL DATA

  private:

    //! symbol tags
    std::vector< std::shared_ptr<symbol_wrapper> > symbols;

    //! index tags
    std::vector< std::shared_ptr<GiNaC::idx> > indices;

    //! index tags with variance
    //! (need to keep the two types of index in distinct buckets
    std::vector< std::shared_ptr<GiNaC::varidx> > var_indices;

  };


namespace std
  {

    //! provide std::hash implementation for cache_tags
    template <>
    struct hash<cache_tags>
      {

        size_t operator()(const cache_tags& tags)
          {
            // hash operation is simple-minded, just returning the total number of items used as tags
            return tags.size();
          }

      };

  }


#endif //CPPTRANSPORT_CACHE_TAGS_H
