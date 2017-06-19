//
// Created by David Seery on 07/08/2016.
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

#include "Atilde.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor>
    canonical_Atilde::compute(const index_literal_list& indices, GiNaC::symbol& k1, GiNaC::symbol& k2,
                              GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        if(indices.size() != A_TILDE_TENSOR_INDICES) throw tensor_exception("Atilde indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(A_TILDE_TENSOR_INDICES));

        const field_index max_i = this->shared.get_max_field_index(indices[0]->get_variance());
        const field_index max_j = this->shared.get_max_field_index(indices[1]->get_variance());
        const field_index max_k = this->shared.get_max_field_index(indices[2]->get_variance());
        
        this->cached = false;

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


    GiNaC::ex canonical_Atilde::compute_component(field_index i, field_index j, field_index k,
                                                  GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        unsigned int index = this->fl.flatten(i, j, k);
        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(
          use_dV_argument | use_ddV_argument | use_dddV_argument, this->printer);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(k3);
        args->push_back(a);

        if(!cached) { this->populate_workspace(); this->cache_symbols(); this->cached = true; }

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::Atilde_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::symbol& deriv_i = (*derivs)[this->fl.flatten(i)];
            GiNaC::symbol& deriv_j = (*derivs)[this->fl.flatten(j)];
            GiNaC::symbol& deriv_k = (*derivs)[this->fl.flatten(k)];

            GiNaC::ex& Vijk = (*dddV)[this->fl.flatten(i,j,k)];

            GiNaC::ex& Vij  = (*ddV)[this->fl.flatten(i,j)];
            GiNaC::ex& Vjk  = (*ddV)[this->fl.flatten(j,k)];
            GiNaC::ex& Vik  = (*ddV)[this->fl.flatten(i,k)];

            GiNaC::ex& Vi   = (*dV)[this->fl.flatten(i)];
            GiNaC::ex& Vj   = (*dV)[this->fl.flatten(j)];
            GiNaC::ex& Vk   = (*dV)[this->fl.flatten(k)];

            GiNaC::idx idx_i = this->shared.generate_index(i);
            GiNaC::idx idx_j = this->shared.generate_index(j);
            GiNaC::idx idx_k = this->shared.generate_index(k);

            result = this->expr(idx_i, idx_j, idx_k, Vijk, Vij, Vjk, Vik, Vi, Vj, Vk,
                                deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::Atilde_item, index, *args, result);
          }

        return(result);
      }


    GiNaC::ex canonical_Atilde::expr(GiNaC::idx& i, GiNaC::idx& j, GiNaC::idx& k,
                                     GiNaC::ex& Vijk, GiNaC::ex& Vij, GiNaC::ex& Vjk, GiNaC::ex& Vik,
                                     GiNaC::ex& Vi, GiNaC::ex& Vj, GiNaC::ex& Vk,
                                     GiNaC::symbol& deriv_i, GiNaC::symbol& deriv_j, GiNaC::symbol& deriv_k,
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

        return(result);
      }


    void canonical_Atilde::cache_symbols()
      {
        Hsq = this->res.Hsq_resource(this->cse_worker, this->printer);
        eps = this->res.eps_resource(this->cse_worker, this->printer);
        Mp = this->shared.generate_Mp();
      }


    void canonical_Atilde::populate_workspace()
      {
        derivs = this->shared.generate_deriv_symbols(this->printer);
        dV = this->res.dV_resource(this->printer);
        ddV = this->res.ddV_resource(this->printer);
        dddV = this->res.dddV_resource(this->printer);
      }


    unroll_behaviour canonical_Atilde::get_unroll()
      {
        if(this->shared.can_roll_coordinates() && this->res.can_roll_dV() && this->res.can_roll_ddV() &&
          this->res.can_roll_dddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<atomic_lambda> canonical_Atilde::compute_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k,
                                                                    GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("Atilde");
        if(j.get_class() != index_class::field_only) throw tensor_exception("Atilde");
        if(k.get_class() != index_class::field_only) throw tensor_exception("Atilde");

        GiNaC::idx idx_i = this->shared.generate_index(i);
        GiNaC::idx idx_j = this->shared.generate_index(j);
        GiNaC::idx idx_k = this->shared.generate_index(k);

        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(
          use_dV_argument | use_ddV_argument | use_dddV_argument, this->printer);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(k3);
        args->push_back(a);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_j.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_k.get_value()));

        this->cache_symbols();

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::Atilde_lambda, 0, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::symbol deriv_i = this->shared.generate_deriv_symbols(i, this->printer);
            GiNaC::symbol deriv_j = this->shared.generate_deriv_symbols(j, this->printer);
            GiNaC::symbol deriv_k = this->shared.generate_deriv_symbols(k, this->printer);

            GiNaC::ex Vijk = this->res.dddV_resource(i, j, k, this->printer);

            GiNaC::ex Vij  = this->res.ddV_resource(i, j, this->printer);
            GiNaC::ex Vjk  = this->res.ddV_resource(j, k, this->printer);
            GiNaC::ex Vik  = this->res.ddV_resource(i, k, this->printer);

            GiNaC::ex Vi   = this->res.dV_resource(i, this->printer);
            GiNaC::ex Vj   = this->res.dV_resource(j, this->printer);
            GiNaC::ex Vk   = this->res.dV_resource(k, this->printer);

            result = this->expr(idx_i, idx_j, idx_k, Vijk, Vij, Vjk, Vik, Vi, Vj, Vk,
                                deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::Atilde_lambda, 0, *args, result);
          }

        return std::make_unique<atomic_lambda>(i, j, k, result, expression_item_types::Atilde_lambda, *args, this->shared.generate_working_type());
      }

  }   // namespace canonical
