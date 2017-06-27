//
// Created by David Seery on 27/06/2017.
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

#ifndef CPPTRANSPORT_VARIANCE_TENSOR_CACHE_H
#define CPPTRANSPORT_VARIANCE_TENSOR_CACHE_H


#include <map>
#include <memory>
#include <array>
#include <stdexcept>

#include "index_literal.h"
#include "concepts/flattened_tensor.h"

#include "msg_en.h"


template <size_t Indices>
class variance_tensor_cache
  {
    
    // TYPES
  
  public:
    
    //! cache key
    using Key = std::array<variance, Indices>;
    
    //! client function to compute tensor
    using ComputeAgent = std::function<std::unique_ptr<flattened_tensor>(Key)>;
  
  protected:
    
    //! cache
    using Database = std::map<Key, std::unique_ptr<flattened_tensor> >;
    
    
    // CONSTRUCTOR, DESTRUCTOR
  
  public:
    
    //! constructor
    variance_tensor_cache(ComputeAgent a)
      : agent(std::move(a))
      {
      }
    
    
    //! destructor is default
    ~variance_tensor_cache() = default;
    
    
    // INTERFACE
  
  public:
    
    //! clear
    void clear() { this->db.clear(); }

    //! get flattened tensor from specified key
    const flattened_tensor& get(Key k);

    //! convenience function: single index
    template <typename IndexObject>
    const flattened_tensor& operator()(const IndexObject& i)
    { return this->get( { i.get_variance() }); }

    //! convenience function: double index
    template <typename IndexObject>
    const flattened_tensor& operator()(const IndexObject& i, const IndexObject& j)
      { return this->get( { i.get_variance(), j.get_variance() }); }

    //! convenience function: triple index
    template <typename IndexObject>
    const flattened_tensor& operator()(const IndexObject& i, const IndexObject& j, const IndexObject& k)
      { return this->get( { i.get_variance(), j.get_variance(), k.get_variance() }); }


    // INTERNAL DATA
  
  private:
    
    //! compute agent: used to compute a tensor when we don't already have it cached
    ComputeAgent agent;
    
    //! cache
    Database db;
    
  };


template <size_t Indices>
const flattened_tensor& variance_tensor_cache<Indices>::get(variance_tensor_cache::Key k)
  {
    auto it = this->db.find(k);
    
    if(it != db.end()) return *(it->second);
    
    auto tensor = this->agent(k);
    auto result = this->db.emplace(std::make_pair(std::move(k), std::move(tensor)));
    
    if(!result.second) throw std::runtime_error(ERROR_VARIANCE_TENSOR_CACHE_EMPLACE);
    
    return *(result.first->second);
  }


// convenience type
using deriv_cache = variance_tensor_cache<1>;
using dV_cache = variance_tensor_cache<1>;
using ddV_cache = variance_tensor_cache<2>;
using dddV_cache = variance_tensor_cache<3>;


#endif //CPPTRANSPORT_VARIANCE_TENSOR_CACHE_H
