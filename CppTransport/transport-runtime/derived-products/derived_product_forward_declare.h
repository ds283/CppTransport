//
// Created by David Seery on 26/03/15.
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
