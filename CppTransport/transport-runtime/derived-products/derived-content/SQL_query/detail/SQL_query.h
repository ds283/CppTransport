//
// Created by David Seery on 28/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __SQL_query_impl_H_
#define __SQL_query_impl_H_


#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_policy.h"


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
