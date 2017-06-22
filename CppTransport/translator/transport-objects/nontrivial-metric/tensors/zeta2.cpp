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

#include "zeta2.h"


namespace nontrivial_metric
  {

    std::unique_ptr<flattened_tensor>
    zeta2::compute(const index_literal_list& indices, GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                   GiNaC::symbol& a)
      {
        if(indices.size() != ZETA2_TENSOR_INDICES) throw tensor_exception("zeta2 indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(ZETA2_TENSOR_INDICES));
    
        const phase_index max_i = this->shared.get_max_phase_index(indices[0]->get_variance());
        const phase_index max_j = this->shared.get_max_phase_index(indices[1]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(phase_index i = phase_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            for(phase_index j = phase_index(0, indices[1]->get_variance()); j < max_j; ++j)
              {
                (*result)[this->fl.flatten(i, j)] = this->compute_component(i, j, k, k1, k2, a);
              }
          }

        return(result);
      }
    
    
    GiNaC::ex zeta2::compute_component(phase_index i, phase_index j,
                                       GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                       GiNaC::symbol& a)
      {
        if(!this->cached) throw tensor_exception("zeta2 cache not ready");

        unsigned int index = this->fl.flatten(i, j);
        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(use_dV_argument, this->printer);
        args->push_back(k);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(a);

        GiNaC::ex result;
        if(!this->cache.query(expression_item_types::zxfm2_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            auto& deriv_i = (*derivs)[this->fl.flatten(this->traits.to_species(i))];
            auto& deriv_j = (*derivs)[this->fl.flatten(this->traits.to_species(j))];

            field_index species_i = this->traits.to_species(i);
            field_index species_j = this->traits.to_species(j);

            auto idx_i = this->shared.generate_index<GiNaC::varidx>(species_i);
            auto idx_j = this->shared.generate_index<GiNaC::varidx>(species_j);

            if(this->traits.is_species(i) && this->traits.is_species(j))
              {
                result = this->expr_field_field(deriv_i, deriv_j, k, k1, k2, a);
              }
            else if(this->traits.is_species(i) && this->traits.is_momentum(j))
              {
                result = this->expr_field_momentum(idx_i, idx_j, deriv_i, deriv_j, k, k1, k2, a);
              }
            else if(this->traits.is_momentum(i) && this->traits.is_species(j))
              {
                result = this->expr_field_momentum(idx_j, idx_i, deriv_j, deriv_i, k, k2, k1, a);
              }
            else if(this->traits.is_momentum(i) && this->traits.is_momentum(j))
              {
                result = 0;
              }
            else
              {
                // TODO: prefer to throw exception
                assert(false);
              }

            this->cache.store(expression_item_types::zxfm2_item, index, *args, result);
          }

        return(result);
      }
    
    
    void zeta2::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("A already cached");

        this->pre_lambda();
        derivs = this->res.generate_deriv_vector(this->printer);
        dV = this->res.dV_resource(this->printer);

        this->cached = true;
      }
    
    
    void zeta2::pre_lambda()
      {
        Hsq = this->res.Hsq_resource(this->cse_worker, this->printer);
        eps = this->res.eps_resource(this->cse_worker, this->printer);
        Mp = this->shared.generate_Mp();

        // formulae from DS calculation 28 May 2014

        std::unique_ptr<symbol_list> ds = this->shared.generate_deriv_symbols(this->printer);
        std::unique_ptr<flattened_tensor> Vi = this->res.dV_resource(this->printer);

        GiNaC::ex p_ex = 0;

        const field_index num_field = this->shared.get_max_field_index(variance::none);

        for(field_index i = field_index(0, variance::none); i < num_field; ++i)
          {
            p_ex += (*Vi)[this->fl.flatten(i)] * (*ds)[this->fl.flatten(i)];
          }
        p_ex = p_ex / (Mp*Mp*Hsq);

        GiNaC::symbol p_sym = this->shared.generate_symbol(ZETA2_P_SYMBOL_NAME);
        p = p_sym;
        this->cse_worker.parse(p_ex, p_sym.get_name());
      }
    
    
    void zeta2::post()
      {
        if(!this->cached) throw tensor_exception("zeta2 not cached");

        // invalidate cache
        this->cached = false;
      }
    
    
    GiNaC::ex zeta2::expr_field_field(GiNaC::ex& deriv_i, GiNaC::ex& deriv_j,
                                      GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                      GiNaC::symbol& a)
      {
        // formulae from DS calculation 28 May 2014
        GiNaC::ex result = (-GiNaC::ex(1)/2 + 3/(2*eps) + p/(4*eps*eps)) * deriv_i * deriv_j / (Mp*Mp*Mp*Mp*eps);
        return(result);
      }
    
    
    GiNaC::ex zeta2::expr_field_momentum(GiNaC::varidx& i, GiNaC::varidx& j, GiNaC::ex& deriv_i, GiNaC::ex& deriv_j,
                                         GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                         GiNaC::symbol& a)
      {
        // formulae from DS calculation 28 May 2014

        GiNaC::ex k1dotk2 = (k*k - k1*k1 - k2*k2) / 2;
        GiNaC::ex k12sq = k*k;

        GiNaC::ex result = deriv_i * deriv_j / (2 * Mp*Mp*Mp*Mp * eps*eps);

        GiNaC::ex delta_ij = GiNaC::delta_tensor(i, j);

        result += - delta_ij * (k1dotk2 / k12sq) / (2 * Mp*Mp * eps);
        result += - delta_ij * (k1*k1 / k12sq)   / (2 * Mp*Mp * eps);

        return(result);
      }
    
    
    unroll_behaviour zeta2::get_unroll()
      {
        if(this->shared.can_roll_coordinates() && this->res.can_roll_dV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<map_lambda>
    zeta2::compute_lambda(const index_literal& i, const index_literal& j, GiNaC::symbol& k,
                          GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("U3");
        if(j.get_class() != index_class::full) throw tensor_exception("U3");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);

        // convert these indices to species-only indices
        const auto i_field_a = this->traits.species_to_species(i);
        const auto i_field_b = this->traits.momentum_to_species(i);
        const auto j_field_a = this->traits.species_to_species(j);
        const auto j_field_b = this->traits.momentum_to_species(j);

        auto deriv_a_i = this->res.generate_deriv_vector(*i_field_a.second, this->printer);
        auto deriv_b_i = this->res.generate_deriv_vector(*i_field_b.second, this->printer);
        auto deriv_a_j = this->res.generate_deriv_vector(*j_field_a.second, this->printer);
        auto deriv_b_j = this->res.generate_deriv_vector(*j_field_b.second, this->printer);

        auto idx_a_i = this->shared.generate_index<GiNaC::varidx>(*i_field_a.second);
        auto idx_b_i = this->shared.generate_index<GiNaC::varidx>(*i_field_b.second);
        auto idx_a_j = this->shared.generate_index<GiNaC::varidx>(*j_field_a.second);
        auto idx_b_j = this->shared.generate_index<GiNaC::varidx>(*j_field_b.second);

        this->pre_lambda();

        map_lambda_table table(lambda_flattened_map_size(2));

        table[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_MOMENTUM)] = 0;

        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(use_dV_argument, this->printer);
        args->push_back(k);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(a);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_j.get_value()));

        if(!this->cache.query(expression_item_types::zxfm2_lambda, lambda_flatten(LAMBDA_FIELD, LAMBDA_FIELD), *args, table[lambda_flatten(LAMBDA_FIELD, LAMBDA_FIELD)]))
          {
            timing_instrument timer(this->compute_timer);

            table[lambda_flatten(LAMBDA_FIELD, LAMBDA_FIELD)] =
              this->expr_field_field(deriv_a_i, deriv_a_j, k, k1, k2, a);

            this->cache.store(expression_item_types::zxfm2_lambda, lambda_flatten(LAMBDA_FIELD, LAMBDA_FIELD), *args, table[lambda_flatten(LAMBDA_FIELD, LAMBDA_FIELD)]);
          }

        if(!this->cache.query(expression_item_types::zxfm2_lambda, lambda_flatten(LAMBDA_FIELD, LAMBDA_MOMENTUM), *args, table[lambda_flatten(LAMBDA_FIELD, LAMBDA_MOMENTUM)]))
          {
            timing_instrument timer(this->compute_timer);

            table[lambda_flatten(LAMBDA_FIELD, LAMBDA_MOMENTUM)] =
              this->expr_field_momentum(idx_a_i, idx_b_j, deriv_a_i, deriv_b_j, k, k1, k2, a);

            this->cache.store(expression_item_types::zxfm2_lambda, lambda_flatten(LAMBDA_FIELD, LAMBDA_MOMENTUM), *args, table[lambda_flatten(LAMBDA_FIELD, LAMBDA_MOMENTUM)]);
          }

        if(!this->cache.query(expression_item_types::zxfm2_lambda, lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD), *args, table[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD)]))
          {
            timing_instrument timer(this->compute_timer);

            table[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD)] =
              this->expr_field_momentum(idx_a_j, idx_b_i, deriv_a_j, deriv_b_i, k, k2, k1, a);

            this->cache.store(expression_item_types::zxfm2_lambda, lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD), *args, table[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD)]);
          }

        return std::make_unique<map_lambda>(i, j, table, expression_item_types::zxfm2_lambda, *args, this->shared.generate_working_type());
      }
    
    
    zeta2::zeta2(language_printer& p, cse& cw, expression_cache& c, resources& r,
                 shared_resources& s, boost::timer::cpu_timer& tm, index_flatten& f,
                 index_traits& t)
      : ::zeta2(),
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

  }   // namespace nontrivial_metric
