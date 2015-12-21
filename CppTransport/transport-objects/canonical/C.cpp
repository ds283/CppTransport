//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "C.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_C::compute(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        std::unique_ptr<flattened_tensor> result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(3));

        const field_index num_field = this->shared.get_number_field();
        this->cached = false;

        for(field_index i = field_index(0); i < num_field; ++i)
          {
            for(field_index j = field_index(0); j < num_field; ++j)
              {
                for(field_index k = field_index(0); k < num_field; ++k)
                  {
                    (*result)[this->fl.flatten(i, j, k)] = this->compute_component(i, j, k, k1, k2, k3, a);
                  }
              }
          }

        return(result);
      }


    GiNaC::ex canonical_C::compute_component(field_index i, field_index j, field_index k,
                                             GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        unsigned int index = this->fl.flatten(i, j, k);
        std::unique_ptr<ginac_cache_args> args = this->res.generate_arguments(0, this->printer);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(k3);
        args->push_back(a);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::C_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) this->populate_cache();

            GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2) / 2;
            GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3) / 2;
            GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3) / 2;

            result = 0;
            if(i == j) result += -(*derivs)[this->fl.flatten(k)] / (2*Mp*Mp);

            result += ( (*derivs)[this->fl.flatten(i)] * (*derivs)[this->fl.flatten(j)] * (*derivs)[this->fl.flatten(k)] / (8 * Mp*Mp*Mp*Mp) ) * (1 - k1dotk2*k1dotk2 / (k1*k1 * k2*k2));

            if(j == k) result += ((*derivs)[this->fl.flatten(i)] / (Mp*Mp)) * (k1dotk3 / (k1*k1)) / 2;
            if(i == k) result += ((*derivs)[this->fl.flatten(j)] / (Mp*Mp)) * (k2dotk3 / (k2*k2)) / 2;

            this->cache.store(expression_item_types::C_item, index, *args, result);
          }

        return(result);
      }


    void canonical_C::populate_cache()
      {
        derivs = this->shared.generate_derivs(this->printer);
        Mp = this->shared.generate_Mp();
        cached = true;
      }

  }   // namespace canonical
