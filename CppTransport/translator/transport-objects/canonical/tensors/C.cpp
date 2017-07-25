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

#include "C.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor>
    C::compute(const index_literal_list& indices, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3,
               GiNaC::symbol& a)
      {
        if(indices.size() != C_TENSOR_INDICES) throw tensor_exception("C indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(C_TENSOR_INDICES));
    
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
    
    
    GiNaC::ex C::compute_component(field_index i, field_index j, field_index k,
                                   GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        if(!this->cached) throw tensor_exception("C cache not ready");

        unsigned int index = this->fl.flatten(i, j, k);
        auto args = this->res.generate_cache_arguments(0, this->printer);
        args += { k1, k2, k3, a };

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::C_item, index, args, result))
          {
            timing_instrument timer(this->compute_timer);

            auto& deriv_i = (*derivs)[this->fl.flatten(i)];
            auto& deriv_j = (*derivs)[this->fl.flatten(j)];
            auto& deriv_k = (*derivs)[this->fl.flatten(k)];

            auto idx_i = this->shared.generate_index<GiNaC::idx>(i);
            auto idx_j = this->shared.generate_index<GiNaC::idx>(j);
            auto idx_k = this->shared.generate_index<GiNaC::idx>(k);

            result = this->expr(idx_i, idx_j, idx_k, deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::C_item, index, args, result);
          }

        return(result);
      }
    
    
    GiNaC::ex C::expr(const GiNaC::idx& i, const GiNaC::idx& j, const GiNaC::idx& k,
                      const GiNaC::ex& deriv_i, const GiNaC::ex& deriv_j, const GiNaC::ex& deriv_k,
                      const GiNaC::symbol& k1, const GiNaC::symbol& k2, const GiNaC::symbol& k3, const GiNaC::symbol& a)
      {
        GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2) / 2;
        GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3) / 2;
        GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3) / 2;

        GiNaC::ex delta_ij = GiNaC::delta_tensor(i, j);

        GiNaC::ex result = - delta_ij * deriv_k / (2*Mp*Mp);

        result += ( deriv_i * deriv_j * deriv_k / (8 * Mp*Mp*Mp*Mp) ) * (1 - k1dotk2*k1dotk2 / (k1*k1 * k2*k2));

        GiNaC::ex delta_jk = GiNaC::delta_tensor(j, k);
        GiNaC::ex delta_ik = GiNaC::delta_tensor(i, k);

        result += delta_jk * (deriv_i / (Mp*Mp)) * (k1dotk3 / (k1*k1)) / 2;
        result += delta_ik * (deriv_j / (Mp*Mp)) * (k2dotk3 / (k2*k2)) / 2;

        return(result);
      }
    
    
    unroll_state C::get_unroll(const index_literal_list& idx_list)
      {
        if(this->shared.can_roll_coordinates()) return unroll_state::allow;
        return unroll_state::force;   // can't unroll
      }


    std::unique_ptr<atomic_lambda>
    C::compute_lambda(const index_literal& i, const index_literal& j, const index_literal& k,
                      GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("C");
        if(j.get_class() != index_class::field_only) throw tensor_exception("C");
        if(k.get_class() != index_class::field_only) throw tensor_exception("C");

        auto idx_i = this->shared.generate_index<GiNaC::idx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::idx>(j);
        auto idx_k = this->shared.generate_index<GiNaC::idx>(k);

        auto args = this->res.generate_cache_arguments(0, this->printer);
        args += { k1, k2, k3, a };
        args += { idx_i, idx_j, idx_k };

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::C_lambda, 0, args, result))
          {
            timing_instrument timer(this->compute_timer);

            auto deriv_i = this->res.generate_deriv_vector(i, this->printer);
            auto deriv_j = this->res.generate_deriv_vector(j, this->printer);
            auto deriv_k = this->res.generate_deriv_vector(k, this->printer);

            result = this->expr(idx_i, idx_j, idx_k, deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::C_lambda, 0, args, result);
          }

        return std::make_unique<atomic_lambda>(i, j, k, result, expression_item_types::C_lambda, args, this->shared.generate_working_type());
      }
    
    
    C::C(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
         boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
      : ::C(),
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
    
    
    void C::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("C already cached");

        derivs = this->res.generate_deriv_vector(this->printer);

        this->cached = true;
      }
    
    
    void C::post()
      {
        if(!this->cached) throw tensor_exception("C not cached");

        // invalidate cache
        this->cached = false;
      }

  }   // namespace canonical
