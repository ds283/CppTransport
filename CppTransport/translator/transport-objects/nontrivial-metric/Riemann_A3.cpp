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


#include "Riemann_A3.h"


namespace nontrivial_metric
  {
    
    nontrivial_metric_Riemann_A3::nontrivial_metric_Riemann_A3(language_printer& p, cse& cw, expression_cache& c,
                                                               resources& r, shared_resources& s,
                                                               boost::timer::cpu_timer& tm, index_flatten& f,
                                                               index_traits& t)
      : ::Riemann_A3(),
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
    nontrivial_metric_Riemann_A3::compute(const index_literal_list& indices)
      {
        return std::unique_ptr<flattened_tensor>();
      }
    
    
    GiNaC::ex nontrivial_metric_Riemann_A3::compute_component(field_index i, field_index j, field_index k)
      {
        return GiNaC::ex();
      }
    
    
    std::unique_ptr<atomic_lambda>
    nontrivial_metric_Riemann_A3::compute_lambda(const index_literal& i, const index_literal& j, const index_literal& k)
      {
        return std::unique_ptr<atomic_lambda>();
      }
    
    
    unroll_behaviour nontrivial_metric_Riemann_A3::get_unroll()
      {
        if(res.can_roll_Riemann_A3()) return unroll_behaviour::allow;
        
        return unroll_behaviour::force;   // can't roll-up
      }
    
    
    void nontrivial_metric_Riemann_A3::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("Riemann_A3 already cached");
        
        this->pre_lambda();
        
        this->cached = false;
      }
    
    
    void nontrivial_metric_Riemann_A3::pre_lambda()
      {
      
      }
    
    
    void nontrivial_metric_Riemann_A3::post()
      {
        if(!this->cached) throw tensor_exception("Riemann_A3 not cached");
        
        // invalidate cache
        this->cached = false;
      }
    
  }   // namespace nontrivial_metric
