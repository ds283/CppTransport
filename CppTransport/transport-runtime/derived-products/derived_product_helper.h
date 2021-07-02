//
// Created by David Seery on 15/05/2014.
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


#ifndef CPPTRANSPORT_DERIVED_PRODUCT_HELPER_H
#define CPPTRANSPORT_DERIVED_PRODUCT_HELPER_H


#include "transport-runtime/derived-products/line-collections/line_plot2d.h"
#include "transport-runtime/derived-products/line-collections/line_asciitable.h"


namespace transport
	{

		namespace derived_data
			{

        namespace derived_product_helper
          {

            template <typename number>
            std::unique_ptr< derived_data::derived_product<number> > deserialize
              (const std::string& name, Json::Value& reader, task_finder<number>& finder)
              {
                std::string type = reader[CPPTRANSPORT_NODE_DERIVED_PRODUCT_TYPE].asString();

                if (type == CPPTRANSPORT_NODE_DERIVED_PRODUCT_LINE_PLOT2D)
                  return std::make_unique< line_plot2d<number> >(name, reader, finder);

                if(type == CPPTRANSPORT_NODE_DERIVED_PRODUCT_LINE_ASCIITABLE)
                  return std::make_unique< line_asciitable<number> >(name, reader, finder);

                std::ostringstream msg;
                msg << CPPTRANSPORT_PRODUCT_UNKNOWN_TYPE << " '" << type << "'";
                throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
              }

          }   // namespace derived_product_helper

			}   // namespace derived_data

	}


#endif //CPPTRANSPORT_DERIVED_PRODUCT_HELPER_H
