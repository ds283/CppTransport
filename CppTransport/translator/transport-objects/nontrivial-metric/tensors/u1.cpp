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


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> u1::compute(const index_literal_list& indices)
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
    
    
    GiNaC::ex u1::compute_component(phase_index i)
      {
        if(!this->cached) throw tensor_exception("U1 cache not ready");

        unsigned int index = this->fl.flatten(i);
    
        // tag with variance information -- need to keep results of different variance distinct
        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
    
        auto args = this->res.generate_cache_arguments(use_dV, std::array<phase_index, 1>{i}, this->printer);
        args += { idx_i };

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::U1_item, index, args, result))
          {
            timing_instrument timer(this->compute_timer);

            field_index species_i = this->traits.to_species(i);
            auto& deriv_i = this->derivs(species_i)[this->fl.flatten(species_i)];

            if(this->traits.is_species(i))
              {
                result = deriv_i;
              }
            else if(this->traits.is_momentum(i))
              {
                auto& Vi = this->dV(species_i)[this->fl.flatten(species_i)];
                result = this->expr_momentum(Vi, deriv_i);
              }
            else throw tensor_exception("U1 index");

            this->cache.store(expression_item_types::U1_item, index, args, result);
          }

        return(result);
      }
    
    
    GiNaC::ex u1::expr_momentum(const GiNaC::ex& Vi, const GiNaC::ex& deriv_i)
      {
        return -(3-eps) * deriv_i - Vi/Hsq;
      }
    
    
    unroll_state u1::get_unroll(const index_literal_list& idx_list)
      {
        const std::array< variance, RESOURCE_INDICES::DV_INDICES > i = { idx_list[0]->get_variance() };
    
        // if the index is covariant then we need the metric to pull down an index on the coordinate vector
        // if it is contravariant then we need the inverse metric to push up an index on dV
        bool has_G = true;
        bool has_Ginv = true;
        if(idx_list[0]->get_variance() == variance::covariant)
          {
            has_G = this->res.can_roll_metric();
          }
        if(idx_list[0]->get_variance() == variance::contravariant)
          {
            has_Ginv = this->res.can_roll_metric_inverse();
          }
    
        if(this->shared.can_roll_coordinates() && has_G && has_Ginv
           && this->res.can_roll_dV(i)) return unroll_state::allow;
        
        return unroll_state::force;   // can't roll-up
      }
    
    
    std::unique_ptr<map_lambda> u1::compute_lambda(const index_literal& i)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("U1");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);

        // convert this index to species-only indices
        const auto i_field_a = this->traits.species_to_species(i);
        const auto i_field_b = this->traits.momentum_to_species(i);

        auto deriv_a_i = this->res.generate_deriv_vector(*i_field_a.second, this->printer);
        auto deriv_b_i = this->res.generate_deriv_vector(*i_field_b.second, this->printer);

        this->pre_lambda();

        std::vector<GiNaC::ex> map(lambda_flattened_map_size(1));

        map[lambda_flatten(LAMBDA_FIELD)] = deriv_a_i;

        auto args =
          this->res.generate_cache_arguments(use_dV, std::array<index_literal, 1>{i}, this->printer);
        args += idx_i;

        if(!this->cache.query(expression_item_types::U1_lambda, 0, args, map[lambda_flatten(LAMBDA_MOMENTUM)]))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::ex V_b_i = this->res.dV_resource(*i_field_b.second, this->printer);

            map[lambda_flatten(LAMBDA_MOMENTUM)] = this->expr_momentum(V_b_i, deriv_b_i);

            this->cache.store(expression_item_types::U1_lambda, 0, args, map[lambda_flatten(LAMBDA_MOMENTUM)]);
          }

        return std::make_unique<map_lambda>(i, map, expression_item_types::U1_lambda, args, this->shared.generate_working_type());
      }
    
    
    u1::u1(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
           boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
      : ::u1(),
        printer(p),
        cse_worker(cw),
        cache(c),
        res(r),
        shared(s),
        fl(f),
        traits(t),
        compute_timer(tm),
        cached(false),
        derivs([&](auto k) -> auto { return res.generate_deriv_vector(k[0], printer); }),
        dV([&](auto k) -> auto { return res.dV_resource(k[0], printer); })
      {
      }
    
    
    void u1::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("U1 already cached");

        this->pre_lambda();

        this->cached = true;
      }
    
    
    void u1::pre_lambda()
      {
        Hsq = this->res.Hsq_resource(this->cse_worker, this->printer);
        eps = this->res.eps_resource(this->cse_worker, this->printer);
      }
    
    
    void u1::post()
      {
        if(!this->cached) throw tensor_exception("U1 not cached");

        this->derivs.clear();
        this->dV.clear();

        // invalidate cache
        this->cached = false;
      }

  }   // namespace nontrivial_metric
