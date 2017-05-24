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

#include "dN1.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_dN1::compute()
      {
        std::unique_ptr<flattened_tensor> result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(1));

        const phase_index num_phase = this->shared.get_number_phase();
        this->cached = false;

        for(phase_index i = phase_index(0); i < num_phase; ++i)
          {
            (*result)[this->fl.flatten(i)] = this->compute_component(i);
          }

        return(result);
      }


    GiNaC::ex canonical_dN1::compute_component(phase_index i)
      {
        unsigned int index = this->fl.flatten(i);
        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(0, this->printer);

        if(!cached) { this->populate_workspace(); this->cache_symbols(); this->cached = true; }

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::dN1_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            field_index i_species = this->traits.to_species(i);
            result = -(1/(2*dotH)) * diff(Hsq, (*fields)[this->fl.flatten(i_species)]);

            this->cache.store(expression_item_types::dN1_item, index, *args, result);
          }

        return(result);
      }


    void canonical_dN1::cache_symbols()
      {
        Hsq = this->res.raw_Hsq_resource(this->printer);
        eps = this->res.raw_eps_resource(this->printer);
        dotH = -eps*Hsq;
      }


    void canonical_dN1::populate_workspace()
      {
        fields = this->shared.generate_fields(this->printer);
      }


    unroll_behaviour canonical_dN1::get_unroll()
      {
        return unroll_behaviour::force;   // currently can't roll-up delta-N expressions
      }
  }   // namespace canonical
