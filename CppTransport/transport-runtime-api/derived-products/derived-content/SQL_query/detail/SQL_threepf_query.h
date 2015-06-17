//
// Created by David Seery on 27/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __SQL_threepf_query_H_
#define __SQL_threepf_query_H_


#include <string>

#include "transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_query.h"


#define CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_TYPE      "threepf-query"
#define CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY           "sql"

#define CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG    "config"
#define CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_KCONFIG   "threepf"
#define CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K1_CONFIG "k1"
#define CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K2_CONFIG "k2"
#define CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K3_CONFIG "k3"

namespace transport
	{

		namespace derived_data
			{

		    //! Manager for SQL-ish query to filter threepf k-configurations
		    class SQL_threepf_kconfig_query: public SQL_query
			    {

		      public:

				    typedef enum { kconfig, k1_config, k2_config, k3_config } config_type;

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! constructor with given SQL query
		        SQL_threepf_kconfig_query(const std::string q);

		        //! deserialization constructor
		        SQL_threepf_kconfig_query(Json::Value& reader);

		        //! destructor is default
		        virtual ~SQL_threepf_kconfig_query() = default;

		        //! comparison operator -- required by 'linecache'
		        virtual bool operator==(const SQL_query& obj) const override;


				    // SET CONFIGURATION TO QUERY FOR

		      public:

				    //! get current setting (defaults to kconfig -- ie. full threepf k-configurations)
				    config_type get_config_type() const { return(this->config); }

				    //! set current setting
				    void set_config_type(config_type t) { this->config = t; }


		        // EXTRACT QUERY

		      public:

		        virtual std::string make_query(const SQL_policy& policy, bool serials_only=false) const override;


		        // CLONE -- required by 'linecache'

		      public:

		        virtual SQL_threepf_kconfig_query* clone() const override { return new SQL_threepf_kconfig_query(dynamic_cast<const SQL_threepf_kconfig_query&>(*this)); }


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        virtual void serialize(Json::Value& writer) const override;


		        // INTERNAL DATA

		      protected:

		        std::string query;

				    //! configuration to query for -- full threepf? or one of the twopf configurations corresponding to a side?
				    config_type config;

				    //! class id
				    unsigned int my_id;

            //! global id counter; initialized below
				    static unsigned int current_id;

			    };


				// initialize static member
				unsigned int SQL_threepf_kconfig_query::current_id = 0;


		    // SQL THREEPF QUERY


		    SQL_threepf_kconfig_query::SQL_threepf_kconfig_query(const std::string q)
			    : query(q),
			      config(kconfig),
		        my_id(current_id++)
			    {
			    }


		    SQL_threepf_kconfig_query::SQL_threepf_kconfig_query(Json::Value& reader)
		      : my_id(current_id++)
			    {
		        query = reader[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY].asString();

		        std::string type = reader[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG].asString();\

				    config = kconfig;
				    if(type == CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_KCONFIG)        config = kconfig;
				    else if(type == CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K1_CONFIG) config = k1_config;
				    else if(type == CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K2_CONFIG) config = k2_config;
				    else if(type == CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K3_CONFIG) config = k3_config;
				    else assert(false);
			    }


		    void SQL_threepf_kconfig_query::serialize(Json::Value& writer) const
			    {
		        writer[CPPTRANSPORT_NODE_SQL_QUERY_TYPE]            = std::string(CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_TYPE);
		        writer[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY] = this->query;

				    switch(this->config)
					    {
				        case kconfig:
					        writer[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG] = std::string(CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_KCONFIG);
					        break;

				        case k1_config:
					        writer[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG] = std::string(CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K1_CONFIG);
							    break;

				        case k2_config:
					        writer[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG] = std::string(CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K2_CONFIG);
					        break;

				        case k3_config:
					        writer[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG] = std::string(CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K3_CONFIG);
					        break;

				        default:
					        assert(false);
					    }
			    }


		    std::string SQL_threepf_kconfig_query::make_query(const SQL_policy& policy, bool serials_only) const
			    {
		        std::ostringstream query;

		        query << "SELECT ";

		        if(serials_only) query << policy.threepf_serial_column();
		        else             query << "*";

				    switch(this->config)
					    {
				        case kconfig:
					        {
				            query << " FROM " << policy.threepf_sample_table() << " WHERE " << this->query;
				            break;
					        }

				        case k1_config:
					        {
				            std::ostringstream temp_table;
						        temp_table << "sql_temp_" << this->my_id;

						        query
							        << " FROM " << policy.twopf_sample_table()
							        << " INNER JOIN ("
					            <<   " SELECT " << policy.threepf_k1_serial_column()
							        <<   " AS col FROM " << policy.threepf_sample_table() << " WHERE " << this->query
						          << ") " << temp_table.str()
						          << " ON " << policy.twopf_sample_table() << "." << policy.twopf_serial_column()
						          << "="
					            << temp_table.str() << ".col";
					        }

				        case k2_config:
					        {
				            std::ostringstream temp_table;
				            temp_table << "sql_temp_" << this->my_id;

				            query
					            << " FROM " << policy.twopf_sample_table()
					            << " INNER JOIN ("
					            <<   " SELECT " << policy.threepf_k2_serial_column()
					            <<   " AS col FROM " << policy.threepf_sample_table() << " WHERE " << this->query
							        << ") " << temp_table.str()
							        << " ON " << policy.twopf_sample_table() << "." << policy.twopf_serial_column()
							        << "="
							        << temp_table.str() << ".col";
					        }

				        case k3_config:
					        {
				            std::ostringstream temp_table;
				            temp_table << "sql_temp_" << this->my_id;

				            query
					            << " FROM " << policy.twopf_sample_table()
					            << " INNER JOIN ("
					            <<   " SELECT " << policy.threepf_k3_serial_column()
					            <<   " AS col FROM " << policy.threepf_sample_table() << " WHERE " << this->query
							        << ") " << temp_table.str()
							        << " ON " << policy.twopf_sample_table() << "." << policy.twopf_serial_column()
							        << "="
							        << temp_table.str() << ".col";
					        }

				        default:
					        assert(false);
					    };



		        return(query.str());
			    }


		    bool SQL_threepf_kconfig_query::operator==(const SQL_query& query) const
			    {
		        const SQL_threepf_kconfig_query* ptr = dynamic_cast<const SQL_threepf_kconfig_query*>(&query);

		        if(ptr == nullptr) return(false);
		        return(this->query == ptr->query && this->config == ptr->config);
			    }

			}   // namespace derived_data

	}   // namespace transport


#endif //__SQL_threepf_query_H_
