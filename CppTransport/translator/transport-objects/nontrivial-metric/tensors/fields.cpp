//
// Created by David Seery on 16/06/2017.
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

#include "fields.h"


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> fields::compute(const index_literal_list& indices)
      {
        if(indices.size() != FIELD_TENSOR_INDICES) throw tensor_exception("field indices");
        if(indices[0]->get_variance() != variance::contravariant) throw tensor_exception(ERROR_FIELD_INDICES_ARE_CONTRAVARIANT);
    
        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(FIELD_TENSOR_INDICES));
    
        const field_index max_i = this->shared.get_max_field_index(variance::contravariant);

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);
    
        for(field_index i = field_index(0, variance::contravariant); i < max_i; ++i)
          {
            (*result)[this->fl.flatten(i)] = this->compute_component(i);
          }
    
        return result;
      }
    
    
    GiNaC::ex fields::compute_component(field_index i)
      {
        if(!this->cached) throw tensor_exception("fields cache not ready");
        if(i.get_variance() != variance::contravariant) throw tensor_exception(ERROR_FIELD_INDICES_ARE_CONTRAVARIANT);

        unsigned int index = this->fl.flatten(i);
        
        GiNaC::ex result = (*this->f)[index];
    
        return result;
      }
    
    
    std::unique_ptr<atomic_lambda> fields::compute_lambda(const index_literal& i)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("fields");
        if(i.get_variance() != variance::contravariant) throw tensor_exception(ERROR_FIELD_INDICES_ARE_CONTRAVARIANT);
    
        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
    
        auto args = std::make_unique<cache_tags>();
    
        auto result = this->res.generate_field_vector(i, this->printer);
    
        return std::make_unique<atomic_lambda>(i, result, expression_item_types::fields_lambda, *args, this->shared.generate_working_type());
      }
    
    
    unroll_behaviour fields::get_unroll(const index_literal_list& idx_list)
      {
        if(this->shared.can_roll_coordinates()) return unroll_behaviour::allow;
        return unroll_behaviour::force;
      }
    
    
    fields::fields(language_printer& p, cse& cw, resources& r, shared_resources& s,
                   index_flatten& f, index_traits& t)
      : ::fields(),
        printer(p),
        cse_worker(cw),
        res(r),
        shared(s),
        fl(f),
        traits(t),
        cached(false)
      {
      }
    
    
    void fields::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("fields already cached");
        
        // res.generate_field_vector() gives us the labels for the fields
        // for these, there is no notion of co- or contravariant indices; the field labels are just scalar fields
        // on the manifold. Conventionally we place their indices in a contravariant position.
        // So, no index raising or lowering is required.
        f = this->res.generate_field_vector(this->printer);

        this->cached = true;
      }
    
    
    void fields::post()
      {
        if(!this->cached) throw tensor_exception("fields not cached");

        // invalidate cache
        this->cached = false;
      }

  }   // namespace nontrivial_metric
