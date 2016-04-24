//
// Created by David Seery on 28/04/15.
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


#ifndef __SQL_query_impl_H_
#define __SQL_query_impl_H_


#include "transport-runtime/serialization/serializable.h"
#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_policy.h"


namespace transport
	{

		namespace derived_data
			{

				class SQL_query: public serializable
					{

						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! base class has default constructor
						SQL_query() = default;

						//! base class has default destructor
						virtual ~SQL_query() = default;

				    //! comparison operator -- required by 'linecache'
				    virtual bool operator==(const SQL_query& obj) const = 0;


						// EXTRACT QUERY

				  public:

            //! build a full SQL query using the supplied policy object
						virtual std::string make_query(const SQL_policy& policy, bool serials_only) const = 0;

            //! extract the raw query snippet
            virtual const std::string& get_query_string() const = 0;


						// CLONE - required by 'linecache'

				  public:

						virtual SQL_query* clone() const = 0;

					};

			}   // namespace derived_data

	}   // namespace transport


#endif //__SQL_query_impl_H_
