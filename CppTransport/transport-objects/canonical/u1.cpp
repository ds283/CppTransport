//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "u1.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_u1::compute()
      {
        std::unique_ptr<flattened_tensor> result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(1));

        const phase_index num_phase = this->shared.get_number_phase();
        this->cached = false;

        for(phase_index i = phase_index(0); i < num_phase; ++i)
          {
            (*result)[this->fl.flatten(i)] = this->compute_component(i);
          }

        return(result);
      }


    GiNaC::ex canonical_u1::compute_component(phase_index i)
      {
        unsigned int index = this->fl.flatten(i);
        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(use_dV_argument, this->printer);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::U1_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) this->populate_cache();

            field_index species_i = this->traits.to_species(i);

            GiNaC::symbol& deriv_i = (*derivs)[this->fl.flatten(species_i)];

            if(this->traits.is_species(i))
              {
                result = deriv_i;
              }
            else if(this->traits.is_momentum(i))
              {
                GiNaC::ex& Vi = (*dV)[this->fl.flatten(species_i)];
                result = this->expr_momentum(Vi, deriv_i);
              }
            else
              {
                // TODO: prefer to throw exception
                assert(false);
              }

            this->cache.store(expression_item_types::U1_item, index, *args, result);
          }

        return(result);
      }


    GiNaC::ex canonical_u1::expr_momentum(GiNaC::ex& Vi, GiNaC::symbol& deriv_i)
      {
        return -(3-eps) * deriv_i - Vi/Hsq;
      }


    void canonical_u1::populate_cache()
      {
        derivs = this->shared.generate_derivs(this->printer);
        dV = this->res.dV_resource(this->printer);
        Hsq = this->res.Hsq_resource(this->printer);
        eps = this->res.eps_resource(this->printer);
        cached = true;
      }


    enum unroll_behaviour canonical_u1::get_unroll()
      {
        if(this->shared.roll_coordinates() && this->res.roll_dV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<map_lambda> canonical_u1::compute_lambda(const abstract_index& i)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("U1");

        GiNaC::idx idx_i = this->shared.generate_index(i);

        // convert this index to species-only indices
        const abstract_index i_field_a = this->traits.species_to_species(i);
        const abstract_index i_field_b = this->traits.momentum_to_species(i);

        GiNaC::symbol deriv_a_i = this->shared.generate_derivs(i_field_a, this->printer);
        GiNaC::symbol deriv_b_i = this->shared.generate_derivs(i_field_b, this->printer);

        std::vector<GiNaC::ex> map(lambda_flattened_map_size(1));

        map[lambda_flatten(LAMBDA_FIELD)] = deriv_a_i;

        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(use_dV_argument, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));

        if(!this->cache.query(expression_item_types::U1_lambda, 0, *args, map[lambda_flatten(LAMBDA_MOMENTUM)]))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::ex V_b_i = this->res.dV_resource(i_field_b, this->printer);

            // expr() expects Hsq and eps to be correctly set up in the cache
            Hsq = this->res.Hsq_resource(this->printer);
            eps = this->res.eps_resource(this->printer);
            map[lambda_flatten(LAMBDA_MOMENTUM)] = this->expr_momentum(V_b_i, deriv_b_i);

            this->cache.store(expression_item_types::U1_lambda, 0, *args, map[lambda_flatten(LAMBDA_MOMENTUM)]);
          }

        return std::make_unique<map_lambda>(i, map, expression_item_types::U1_lambda, *args, this->shared.generate_working_type());
      }

  }   // namespace canonical
