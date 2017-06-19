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

#include "coordinates.h"


namespace canonical
  {
    
    std::unique_ptr<flattened_tensor> canonical_coordinates::compute(const index_literal_list& indices)
      {
        if(indices.size() != COORDINATE_TENSOR_INDICES) throw tensor_exception("coordinate indices");
        
        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(COORDINATE_TENSOR_INDICES));
        
        const field_index max_i = this->shared.get_max_field_index(indices[0]->get_variance());

        unsigned deriv_offset = result->size()/2;

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(field_index i = field_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            GiNaC::ex f;
            GiNaC::ex d;
            
            std::tie(f, d) = this->compute_component(i);
            
            (*result)[this->fl.flatten(i)] = f;
            (*result)[this->fl.flatten(i) + deriv_offset] = d;
          }
        
        return result;
      }
    
    
    std::pair<GiNaC::ex, GiNaC::ex> canonical_coordinates::compute_component(field_index i)
      {
        if(!this->cached) throw tensor_exception("coordinates cache not ready");

        unsigned int index = this->fl.flatten(i);

        GiNaC::ex f = (*this->fields)[index];
        GiNaC::ex d = (*this->derivs)[index];
        
        return std::make_pair(f, d);
      }


    void canonical_coordinates::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("coordinates already cached");

        fields = this->shared.generate_field_symbols(this->printer);
        derivs = this->shared.generate_deriv_symbols(this->printer);

        this->cached = true;
      }


    void canonical_coordinates::post()
      {
        if(!this->cached) throw tensor_exception("A not cached");

        // invalidate cache
        this->cached = false;
      }


    std::unique_ptr<atomic_lambda> canonical_coordinates::compute_lambda(const abstract_index& i)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("coordinates");
        
        GiNaC::idx idx_i = this->shared.generate_index(i);
        
        auto args = std::make_unique<cache_tags>();
        
        auto result = this->res.generate_field_vector(i, this->printer);
    
        return std::make_unique<atomic_lambda>(i, result, expression_item_types::coordinates_lambda, *args, this->shared.generate_working_type());
      }
    
    
    unroll_behaviour canonical_coordinates::get_unroll()
      {
        if(this->shared.can_roll_coordinates()) return unroll_behaviour::allow;
        return unroll_behaviour::force;
      }


    canonical_coordinates::canonical_coordinates(language_printer& p, cse& cw, resources& r, shared_resources& s,
                                                 index_flatten& f, index_traits& t)
      : coordinates(),
        printer(p),
        cse_worker(cw),
        res(r),
        shared(s),
        fl(f),
        traits(t),
        cached(false)
      {
      }

  }   // namespace canonical
