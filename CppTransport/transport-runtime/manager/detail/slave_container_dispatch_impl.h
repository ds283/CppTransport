//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SLAVE_CONTAINER_DISPATCH_IMPL_H
#define CPPTRANSPORT_SLAVE_CONTAINER_DISPATCH_IMPL_H


#include "transport-runtime-api/manager/detail/slave_container_dispatch_decl.h"


namespace transport
  {

    template <typename number>
    void slave_container_dispatch<number>::operator()(generic_batcher& batcher)
      {
        this->controller.push_temp_container(batcher, this->message, this->tag);
      }

  }   // namespace transport


#endif //CPPTRANSPORT_SLAVE_CONTAINER_DISPATCH_IMPL_H
