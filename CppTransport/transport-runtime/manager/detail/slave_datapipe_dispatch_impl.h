//
// Created by David Seery on 26/01/2016.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_SLAVE_DISPATCH_IMPL_H
#define CPPTRANSPORT_SLAVE_DISPATCH_IMPL_H


#include "transport-runtime/manager/detail/slave_datapipe_dispatch_decl.h"


namespace transport
  {


    template <typename number>
    void slave_datapipe_dispatch<number>::operator()(datapipe<number>* pipe, typename derived_data::derived_product<number>* product, const content_group_name_set& used_groups)
      {
        this->controller.push_derived_content(pipe, product, used_groups);
      }


    template <typename number>
    void slave_null_dispatch_function<number>::operator()(datapipe<number>* pipe, typename derived_data::derived_product<number>* product, const content_group_name_set& used_groups)
      {
        this->controller.disallow_push_content(pipe, product);
      }


  }   // namespace transport


#endif //CPPTRANSPORT_SLAVE_DISPATCH_IMPL_H
