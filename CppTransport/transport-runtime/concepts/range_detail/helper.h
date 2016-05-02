//
// Created by David Seery on 15/04/15.
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


#ifndef CPPTRANSPORT_RANGE_HELPER_H
#define CPPTRANSPORT_RANGE_HELPER_H


#include "transport-runtime/concepts/range_detail/common.h"


namespace transport
	{

    namespace range_helper
			{

				template <typename value>
				std::unique_ptr< range<value> > deserialize(Json::Value& reader)
					{
				    std::string type = reader[CPPTRANSPORT_NODE_RANGE_TYPE].asString();

						if(type == CPPTRANSPORT_NODE_RANGE_STEPPING)       return std::make_unique< basic_range<value> >(reader);
						else if(type == CPPTRANSPORT_NODE_RANGE_AGGREGATE) return std::make_unique< aggregate_range<value> >(reader);

				    std::ostringstream msg;
				    msg << CPPTRANSPORT_RANGE_UNKNOWN_TYPE << " '" << type << "'";
				    throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
					}

			}   // namespace range_helper

	}   // namespace transport


#endif //CPPTRANSPORT_RANGE_HELPER_H
