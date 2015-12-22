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
  }   // namespace canonical
