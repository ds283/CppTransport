//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "B.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_B::compute(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
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


    GiNaC::ex canonical_B::compute_component(field_index i, field_index j, field_index k,
                                             GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        unsigned int index = this->fl.flatten(i, j, k);
        std::unique_ptr<ginac_cache_args> args = this->res.generate_arguments(use_dV_argument, this->printer);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(k3);
        args->push_back(a);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::B_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) this->populate_cache();

            GiNaC::ex& Vi   = (*dV)[this->fl.flatten(i)];
            GiNaC::ex& Vj   = (*dV)[this->fl.flatten(j)];
            GiNaC::ex& Vk   = (*dV)[this->fl.flatten(k)];

            GiNaC::symbol& deriv_i = (*derivs)[this->fl.flatten(i)];
            GiNaC::symbol& deriv_j = (*derivs)[this->fl.flatten(j)];
            GiNaC::symbol& deriv_k = (*derivs)[this->fl.flatten(k)];

            result = this->expr(i, j, k, Vi, Vj, Vk,
                                deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::B_item, index, *args, result);
          }

        return(result);
      }


    GiNaC::ex canonical_B::expr(field_index& i, field_index& j, field_index& k,
                                GiNaC::ex& Vi, GiNaC::ex& Vj, GiNaC::ex& Vk,
                                GiNaC::symbol& deriv_i, GiNaC::symbol& deriv_j, GiNaC::symbol& deriv_k,
                                GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        GiNaC::ex xi_i = -2*(3-eps) * deriv_i - 2 * Vi/Hsq;
        GiNaC::ex xi_j = -2*(3-eps) * deriv_j - 2 * Vj/Hsq;

        GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2) / 2;
        GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3) / 2;
        GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3) / 2;

        GiNaC::ex result = deriv_i * deriv_j * deriv_k / (4*Mp*Mp*Mp*Mp);

        result += - ( deriv_i * xi_j * deriv_k / (8*Mp*Mp*Mp*Mp) ) * (1 - k2dotk3*k2dotk3 / (k2*k2 * k3*k3)) / 2
                  - ( deriv_j * xi_i * deriv_k / (8*Mp*Mp*Mp*Mp) ) * (1 - k1dotk3*k1dotk3 / (k1*k1 * k3*k3)) / 2;

        if(j == k) result += - (xi_i / (2*Mp*Mp)) * k1dotk2 / (k1*k1) / 2;
        if(i == k) result += - (xi_j / (2*Mp*Mp)) * k1dotk2 / (k2*k2) / 2;

        return(result);
      }


    void canonical_B::populate_cache()
      {
        derivs = this->shared.generate_derivs(this->printer);
        dV = this->res.dV_resource(this->printer);
        Hsq = this->res.Hsq_resource(this->printer);
        eps = this->res.eps_resource(this->printer);
        Mp = this->shared.generate_Mp();
        cached = true;
      }


    enum unroll_behaviour canonical_B::get_unroll()
      {
        if(this->shared.roll_coordinates() && this->res.roll_dV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }
  }   // namespace canonical
