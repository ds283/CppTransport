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

#include "SR_velocity.h"


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> SR_velocity::compute(const index_literal_list& indices)
      {
        if(indices.size() != SR_VELOCITY_TENSOR_INDICES) throw tensor_exception("SRvelocity indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(SR_VELOCITY_TENSOR_INDICES));
    
        const field_index max_i = this->shared.get_max_field_index(indices[0]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(field_index i = field_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            (*result)[this->fl.flatten(i)] = this->compute_component(i);
          }

        return(result);
      }
    
    
    GiNaC::ex SR_velocity::compute_component(field_index i)
      {
        if(!this->cached) throw tensor_exception("SR_velocity cache not ready");

        unsigned int index = this->fl.flatten(i);
        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(use_dV_argument, this->printer);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::sr_U_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::ex& Vi = (*dV)[this->fl.flatten(i)];
            result = this->expr(Vi);

            this->cache.store(expression_item_types::sr_U_item, index, *args, result);
          }

        return(result);
      }
    
    
    GiNaC::ex SR_velocity::expr(GiNaC::ex& Vi)
      {
        return -Vi * (Mp*Mp) / V;
      }
    
    
    unroll_behaviour SR_velocity::get_unroll()
      {
        if(this->res.can_roll_dV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }
    
    
    std::unique_ptr<atomic_lambda> SR_velocity::compute_lambda(const index_literal& i)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("SR_velocity");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);

        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(use_dV_argument, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));

        this->pre_lambda();

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::sr_U_lambda, 0, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::ex Vi = this->res.dV_resource(i, this->printer);

            result = this->expr(Vi);

            this->cache.store(expression_item_types::sr_U_lambda, 0, *args, result);
          }

        return std::make_unique<atomic_lambda>(i, result, expression_item_types::sr_U_lambda, *args, this->shared.generate_working_type());
      }
    
    
    SR_velocity::SR_velocity(language_printer& p, cse& cw, expression_cache& c, resources& r,
                             shared_resources& s, boost::timer::cpu_timer& tm, index_flatten& f)
      : ::SR_velocity(),
        printer(p),
        cse_worker(cw),
        cache(c),
        res(r),
        shared(s),
        fl(f),
        compute_timer(tm),
        cached(false)
      {
      }
    
    
    void SR_velocity::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("SR_velocity already cached");

        this->pre_lambda();
        dV = this->res.dV_resource(this->printer);

        this->cached = true;
      }
    
    
    void SR_velocity::pre_lambda()
      {
        V = this->res.V_resource(this->cse_worker, this->printer);
        Mp = this->shared.generate_Mp();
      }
    
    
    void SR_velocity::post()
      {
        if(!this->cached) throw tensor_exception("SR_velocity not cached");

        // invalidate cache
        this->cached = false;
      }

  }   // namespace nontrivial_metric
