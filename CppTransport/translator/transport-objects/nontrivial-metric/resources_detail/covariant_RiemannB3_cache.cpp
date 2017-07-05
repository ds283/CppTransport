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

#include "covariant_RiemannB3_cache.h"
#include "substitution_map_cache.h"
#include "derivative_symbols.h"


namespace nontrivial_metric
  {

    const flattened_tensor& CovariantRiemannB3Cache::get()
      {
        if(this->B3) return *this->B3;
        
        this->B3 = std::make_unique<flattened_tensor>(res.fl.get_flattened_size<field_index>(RESOURCE_INDICES::RIEMANN_B3_INDICES));
    
        const auto max = res.share.get_max_field_index(variance::covariant);
    
        for(field_index i = field_index(0, variance::covariant); i < max; ++i)
          {
            for(field_index j = field_index(0, variance::covariant); j < max; ++j)
              {
                for(field_index k = field_index(0, variance::covariant); k < max; ++k)
                  {
                    unsigned int index = res.fl.flatten(i,j,k);
                
                    (*this->B3)[index] = 0;
                  }
              }
          }
    
        return *this->B3;
      }
    
  }   // namespace nontrivial_metric
