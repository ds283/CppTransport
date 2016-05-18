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


#ifndef CPPTRANSPORT_SQL_TIME_QUERY_H
#define CPPTRANSPORT_SQL_TIME_QUERY_H


#include <string>

#include "transport-runtime/derived-products/derived-content/SQL_query/detail/SQL_query.h"


#define CPPTRANSPORT_NODE_SQL_TIME_QUERY_TYPE "time-query"
#define CPPTRANSPORT_NODE_SQL_TIME_QUERY      "sql"


namespace transport
	{

    namespace derived_data
	    {

        //! Manager for SQL-ish query to filter time configurations
        class SQL_time_query: public SQL_query
	        {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor with given SQL query
            SQL_time_query(const std::string q);

            //! deserialization constructor
            SQL_time_query(Json::Value& reader);

            //! destructor is default
            virtual ~SQL_time_query() = default;

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

		        virtual SQL_time_query* clone() const override { return new SQL_time_query(dynamic_cast<const SQL_time_query&>(*this)); }


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! raw query string
            std::string query;

	        };


        // SQL TIME QUERY


        SQL_time_query::SQL_time_query(const std::string q)
	        : query(q)
	        {
	        }


        SQL_time_query::SQL_time_query(Json::Value& reader)
	        {
		        query = reader[CPPTRANSPORT_NODE_SQL_TIME_QUERY].asString();
	        }


        void SQL_time_query::serialize(Json::Value& writer) const
	        {
            writer[CPPTRANSPORT_NODE_SQL_QUERY_TYPE] = std::string(CPPTRANSPORT_NODE_SQL_TIME_QUERY_TYPE);
            writer[CPPTRANSPORT_NODE_SQL_TIME_QUERY] = this->query;
	        }


        std::string SQL_time_query::make_query(const SQL_policy& policy, bool serials_only) const
	        {
            std::ostringstream query;

            query << "SELECT ";

            if(serials_only) query << policy.time_serial_column();
            else             query << "*";

            query << " FROM " << policy.time_sample_table() << " WHERE " << this->query;

            return(query.str());
	        }


        bool SQL_time_query::operator==(const SQL_query& query) const
	        {
            const SQL_time_query* ptr = dynamic_cast<const SQL_time_query*>(&query);

            if(ptr == nullptr) return(false);
            return(this->query == ptr->query);
	        }

	    }   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_SQL_TIME_QUERY_H
