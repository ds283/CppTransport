//
// Created by David Seery on 30/04/15.
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


#ifndef __SQL_query_helper_H_
#define __SQL_query_helper_H_


#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"

#include "transport-runtime/exceptions.h"
#include "transport-runtime/messages.h"

namespace transport
	{

		namespace derived_data
			{

				namespace SQL_query_helper
					{

						std::unique_ptr<SQL_query> deserialize(Json::Value& reader)
							{
						    std::string type = reader[CPPTRANSPORT_NODE_SQL_QUERY_TYPE].asString();

								if(type == CPPTRANSPORT_NODE_SQL_TIME_QUERY_TYPE)                 return std::make_unique<SQL_time_config_query>(reader);
								else if(type == CPPTRANSPORT_NODE_SQL_TWOPF_KCONFIG_QUERY)        return std::make_unique<SQL_twopf_kconfig_query>(reader);
								else if(type == CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_TYPE) return std::make_unique<SQL_threepf_kconfig_query>(reader);

						    std::ostringstream msg;
						    msg << CPPTRANSPORT_SQL_QUERY_UNKNOWN_TYPE << " '" << type << "'";
						    throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
							}

					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__SQL_query_helper_H_
