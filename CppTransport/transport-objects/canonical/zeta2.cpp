//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "zeta2.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_zeta2::compute(GiNaC::symbol& k, GiNaC::symbol& k1,
                                                            GiNaC::symbol& k2, GiNaC::symbol& a)
      {
        std::unique_ptr<flattened_tensor> result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(2));

        const phase_index num_phase = this->shared.get_number_phase();
        this->cached = false;

        for(phase_index i = phase_index(0); i < num_phase; ++i)
          {
            for(phase_index j = phase_index(0); j < num_phase; ++j)
              {
                (*result)[this->fl.flatten(i, j)] = this->compute_component(i, j, k, k1, k2, a);
              }
          }

        return(result);
      }


    GiNaC::ex canonical_zeta2::compute_component(phase_index i, phase_index j,
                                              GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a)
      {
        unsigned int index = this->fl.flatten(i, j);
        std::unique_ptr<ginac_cache_args> args = this->res.generate_arguments(use_dV_argument, this->printer);
        args->push_back(k);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(a);

        GiNaC::ex result;
        if(!this->cache.query(expression_item_types::zxfm2_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            if(!cached) this->populate_cache();

            GiNaC::symbol& deriv_i = (*derivs)[this->fl.flatten(this->traits.to_species(i))];
            GiNaC::symbol& deriv_j = (*derivs)[this->fl.flatten(this->traits.to_species(j))];

            field_index species_i = this->traits.to_species(i);
            field_index species_j = this->traits.to_species(j);

            if(this->traits.is_species(i) && this->traits.is_species(j))
              {
                result = this->expr_field_field(species_i, species_j, deriv_i, deriv_j, k, k1, k2, a);
              }
            else if(this->traits.is_species(i) && this->traits.is_momentum(j))
              {
                result = this->expr_field_momentum(species_i, species_j, deriv_i, deriv_j, k, k1, k2, a);
              }
            else if(this->traits.is_momentum(i) && this->traits.is_species(j))
              {
                result = this->expr_field_momentum(species_j, species_i, deriv_i, deriv_j, k, k2, k1, a);
              }
            else if(this->traits.is_momentum(i) && this->traits.is_momentum(j))
              {
                result = 0;
              }
            else
              {
                // TODO: prefer to throw exception
                assert(false);
              }

            this->cache.store(expression_item_types::zxfm2_item, index, *args, result);
          }

        return(result);
      }


    GiNaC::ex canonical_zeta2::expr_field_field(field_index& i, field_index& j,
                                                GiNaC::symbol& deriv_i, GiNaC::symbol& deriv_j,
                                                GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                                GiNaC::symbol& a)
      {
        // formulae from DS calculation 28 May 2014
        GiNaC::ex result = (-GiNaC::ex(1)/2 + 3/(2*eps) + p/(4*eps*eps)) * deriv_i * deriv_j / (Mp*Mp*Mp*Mp*eps);
        return(result);
      }


    GiNaC::ex canonical_zeta2::expr_field_momentum(field_index& i, field_index& j, GiNaC::symbol& deriv_i, GiNaC::symbol& deriv_j,
                                                   GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a)
      {
        // formulae from DS calculation 28 May 2014

        GiNaC::ex k1dotk2 = (k*k - k1*k1 - k2*k2) / 2;
        GiNaC::ex k12sq = k*k;

        GiNaC::ex result = deriv_i * deriv_j / (2 * Mp*Mp*Mp*Mp * eps*eps);

        if(i == j)
          {
            result += - (k1dotk2 / k12sq) / (2 * Mp*Mp * eps);
            result += - (k1*k1 / k12sq)   / (2 * Mp*Mp * eps);
          }

        return(result);
      }


    void canonical_zeta2::populate_cache()
      {
        derivs = this->shared.generate_derivs(this->printer);
        dV = this->res.dV_resource(this->printer);
        Hsq = this->res.Hsq_resource(this->printer);
        eps = this->res.eps_resource(this->printer);
        Mp = this->shared.generate_Mp();

        // formulae from DS calculation 28 May 2014

        p = 0;
        const field_index num_field = this->shared.get_number_field();
        for(field_index i = field_index(0); i < num_field; ++i)
          {
            p += (*dV)[this->fl.flatten(i)] * (*derivs)[this->fl.flatten(i)];
          }
        p = p / (Mp*Mp*Hsq);

        cached = true;
      }


    enum unroll_behaviour canonical_zeta2::get_unroll()
      {
        if(this->shared.roll_coordinates() && this->res.roll_dV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }
  }   // namespace canonical
