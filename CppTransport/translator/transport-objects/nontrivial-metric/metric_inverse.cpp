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


#include "metric_inverse.h"


namespace nontrivial_metric
  {
    
    nontrivial_metric_metric_inverse::nontrivial_metric_metric_inverse(language_printer& p, cse& cw,
                                                                       expression_cache& c, resources& r,
                                                                       shared_resources& s, boost::timer::cpu_timer& tm,
                                                                       index_flatten& f, index_traits& t)
      : ::metric_inverse(),
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
    nontrivial_metric_metric_inverse::compute(const index_literal_list& indices)
      {
        return std::unique_ptr<flattened_tensor>();
      }
    
    
    GiNaC::ex nontrivial_metric_metric_inverse::compute_component(field_index i, field_index j)
      {
        return GiNaC::ex();
      }
    
    
    std::unique_ptr<atomic_lambda>
    nontrivial_metric_metric_inverse::compute_lambda(const index_literal& i, const index_literal& j)
      {
        return std::unique_ptr<atomic_lambda>();
      }
    
    
    unroll_behaviour nontrivial_metric_metric_inverse::get_unroll()
      {
        if(res.can_roll_metric_inverse()) return unroll_behaviour::allow;
        
        return unroll_behaviour::force;   // can't roll-up
      }
    
    
    void nontrivial_metric_metric_inverse::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("metric inverse already cached");
        
        this->pre_lambda();
        
        this->cached = false;
      }
    
    
    void nontrivial_metric_metric_inverse::pre_lambda()
      {
        
      }
    
    
    void nontrivial_metric_metric_inverse::post()
      {
        if(!this->cached) throw tensor_exception("metric inverse not cached");
        
        // invalidate cache
        this->cached = false;
      }
    
  }   // namespace nontrivial_metric
