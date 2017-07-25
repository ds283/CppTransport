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

#include "connexion_cache.h"
#include "substitution_map_cache.h"

namespace nontrivial_metric
  {
    
    const flattened_tensor& ConnexionCache::get()
      {
        if(this->Gamma) return *this->Gamma;
        
        this->Gamma = std::make_unique<flattened_tensor>(res.fl.get_flattened_size<field_index>(RESOURCE_INDICES::CONNEXION_INDICES));
        
        auto args = res.generate_cache_arguments(printer);
        // no need to tag with indices, since only one index arrangement is possible
        
        const auto max_i = res.share.get_max_field_index(variance::contravariant);
        const auto max_jk = res.share.get_max_field_index(variance::covariant);
        
        SubstitutionMapCache subs_cache(res, printer);
        
        for(field_index i = field_index(0, variance::contravariant); i < max_i; ++i)
          {
            for(field_index j = field_index(0, variance::covariant); j < max_jk; ++j)
              {
                for(field_index k = field_index(0, variance::covariant); k <= j; ++k)
                  {
                    unsigned int index_ijk = res.fl.flatten(i,j,k);
                    
                    // if no substitutions to be done, avoid checking cache
                    if(args.empty())
                      {
                        (*Gamma)[index_ijk] =  (*res.Crstfl)(static_cast<unsigned int>(i), static_cast<unsigned int>(j), static_cast<unsigned int>(k));
                      }
                    else
                      {
                        GiNaC::ex subs_Gamma = 0;
                        
                        if(!res.cache.query(expression_item_types::connexion_item, index_ijk, args, subs_Gamma))
                          {
                            timing_instrument timer(res.compute_timer);
                            
                            // get substitution map
                            GiNaC::exmap& subs_map = subs_cache.get();
                            
                            //! apply substitution to connexion component and cache the result
                            subs_Gamma = (*res.Crstfl)(static_cast<unsigned int>(i), static_cast<unsigned int>(j), static_cast<unsigned int>(k)).subs(subs_map, GiNaC::subs_options::no_pattern);
                            res.cache.store(expression_item_types::connexion_item, index_ijk, args, subs_Gamma);
                          }
                        
                        (*Gamma)[index_ijk] = subs_Gamma;
                      }
                    
                    if(j != k)
                      {
                        unsigned int index_ikj = res.fl.flatten(i,k,j);
                        (*Gamma)[index_ikj] = (*Gamma)[index_ijk];
                      }
                  }
              }
          }
        
        return *this->Gamma;
      }
    
  }   // namespace nontrivial_metric