//
// Created by David Seery on 04/07/2017.
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

#include "covariant_dV_cache.h"
#include "potential_cache.h"


namespace nontrivial_metric
  {
    
    const flattened_tensor& CovariantdVCache::get()
      {
        if(this->dV) return *this->dV;
        
        this->dV = std::make_unique<flattened_tensor>(res.fl.get_flattened_size<field_index>(RESOURCE_INDICES::DV_INDICES));
        
        const field_index max = res.share.get_max_field_index(variance::covariant);
        
        //! build argument list and tag as a covariant index
        auto args = res.generate_cache_arguments(printer);
        
        GiNaC::varidx idx_i(res.sym_factory.get_real_symbol(I_INDEX_NAME), static_cast<unsigned int>(max), true);
        args += idx_i;
        
        // obtain a resource cache for the components of the potential
        PotentialResourceCache cache(res, res.share, printer);
        
        for(field_index i = field_index(0, variance::covariant); i < max; ++i)
          {
            GiNaC::ex dV;
            unsigned int index = res.fl.flatten(i);
            
            if(!res.cache.query(expression_item_types::dV_item, index, args, dV))
              {
                timing_instrument timer(res.compute_timer);
    
                const GiNaC::ex& V = cache.get_V();
                const symbol_list& f_list = cache.get_symbol_list();
                
                const symbol_wrapper& x1 = f_list[res.fl.flatten(i)];
                dV = GiNaC::diff(V, x1);
                
                res.cache.store(expression_item_types::dV_item, index, args, dV);
              }
            
            (*this->dV)[index] = dV;
          }
        
        return *this->dV;
      }
    
  }   // namespace nontrivial_metric
