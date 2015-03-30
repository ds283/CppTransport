//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __derived_product_forward_declare_H_
#define __derived_product_forward_declare_H_


#include <string>

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/repository/records/record_finder.h"


namespace transport
	{

		namespace derived_data
			{

#ifndef __derived_product_H_
		    template <typename number> class derived_product;
#endif

#ifndef __derived_product_helper_H_
		    namespace derived_product_helper
			    {
		        template <typename number>
		        derived_data::derived_product<number>* deserialize(const std::string& name, Json::Value& reader, typename repository_finder<number>::task_finder& finder);
			    }
#endif

			}   // namespace derived_data

	}   // namespace transport


#endif //__derived_product_forward_declare_H_
