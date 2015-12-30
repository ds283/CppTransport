//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "dV.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_dV::compute()
      {
        return(this->res.dV_resource(this->printer));
      }


    enum unroll_behaviour canonical_dV::get_unroll()
      {
        if(this->res.roll_dV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<atomic_lambda> canonical_dV::compute_lambda(const abstract_index& i)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("dV");

        GiNaC::idx idx_i = this->shared.generate_index(i);

        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(use_dddV_argument, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));

        GiNaC::ex result = this->res.dV_resource(i, this->printer);

        return std::make_unique<atomic_lambda>(i, result, expression_item_types::dV_lambda, *args, this->shared.generate_working_type());
      }

  }   // namespace canonical
