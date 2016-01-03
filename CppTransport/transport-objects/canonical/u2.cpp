//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "u2.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_u2::compute(GiNaC::symbol& k, GiNaC::symbol& a)
      {
        std::unique_ptr<flattened_tensor> result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(2));

        const phase_index num_phase = this->shared.get_number_phase();
        this->cached = false;

        for(phase_index i = phase_index(0); i < num_phase; ++i)
          {
            for(phase_index j = phase_index(0); j < num_phase; ++j)
              {
                (*result)[this->fl.flatten(i, j)] = this->compute_component(i, j, k, a);
              }
          }

        return(result);
      }


    GiNaC::ex canonical_u2::compute_component(phase_index i, phase_index j, GiNaC::symbol& k, GiNaC::symbol& a)
      {
        unsigned int index = this->fl.flatten(i, j);
        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(use_dV_argument | use_ddV_argument, this->printer);
        args->push_back(k);
        args->push_back(a);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::U2_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) { this->populate_workspace(); this->cache_symbols(); this->cached = true; }

            field_index species_i = this->traits.to_species(i);
            field_index species_j = this->traits.to_species(j);

            GiNaC::idx idx_i = this->shared.generate_index(species_i);
            GiNaC::idx idx_j = this->shared.generate_index(species_j);

            if(this->traits.is_species(i) && this->traits.is_species(j))
              {
                result = 0;
              }
            else if(this->traits.is_species(i) && this->traits.is_momentum(j))
              {
                result = GiNaC::delta_tensor(idx_i, idx_j);
              }
            else if(this->traits.is_momentum(i) && this->traits.is_species(j))
              {
                GiNaC::ex& Vij = (*ddV)[this->fl.flatten(species_i, species_j)];
                GiNaC::ex& Vi  = (*dV)[this->fl.flatten(species_i)];
                GiNaC::ex& Vj  = (*dV)[this->fl.flatten(species_j)];

                GiNaC::symbol& deriv_i = (*derivs)[this->fl.flatten(species_i)];
                GiNaC::symbol& deriv_j = (*derivs)[this->fl.flatten(species_j)];

                result = this->expr_field_momentum(idx_i, idx_j, Vij, Vi, Vj, deriv_i, deriv_j, k, a);
              }
            else if(this->traits.is_momentum(i) && this->traits.is_momentum(j))
              {
                result = GiNaC::delta_tensor(idx_i, idx_j) * (eps-3);
              }
            else
              {
                // TODO: prefer to throw exception
                assert(false);
              }

            this->cache.store(expression_item_types::U2_item, index, *args, result);
          }

        return(result);
      }


    GiNaC::ex canonical_u2::expr_field_momentum(GiNaC::idx& i, GiNaC::idx& j,
                                                GiNaC::ex& Vij, GiNaC::ex& Vi, GiNaC::ex& Vj,
                                                GiNaC::symbol& deriv_i, GiNaC::symbol& deriv_j,
                                                GiNaC::symbol& k, GiNaC::symbol& a)
      {
        GiNaC::ex delta_ij = GiNaC::delta_tensor(i, j);
        GiNaC::ex result = delta_ij * ( -k*k / (a*a * Hsq) );

        result += -Vij/Hsq;
        result += -(3-eps) * deriv_i * deriv_j / (Mp*Mp);
        result += -1/(Mp*Mp*Hsq) * ( deriv_i*Vj + deriv_j*Vi );

        return(result);
      }


    void canonical_u2::cache_symbols()
      {
        Hsq = this->res.Hsq_resource(this->cse_worker, this->printer);
        eps = this->res.eps_resource(this->cse_worker, this->printer);
        Mp = this->shared.generate_Mp();
      }


    void canonical_u2::populate_workspace()
      {
        derivs = this->shared.generate_derivs(this->printer);
        dV = this->res.dV_resource(this->printer);
        ddV = this->res.ddV_resource(this->printer);
      }


    enum unroll_behaviour canonical_u2::get_unroll()
      {
        if(this->shared.roll_coordinates() && this->res.roll_dV() && this->res.roll_ddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<map_lambda> canonical_u2::compute_lambda(const abstract_index& i, const abstract_index& j,
                                                             GiNaC::symbol& k, GiNaC::symbol& a)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("U2");
        if(j.get_class() != index_class::full) throw tensor_exception("U2");

        GiNaC::idx idx_i = this->shared.generate_index(i);
        GiNaC::idx idx_j = this->shared.generate_index(i);

        // convert these indices to species-only indices
        const abstract_index i_field_a = this->traits.species_to_species(i);
        const abstract_index i_field_b = this->traits.momentum_to_species(i);
        const abstract_index j_field_a = this->traits.species_to_species(j);
        const abstract_index j_field_b = this->traits.momentum_to_species(j);

        GiNaC::symbol deriv_a_i = this->shared.generate_derivs(i_field_a, this->printer);
        GiNaC::symbol deriv_b_i = this->shared.generate_derivs(i_field_b, this->printer);
        GiNaC::symbol deriv_a_j = this->shared.generate_derivs(j_field_a, this->printer);
        GiNaC::symbol deriv_b_j = this->shared.generate_derivs(j_field_b, this->printer);

        GiNaC::idx idx_a_i = this->shared.generate_index(i_field_a);
        GiNaC::idx idx_b_i = this->shared.generate_index(i_field_b);
        GiNaC::idx idx_a_j = this->shared.generate_index(j_field_a);
        GiNaC::idx idx_b_j = this->shared.generate_index(j_field_b);

        std::vector<GiNaC::ex> map(lambda_flattened_map_size(2));

        map[lambda_flatten(LAMBDA_FIELD, LAMBDA_FIELD)] = deriv_a_i;
        map[lambda_flatten(LAMBDA_FIELD, LAMBDA_MOMENTUM)] = GiNaC::delta_tensor(idx_a_i, idx_b_j);
        map[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_MOMENTUM)] = GiNaC::delta_tensor(idx_b_i, idx_b_j) * (eps-3);

        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(use_dV_argument | use_ddV_argument, this->printer);
        args->push_back(k);
        args->push_back(a);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_j.get_value()));

        if(!this->cache.query(expression_item_types::U2_lambda, 0, *args, map[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD)]))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::ex V_ba_ij = this->res.ddV_resource(i_field_b, j_field_a, this->printer);

            GiNaC::ex V_b_i = this->res.dV_resource(i_field_b, this->printer);
            GiNaC::ex V_a_j = this->res.dV_resource(j_field_a, this->printer);

            // expr() expects Hsq, eps and Mp to be correctly set up in the cache
            this->cache_symbols();

            map[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD)] = this->expr_field_momentum(idx_b_i, idx_a_j, V_ba_ij, V_b_i, V_a_j, deriv_b_i, deriv_a_j, k, a);

            this->cache.store(expression_item_types::U2_lambda, 0, *args, map[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD)]);
          }

        return std::make_unique<map_lambda>(i, j, map, expression_item_types::U2_lambda, *args, this->shared.generate_working_type());
      }

  }   // namespace canonical
