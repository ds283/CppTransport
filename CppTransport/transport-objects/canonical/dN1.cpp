//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "dN1.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_dN1::compute()
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


    GiNaC::ex canonical_dN1::compute_component(phase_index i)
      {
        unsigned int index = this->fl.flatten(i);
        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(0, this->printer);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::dN1_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) this->populate_cache();

            field_index i_species = this->traits.to_species(i);
            result = -(1/(2*dotH)) * diff(Hsq, (*fields)[this->fl.flatten(i_species)]);

            this->cache.store(expression_item_types::dN1_item, index, *args, result);
          }

        return(result);
      }


    void canonical_dN1::populate_cache()
      {
        fields = this->shared.generate_fields(this->printer);
        Hsq = this->res.Hsq_resource(this->printer);
        eps = this->res.eps_resource(this->printer);
        dotH = -eps*Hsq;
        cached = true;
      }


    enum unroll_behaviour canonical_dN1::get_unroll()
      {
        return unroll_behaviour::force;   // currently can't roll-up delta-N expressions
      }
  }   // namespace canonical
