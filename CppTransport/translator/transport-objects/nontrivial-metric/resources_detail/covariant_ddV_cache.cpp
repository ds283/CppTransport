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

#include "covariant_ddV_cache.h"
#include "covariant_dV_cache.h"
#include "connexion_cache.h"
#include "potential_cache.h"


namespace nontrivial_metric
  {
    
    const flattened_tensor& CovariantddVCache::get()
      {
        if(this->ddV) return *this->ddV;
        
        this->ddV = std::make_unique<flattened_tensor>(res.fl.get_flattened_size<field_index>(RESOURCE_INDICES::DDV_INDICES));
        
        const field_index max = res.share.get_max_field_index(variance::covariant);
        const field_index max_k = res.share.get_max_field_index(variance::contravariant);
        
        // build argument list and tag as two covariant indices
        auto args = res.generate_cache_arguments(printer);
        
        GiNaC::varidx idx_i(res.sym_factory.get_symbol(I_INDEX_NAME), static_cast<unsigned int>(max), true);
        GiNaC::varidx idx_j(res.sym_factory.get_symbol(J_INDEX_NAME), static_cast<unsigned int>(max), true);
        args += { idx_i, idx_j };
        
        // obtain resource caches
        PotentialResourceCache cache(res, res.share, printer);
        CovariantdVCache dV_cache(res, printer);
        ConnexionCache Gamma_cache(res, printer);
        
        for(field_index i = field_index(0, variance::covariant); i < max; ++i)
          {
            for(field_index j = field_index(0, variance::covariant); j <= i; ++j)
              {
                GiNaC::ex ddV;
                unsigned int index_ij = res.fl.flatten(i,j);
                
                if(!res.cache.query(expression_item_types::ddV_item, index_ij, args, ddV))
                  {
                    timing_instrument timer(res.compute_timer);
    
                    auto& dV = dV_cache.get();
                    const symbol_list& f_list = cache.get_symbol_list();

                    // partial derivative term is partial_j V;i
                    const GiNaC::symbol& x2 = f_list[res.fl.flatten(j)];
                    ddV = GiNaC::diff(dV[res.fl.flatten(i)], x2);
                    
                    // include connexion term -Gamma^k_ij V;k
                    auto& Gamma = Gamma_cache.get();
                    
                    for(field_index k = field_index(0, variance::contravariant); k < max_k; ++k)
                      {
                        ddV -= Gamma[res.fl.flatten(k,i,j)] * dV[res.fl.flatten(k)];
                      }
                    
                    res.cache.store(expression_item_types::ddV_item, index_ij, args, ddV);
                  }
                
                (*this->ddV)[index_ij] = ddV;
                
                // second derivative is symmetric, so don't bother computing the ji term
                unsigned int index_ji = res.fl.flatten(j,i);
                (*this->ddV)[index_ji] = ddV;
              }
          }
        
        return *this->ddV;
      }
    
  }   // namespace nontrivial_metric
