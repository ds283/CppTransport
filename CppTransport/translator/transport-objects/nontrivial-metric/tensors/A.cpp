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
        auto args = this->res.generate_cache_arguments(use_dV | use_ddV | use_dddV, this->printer);
        args += { k1, k2, k3, a };

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::A_item, index, args, result))
          {
            timing_instrument timer(this->compute_timer);
    
            auto& deriv_i = this->derivs(i)[this->fl.flatten(i)];
            auto& deriv_j = this->derivs(j)[this->fl.flatten(j)];
            auto& deriv_k = this->derivs(k)[this->fl.flatten(k)];

            // symmetrize Vijk
            auto Vijk = (this->dddV(i, j, k)[this->fl.flatten(i, j, k)]
                         + this->dddV(i, k, j)[this->fl.flatten(i, k, j)]
                         + this->dddV(j, i, k)[this->fl.flatten(j, i, k)]
                         + this->dddV(j, k, i)[this->fl.flatten(j, k, i)]
                         + this->dddV(k, i, j)[this->fl.flatten(k, i, j)]
                         + this->dddV(k, j, i)[this->fl.flatten(k, j, i)]) / 6;

            // don't need to symmetrize these, since they will be already symmetric with
            // a Levi-Civita connexion
            auto& Vij  = this->ddV(i,j)[this->fl.flatten(i,j)];
            auto& Vjk  = this->ddV(j,k)[this->fl.flatten(j,k)];
            auto& Vik  = this->ddV(i,k)[this->fl.flatten(i,k)];
    
            auto& Vi = this->dV(i)[this->fl.flatten(i)];
            auto& Vj = this->dV(j)[this->fl.flatten(j)];
            auto& Vk = this->dV(k)[this->fl.flatten(k)];

            auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
            auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);
            auto idx_k = this->shared.generate_index<GiNaC::varidx>(k);

            result = this->expr(idx_i, idx_j, idx_k, Vijk, Vij, Vjk, Vik, Vi, Vj, Vk,
                                deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::A_item, index, args, result);
          }

        return(result);
      }
    
    
    GiNaC::ex A::expr(const GiNaC::varidx& i, const GiNaC::varidx& j, const GiNaC::varidx& k,
                      const GiNaC::ex& Vijk, const GiNaC::ex& Vij, const GiNaC::ex& Vjk, const GiNaC::ex& Vik,
                      const GiNaC::ex& Vi, const GiNaC::ex& Vj, const GiNaC::ex& Vk,
                      const GiNaC::ex& deriv_i, const GiNaC::ex& deriv_j, const GiNaC::ex& deriv_k,
                      const GiNaC::symbol& k1, const GiNaC::symbol& k2, const GiNaC::symbol& k3, const GiNaC::symbol& a)
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
    
    
    unroll_behaviour A::get_unroll(const index_literal_list& idx_list)
      {
        const std::array< variance, RESOURCE_INDICES::DV_INDICES > i = { idx_list[0]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DV_INDICES > j = { idx_list[1]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DV_INDICES > k = { idx_list[2]->get_variance() };
    
        const std::array< variance, RESOURCE_INDICES::DDV_INDICES > ij = { idx_list[0]->get_variance(), idx_list[1]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DDV_INDICES > jk = { idx_list[1]->get_variance(), idx_list[2]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DDV_INDICES > ik = { idx_list[0]->get_variance(), idx_list[2]->get_variance() };
    
        const std::array< variance, RESOURCE_INDICES::DDDV_INDICES > ijk = { idx_list[0]->get_variance(), idx_list[1]->get_variance(), idx_list[2]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DDDV_INDICES > ikj = { idx_list[0]->get_variance(), idx_list[2]->get_variance(), idx_list[1]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DDDV_INDICES > jik = { idx_list[1]->get_variance(), idx_list[0]->get_variance(), idx_list[2]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DDDV_INDICES > jki = { idx_list[1]->get_variance(), idx_list[2]->get_variance(), idx_list[0]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DDDV_INDICES > kij = { idx_list[2]->get_variance(), idx_list[0]->get_variance(), idx_list[1]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DDDV_INDICES > kji = { idx_list[2]->get_variance(), idx_list[1]->get_variance(), idx_list[0]->get_variance() };

        // if any index is covariant then we need the metric to pull down an index on the coordinate vector
        // if any index is contravariant then we need the inverse metric to push up an index on the potential derivatives
        bool has_G = true;
        bool has_Ginv = true;
        if(idx_list[0]->get_variance() == variance::covariant
           || idx_list[1]->get_variance() == variance::covariant
           || idx_list[2]->get_variance() == variance::covariant)
          {
            has_G = this->res.can_roll_metric();
          }

        if(idx_list[0]->get_variance() == variance::contravariant
           || idx_list[1]->get_variance() == variance::contravariant
           || idx_list[2]->get_variance() == variance::contravariant)
          {
            has_Ginv = this->res.can_roll_metric_inverse();
          }

        if(this->shared.can_roll_coordinates() && has_G && has_Ginv
           && this->res.can_roll_dV(i)
           && this->res.can_roll_dV(j)
           && this->res.can_roll_dV(k)
           && this->res.can_roll_ddV(ij)
           && this->res.can_roll_ddV(ik)
           && this->res.can_roll_ddV(jk)
           && this->res.can_roll_dddV(ijk)
           && this->res.can_roll_dddV(ikj)
           && this->res.can_roll_dddV(jik)
           && this->res.can_roll_dddV(jki)
           && this->res.can_roll_dddV(kij)
           && this->res.can_roll_dddV(kji))
          return unroll_behaviour::allow;

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

        auto args = this->res.generate_cache_arguments(use_dV | use_ddV | use_dddV, this->printer);
        args += { k1, k2, k3, a };
        args += { idx_i, idx_j, idx_k };

        this->pre_lambda();

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::A_lambda, 0, args, result))
          {
            timing_instrument timer(this->compute_timer);

            auto deriv_i = this->res.generate_deriv_vector(i, this->printer);
            auto deriv_j = this->res.generate_deriv_vector(j, this->printer);
            auto deriv_k = this->res.generate_deriv_vector(k, this->printer);

            // symmetrize Vijk
            auto Vijk = (this->res.dddV_resource(i, j, k, this->printer)
                         + this->res.dddV_resource(i, k, j, this->printer)
                         + this->res.dddV_resource(j, i, k, this->printer)
                         + this->res.dddV_resource(j, k, i, this->printer)
                         + this->res.dddV_resource(k, i, j, this->printer)
                         + this->res.dddV_resource(k, j, i, this->printer)) / 6;

            // don't need to symmetrize these, since they will be already symmetric with
            // a Levi-Civita connexion
            auto Vij  = this->res.ddV_resource(i, j, this->printer);
            auto Vjk  = this->res.ddV_resource(j, k, this->printer);
            auto Vik  = this->res.ddV_resource(i, k, this->printer);

            auto Vi   = this->res.dV_resource(i, this->printer);
            auto Vj   = this->res.dV_resource(j, this->printer);
            auto Vk   = this->res.dV_resource(k, this->printer);

            result = this->expr(idx_i, idx_j, idx_k, Vijk, Vij, Vjk, Vik, Vi, Vj, Vk,
                                deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::A_lambda, 0, args, result);
          }

        return std::make_unique<atomic_lambda>(i, j, k, result, expression_item_types::A_lambda, args,
                                               this->shared.generate_working_type());
      }
    
    
    void A::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("A already cached");

        this->pre_lambda();

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
    
        this->derivs.clear();
        this->dV.clear();
        this->ddV.clear();
        this->dddV.clear();
        
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
        cached(false),
        derivs([&](auto k) -> auto { return res.generate_deriv_vector(k[0], printer); }),
        dV([&](auto k) -> auto { return res.dV_resource(k[0], printer); }),
        ddV([&](auto k) -> auto { return res.ddV_resource(k[0], k[1], printer); }),
        dddV([&](auto k) -> auto { return res.dddV_resource(k[0], k[1], k[2], printer); })
      {
        Mp = this->shared.generate_Mp();
      }

  }   // namespace nontrivial_metric
