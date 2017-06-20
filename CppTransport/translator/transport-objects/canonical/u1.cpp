//
// Created by David Seery on 19/12/2015.
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

#include "u1.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_u1::compute(const index_literal_list& indices)
      {
        if(indices.size() != U1_TENSOR_INDICES) throw tensor_exception("U1 indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(U1_TENSOR_INDICES));
    
        const phase_index max_i = this->shared.get_max_phase_index(indices[0]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(phase_index i = phase_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            (*result)[this->fl.flatten(i)] = this->compute_component(i);
          }

        return(result);
      }


    GiNaC::ex canonical_u1::compute_component(phase_index i)
      {
        if(!this->cached) throw tensor_exception("U1 cache not ready");

        unsigned int index = this->fl.flatten(i);
        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(use_dV_argument, this->printer);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::U1_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            field_index species_i = this->traits.to_species(i);

            auto& deriv_i = (*derivs)[this->fl.flatten(species_i)];

            if(this->traits.is_species(i))
              {
                result = deriv_i;
              }
            else if(this->traits.is_momentum(i))
              {
                GiNaC::ex& Vi = (*dV)[this->fl.flatten(species_i)];
                result = this->expr_momentum(Vi, deriv_i);
              }
            else
              {
                // TODO: prefer to throw exception
                assert(false);
              }

            this->cache.store(expression_item_types::U1_item, index, *args, result);
          }

        return(result);
      }


    GiNaC::ex canonical_u1::expr_momentum(GiNaC::ex& Vi, GiNaC::ex& deriv_i)
      {
        return -(3-eps) * deriv_i - Vi/Hsq;
      }


    unroll_behaviour canonical_u1::get_unroll()
      {
        if(this->shared.can_roll_coordinates() && this->res.can_roll_dV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<map_lambda> canonical_u1::compute_lambda(const index_literal& i)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("U1");

        auto idx_i = this->shared.generate_index<GiNaC::idx>(i);

        // convert this index to species-only indices
        const auto i_field_a = this->traits.species_to_species(i);
        const auto i_field_b = this->traits.momentum_to_species(i);

        auto deriv_a_i = this->res.generate_deriv_vector(*i_field_a.second, this->printer);
        auto deriv_b_i = this->res.generate_deriv_vector(*i_field_b.second, this->printer);

        this->pre_lambda();

        std::vector<GiNaC::ex> map(lambda_flattened_map_size(1));

        map[lambda_flatten(LAMBDA_FIELD)] = deriv_a_i;

        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(use_dV_argument, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));

        if(!this->cache.query(expression_item_types::U1_lambda, 0, *args, map[lambda_flatten(LAMBDA_MOMENTUM)]))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::ex V_b_i = this->res.dV_resource(*i_field_b.second, this->printer);

            map[lambda_flatten(LAMBDA_MOMENTUM)] = this->expr_momentum(V_b_i, deriv_b_i);

            this->cache.store(expression_item_types::U1_lambda, 0, *args, map[lambda_flatten(LAMBDA_MOMENTUM)]);
          }

        return std::make_unique<map_lambda>(i, map, expression_item_types::U1_lambda, *args, this->shared.generate_working_type());
      }


    canonical_u1::canonical_u1(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
                               boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
      : u1(),
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


    void canonical_u1::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("U1 already cached");

        this->pre_lambda();
        derivs = this->res.generate_deriv_vector(this->printer);
        dV = this->res.dV_resource(this->printer);

        this->cached = true;
      }


    void canonical_u1::pre_lambda()
      {
        Hsq = this->res.Hsq_resource(this->cse_worker, this->printer);
        eps = this->res.eps_resource(this->cse_worker, this->printer);
      }


    void canonical_u1::post()
      {
        if(!this->cached) throw tensor_exception("U1 not cached");

        // invalidate cache
        this->cached = false;
      }

  }   // namespace canonical
