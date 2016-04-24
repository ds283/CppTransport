//
// Created by David Seery on 30/03/15.
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


#ifndef CPPTRANSPORT_REPOSITORY_ADMIN_H
#define CPPTRANSPORT_REPOSITORY_ADMIN_H


#include "transport-runtime/sqlite3/operations/repository_common.h"


namespace transport
	{

		namespace sqlite3_operations
			{

				std::string find_package(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, CPPTRANSPORT_SQLITE_PACKAGE_TABLE, "name", "path", missing_msg);
					}


				std::string find_integration_task(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE, "name", "path", missing_msg);
					}


				std::string find_postintegration_task(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, CPPTRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE, "name", "path", missing_msg);
					}


				std::string find_output_task(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, CPPTRANSPORT_SQLITE_OUTPUT_TASKS_TABLE, "name", "path", missing_msg);
					}


				std::string find_product(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, CPPTRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE, "name", "path", missing_msg);
					}


				template <typename Payload>
				std::string find_group(sqlite3* db, const std::string& name, const std::string& missing_msg);


				template <>
				std::string find_group<integration_payload>(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE, "name", "path", missing_msg);
					}


		    template <>
		    std::string find_group<postintegration_payload>(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
				    return internal_find(db, name, CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE, "name", "path", missing_msg);
					}


		    template <>
		    std::string find_group<output_payload>(sqlite3* db, const std::string& name, const std::string& missing_msg)
			    {
		        return internal_find(db, name, CPPTRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE, "name", "path", missing_msg);
			    }


				void internal_enumerate_content_groups(sqlite3* db, const std::string& name, std::list<std::string>& groups, const std::string& table)
					{
				    std::stringstream find_stmt;
						find_stmt << "SELECT name FROM " << table;

            if(name.length() > 0)
              {
                find_stmt << " WHERE " << table << ".task='" << name << "'";
              }
            find_stmt << ";";

				    sqlite3_stmt* stmt;
				    check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

				    int status;
				    while((status = sqlite3_step(stmt)) != SQLITE_DONE)
					    {
				        if(status == SQLITE_ROW)
					        {
						        const char* sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
				            std::string result = std::string(sqlite_str);
						        groups.push_back(result);
					        }
				        else
					        {
				            std::ostringstream msg;
				            msg << CPPTRANSPORT_REPO_SEARCH_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
				            throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
					        }
					    }

				    check_stmt(db, sqlite3_finalize(stmt));
					}


				template <typename Payload>
				void enumerate_content_groups(sqlite3* db, std::list<std::string>& groups, const std::string& name="");


				template <>
				void enumerate_content_groups<integration_payload>(sqlite3* db, std::list<std::string>& groups, const std::string& name)
					{
						internal_enumerate_content_groups(db, name, groups, CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE);
					}


		    template <>
		    void enumerate_content_groups<postintegration_payload>(sqlite3* db, std::list<std::string>& groups, const std::string& name)
			    {
		        internal_enumerate_content_groups(db, name, groups, CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE);
			    }


		    template <>
		    void enumerate_content_groups<output_payload>(sqlite3* db, std::list<std::string>& groups, const std::string& name)
			    {
		        internal_enumerate_content_groups(db, name, groups, CPPTRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE);
			    }


        void internal_enumerate_records(sqlite3* db, std::list<std::string>& list, const std::string& table)
          {
            std::stringstream find_stmt;
            find_stmt << "SELECT name FROM " << table << ";";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    const char* sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    std::string result = std::string(sqlite_str);
                    list.push_back(result);
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_REPO_SEARCH_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));
          }


        void enumerate_packages(sqlite3* db, std::list<std::string>& packages)
          {
            internal_enumerate_records(db, packages, CPPTRANSPORT_SQLITE_PACKAGE_TABLE);
          }


        void enumerate_tasks(sqlite3* db, std::list<std::string>& integration, std::list<std::string>& postintegration, std::list<std::string>& output)
          {
            internal_enumerate_records(db, integration, CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE);
            internal_enumerate_records(db, postintegration, CPPTRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE);
            internal_enumerate_records(db, output, CPPTRANSPORT_SQLITE_OUTPUT_TASKS_TABLE);
          }


        void enumerate_derived_products(sqlite3* db, std::list<std::string>& derived_products)
          {
            internal_enumerate_records(db, derived_products, CPPTRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE);
          }

			}   // namespace sqlite3_operations


	}   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_ADMIN_H
