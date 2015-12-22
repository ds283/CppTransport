//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "zeta1.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_zeta1::compute()
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


    GiNaC::ex canonical_zeta1::compute_component(phase_index i)
      {
        unsigned int index = this->fl.flatten(i);
        std::unique_ptr<ginac_cache_args> args = this->res.generate_arguments(0, this->printer);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::zxfm1_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) this->populate_cache();

            if(this->traits.is_species(i))
              {
                result = -(*derivs)[this->fl.flatten(i)] / (2*Mp*Mp*eps);
              }
            else if(this->traits.is_momentum(i))
              {
                result = 0;
              }
            else
              {
                // TODO: prefer to throw exception
                assert(false);
              }

            this->cache.store(expression_item_types::zxfm1_item, index, *args, result);
          }

        return(result);
      }


    void canonical_zeta1::populate_cache()
      {
        derivs = this->shared.generate_derivs(this->printer);
        eps = this->res.eps_resource(this->printer);
        Mp = this->shared.generate_Mp();
        cached = true;
      }


    enum unroll_behaviour canonical_zeta1::get_unroll()
      {
        if(this->shared.roll_coordinates()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }
  }   // namespace canonical
