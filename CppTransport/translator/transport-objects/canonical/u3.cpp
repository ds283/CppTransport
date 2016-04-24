//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
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
        GiNaC::ex result;

        // we don't actively cache u3 results; the A, B, C agents individually cache their own results, so
        // we can just rely on these

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

        return(result);
      }


    enum unroll_behaviour canonical_u3::get_unroll()
      {
        if(this->shared.roll_coordinates() && this->res.roll_dV() && this->res.roll_ddV() && this->res.roll_dddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<map_lambda> canonical_u3::compute_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k,
                                                             GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("U3");
        if(j.get_class() != index_class::full) throw tensor_exception("U3");
        if(k.get_class() != index_class::full) throw tensor_exception("U3");

        GiNaC::idx idx_i = this->shared.generate_index(i);
        GiNaC::idx idx_j = this->shared.generate_index(j);
        GiNaC::idx idx_k = this->shared.generate_index(k);

        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(use_dV_argument | use_ddV_argument | use_dddV_argument, this->printer);
        args->push_back(k1);
        args->push_back(k2);
        args->push_back(k3);
        args->push_back(a);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_j.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_k.get_value()));

        // convert these indices to species-only indices
        const abstract_index i_field_a = this->traits.species_to_species(i);
        const abstract_index i_field_b = this->traits.momentum_to_species(i);
        const abstract_index j_field_a = this->traits.species_to_species(j);
        const abstract_index j_field_b = this->traits.momentum_to_species(j);
        const abstract_index k_field_a = this->traits.species_to_species(k);
        const abstract_index k_field_b = this->traits.momentum_to_species(k);

        map_lambda_table table(lambda_flattened_map_size(3));

        std::unique_ptr<atomic_lambda> fff = B_agent.compute_lambda(j_field_a, k_field_a, i_field_a, k2, k3, k1, a);
        std::unique_ptr<atomic_lambda> ffm = C_agent.compute_lambda(i_field_a, k_field_b, j_field_a, k1, k3, k2, a);
        std::unique_ptr<atomic_lambda> fmf = C_agent.compute_lambda(i_field_a, j_field_b, k_field_a, k1, k2, k3, a);
        std::unique_ptr<atomic_lambda> mff = A_agent.compute_lambda(i_field_b, j_field_a, k_field_a, k1, k2, k3, a);
        std::unique_ptr<atomic_lambda> mfm = B_agent.compute_lambda(i_field_b, j_field_a, k_field_b, k1, k2, k3, a);
        std::unique_ptr<atomic_lambda> mmf = B_agent.compute_lambda(i_field_b, k_field_a, j_field_b, k1, k3, k2, a);
        std::unique_ptr<atomic_lambda> mmm = C_agent.compute_lambda(j_field_b, k_field_b, i_field_b, k2, k3, k1, a);

        table[lambda_flatten(LAMBDA_FIELD, LAMBDA_FIELD, LAMBDA_FIELD)] = -(**fff);
        table[lambda_flatten(LAMBDA_FIELD, LAMBDA_FIELD, LAMBDA_MOMENTUM)] = -(**ffm);
        table[lambda_flatten(LAMBDA_FIELD, LAMBDA_MOMENTUM, LAMBDA_FIELD)] = -(**fmf);
        table[lambda_flatten(LAMBDA_FIELD, LAMBDA_MOMENTUM, LAMBDA_MOMENTUM)] = 0;
        table[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD, LAMBDA_FIELD)] = 3 * (**mff);
        table[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_FIELD, LAMBDA_MOMENTUM)] = **mfm;
        table[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_MOMENTUM, LAMBDA_FIELD)] = **mmf;
        table[lambda_flatten(LAMBDA_MOMENTUM, LAMBDA_MOMENTUM, LAMBDA_MOMENTUM)] = **mmm;

        return std::make_unique<map_lambda>(i, j, k, table, expression_item_types::U3_lambda, *args, this->shared.generate_working_type());
      }

  }   // namespace canonical
