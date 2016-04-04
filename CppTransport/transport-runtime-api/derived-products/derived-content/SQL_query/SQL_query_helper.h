//
// Created by David Seery on 30/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __SQL_query_helper_H_
#define __SQL_query_helper_H_


#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/messages.h"

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
