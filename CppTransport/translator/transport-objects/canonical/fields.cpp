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


namespace canonical
  {
    
    std::unique_ptr<flattened_tensor> canonical_fields::compute(const index_literal_list& indices)
      {
        if(indices.size() != FIELD_TENSOR_INDICES) throw tensor_exception("field indices");
    
        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(FIELD_TENSOR_INDICES));
    
        const field_index max_i = this->shared.get_max_field_index(indices[0]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);
    
        for(field_index i = field_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            (*result)[this->fl.flatten(i)] = this->compute_component(i);
          }
    
        return result;
      }
    
    
    GiNaC::ex canonical_fields::compute_component(field_index i)
      {
        if(!this->cached) throw tensor_exception("fields cache not ready");

        unsigned int index = this->fl.flatten(i);
    
        GiNaC::ex result = (*this->fields)[index];
    
        return result;
      }
    
    
    std::unique_ptr<atomic_lambda> canonical_fields::compute_lambda(const abstract_index& i)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("fields");
    
        auto idx_i = this->shared.generate_index<GiNaC::idx>(i);
    
        auto args = std::make_unique<cache_tags>();
    
        auto result = this->res.generate_field_vector(i, this->printer);
    
        return std::make_unique<atomic_lambda>(i, result, expression_item_types::fields_lambda, *args, this->shared.generate_working_type());
      }
    
    
    unroll_behaviour canonical_fields::get_unroll()
      {
        if(this->shared.can_roll_coordinates()) return unroll_behaviour::allow;
        return unroll_behaviour::force;
      }


    canonical_fields::canonical_fields(language_printer& p, cse& cw, resources& r, shared_resources& s,
                                       index_flatten& f, index_traits& t)
      : fields(),
        printer(p),
        cse_worker(cw),
        res(r),
        shared(s),
        fl(f),
        traits(t),
        cached(false)
      {
      }


    void canonical_fields::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("fields already cached");

        fields = this->shared.generate_field_symbols(this->printer);

        this->cached = true;
      }


    void canonical_fields::post()
      {
        if(!this->cached) throw tensor_exception("fields not cached");

        // invalidate cache
        this->cached = false;
      }

  }   // namespace canonical
