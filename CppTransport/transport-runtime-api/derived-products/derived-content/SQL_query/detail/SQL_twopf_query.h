//
// Created by David Seery on 27/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __SQL_twopf_query_H_
#define __SQL_twopf_query_H_


#include <string>

#include "transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_query.h"


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

            virtual std::string make_query(const SQL_policy& policy, bool serials_only=false) const override;


		        // CLONE -- required by 'linecache'

          public:

		        virtual SQL_twopf_kconfig_query* clone() const override { return new SQL_twopf_kconfig_query(dynamic_cast<const SQL_twopf_kconfig_query&>(*this)); }


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

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
