//
// Created by David Seery on 26/03/15.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_DERIVED_PRODUCT_FORWARD_DECLARE_H
#define CPPTRANSPORT_DERIVED_PRODUCT_FORWARD_DECLARE_H


#include <string>

#include "transport-runtime/serialization/serializable.h"
#include "transport-runtime/repository/records/record_finder_decl.h"


namespace transport
	{

		namespace derived_data
			{

#ifndef CPPTRANSPORT_DERIVED_PRODUCT_H
		    template <typename number> class derived_product;
#endif

#ifndef CPPTRANSPORT_DERIVED_PRODUCT_HELPER_H
		    namespace derived_product_helper
			    {

		        template <typename number>
		        std::unique_ptr< derived_data::derived_product<number> > deserialize(const std::string& name, Json::Value& reader, task_finder<number>& finder);

			    }
#endif

			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_DERIVED_PRODUCT_FORWARD_DECLARE_H
