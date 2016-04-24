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


#ifndef __SQL_threepf_query_H_
#define __SQL_threepf_query_H_


#include <string>

#include "transport-runtime/derived-products/derived-content/SQL_query/detail/SQL_query.h"


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

        enum class SQL_config_type { kconfig, k1_config, k2_config, k3_config };


		    //! Manager for SQL-ish query to filter threepf k-configurations
		    class SQL_threepf_kconfig_query: public SQL_query
			    {

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
            SQL_config_type get_config_type() const { return(this->config); }

				    //! set current setting
				    void set_config_type(SQL_config_type t) { this->config = t; }


		        // EXTRACT QUERY

		      public:

            //! build a full SQL query using the supplied policy object
		        virtual std::string make_query(const SQL_policy& policy, bool serials_only=false) const override;

            //! extract the raw query snippet
            virtual const std::string& get_query_string() const override { return(this->query); }


		        // CLONE -- required by 'linecache'

		      public:

		        virtual SQL_threepf_kconfig_query* clone() const override { return new SQL_threepf_kconfig_query(dynamic_cast<const SQL_threepf_kconfig_query&>(*this)); }


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        virtual void serialize(Json::Value& writer) const override;


		        // INTERNAL DATA

		      protected:

            //! raw query string
		        std::string query;

				    //! configuration to query for -- full threepf? or one of the twopf configurations corresponding to a side?
            SQL_config_type config;

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
			      config(SQL_config_type::kconfig),
		        my_id(current_id++)
			    {
			    }


		    SQL_threepf_kconfig_query::SQL_threepf_kconfig_query(Json::Value& reader)
		      : my_id(current_id++)
			    {
		        query = reader[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY].asString();

		        std::string type = reader[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG].asString();\

				    config = SQL_config_type::kconfig;
				    if(type == CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_KCONFIG)        config = SQL_config_type::kconfig;
				    else if(type == CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K1_CONFIG) config = SQL_config_type::k1_config;
				    else if(type == CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K2_CONFIG) config = SQL_config_type::k2_config;
				    else if(type == CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K3_CONFIG) config = SQL_config_type::k3_config;
				    else assert(false);
			    }


		    void SQL_threepf_kconfig_query::serialize(Json::Value& writer) const
			    {
		        writer[CPPTRANSPORT_NODE_SQL_QUERY_TYPE]            = std::string(CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_TYPE);
		        writer[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY] = this->query;

				    switch(this->config)
					    {
				        case SQL_config_type::kconfig:
					        writer[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG] = std::string(CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_KCONFIG);
					        break;

				        case SQL_config_type::k1_config:
					        writer[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG] = std::string(CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K1_CONFIG);
							    break;

				        case SQL_config_type::k2_config:
					        writer[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG] = std::string(CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K2_CONFIG);
					        break;

                case SQL_config_type::k3_config:
					        writer[CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_CONFIG] = std::string(CPPTRANSPORT_NODE_SQL_THREEPF_KCONFIG_QUERY_K3_CONFIG);
					        break;
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
				        case SQL_config_type::kconfig:
					        {
				            query << " FROM " << policy.threepf_sample_table() << " WHERE " << this->query;
				            break;
					        }

				        case SQL_config_type::k1_config:
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
                      break;
					        }

				        case SQL_config_type::k2_config:
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
                      break;
					        }

                case SQL_config_type::k3_config:
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
                      break;
					        }
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
