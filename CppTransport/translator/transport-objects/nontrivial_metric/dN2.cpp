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


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_dN2::compute()
      {
        std::unique_ptr<flattened_tensor> result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(2));

        const phase_index num_phase = this->shared.get_number_phase();
        this->cached = false;

        for(phase_index i = phase_index(0); i < num_phase; ++i)
          {
            for(phase_index j = phase_index(0); j < num_phase; ++j)
              {
                (*result)[this->fl.flatten(i, j)] = this->compute_component(i, j);
              }
          }

        return(result);
      }


    GiNaC::ex canonical_dN2::compute_component(phase_index i, phase_index j)
      {
        unsigned int index = this->fl.flatten(i, j);
        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(use_dV_argument, this->printer);

        if(!cached) { this->populate_workspace(); this->cache_symbols(); this->cached = true; }

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::dN2_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::symbol coord_i = this->traits.is_species(i) ? (*fields)[this->fl.flatten(i)] : (*derivs)[this->fl.flatten(this->traits.to_species(i))];
            GiNaC::symbol coord_j = this->traits.is_species(j) ? (*fields)[this->fl.flatten(j)] : (*derivs)[this->fl.flatten(this->traits.to_species(j))];

            result = -1/(2*dotH) * diff(diff(Hsq, coord_i), coord_j)
                     - diff(1/(2*dotH), coord_i) * diff(Hsq, coord_j)
                     - diff(1/(2*dotH), coord_j) * diff(Hsq, coord_i)
                     - 1/(2*dotH) * p * diff(Hsq, coord_i) * diff(Hsq, coord_j);

            this->cache.store(expression_item_types::dN2_item, index, *args, result);
          }

        return(result);
      }


    void canonical_dN2::cache_symbols()
      {
        Hsq = this->res.raw_Hsq_resource(this->printer);
        eps = this->res.raw_eps_resource(this->printer);
        dotH = -eps*Hsq;

        std::unique_ptr<symbol_list> f = this->shared.generate_fields(this->printer);
        std::unique_ptr<symbol_list> d = this->shared.generate_derivs(this->printer);
        std::unique_ptr<flattened_tensor> Vi = this->res.dV_resource(this->printer);

        p = 0;
        field_index num_fields = this->shared.get_number_field();
        for(field_index i = field_index(0); i < num_fields; ++i)
          {
            p += diff(1/(2*dotH), (*f)[this->fl.flatten(i)]) * (*d)[this->fl.flatten(i)];

            GiNaC::ex dXdN = (eps-3) * (*d)[this->fl.flatten(i)] - (*Vi)[this->fl.flatten(i)]/Hsq;

            p += diff(1/(2*dotH), (*d)[this->fl.flatten(i)]) * dXdN;
          }
      }


    void canonical_dN2::populate_workspace()
      {
        fields = this->shared.generate_fields(this->printer);
        derivs = this->shared.generate_derivs(this->printer);
        dV = this->res.dV_resource(this->printer);
      }


    unroll_behaviour canonical_dN2::get_unroll()
      {
        return unroll_behaviour::force;   // currently can't roll-up delta-N expressions
      }
  }   // namespace canonical
