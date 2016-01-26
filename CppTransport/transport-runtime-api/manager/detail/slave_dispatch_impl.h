//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SLAVE_DISPATCH_IMPL_H
#define CPPTRANSPORT_SLAVE_DISPATCH_IMPL_H


#include "transport-runtime-api/manager/detail/slave_dispatch_decl.h"


namespace transport
  {


    template <typename number>
    void slave_dispatch_function<number>::operator()(datapipe<number>* pipe, typename derived_data::derived_product<number>* product, const std::list<std::string>& used_groups)
      {
        this->controller.push_derived_content(pipe, product, used_groups);
      }


    template <typename number>
    void slave_null_dispatch_function<number>::operator()(datapipe<number>* pipe, typename derived_data::derived_product<number>* product, const std::list<std::string>& used_groups)
      {
        this->controller.disallow_push_content(pipe, product);
      }


  }   // namespace transport


#endif //CPPTRANSPORT_SLAVE_DISPATCH_IMPL_H
