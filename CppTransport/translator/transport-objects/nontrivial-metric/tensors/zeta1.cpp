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


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> zeta1::compute(const index_literal_list& indices)
      {
        if(indices.size() != ZETA1_TENSOR_INDICES) throw tensor_exception("zeta1 indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(ZETA1_TENSOR_INDICES));
    
        const phase_index max_i = this->shared.get_max_phase_index(indices[0]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(phase_index i = phase_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            (*result)[this->fl.flatten(i)] = this->compute_component(i);
          }

        return(result);
      }
    
    
    GiNaC::ex zeta1::compute_component(phase_index i)
      {
        if(!this->cached) throw tensor_exception("zeta1 cache not ready");

        unsigned int index = this->fl.flatten(i);
    
        // tag with variance information -- need to keep results of different variance distinct
        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
    
        auto args = this->res.generate_cache_arguments<phase_index>(0, {i}, this->printer);
        args += { idx_i };

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::zxfm1_item, index, args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(this->traits.is_species(i))
              {
                auto& deriv_i = this->derivs(i)[this->fl.flatten(i)];
                result = this->expr(deriv_i);
              }
            else if(this->traits.is_momentum(i))
              {
                result = 0;
              }
            else throw tensor_exception("zeta1 index");

            this->cache.store(expression_item_types::zxfm1_item, index, args, result);
          }

        return(result);
      }
    
    
    GiNaC::ex zeta1::expr(const GiNaC::ex& deriv_i)
      {
        return -deriv_i / (2*Mp*Mp*eps);
      }


    unroll_state zeta1::get_unroll(const index_literal_list& idx_list)
      {
        // if our index is covariant then we need the metric to pull down an index on the coordinate vector
        // there are no occurrence of the potential, so we don't need the inverse metric to push their indices up
        bool has_G = true;
        if(idx_list[0]->get_variance() == variance::covariant)
          {
            has_G = this->res.can_roll_metric();
          }

        if(this->shared.can_roll_coordinates() && has_G) return unroll_state::allow;
        return unroll_state::force;   // can't roll-up
      }
    
    
    std::unique_ptr<map_lambda> zeta1::compute_lambda(const index_literal& i)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("U3");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);

        auto args = this->res.generate_cache_arguments<index_literal>(0, {i}, this->printer);
        args += idx_i;

        // convert these indices to species-only indices
        const auto i_field_a = this->traits.species_to_species(i);
    
        this->pre_lambda();

        map_lambda_table table(lambda_flattened_map_size(1));

        auto deriv_a_i = this->res.generate_deriv_vector(*i_field_a.second, this->printer);

        table[lambda_flatten(LAMBDA_FIELD)] = this->expr(deriv_a_i);
        table[lambda_flatten(LAMBDA_MOMENTUM)] = 0;

        return std::make_unique<map_lambda>(i, table, expression_item_types::zxfm1_lambda, args, this->shared.generate_working_type());
      }
    
    
    zeta1::zeta1(language_printer& p, cse& cw, expression_cache& c, resources& r,
                 shared_resources& s, boost::timer::cpu_timer& tm, index_flatten& f,
                 index_traits& t)
      : ::zeta1(),
        printer(p),
        cse_worker(cw),
        cache(c),
        res(r),
        shared(s),
        fl(f),
        traits(t),
        compute_timer(tm),
        cached(false),
        derivs([&](auto k) -> auto { return res.generate_deriv_vector(k[0], printer); })
      {
        Mp = this->shared.generate_Mp();
      }
    
    
    void zeta1::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("zeta1 already cached");

        this->pre_lambda();

        this->cached = true;
      }
    
    
    void zeta1::pre_lambda()
      {
        eps = this->res.eps_resource(this->cse_worker, this->printer);
      }
    
    
    void zeta1::post()
      {
        if(!this->cached) throw tensor_exception("zeta1 not cached");

        this->derivs.clear();

        // invalidate cache
        this->cached = false;
      }

  }   // namespace nontrivial_metric
