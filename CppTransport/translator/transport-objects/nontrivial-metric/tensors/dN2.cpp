//
// Created by David Seery on 20/12/2015.
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

#include "dN2.h"


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> dN2::compute(const index_literal_list& indices)
      {
        if(indices.size() != DN2_TENSOR_INDICES) throw tensor_exception("dN2 indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(DN2_TENSOR_INDICES));

        const phase_index max_i = this->shared.get_max_phase_index(indices[0]->get_variance());
        const phase_index max_j = this->shared.get_max_phase_index(indices[1]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(phase_index i = phase_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            for(phase_index j = phase_index(0, indices[1]->get_variance()); j < max_j; ++j)
              {
                (*result)[this->fl.flatten(i, j)] = this->compute_component(i, j);
              }
          }

        return(result);
      }
    
    
    GiNaC::ex dN2::compute_component(phase_index i, phase_index j)
      {
        if(!this->cached) throw tensor_exception("dN2 cache not ready");

        unsigned int index = this->fl.flatten(i, j);
        auto args = this->res.generate_cache_arguments(use_dV, this->printer);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::dN2_item, index, args, result))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::symbol coord_i = this->traits.is_species(i) ? (*fields)[this->fl.flatten(i)] : (*derivs)[this->fl.flatten(this->traits.to_species(i))];
            GiNaC::symbol coord_j = this->traits.is_species(j) ? (*fields)[this->fl.flatten(j)] : (*derivs)[this->fl.flatten(this->traits.to_species(j))];

            result = -1/(2*dotH) * diff(diff(Hsq, coord_i), coord_j)
                     - diff(1/(2*dotH), coord_i) * diff(Hsq, coord_j)
                     - diff(1/(2*dotH), coord_j) * diff(Hsq, coord_i)
                     - 1/(2*dotH) * p * diff(Hsq, coord_i) * diff(Hsq, coord_j);

            this->cache.store(expression_item_types::dN2_item, index, args, result);
          }

        return(result);
      }
    
    
    unroll_behaviour dN2::get_unroll(const index_literal_list& idx_list)
      {
        return unroll_behaviour::force;   // currently can't roll-up delta-N expressions
      }
    
    
    dN2::dN2(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
             boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
      : ::dN2(),
        printer(p),
        cse_worker(cw),
        cache(c),
        res(r),
        shared(s),
        fl(f),
        traits(t),
        compute_timer(tm),
        cached(false)
      {
      }
    
    
    void dN2::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("dN2 already cached");

        fields = this->shared.generate_field_symbols(this->printer);
        derivs = this->shared.generate_deriv_symbols(this->printer);
    
        dV = this->res.dV_resource(variance::covariant, this->printer);

        Hsq = this->res.raw_Hsq_resource(this->printer);
        eps = this->res.raw_eps_resource(this->printer);
        dotH = -eps*Hsq;

        this->p = 0;

        field_index max_i = this->shared.get_max_field_index(variance::none);

        for(field_index i = field_index(0, variance::none); i < max_i; ++i)
          {
            this->p += diff(1 / (2 * dotH), (*fields)[this->fl.flatten(i)]) * (*derivs)[this->fl.flatten(i)];

            // derivs is contravariance, dV is covariant
            GiNaC::ex dXdN = (eps - 3) * (*derivs)[this->fl.flatten(i)] - (*dV)[this->fl.flatten(i)] / Hsq;

            this->p += diff(1 / (2 * dotH), (*derivs)[this->fl.flatten(i)]) * dXdN;
          }

        this->cached = true;
      }
    
    
    void dN2::post()
      {
        if(!this->cached) throw tensor_exception("dN2 not cached");

        // invalidate cache
        this->cached = false;
      }

  }   // namespace nontrivial_metric
