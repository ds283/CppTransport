//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "ddV.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_ddV::compute()
      {
        return(this->res.ddV_resource(this->printer));
      }


    enum unroll_behaviour canonical_ddV::get_unroll()
      {
        if(this->res.roll_ddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll
      }


    std::unique_ptr<atomic_lambda> canonical_ddV::compute_lambda(const abstract_index& i, const abstract_index& j)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("ddV");
        if(j.get_class() != index_class::field_only) throw tensor_exception("ddV");

        GiNaC::idx idx_i = this->shared.generate_index(i);
        GiNaC::idx idx_j = this->shared.generate_index(j);

        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(use_dddV_argument, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_j.get_value()));

        GiNaC::ex result = this->res.ddV_resource(i, j, this->printer);

        return std::make_unique<atomic_lambda>(i, j, result, expression_item_types::ddV_lambda, *args);
      }

  }   // namespace canonical
