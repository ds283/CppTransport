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

#ifndef CPPTRANSPORT_DATAPIPE_DISPATCH_FUNCTION_H
#define CPPTRANSPORT_DATAPIPE_DISPATCH_FUNCTION_H


#include <string>

#include "transport-runtime/derived-products/derived_product_forward_declare.h"


namespace transport
  {

    // forward-declare datapipe
    template <typename number> class datapipe;

    //! dispatch_function is a visitor-pattern type callback used to 'dispatch' derived content (ie. send to the client)
    template <typename number>
    class datapipe_dispatch_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        datapipe_dispatch_function() = default;

        //! destructor is default
        virtual ~datapipe_dispatch_function() = default;


        // INTERFACE

      public:

        //! dispatch
        virtual void operator()(datapipe<number>* pipe, typename derived_data::derived_product<number>* product, const content_group_name_set& used_groups) = 0;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_DATAPIPE_DISPATCH_FUNCTION_H
