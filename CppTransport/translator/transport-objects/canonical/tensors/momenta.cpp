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

#include "momenta.h"


namespace canonical
  {
    
    std::unique_ptr<flattened_tensor> momenta::compute(const index_literal_list& indices)
      {
        if(indices.size() != MOMENTA_TENSOR_INDICES) throw tensor_exception("coordinate indices");
        
        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(MOMENTA_TENSOR_INDICES));
        
        const field_index max_i = this->shared.get_max_field_index(indices[0]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(field_index i = field_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            unsigned int index = this->fl.flatten(i);
            (*result)[index] = this->compute_component(i);
          }
        
        return result;
      }
    
    
    GiNaC::ex momenta::compute_component(field_index i)
      {
        if(!this->cached) throw tensor_exception("momenta cache not ready");

        unsigned int index = this->fl.flatten(i);
        return (*derivs)[index];
      }
    
    
    void momenta::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("momenta already cached");

        derivs = this->res.generate_deriv_vector(this->printer);

        this->cached = true;
      }
    
    
    void momenta::post()
      {
        if(!this->cached) throw tensor_exception("momenta not cached");

        // invalidate cache
        this->cached = false;
      }
    
    
    std::unique_ptr<atomic_lambda> momenta::compute_lambda(const index_literal& i)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("momenta");
        
        auto idx_i = this->shared.generate_index<GiNaC::idx>(i);
        
        auto args = std::make_unique<cache_tags>();
        
        auto result = this->res.generate_deriv_vector(i, this->printer);
    
        return std::make_unique<atomic_lambda>(i, result, expression_item_types::momenta_lambda, *args, this->shared.generate_working_type());
      }
    
    
    unroll_state momenta::get_unroll(const index_literal_list& idx_list)
      {
        if(this->shared.can_roll_coordinates()) return unroll_state::allow;
        return unroll_state::force;
      }
    
    
    momenta::momenta(language_printer& p, cse& cw, resources& r, shared_resources& s,
                     index_flatten& f, index_traits& t)
      : ::momenta(),
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
