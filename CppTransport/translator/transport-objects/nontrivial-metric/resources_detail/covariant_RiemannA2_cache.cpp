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

#include "covariant_RiemannA2_cache.h"
#include "substitution_map_cache.h"
#include "derivative_symbols.h"


namespace nontrivial_metric
  {
    
    const flattened_tensor& CovariantRiemannA2Cache::get()
      {
        if(this->A2) return *this->A2;
        
        auto args = res.generate_cache_arguments(printer);
        
        this->A2 = std::make_unique<flattened_tensor>(res.fl.get_flattened_size<field_index>(RESOURCE_INDICES::RIEMANN_A2_INDICES));
        
        const auto max = res.share.get_max_field_index(variance::covariant);
        const auto max_lm = res.share.get_max_field_index(variance::contravariant);
        
        SubstitutionMapCache subs_cache(res, printer);
        DerivativeSymbolsCache deriv_cache(res, res.share, printer);
        
        for(field_index i = field_index(0, variance::covariant); i < max; ++i)
          {
            for(field_index j = field_index(0, variance::covariant); j <= i; ++j)
              {
                unsigned int index_ij = res.fl.flatten(i,j);
                
                GiNaC::ex subs_expr = 0;
                
                if(!res.cache.query(expression_item_types::Riemann_A2_item, index_ij, args, subs_expr))
                  {
                    timing_instrument timer(res.compute_timer);
                    
                    auto& deriv_syms = deriv_cache.get();
                    GiNaC::ex expr = 0;
                    
                    for(field_index l = field_index(0, variance::contravariant); l < max_lm; ++l)
                      {
                        for(field_index m = field_index(0, variance::contravariant); m < max_lm; ++m)
                          {
                            auto Rie_ij = (*res.Rie_T)(static_cast<unsigned int>(l), static_cast<unsigned int>(i),
                                                       static_cast<unsigned int>(j), static_cast<unsigned int>(m));
                            auto Rie_ji = (*res.Rie_T)(static_cast<unsigned int>(l), static_cast<unsigned int>(j),
                                                       static_cast<unsigned int>(i), static_cast<unsigned int>(m));
                            auto Rie_sym = (Rie_ij + Rie_ji) / 2;
                            
                            expr += Rie_sym * deriv_syms[res.fl.flatten(l)] * deriv_syms[res.fl.flatten(m)];
                          }
                      }
                    
                    // get substitution map
                    GiNaC::exmap& subs_map = subs_cache.get();
                    subs_expr = expr.subs(subs_map, GiNaC::subs_options::no_pattern);
    
                    res.cache.store(expression_item_types::Riemann_A2_item, index_ij, args, subs_expr);
                  }
                
                (*this->A2)[index_ij] = subs_expr;
                
                if(i != j)
                  {
                    unsigned int index_ji = res.fl.flatten(j,i);
                    (*this->A2)[index_ji] = subs_expr;
                  }
              }
          }
        
        return *this->A2;
      }
    
  }   // namespace nontrivial_metric
