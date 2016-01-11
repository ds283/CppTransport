//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __range_helper_H_
#define __range_helper_H_


#include "transport-runtime-api/concepts/range_detail/common.h"


namespace transport
	{

    namespace range_helper
			{

				template <typename value>
				range<value>* deserialize(Json::Value& reader)
					{
				    std::string type = reader[CPPTRANSPORT_NODE_RANGE_TYPE].asString();

						if(type == CPPTRANSPORT_NODE_RANGE_STEPPING)       return new stepping_range<value>(reader);
						else if(type == CPPTRANSPORT_NODE_RANGE_AGGREGATE) return new aggregation_range<value>(reader);

				    std::ostringstream msg;
				    msg << CPPTRANSPORT_RANGE_UNKNOWN_TYPE << " '" << type << "'";
				    throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
					}

			}   // namespace range_helper

	}   // namespace transport


#endif //__range_helper_H_
