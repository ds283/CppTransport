//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "dddV.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_dddV::compute()
      {
        return(this->res.dddV_resource(this->printer));
      }


    enum unroll_behaviour canonical_dddV::get_unroll()
      {
        if(this->res.roll_dddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll
      }


    std::unique_ptr<atomic_lambda> canonical_dddV::compute_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("dddV");
        if(j.get_class() != index_class::field_only) throw tensor_exception("dddV");
        if(k.get_class() != index_class::field_only) throw tensor_exception("dddV");

        GiNaC::idx idx_i = this->shared.generate_index(i);
        GiNaC::idx idx_j = this->shared.generate_index(j);
        GiNaC::idx idx_k = this->shared.generate_index(k);

        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(use_dddV_argument, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_j.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_k.get_value()));

        GiNaC::ex result = this->res.dddV_resource(i, j, k, this->printer);

        return std::make_unique<atomic_lambda>(i, j, k, result, expression_item_types::dddV_lambda, *args, this->shared.generate_working_type());
      }

  }
