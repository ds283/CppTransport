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

#include "M.h"


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> M::compute(const index_literal_list& indices)
      {
        if(indices.size() != M_TENSOR_INDICES) throw tensor_exception("M indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(M_TENSOR_INDICES));
    
        const field_index max_i = this->shared.get_max_field_index(indices[0]->get_variance());
        const field_index max_j = this->shared.get_max_field_index(indices[1]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(field_index i = field_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            for(field_index j = field_index(0, indices[1]->get_variance()); j < max_j; ++j)
              {
                (*result)[this->fl.flatten(i, j)] = this->compute_component(i, j);
              }
          }

        return(result);
      }
    
    
    GiNaC::ex M::compute_component(field_index i, field_index j)
      {
        if(!this->cached) throw tensor_exception("M cache not ready");

        unsigned int index = this->fl.flatten(i, j);
        auto args = this->res.generate_cache_arguments(use_dV | use_ddV, this->printer);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::M_item, index, args, result))
          {
            timing_instrument timer(this->compute_timer);

            auto& deriv_i = this->derivs(i)[this->fl.flatten(i)];
            auto& deriv_j = this->derivs(j)[this->fl.flatten(j)];

            auto& Vij = (*ddV)[this->fl.flatten(i,j)];

            auto& Vi = this->dV(i)[this->fl.flatten(i)];
            auto& Vj = this->dV(j)[this->fl.flatten(j)];

            auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
            auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);

            result = this->expr(idx_i, idx_j, Vij, Vi, Vj, deriv_i, deriv_j);

            this->cache.store(expression_item_types::M_item, index, args, result);
          }

        return(result);
      }
    
    
    GiNaC::ex M::expr(const GiNaC::varidx& i, const GiNaC::varidx& j, const GiNaC::ex& Vij, const GiNaC::ex& Vi,
                      const GiNaC::ex& Vj,
                      const GiNaC::ex& deriv_i, const GiNaC::ex& deriv_j)
      {
        GiNaC::ex u = -Vij/Hsq;
        u += -(3-eps) * deriv_i * deriv_j / (Mp*Mp);
        u += -1/(Mp*Mp*Hsq) * ( deriv_i*Vj + deriv_j*Vi );

        GiNaC::ex delta_ij = GiNaC::delta_tensor(i, j);

        return delta_ij*eps + u/3;
      }
    
    
    unroll_behaviour M::get_unroll(const index_literal_list& idx_list)
      {
        const std::array< variance, RESOURCE_INDICES::DV_INDICES > i = { idx_list[0]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DV_INDICES > j = { idx_list[1]->get_variance() };
    
        const std::array< variance, RESOURCE_INDICES::DDV_INDICES > ij = { idx_list[0]->get_variance(), idx_list[1]->get_variance() };
        
        if(this->shared.can_roll_coordinates()
           && this->res.can_roll_dV(i)
           && this->res.can_roll_dV(j)
           && this->res.can_roll_ddV(ij)) return unroll_behaviour::allow;
        
        return unroll_behaviour::force;   // can't roll-up
      }
    
    
    std::unique_ptr<atomic_lambda> M::compute_lambda(const index_literal& i, const index_literal& j)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("M");
        if(j.get_class() != index_class::field_only) throw tensor_exception("M");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);

        auto args = this->res.generate_cache_arguments(0, this->printer);
        args += { idx_i, idx_j };

        this->pre_lambda();

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::M_lambda, 0, args, result))
          {
            timing_instrument timer(this->compute_timer);

            auto deriv_i = this->res.generate_deriv_vector(i, this->printer);
            auto deriv_j = this->res.generate_deriv_vector(j, this->printer);

            auto Vij  = this->res.ddV_resource(i, j, this->printer);

            auto Vi   = this->res.dV_resource(i, this->printer);
            auto Vj   = this->res.dV_resource(j, this->printer);

            result = this->expr(idx_i, idx_j, Vij, Vi, Vj, deriv_i, deriv_j);

            this->cache.store(expression_item_types::M_lambda, 0, args, result);
          }

        return std::make_unique<atomic_lambda>(i, j, result, expression_item_types::M_lambda, args, this->shared.generate_working_type());
      }
    
    
    void M::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("M already cached");

        this->pre_lambda();

        ddV = this->res.ddV_resource(this->printer);

        this->cached = true;
      }
    
    
    void M::pre_lambda()
      {
        Hsq = this->res.Hsq_resource(this->cse_worker, this->printer);
        eps = this->res.eps_resource(this->cse_worker, this->printer);
      }
    
    
    void M::post()
      {
        if(!this->cached) throw tensor_exception("fields not cached");

        this->derivs.clear();
        this->dV.clear();

        // invalidate cache
        this->cached = false;
      }
    
    
    M::M(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
         boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
      : ::M(),
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
        Mp = this->shared.generate_Mp();
      }

  }   // namespace nontrivial_metric
