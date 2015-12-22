//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "SR_velocity.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_SR_velocity::compute()
      {
        std::unique_ptr<flattened_tensor> result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(1));

        const field_index num_field = this->shared.get_number_field();
        this->cached = false;

        for(field_index i = field_index(0); i < num_field; ++i)
          {
            (*result)[this->fl.flatten(i)] = this->compute_component(i);
          }

        return(result);
      }


    GiNaC::ex canonical_SR_velocity::compute_component(field_index i)
      {
        unsigned int index = this->fl.flatten(i);
        std::unique_ptr<ginac_cache_args> args = this->res.generate_arguments(use_dV_argument, this->printer);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::sr_U_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) this->populate_cache();

            result = - (*dV)[this->fl.flatten(i)] * (Mp*Mp) / V;

            this->cache.store(expression_item_types::sr_U_item, index, *args, result);
          }

        return(result);
      }


    void canonical_SR_velocity::populate_cache()
      {
        V = this->res.V_resource(this->printer);
        dV = this->res.dV_resource(this->printer);
        Hsq = this->res.Hsq_resource(this->printer);
        Mp = this->shared.generate_Mp();
        cached = true;
      }


    enum unroll_behaviour canonical_SR_velocity::get_unroll()
      {
        if(this->res.roll_dV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }
  }   // namespace canonical
