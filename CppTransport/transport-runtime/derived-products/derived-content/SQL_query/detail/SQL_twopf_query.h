//
// Created by David Seery on 27/04/15.
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


#ifndef __SQL_twopf_query_H_
#define __SQL_twopf_query_H_


#include <string>

#include "transport-runtime/derived-products/derived-content/SQL_query/detail/SQL_query.h"


#define CPPTRANSPORT_NODE_SQL_TWOPF_KCONFIG_QUERY_TYPE "twopf-query"
#define CPPTRANSPORT_NODE_SQL_TWOPF_KCONFIG_QUERY      "sql"


namespace transport
	{

    namespace derived_data
	    {

        //! Manager for SQL-ish query to filter twopf k-configurations
        class SQL_twopf_kconfig_query: public SQL_query
	        {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor with given SQL query
            SQL_twopf_kconfig_query(const std::string q);

            //! deserialization constructor
            SQL_twopf_kconfig_query(Json::Value& reader);

            //! destructor is default
            virtual ~SQL_twopf_kconfig_query() = default;

		        //! comparison operator -- required by 'linecache'
		        virtual bool operator==(const SQL_query& obj) const override;


            // EXTRACT QUERY

          public:

            //! build a full SQL query using the supplied policy object
            virtual std::string make_query(const SQL_policy& policy, bool serials_only=false) const override;

            //! extract the raw query snippet
            virtual const std::string& get_query_string() const override { return(this->query); }


		        // CLONE -- required by 'linecache'

          public:

		        virtual SQL_twopf_kconfig_query* clone() const override { return new SQL_twopf_kconfig_query(dynamic_cast<const SQL_twopf_kconfig_query&>(*this)); }


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! raw query string
            std::string query;

	        };


        // SQL TWOPF QUERY


        SQL_twopf_kconfig_query::SQL_twopf_kconfig_query(const std::string q)
	        : query(q)
	        {
	        }


        SQL_twopf_kconfig_query::SQL_twopf_kconfig_query(Json::Value& reader)
	        {
            query = reader[CPPTRANSPORT_NODE_SQL_TWOPF_KCONFIG_QUERY].asString();
	        }


        void SQL_twopf_kconfig_query::serialize(Json::Value& writer) const
	        {
            writer[CPPTRANSPORT_NODE_SQL_QUERY_TYPE]          = std::string(CPPTRANSPORT_NODE_SQL_TWOPF_KCONFIG_QUERY_TYPE);
            writer[CPPTRANSPORT_NODE_SQL_TWOPF_KCONFIG_QUERY] = this->query;
	        }


		    std::string SQL_twopf_kconfig_query::make_query(const SQL_policy& policy, bool serials_only) const
			    {
		        std::ostringstream query;

				    query << "SELECT ";

				    if(serials_only) query << policy.twopf_serial_column();
				    else             query << "*";

				    query << " FROM " << policy.twopf_sample_table() << " WHERE " << this->query;

				    return(query.str());
			    }


		    bool SQL_twopf_kconfig_query::operator==(const SQL_query& query) const
			    {
				    const SQL_twopf_kconfig_query* ptr = dynamic_cast<const SQL_twopf_kconfig_query*>(&query);

				    if(ptr == nullptr) return(false);
				    return(this->query == ptr->query);
			    }

	    }   // namespace derived_data

	}   // namespace transport


#endif //__SQL_twopf_query_H_
