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

        GiNaC::ex result = this->res.ddV_resource(i, j, this->printer);

        return std::make_unique<atomic_lambda>(i, j, result);
      }

  }   // namespace canonical
