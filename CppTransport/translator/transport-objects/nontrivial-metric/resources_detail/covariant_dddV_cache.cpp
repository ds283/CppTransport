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

#include "covariant_dddV_cache.h"
#include "covariant_ddV_cache.h"
#include "connexion_cache.h"
#include "potential_cache.h"


namespace nontrivial_metric
  {

    const flattened_tensor& CovariantdddVCache::get()
      {
        if(this->dddV) return *this->dddV;
        
        this->dddV = std::make_unique<flattened_tensor>(res.fl.get_flattened_size<field_index>(RESOURCE_INDICES::DDDV_INDICES));
        
        const field_index max = res.share.get_max_field_index(variance::covariant);
        const field_index max_l = res.share.get_max_field_index(variance::contravariant);
        
        // build argument list and tag as two covariant indices
        auto args = res.generate_cache_arguments(printer);
        
        GiNaC::varidx idx_i(res.sym_factory.get_symbol(I_INDEX_NAME), static_cast<unsigned int>(max), true);
        GiNaC::varidx idx_j(res.sym_factory.get_symbol(J_INDEX_NAME), static_cast<unsigned int>(max), true);
        GiNaC::varidx idx_k(res.sym_factory.get_symbol(K_INDEX_NAME), static_cast<unsigned int>(max), true);
        args += { idx_i, idx_j, idx_k };
        
        // obtain resource caches
        PotentialResourceCache cache(res, res.share, printer);
        CovariantddVCache ddV_cache(res, printer);
        ConnexionCache Gamma_cache(res, printer);
        
        for(field_index i = field_index(0, variance::covariant); i < max; ++i)
          {
            for(field_index j = field_index(0, variance::covariant); j < max; ++j)
              {
                for(field_index k = field_index(0, variance::covariant); k < max; ++k)
                  {
                    GiNaC::ex dddV;
                    unsigned int index = res.fl.flatten(i,j);
                    
                    if(!res.cache.query(expression_item_types::dddV_item, index, args, dddV))
                      {
                        timing_instrument timer(res.compute_timer);
                        
                        const GiNaC::ex& V = cache.get_V();
                        const symbol_list& f_list = cache.get_symbol_list();
                        
                        const GiNaC::symbol& x1 = f_list[res.fl.flatten(i)];
                        const GiNaC::symbol& x2 = f_list[res.fl.flatten(j)];
                        const GiNaC::symbol& x3 = f_list[res.fl.flatten(k)];
                        dddV = GiNaC::diff(GiNaC::diff(GiNaC::diff(V, x1), x2), x3);
                        
                        // include connexion term
                        auto& ddV = ddV_cache.get();
                        auto& Gamma = Gamma_cache.get();
                        
                        for(field_index l = field_index(0, variance::contravariant); l < max_l; ++l)
                          {
                            dddV -= Gamma[res.fl.flatten(l,i,k)] * ddV[res.fl.flatten(l,j)]
                                    + Gamma[res.fl.flatten(l,j,k)] * ddV[res.fl.flatten(l,i)];
                          }
                        
                        res.cache.store(expression_item_types::dddV_item, index, args, dddV);
                      }
                    
                    (*this->dddV)[index] = dddV;
                  }
              }
          }
        
        return *this->dddV;
      }
    
  }   // namespace nontrivial_metric
