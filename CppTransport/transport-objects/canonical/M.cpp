//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "M.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_M::compute()
      {
        std::unique_ptr<flattened_tensor> result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));

        const field_index num_field = this->shared.get_number_field();
        this->cached = false;

        for(field_index i = field_index(0); i < num_field; ++i)
          {
            for(field_index j = field_index(0); j < num_field; ++j)
              {
                (*result)[this->fl.flatten(i, j)] = this->compute_component(i, j);
              }
          }

        return(result);
      }


    GiNaC::ex canonical_M::compute_component(field_index i, field_index j)
      {
        unsigned int index = this->fl.flatten(i, j);
        std::unique_ptr<ginac_cache_args> args = this->res.generate_arguments(use_dV_argument | use_ddV_argument, this->printer);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::M_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) this->populate_cache();

            GiNaC::ex& Vij = (*ddV)[this->fl.flatten(i,j)];
            GiNaC::ex& Vi  = (*dV)[this->fl.flatten(i)];
            GiNaC::ex& Vj  = (*dV)[this->fl.flatten(j)];

            GiNaC::symbol& deriv_i = (*derivs)[this->fl.flatten(i)];
            GiNaC::symbol& deriv_j = (*derivs)[this->fl.flatten(j)];

            GiNaC::idx idx_i = this->shared.generate_index(i);
            GiNaC::idx idx_j = this->shared.generate_index(j);

            result = this->expr(idx_i, idx_j, Vij, Vi, Vj, deriv_i, deriv_j);

            this->cache.store(expression_item_types::M_item, index, *args, result);
          }

        return(result);
      }


    GiNaC::ex canonical_M::expr(GiNaC::idx& i, GiNaC::idx& j, GiNaC::ex& Vij, GiNaC::ex& Vi, GiNaC::ex& Vj,
                                GiNaC::symbol& deriv_i, GiNaC::symbol& deriv_j)
      {
        GiNaC::ex u = -Vij/Hsq;
        u += -(3-eps) * deriv_i * deriv_j / (Mp*Mp);
        u += -1/(Mp*Mp*Hsq) * ( deriv_i*Vj + deriv_j*Vi );

        GiNaC::ex delta_ij = GiNaC::delta_tensor(i, j);

        return delta_ij*eps + u/3;
      }


    void canonical_M::populate_cache()
      {
        derivs = this->shared.generate_derivs(this->printer);
        dV = this->res.dV_resource(this->printer);
        ddV = this->res.ddV_resource(this->printer);
        Hsq = this->res.Hsq_resource(this->printer);
        eps = this->res.eps_resource(this->printer);
        Mp = this->shared.generate_Mp();
        cached = true;
      }


    enum unroll_behaviour canonical_M::get_unroll()
      {
        if(this->shared.roll_coordinates() && this->res.roll_dV() && this->res.roll_ddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<atomic_lambda> canonical_M::compute_lambda(const abstract_index& i, const abstract_index& j)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("M");
        if(j.get_class() != index_class::field_only) throw tensor_exception("M");

        GiNaC::symbol deriv_i = this->shared.generate_derivs(i, this->printer);
        GiNaC::symbol deriv_j = this->shared.generate_derivs(j, this->printer);

        GiNaC::ex Vij  = this->res.ddV_resource(i, j, this->printer);

        GiNaC::ex Vi   = this->res.dV_resource(i, this->printer);
        GiNaC::ex Vj   = this->res.dV_resource(j, this->printer);

        GiNaC::idx idx_i = this->shared.generate_index(i);
        GiNaC::idx idx_j = this->shared.generate_index(j);

        // expr() expects Hsq, eps and Mp to be correctly set up in the cache
        Hsq = this->res.Hsq_resource(this->printer);
        eps = this->res.eps_resource(this->printer);
        this->Mp = this->shared.generate_Mp();

        GiNaC::ex result = this->expr(idx_i, idx_j, Vij, Vi, Vj, deriv_i, deriv_j);

        return std::make_unique<atomic_lambda>(i, j, result);
      }

  }   // namespace canonical
