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

#ifndef CPPTRANSPORT_CACHE_KEY_H
#define CPPTRANSPORT_CACHE_KEY_H


#include "cache_tags.h"
#include "hash_combine.h"


// forward-declare operator overloads
template <typename ItemClass> class cache_key;

//! comparison operator
template <typename ItemClass>
bool operator==(const cache_key<ItemClass>& A, const cache_key<ItemClass>& B);

//! std::hash implementation
//! g++ < 7.0 has a bug that requires this to be embedded within an explicit namespace
namespace std
  {
    
    template <typename ItemClass>
    struct hash< cache_key<ItemClass> >;
    
  }   // namespace std


template <typename ItemClass>
class cache_key
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor; assumes ownership of the cache_tags
    cache_key(ItemClass c, unsigned int i, cache_tags t)
      : item_class(c),
        index(i),
        tags(t)
      {
      }

    //! destructor
    ~cache_key() = default;


    friend bool operator==<>(const cache_key<ItemClass>& A, const cache_key<ItemClass>& B);

    friend struct std::hash< cache_key<ItemClass> >;


    // INTERNAL DATA

  private:

    // class of this item (eg. A-tensor, B-tensor, ...)
    ItemClass item_class;

    //! index of this item (generally the flattened tensor index)
    unsigned int index;

    //! cache tags
    cache_tags tags;

  };


template <typename ItemClass>
bool operator==(const cache_key<ItemClass>& A, const cache_key<ItemClass>& B)
  {
    if(A.item_class != B.item_class) return false;
    if(A.index != B.index) return false;
    if(A.tags != B.tags) return false;

    return true;
  }


namespace std
  {

    //! provide std::hash implementation
    template <typename ItemClass>
    struct hash< cache_key<ItemClass> >
      {

        size_t operator()(const cache_key<ItemClass>& key) const
          {
            size_t hash = static_cast<typename std::underlying_type<ItemClass>::type>(key.item_class);
            hash_combine(hash, key.index, key.tags);

            return hash;
          }

      };

  }


#endif //CPPTRANSPORT_CACHE_KEY_H
