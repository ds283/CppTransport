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
        std::unique_ptr<ginac_cache_args> args = this->res.generate_arguments(use_dV_argument | use_ddV_argument, this->printer);
        args->push_back(k);
        args->push_back(a);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::U2_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) this->populate_cache();

            if(this->traits.is_species(i) && this->traits.is_species(j))
              {
                result = 0;
              }
            else if(this->traits.is_species(i) && this->traits.is_momentum(j))
              {
                field_index species_i = this->traits.to_species(i);
                field_index species_j = this->traits.to_species(j);

                result = (species_i == species_j ? 1 : 0);
              }
            else if(this->traits.is_momentum(i) && this->traits.is_species(j))
              {
                field_index species_i = this->traits.to_species(i);
                field_index species_j = this->traits.to_species(j);

                result = (species_i == species_j ? -(k*k) / (a*a * Hsq) : 0);

                GiNaC::ex Vab = (*ddV)[this->fl.flatten(species_i, species_j)];
                GiNaC::ex Va  = (*dV)[this->fl.flatten(species_i)];
                GiNaC::ex Vb  = (*dV)[this->fl.flatten(species_j)];

                result += -Vab/Hsq;
                result += -(3-eps) * (*derivs)[this->fl.flatten(species_i)] * (*derivs)[this->fl.flatten(species_j)] / (Mp*Mp);
                result += -1/(Mp*Mp*Hsq) * ( (*derivs)[this->fl.flatten(species_i)]*Vb + (*derivs)[this->fl.flatten(species_j)]*Va );
              }
            else if(this->traits.is_momentum(i) && this->traits.is_momentum(j))
              {
                result = (this->traits.to_species(i) == this->traits.to_species(j) ? (eps-3) : 0);
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


    void canonical_u2::populate_cache()
      {
        derivs = this->shared.generate_derivs(this->printer);
        dV = this->res.dV_resource(this->printer);
        ddV = this->res.ddV_resource(this->printer);
        Hsq = this->res.Hsq_resource(this->printer);
        eps = this->res.eps_resource(this->printer);
        Mp = this->shared.generate_Mp();
        cached = true;
      }


    enum unroll_behaviour canonical_u2::get_unroll()
      {
        if(this->shared.roll_coordinates() && this->res.roll_dV() && this->res.roll_ddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }
  }   // namespace canonical
