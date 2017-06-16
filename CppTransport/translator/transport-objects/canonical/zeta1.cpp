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

#include "zeta1.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_zeta1::compute(const index_literal_list& indices)
      {
        if(indices.size() != ZETA1_TENSOR_INDICES) throw tensor_exception("zeta1 indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(ZETA1_TENSOR_INDICES));
    
        const phase_index max_i = this->shared.get_max_phase_index(indices[0]->get_variance());

        this->cached = false;

        for(phase_index i = phase_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            (*result)[this->fl.flatten(i)] = this->compute_component(i);
          }

        return(result);
      }


    GiNaC::ex canonical_zeta1::compute_component(phase_index i)
      {
        unsigned int index = this->fl.flatten(i);
        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(0, this->printer);

        if(!cached) { this->populate_workspace(); this->cache_symbols(); this->cached = true; }

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::zxfm1_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(this->traits.is_species(i))
              {
                GiNaC::symbol& deriv_i = (*derivs)[this->fl.flatten(i)];
                result = this->expr(deriv_i);
              }
            else if(this->traits.is_momentum(i))
              {
                result = 0;
              }
            else
              {
                // TODO: prefer to throw exception
                assert(false);
              }

            this->cache.store(expression_item_types::zxfm1_item, index, *args, result);
          }

        return(result);
      }


    GiNaC::ex canonical_zeta1::expr(GiNaC::symbol& deriv_i)
      {
        return -deriv_i / (2*Mp*Mp*eps);
      }


    void canonical_zeta1::cache_symbols()
      {
        eps = this->res.eps_resource(this->cse_worker, this->printer);
        Mp = this->shared.generate_Mp();
      }


    void canonical_zeta1::populate_workspace()
      {
        derivs = this->shared.generate_derivs(this->printer);
      }


    unroll_behaviour canonical_zeta1::get_unroll()
      {
        if(this->shared.can_roll_coordinates()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<map_lambda> canonical_zeta1::compute_lambda(const abstract_index& i)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("U3");

        GiNaC::idx idx_i = this->shared.generate_index(i);

        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(0, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));

        // convert these indices to species-only indices
        const abstract_index i_field_a = this->traits.species_to_species(i);
        const abstract_index i_field_b = this->traits.momentum_to_species(i);

        map_lambda_table table(lambda_flattened_map_size(1));

        GiNaC::symbol deriv_a_i = this->shared.generate_derivs(i_field_a, this->printer);

        this->cache_symbols();

        table[lambda_flatten(LAMBDA_FIELD)] = this->expr(deriv_a_i);
        table[lambda_flatten(LAMBDA_MOMENTUM)] = 0;

        return std::make_unique<map_lambda>(i, table, expression_item_types::zxfm1_lambda, *args, this->shared.generate_working_type());
      }

  }   // namespace canonical
