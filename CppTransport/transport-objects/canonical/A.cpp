//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "A.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_A::compute(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
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


    GiNaC::ex canonical_A::compute_component(field_index i, field_index j, field_index k,
                                             GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        unsigned int index = this->fl.flatten(i, j, k);
        std::unique_ptr<ginac_cache_args> args = this->res.generate_arguments(use_dV_argument | use_ddV_argument | use_dddV_argument, this->printer);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(k3);
        args->push_back(a);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::A_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) this->populate_cache();

            GiNaC::ex Vijk = (*dddV)[this->fl.flatten(i,j,k)];
            GiNaC::ex Vij  = (*ddV)[this->fl.flatten(i,j)];
            GiNaC::ex Vjk  = (*ddV)[this->fl.flatten(j,k)];
            GiNaC::ex Vik  = (*ddV)[this->fl.flatten(i,k)];

            GiNaC::ex xi_i = -2*(3-eps)*(*derivs)[this->fl.flatten(i)] - 2*(*dV)[this->fl.flatten(i)]/Hsq;
            GiNaC::ex xi_j = -2*(3-eps)*(*derivs)[this->fl.flatten(j)] - 2*(*dV)[this->fl.flatten(j)]/Hsq;
            GiNaC::ex xi_k = -2*(3-eps)*(*derivs)[this->fl.flatten(k)] - 2*(*dV)[this->fl.flatten(k)]/Hsq;

            GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2) / 2;
            GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3) / 2;
            GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3) / 2;

            result = - Vijk / (3*Hsq);

            result += - ((*derivs)[this->fl.flatten(i)] / (2*Mp*Mp)) * (Vjk / (3*Hsq))
                      - ((*derivs)[this->fl.flatten(j)] / (2*Mp*Mp)) * (Vik / (3*Hsq))
                      - ((*derivs)[this->fl.flatten(k)] / (2*Mp*Mp)) * (Vij / (3*Hsq));

            result += + ((*derivs)[this->fl.flatten(i)] * (*derivs)[this->fl.flatten(j)] * xi_k) / (8 * 3 * Mp*Mp*Mp*Mp)
                      + ((*derivs)[this->fl.flatten(j)] * (*derivs)[this->fl.flatten(k)] * xi_i) / (8 * 3 * Mp*Mp*Mp*Mp)
                      + ((*derivs)[this->fl.flatten(i)] * (*derivs)[this->fl.flatten(k)] * xi_j) / (8 * 3 * Mp*Mp*Mp*Mp);

            result += + ((*derivs)[this->fl.flatten(i)] * xi_j * xi_k) / (32*Mp*Mp*Mp*Mp) * (1 - k2dotk3*k2dotk3 / (k2*k2 * k3*k3)) / 3
                      + ((*derivs)[this->fl.flatten(j)] * xi_i * xi_k) / (32*Mp*Mp*Mp*Mp) * (1 - k1dotk3*k1dotk3 / (k1*k1 * k3*k3)) / 3
                      + ((*derivs)[this->fl.flatten(k)] * xi_i * xi_j) / (32*Mp*Mp*Mp*Mp) * (1 - k1dotk2*k1dotk2 / (k1*k1 * k2*k2)) / 3;

            result +=( (*derivs)[this->fl.flatten(i)] * (*derivs)[this->fl.flatten(j)] * (*derivs)[this->fl.flatten(k)] ) / (8 * Mp*Mp*Mp*Mp) * (6 - 2*eps);

            if(j == k) result += ( (*derivs)[this->fl.flatten(i)] / (2*Mp*Mp) ) * k2dotk3 / (3*a*a*Hsq);
            if(i == k) result += ( (*derivs)[this->fl.flatten(j)] / (2*Mp*Mp) ) * k1dotk3 / (3*a*a*Hsq);
            if(i == j) result += ( (*derivs)[this->fl.flatten(k)] / (2*Mp*Mp) ) * k1dotk2 / (3*a*a*Hsq);

            this->cache.store(expression_item_types::A_item, index, *args, result);
          }

        return(result);
      }


    void canonical_A::populate_cache()
      {
        derivs = this->shared.generate_derivs(this->printer);
        dV = this->res.dV_resource(this->printer);
        ddV = this->res.ddV_resource(this->printer);
        dddV = this->res.dddV_resource(this->printer);
        Hsq = this->res.Hsq_resource(this->printer);
        eps = this->res.eps_resource(this->printer);
        Mp = this->shared.generate_Mp();
        cached = true;
      }


    enum unroll_behaviour canonical_A::get_unroll()
      {
        if(this->shared.roll_coordinates() && this->res.roll_dV() && this->res.roll_ddV() && this->res.roll_dddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }
  }   // namespace canonical
