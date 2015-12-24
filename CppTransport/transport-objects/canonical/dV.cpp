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

        GiNaC::ex result = this->res.dV_resource(i, this->printer);

        return std::make_unique<atomic_lambda>(i, result);
      }

  }   // namespace canonical
