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

  }   // namespace canonical
