//
// Created by David Seery on 22/03/2016.
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

#ifndef CPPTRANSPORT_DERIVED_PRODUCT_TYPE_H
#define CPPTRANSPORT_DERIVED_PRODUCT_TYPE_H


#include <string>


#include "transport-runtime/messages.h"


namespace transport
  {

    namespace derived_data
      {

        enum class derived_product_type
          {
            line_2dplot,
            line_table
          };


        inline std::string derived_product_type_to_string(derived_product_type type)
          {
            switch(type)
              {
                case derived_product_type::line_2dplot:
                  return std::string(CPPTRANSPORT_DERIVED_PRODUCT_LINE_2D_PLOT);

                case derived_product_type::line_table:
                  return std::string(CPPTRANSPORT_DERIVED_PRODUCT_LINE_TABLE);
              }
          }

      }   // namespace derived_data

  }   // namespace transport


#endif //CPPTRANSPORT_DERIVED_PRODUCT_TYPE_H
