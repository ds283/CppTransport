//
// Created by David Seery on 27/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __SQL_time_query_H_
#define __SQL_time_query_H_


#include <string>

#include "transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_query.h"


#define __CPP_TRANSPORT_NODE_SQL_TIME_QUERY_TYPE "time-query"
#define __CPP_TRANSPORT_NODE_SQL_TIME_QUERY      "sql"


namespace transport
	{

    namespace derived_data
	    {

        //! Manager for SQL-ish query to filter time configurations
        class SQL_time_config_query: public SQL_query
	        {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor with given SQL query
            SQL_time_config_query(const std::string q);

            //! deserialization constructor
            SQL_time_config_query(Json::Value& reader);

            //! destructor is default
            virtual ~SQL_time_config_query() = default;

            //! comparison operator -- required by 'linecache'
            virtual bool operator==(const SQL_query& obj) const override;


            // EXTRACT QUERY

          public:

            virtual std::string make_query(const SQL_policy& policy, bool serials_only=false) const override;


		        // CLONE -- required by 'linecache'

          public:

		        virtual SQL_time_config_query* clone() const override { return new SQL_time_config_query(dynamic_cast<const SQL_time_config_query&>(*this)); }


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            std::string query;

	        };


        // SQL TIME QUERY


        SQL_time_config_query::SQL_time_config_query(const std::string q)
	        : query(q)
	        {
	        }


        SQL_time_config_query::SQL_time_config_query(Json::Value& reader)
	        {
		        query = reader[__CPP_TRANSPORT_NODE_SQL_TIME_QUERY].asString();
	        }


        void SQL_time_config_query::serialize(Json::Value& writer) const
	        {
            writer[__CPP_TRANSPORT_NODE_SQL_QUERY_TYPE] = std::string(__CPP_TRANSPORT_NODE_SQL_TIME_QUERY_TYPE);
            writer[__CPP_TRANSPORT_NODE_SQL_TIME_QUERY] = this->query;
	        }


        std::string SQL_time_config_query::make_query(const SQL_policy& policy, bool serials_only) const
	        {
            std::ostringstream query;

            query << "SELECT ";

            if(serials_only) query << policy.time_serial_column();
            else             query << "*";

            query << " FROM " << policy.time_sample_table() << " WHERE " << this->query;

            return(query.str());
	        }


        bool SQL_time_config_query::operator==(const SQL_query& query) const
	        {
            const SQL_time_config_query* ptr = dynamic_cast<const SQL_time_config_query*>(&query);

            if(ptr == nullptr) return(false);
            return(this->query == ptr->query);
	        }

	    }   // namespace derived_data

	}   // namespace transport


#endif //__SQL_time_query_H_
