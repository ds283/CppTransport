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

  }
