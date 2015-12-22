//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "u3.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_u3::compute(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        std::unique_ptr<flattened_tensor> result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(3));

        const phase_index num_phase = this->shared.get_number_phase();
        this->A_agent.reset_cache();
        this->B_agent.reset_cache();
        this->C_agent.reset_cache();

        for(phase_index i = phase_index(0); i < num_phase; ++i)
          {
            for(phase_index j = phase_index(0); j < num_phase; ++j)
              {
                for(phase_index k = phase_index(0); k < num_phase; ++k)
                  {
                    (*result)[this->fl.flatten(i, j, k)] = this->compute_component(i, j, k, k1, k2, k3, a);
                  }
              }
          }

        return(result);
      }


    GiNaC::ex canonical_u3::compute_component(phase_index i, phase_index j, phase_index k,
                                              GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        unsigned int index = this->fl.flatten(i, j, k);
        std::unique_ptr<ginac_cache_args> args = this->res.generate_arguments(use_dV_argument | use_ddV_argument | use_dddV_argument, this->printer);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(k3);
        args->push_back(a);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::U3_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            // note that we flip the sign of momenta attached to the i, j, k components compared to the
            // analytic definition
            // this accounts for integrating out the delta-functions when contracting u3 with something else

            // also, note factor of 2 from definition of 2nd order term in transport eq: dX/dN = u2.X + (1/2) u3.X.X + ...

            field_index species_i = this->traits.to_species(i);
            field_index species_j = this->traits.to_species(j);
            field_index species_k = this->traits.to_species(k);

            bool is_species_i = this->traits.is_species(i);
            bool is_species_j = this->traits.is_species(j);
            bool is_species_k = this->traits.is_species(k);

            bool is_momentum_i = !is_species_i;
            bool is_momentum_j = !is_species_j;
            bool is_momentum_k = !is_species_k;

            if(is_species_i && is_species_j && is_species_k)
              {
                result = - B_agent.compute_component(species_j, species_k, species_i, k2, k3, k1, a);
              }
            else if(is_species_i && is_species_j && is_momentum_k)
              {
                result = - C_agent.compute_component(species_i, species_k, species_j, k1, k3, k2, a);
              }
            else if(is_species_i && is_momentum_j && is_species_k)
              {
                result = - C_agent.compute_component(species_i, species_j, species_k, k1, k2, k3, a);
              }
            else if(is_species_i && is_momentum_j && is_momentum_k)
              {
                result = 0;
              }
            else if(is_momentum_i && is_species_j && is_species_k)
              {
                result = 3 * A_agent.compute_component(species_i, species_j, species_k, k1, k2, k3, a);
              }
            else if(is_momentum_i && is_species_j && is_momentum_k)
              {
                result = B_agent.compute_component(species_i, species_j, species_k, k1, k2, k3, a);
              }
            else if(is_momentum_i && is_momentum_j && is_species_k)
              {
                result = B_agent.compute_component(species_i, species_k, species_j, k1, k3, k2, a);
              }
            else if(is_momentum_i && is_momentum_j && is_momentum_k)
              {
                result = C_agent.compute_component(species_j, species_k, species_i, k2, k3, k1, a);
              }
            else
              {
                // TODO: prefer to throw exception
                assert(false);
              }

            this->cache.store(expression_item_types::U3_item, index, *args, result);
          }

        return(result);
      }


    enum unroll_behaviour canonical_u3::get_unroll()
      {
        if(this->shared.roll_coordinates() && this->res.roll_dV() && this->res.roll_ddV() && this->res.roll_dddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }
  }   // namespace canonical
