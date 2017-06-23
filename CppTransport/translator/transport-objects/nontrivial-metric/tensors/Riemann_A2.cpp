//
// Created by David Seery on 21/06/2017.
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


#include "Riemann_A2.h"


namespace nontrivial_metric
  {
    
    Riemann_A2::Riemann_A2(language_printer& p, cse& cw, expression_cache& c,
                           resources& r, shared_resources& s,
                           boost::timer::cpu_timer& tm, index_flatten& f,
                           index_traits& t)
      : ::Riemann_A2(),
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
    
    
    std::unique_ptr<flattened_tensor>
    Riemann_A2::compute(const index_literal_list& indices)
      {
        return std::unique_ptr<flattened_tensor>();
      }
    
    
    GiNaC::ex Riemann_A2::compute_component(field_index i, field_index j)
      {
        return GiNaC::ex();
      }
    
    
    std::unique_ptr<atomic_lambda>
    Riemann_A2::compute_lambda(const index_literal& i, const index_literal& j)
      {
        return std::unique_ptr<atomic_lambda>();
      }
    
    
    unroll_behaviour Riemann_A2::get_unroll(const index_literal_list& idx_list)
      {
        const std::array< variance, RESOURCE_INDICES::RIEMANN_A2_INDICES > ij = { idx_list[0]->get_variance(), idx_list[1]->get_variance() };

        if(res.can_roll_Riemann_A2(ij)) return unroll_behaviour::allow;
        
        return unroll_behaviour::force;   // can't roll-up
      }
    
    
    void Riemann_A2::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("Riemann_A2 already cached");
        
        this->pre_lambda();
        
        this->cached = false;
      }
    
    
    void Riemann_A2::pre_lambda()
      {
        
      }
    
    
    void Riemann_A2::post()
      {
        if(!this->cached) throw tensor_exception("Riemann_A2 not cached");
        
        // invalidate cache
        this->cached = false;
      }
    
  }   // namespace nontrivial_metric
