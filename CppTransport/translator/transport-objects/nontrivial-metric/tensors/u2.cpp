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

#include "u2.h"


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> u2::compute(const index_literal_list& indices, GiNaC::symbol& k,
                                                  GiNaC::symbol& a)
      {
        if(indices.size() != U2_TENSOR_INDICES) throw tensor_exception("U2 indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(U2_TENSOR_INDICES));
    
        const phase_index max_i = this->shared.get_max_phase_index(indices[0]->get_variance());
        const phase_index max_j = this->shared.get_max_phase_index(indices[1]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(phase_index i = phase_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            for(phase_index j = phase_index(0, indices[1]->get_variance()); j < max_j; ++j)
              {
                (*result)[this->fl.flatten(i, j)] = this->compute_component(i, j, k, a);
              }
          }

        return(result);
      }
    
    
    GiNaC::ex u2::compute_component(phase_index i, phase_index j, GiNaC::symbol& k, GiNaC::symbol& a)
      {
        if(!this->cached) throw tensor_exception("U2 cache not ready");

        unsigned int index = this->fl.flatten(i, j);
    
        // tag with variance information -- need to keep results of different variance distinct
        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);

        auto args = this->res.generate_cache_arguments<phase_index>(use_dV | use_ddV | use_Riemann_A2, {i, j}, this->printer);
        args += { k, a };
        args += { idx_i, idx_j };

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::U2_item, index, args, result))
          {
            timing_instrument timer(this->compute_timer);

            field_index species_i = this->traits.to_species(i);
            field_index species_j = this->traits.to_species(j);

            if(this->traits.is_species(i) && this->traits.is_species(j))
              {
                result = 0;
              }
            else if(this->traits.is_species(i) && this->traits.is_momentum(j))
              {
                result = this->G(species_i, species_j);
              }
            else if(this->traits.is_momentum(i) && this->traits.is_species(j))
              {
                // don't need to symmetrize; will be symmetric with a Levi-Civita connexion
                auto& Vij = this->ddV(species_i, species_j)[this->fl.flatten(species_i, species_j)];

                auto& Vi = this->dV(species_i)[this->fl.flatten(species_i)];
                auto& Vj = this->dV(species_j)[this->fl.flatten(species_j)];

                auto& deriv_i = this->derivs(species_i)[this->fl.flatten(species_i)];
                auto& deriv_j = this->derivs(species_j)[this->fl.flatten(species_j)];

                auto& A2_ij = this->A2(species_i, species_j)[this->fl.flatten(species_i, species_j)];

                auto delta_ij = this->G(species_i, species_j);

                result = this->expr_field_momentum(delta_ij, Vij, Vi, Vj, deriv_i, deriv_j, A2_ij, k, a);
              }
            else if(this->traits.is_momentum(i) && this->traits.is_momentum(j))
              {
                result = this->G(species_i, species_j) * (eps-3);
              }
            else throw tensor_exception("U2 index");

            this->cache.store(expression_item_types::U2_item, index, args, result);
          }

        return(result);
      }


    GiNaC::ex
    u2::expr_field_momentum(const GiNaC::ex& delta_ij, const GiNaC::ex& Vij, const GiNaC::ex& Vi, const GiNaC::ex& Vj,
                            const GiNaC::ex& deriv_i, const GiNaC::ex& deriv_j, const GiNaC::ex& A2_ij,
                            const GiNaC::symbol& k, const GiNaC::symbol& a)
      {
        GiNaC::ex result = delta_ij * ( -k*k / (a*a * Hsq) );

        result += -Vij/Hsq;
        result += -(3-eps) * deriv_i * deriv_j / (Mp*Mp);
        result += -( deriv_i*Vj + deriv_j*Vi ) / (Mp*Mp*Hsq);
        result += A2_ij;

        return(result);
      }
    
    
    unroll_state u2::get_unroll(const index_literal_list& idx_list)
      {
        const std::array< variance, RESOURCE_INDICES::DV_INDICES > i = { idx_list[0]->get_variance() };
        const std::array< variance, RESOURCE_INDICES::DV_INDICES > j = { idx_list[1]->get_variance() };
    
        const std::array< variance, RESOURCE_INDICES::DDV_INDICES > ij = { idx_list[0]->get_variance(), idx_list[1]->get_variance() };
    
        // if any index is covariant then we need the metric to pull down an index on the coordinate vector
        // if any index is contravariant then we need the inverser metric to push up indices on the potential derivatives
        bool has_G = true;
        bool has_Ginv = true;
        if(idx_list[0]->get_variance() == variance::covariant
           || idx_list[1]->get_variance() == variance::covariant)
          {
            has_G = this->res.can_roll_metric();
          }
        if(idx_list[0]->get_variance() == variance::contravariant
           || idx_list[1]->get_variance() == variance::contravariant)
          {
            has_Ginv = this->res.can_roll_metric_inverse();
          }

        if(this->shared.can_roll_coordinates() && has_G && has_Ginv
           && this->res.can_roll_dV(i)
           && this->res.can_roll_dV(j)
           && this->res.can_roll_ddV(ij)
           && this->res.can_roll_Riemann_A2(ij)) return unroll_state::allow;
        
        return unroll_state::force;   // can't roll-up
      }


    std::unique_ptr<map_lambda>
    u2::compute_lambda(const index_literal& i, const index_literal& j, GiNaC::symbol& k, GiNaC::symbol& a)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("U2");
        if(j.get_class() != index_class::full) throw tensor_exception("U2");

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

        this->pre_lambda();

        std::vector<GiNaC::ex> map(lambda_flattened_map_size(2));

        map[lambda_flatten(LAMBDA_FIELD, LAMBDA_FIELD)] = 0;
        map[lambda_flatten(LAMBDA_FIELD, LAMBDA_MOMENTUM)] = this->G(*i_field_a.second, *i_field_b.second);
        map[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_MOMENTUM)] = this->G(*i_field_b.second, *j_field_b.second) * (eps-3);

        auto args = this->res.generate_cache_arguments<index_literal>(use_dV | use_ddV | use_Riemann_A2, {i, j}, this->printer);
        args += { k, a };
        args += { idx_i, idx_j };

        if(!this->cache.query(expression_item_types::U2_lambda, 0, args, map[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD)]))
          {
            timing_instrument timer(this->compute_timer);

            // don't need to symmetrize; will be symmetric with a Levi-Civita connexion
            auto V_ba_ij = this->res.ddV_resource(*i_field_b.second, *j_field_a.second, this->printer);

            auto V_b_i = this->res.dV_resource(*i_field_b.second, this->printer);
            auto V_a_j = this->res.dV_resource(*j_field_a.second, this->printer);

            auto A2_ij = this->res.Riemann_A2_resource(*i_field_b.second, *j_field_a.second, this->printer);

            auto delta_ij = this->G(*i_field_b.second, *j_field_a.second);

            map[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD)] =
              this->expr_field_momentum(delta_ij, V_ba_ij, V_b_i, V_a_j, deriv_b_i, deriv_a_j, A2_ij, k, a);

            this->cache.store(expression_item_types::U2_lambda, 0, args, map[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD)]);
          }

        return std::make_unique<map_lambda>(i, j, map, expression_item_types::U2_lambda, args, this->shared.generate_working_type());
      }
    
    
    u2::u2(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
           boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
      : ::u2(),
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
        ddV([&](auto k) -> auto
              { return res.ddV_resource(k[0], k[1], printer); }),
        A2([&](auto k) -> auto
             { return res.Riemann_A2_resource(k[0], k[1], printer); }),
        G(r, s, f, p)
      {
        Mp = this->shared.generate_Mp();
      }
    
    
    void u2::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("U2 already cached");

        this->pre_lambda();

        this->cached = true;
      }
    
    
    void u2::pre_lambda()
      {
        Hsq = this->res.Hsq_resource(this->cse_worker, this->printer);
        eps = this->res.eps_resource(this->cse_worker, this->printer);
      }
    
    
    void u2::post()
      {
        if(!this->cached) throw tensor_exception("U2 not cached");

        this->derivs.clear();
        this->dV.clear();
        this->ddV.clear();
        this->A2.clear();

        // invalidate cache
        this->cached = false;
      }

  }   // namespace nontrivial_metric
