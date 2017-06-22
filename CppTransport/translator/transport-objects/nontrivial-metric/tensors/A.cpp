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

#include "A.h"


namespace nontrivial_metric
  {

    std::unique_ptr<flattened_tensor>
    A::compute(const index_literal_list& indices, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3,
               GiNaC::symbol& a)
      {
        if(indices.size() != A_TENSOR_INDICES) throw tensor_exception("A indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(A_TENSOR_INDICES));

        const field_index max_i = this->shared.get_max_field_index(indices[0]->get_variance());
        const field_index max_j = this->shared.get_max_field_index(indices[1]->get_variance());
        const field_index max_k = this->shared.get_max_field_index(indices[2]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(field_index i = field_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            for(field_index j = field_index(0, indices[1]->get_variance()); j < max_j; ++j)
              {
                for(field_index k = field_index(0, indices[2]->get_variance()); k < max_k; ++k)
                  {
                    (*result)[this->fl.flatten(i, j, k)] = this->compute_component(i, j, k, k1, k2, k3, a);
                  }
              }
          }

        return(result);
      }
    
    
    GiNaC::ex A::compute_component(field_index i, field_index j, field_index k,
                                   GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        if(!this->cached) throw tensor_exception("A cache not ready");

        unsigned int index = this->fl.flatten(i, j, k);
        auto args = this->res.generate_cache_arguments(use_dV_argument | use_ddV_argument | use_dddV_argument,
                                                       this->printer);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(k3);
        args->push_back(a);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::A_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            auto& deriv_i = (*derivs)[this->fl.flatten(i)];
            auto& deriv_j = (*derivs)[this->fl.flatten(j)];
            auto& deriv_k = (*derivs)[this->fl.flatten(k)];

            auto& Vijk = (*dddV)[this->fl.flatten(i,j,k)];

            auto& Vij  = (*ddV)[this->fl.flatten(i,j)];
            auto& Vjk  = (*ddV)[this->fl.flatten(j,k)];
            auto& Vik  = (*ddV)[this->fl.flatten(i,k)];

            auto& Vi   = (*dV)[this->fl.flatten(i)];
            auto& Vj   = (*dV)[this->fl.flatten(j)];
            auto& Vk   = (*dV)[this->fl.flatten(k)];

            auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
            auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);
            auto idx_k = this->shared.generate_index<GiNaC::varidx>(k);

            result = this->expr(idx_i, idx_j, idx_k, Vijk, Vij, Vjk, Vik, Vi, Vj, Vk,
                                deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::A_item, index, *args, result);
          }

        return(result);
      }
    
    
    GiNaC::ex A::expr(GiNaC::varidx& i, GiNaC::varidx& j, GiNaC::varidx& k,
                      GiNaC::ex& Vijk, GiNaC::ex& Vij, GiNaC::ex& Vjk, GiNaC::ex& Vik,
                      GiNaC::ex& Vi, GiNaC::ex& Vj, GiNaC::ex& Vk,
                      GiNaC::ex& deriv_i, GiNaC::ex& deriv_j, GiNaC::ex& deriv_k,
                      GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        GiNaC::ex xi_i = -2*(3-eps) * deriv_i - 2 * Vi/Hsq;
        GiNaC::ex xi_j = -2*(3-eps) * deriv_j - 2 * Vj/Hsq;
        GiNaC::ex xi_k = -2*(3-eps) * deriv_k - 2 * Vk/Hsq;

        GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2) / 2;
        GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3) / 2;
        GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3) / 2;

        GiNaC::ex result = - Vijk / (3*Hsq);

        result += - (deriv_i / (2*Mp*Mp)) * (Vjk / (3*Hsq))
                  - (deriv_j / (2*Mp*Mp)) * (Vik / (3*Hsq))
                  - (deriv_k / (2*Mp*Mp)) * (Vij / (3*Hsq));

        result += + (deriv_i * deriv_j * xi_k) / (8 * 3 * Mp*Mp*Mp*Mp)
                  + (deriv_j * deriv_k * xi_i) / (8 * 3 * Mp*Mp*Mp*Mp)
                  + (deriv_i * deriv_k * xi_j) / (8 * 3 * Mp*Mp*Mp*Mp);

        result += + (deriv_i * xi_j * xi_k) / (32*Mp*Mp*Mp*Mp) * (1 - k2dotk3*k2dotk3 / (k2*k2 * k3*k3)) / 3
                  + (deriv_j * xi_i * xi_k) / (32*Mp*Mp*Mp*Mp) * (1 - k1dotk3*k1dotk3 / (k1*k1 * k3*k3)) / 3
                  + (deriv_k * xi_i * xi_j) / (32*Mp*Mp*Mp*Mp) * (1 - k1dotk2*k1dotk2 / (k1*k1 * k2*k2)) / 3;

        result +=( deriv_i * deriv_j * deriv_k ) / (8 * Mp*Mp*Mp*Mp) * (6 - 2*eps);

        GiNaC::ex delta_ij = GiNaC::delta_tensor(i, j);
        GiNaC::ex delta_jk = GiNaC::delta_tensor(j, k);
        GiNaC::ex delta_ik = GiNaC::delta_tensor(i, k);

        result += delta_jk * ( deriv_i / (2*Mp*Mp) ) * k2dotk3 / (3*a*a*Hsq);
        result += delta_ik * ( deriv_j / (2*Mp*Mp) ) * k1dotk3 / (3*a*a*Hsq);
        result += delta_ij * ( deriv_k / (2*Mp*Mp) ) * k1dotk2 / (3*a*a*Hsq);

        return(result);
      }
    
    
    unroll_behaviour A::get_unroll()
      {
        if(this->shared.can_roll_coordinates() && this->res.can_roll_dV() && this->res.can_roll_ddV() &&
          this->res.can_roll_dddV()) return unroll_behaviour::allow;

        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<atomic_lambda>
    A::compute_lambda(const index_literal& i, const index_literal& j, const index_literal& k,
                      GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("A");
        if(j.get_class() != index_class::field_only) throw tensor_exception("A");
        if(k.get_class() != index_class::field_only) throw tensor_exception("A");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);
        auto idx_k = this->shared.generate_index<GiNaC::varidx>(k);

        auto args = this->res.generate_cache_arguments(use_dV_argument | use_ddV_argument | use_dddV_argument, this->printer);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(k3);
        args->push_back(a);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_j.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_k.get_value()));

        this->pre_lambda();

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::A_lambda, 0, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            auto deriv_i = this->res.generate_deriv_vector(i, this->printer);
            auto deriv_j = this->res.generate_deriv_vector(j, this->printer);
            auto deriv_k = this->res.generate_deriv_vector(k, this->printer);

            auto Vijk = this->res.dddV_resource(i, j, k, this->printer);

            auto Vij  = this->res.ddV_resource(i, j, this->printer);
            auto Vjk  = this->res.ddV_resource(j, k, this->printer);
            auto Vik  = this->res.ddV_resource(i, k, this->printer);

            auto Vi   = this->res.dV_resource(i, this->printer);
            auto Vj   = this->res.dV_resource(j, this->printer);
            auto Vk   = this->res.dV_resource(k, this->printer);

            result = this->expr(idx_i, idx_j, idx_k, Vijk, Vij, Vjk, Vik, Vi, Vj, Vk,
                                deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::A_lambda, 0, *args, result);
          }

        return std::make_unique<atomic_lambda>(i, j, k, result, expression_item_types::A_lambda, *args,
                                               this->shared.generate_working_type());
      }
    
    
    void A::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("A already cached");

        this->pre_lambda();
        derivs = this->res.generate_deriv_vector(this->printer);
        dV = this->res.dV_resource(this->printer);
        ddV = this->res.ddV_resource(this->printer);
        dddV = this->res.dddV_resource(this->printer);

        this->cached = true;
      }
    
    
    void A::pre_lambda()
      {
        Hsq = this->res.Hsq_resource(this->cse_worker, this->printer);
        eps = this->res.eps_resource(this->cse_worker, this->printer);
      }
    
    
    void A::post()
      {
        if(!this->cached) throw tensor_exception("A not cached");

        // invalidate cache
        this->cached = false;
      }
    
    
    A::A(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
         boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
      : ::A(),
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
        Mp = this->shared.generate_Mp();
      }

  }   // namespace nontrivial_metric
