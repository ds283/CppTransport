//
// Created by David Seery on 30/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __repository_admin_H_
#define __repository_admin_H_


#include <set>
#include <vector>
#include <string>
#include <sstream>

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "transport-runtime-api/repository/records/repository_records.h"
#include "transport-runtime-api/repository/transaction_manager.h"

#include "sqlite3.h"

#include "transport-runtime-api/sqlite3/operations/sqlite3_utility.h"


#define __CPP_TRANSPORT_SQLITE_PACKAGE_TABLE                "packages"
#define __CPP_TRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE      "integration_tasks"
#define __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE  "postintegration_tasks"
#define __CPP_TRANSPORT_SQLITE_OUTPUT_TASKS_TABLE           "output_tasks"
#define __CPP_TRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE       "derived_products"
#define __CPP_TRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE     "integration_groups"
#define __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE "postintegration_groups"
#define __CPP_TRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE          "output_groups"


namespace transport
	{

		namespace sqlite3_operations
			{

				void create_repository_tables(sqlite3* db)
					{
				    std::ostringstream packages_stmt;
						packages_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_PACKAGE_TABLE << "("
							<< "name TEXT PRIMARY KEY, "
							<< "path TEXT)";
						exec(db, packages_stmt.str());

				    std::ostringstream i_tasks_stmt;
						i_tasks_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << "("
							<< "name    TEXT PRIMARY KEY, "
							<< "package TEXT, "
							<< "path    TEXT, "
							<< "FOREIGN KEY(package) REFERENCES " << __CPP_TRANSPORT_SQLITE_PACKAGE_TABLE << "(name));";
						exec(db, i_tasks_stmt.str());

				    std::ostringstream p_tasks_stmt;
				    p_tasks_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE << "("
					    << "name   TEXT PRIMARY KEY, "
					    << "parent TEXT, "
					    << "path   TEXT, "
					    << "FOREIGN KEY(parent) REFERENCES " << __CPP_TRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << "(name));";
				    exec(db, p_tasks_stmt.str());

				    std::ostringstream o_tasks_stmt;
				    o_tasks_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_OUTPUT_TASKS_TABLE << "("
					    << "name   TEXT PRIMARY KEY, "
					    << "path   TEXT);";
				    exec(db, o_tasks_stmt.str());

				    std::ostringstream products_stmt;
						products_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE << "("
							<< "name TEXT PRIMARY KEY, "
							<< "path TEXT);";
						exec(db, products_stmt.str());

				    std::ostringstream i_groups_stmt;
						i_groups_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE << "("
							<< "name TEXT PRIMARY KEY, "
							<< "task TEXT, "
							<< "path TEXT, "
							<< "FOREIGN KEY(task) REFERENCES " << __CPP_TRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << "(name));";
						exec(db, i_groups_stmt.str());

				    std::ostringstream p_groups_stmt;
				    p_groups_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE << "("
					    << "name TEXT PRIMARY KEY, "
					    << "task TEXT, "
					    << "path TEXT, "
					    << "FOREIGN KEY(task) REFERENCES " << __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE << "(name));";
				    exec(db, p_groups_stmt.str());

				    std::ostringstream o_groups_stmt;
				    o_groups_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE << "("
					    << "name TEXT PRIMARY KEY, "
					    << "task TEXT, "
					    << "path TEXT, "
					    << "FOREIGN KEY(task) REFERENCES " << __CPP_TRANSPORT_SQLITE_OUTPUT_TASKS_TABLE << "(name));";
				    exec(db, o_groups_stmt.str());
					}


				unsigned int internal_count(sqlite3* db, const std::string& name, const std::string& table, const std::string& column)
					{
				    std::stringstream select_stmt;
						select_stmt << "SELECT COUNT(*) FROM " << table << " WHERE " << table << "." << column << "='" << name << "'";

						sqlite3_stmt* stmt;
						check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

						unsigned int result = 0;
						unsigned int num_rows = 0;

						int status;
						while((status = sqlite3_step(stmt)) != SQLITE_DONE)
							{
								if(status == SQLITE_ROW)
									{
										result = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));
										num_rows++;
									}
								else
									{
								    std::ostringstream msg;
										msg << __CPP_TRANSPORT_REPO_COUNT_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
										throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
									}
							}

						check_stmt(db, sqlite3_finalize(stmt));

						if(num_rows != 1)
							{
						    std::stringstream msg;
								msg << __CPP_TRANSPORT_REPO_COUNT_MULTIPLE_RETURN << " (" << num_rows << ")";
								throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
							}

						return(result);
					}


				unsigned int count_packages(sqlite3* db, const std::string& name)
					{
						return internal_count(db, name, __CPP_TRANSPORT_SQLITE_PACKAGE_TABLE, "name");
					}


				unsigned int count_integration_tasks(sqlite3* db, const std::string& name)
					{
						return internal_count(db, name, __CPP_TRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE, "name");
					}


		    unsigned int count_postintegration_tasks(sqlite3* db, const std::string& name)
			    {
		        return internal_count(db, name, __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE, "name");
			    }


		    unsigned int count_output_tasks(sqlite3* db, const std::string& name)
			    {
		        return internal_count(db, name, __CPP_TRANSPORT_SQLITE_OUTPUT_TASKS_TABLE, "name");
			    }


				unsigned int count_tasks(sqlite3* db, const std::string& name)
					{
				    unsigned int integration_tasks     = count_integration_tasks(db, name);
				    unsigned int postintegration_tasks = count_postintegration_tasks(db, name);
				    unsigned int output_tasks          = count_output_tasks(db, name);

						return(integration_tasks + postintegration_tasks + output_tasks);
					}


				unsigned int count_products(sqlite3* db, const std::string& name)
					{
						return internal_count(db, name, __CPP_TRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE, "name");
					}


				unsigned int count_integration_groups(sqlite3* db, const std::string& name)
					{
						return internal_count(db, name, __CPP_TRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE, "name");
					}


		    unsigned int count_postintegration_groups(sqlite3* db, const std::string& name)
			    {
		        return internal_count(db, name, __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE, "name");
			    }


		    unsigned int count_output_groups(sqlite3* db, const std::string& name)
			    {
		        return internal_count(db, name, __CPP_TRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE, "name");
			    }


				unsigned int count_groups(sqlite3* db, const std::string& name)
					{
				    unsigned int integration_groups     = count_integration_groups(db, name);
				    unsigned int postintegration_groups = count_postintegration_groups(db, name);
				    unsigned int output_groups          = count_output_groups(db, name);

						return(integration_groups + postintegration_groups + output_groups);
					}


				void store_package(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename)
					{
				    std::stringstream store_stmt;
						store_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_PACKAGE_TABLE << " VALUES (@name, @path)";

						sqlite3_stmt* stmt;
						check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

						check_stmt(db, sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC));
						check_stmt(db, sqlite3_bind_text(stmt, 2, filename.c_str(), filename.length(), SQLITE_STATIC));

						check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

						check_stmt(db, sqlite3_clear_bindings(stmt));
						check_stmt(db, sqlite3_finalize(stmt));
					}


		    void store_integration_task(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& pkg)
			    {
		        std::stringstream store_stmt;
		        store_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << " VALUES (@name, @package, @path)";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

		        check_stmt(db, sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 2, pkg.c_str(), pkg.length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, 3, filename.c_str(), filename.length(), SQLITE_STATIC));

		        check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

		        check_stmt(db, sqlite3_clear_bindings(stmt));
		        check_stmt(db, sqlite3_finalize(stmt));
			    }


		    void store_postintegration_task(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& parent)
			    {
		        std::stringstream store_stmt;
		        store_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE << " VALUES (@name, @parent, @path)";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

		        check_stmt(db, sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, 2, parent.c_str(), parent.length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, 3, filename.c_str(), filename.length(), SQLITE_STATIC));

		        check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

		        check_stmt(db, sqlite3_clear_bindings(stmt));
		        check_stmt(db, sqlite3_finalize(stmt));
			    }


		    void store_output_task(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename)
			    {
		        std::stringstream store_stmt;
		        store_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_OUTPUT_TASKS_TABLE << " VALUES (@name, @path)";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

		        check_stmt(db, sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, 2, filename.c_str(), filename.length(), SQLITE_STATIC));

		        check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

		        check_stmt(db, sqlite3_clear_bindings(stmt));
		        check_stmt(db, sqlite3_finalize(stmt));
			    }


		    void store_product(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename)
			    {
		        std::stringstream store_stmt;
		        store_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE << " VALUES (@name, @path)";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

		        check_stmt(db, sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, 2, filename.c_str(), filename.length(), SQLITE_STATIC));

		        check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

		        check_stmt(db, sqlite3_clear_bindings(stmt));
		        check_stmt(db, sqlite3_finalize(stmt));
			    }


				template <typename Payload>
				void store_group(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& task);


				template <>
				void store_group<integration_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& task)
					{
				    std::stringstream store_stmt;
				    store_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE << " VALUES (@name, @task, @path)";

				    sqlite3_stmt* stmt;
				    check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

				    check_stmt(db, sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 2, task.c_str(), task.length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 3, filename.c_str(), filename.length(), SQLITE_STATIC));

				    check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

				    check_stmt(db, sqlite3_clear_bindings(stmt));
				    check_stmt(db, sqlite3_finalize(stmt));
					}


		    template <>
		    void store_group<postintegration_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& task)
			    {
		        std::stringstream store_stmt;
		        store_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE << " VALUES (@name, @task, @path)";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

		        check_stmt(db, sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, 2, task.c_str(), task.length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, 3, filename.c_str(), filename.length(), SQLITE_STATIC));

		        check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

		        check_stmt(db, sqlite3_clear_bindings(stmt));
		        check_stmt(db, sqlite3_finalize(stmt));
			    }


		    template <>
		    void store_group<output_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& task)
			    {
		        std::stringstream store_stmt;
		        store_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE << " VALUES (@name, @task, @path)";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

		        check_stmt(db, sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, 2, task.c_str(), task.length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, 3, filename.c_str(), filename.length(), SQLITE_STATIC));

		        check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

		        check_stmt(db, sqlite3_clear_bindings(stmt));
		        check_stmt(db, sqlite3_finalize(stmt));
			    }


				std::string internal_find(sqlite3* db, const std::string& name, const std::string& table, const std::string& column, const std::string& target, const std::string& missing_msg)
					{
				    std::stringstream find_stmt;
				    find_stmt << "SELECT " << target << " FROM " << table << " WHERE " << table << "." << column << "='" << name << "'";

						sqlite3_stmt* stmt;
						check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

				    std::string result;
				    unsigned int num_rows = 0;

				    int status;
				    while((status = sqlite3_step(stmt)) != SQLITE_DONE)
					    {
				        if(status == SQLITE_ROW)
					        {
						        const char* sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
				            result = std::string(sqlite_str);
				            num_rows++;
					        }
				        else
					        {
				            std::ostringstream msg;
				            msg << __CPP_TRANSPORT_REPO_SEARCH_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
				            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
					        }
					    }

				    check_stmt(db, sqlite3_finalize(stmt));

				    if(num_rows == 0)
					    {
				        throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, name);   // RECORD_NOT_FOUND expects task name in message
					    }
						else if(num_rows > 1)
					    {
				        std::stringstream msg;
						    msg << __CPP_TRANSPORT_REPO_SEARCH_MULTIPLE_RETURN << " (" << num_rows << ")";
						    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
					    }

				    return(result);
					}


				std::string find_package(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, __CPP_TRANSPORT_SQLITE_PACKAGE_TABLE, "name", "path", missing_msg);
					}


				std::string find_integration_task(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, __CPP_TRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE, "name", "path", missing_msg);
					}


				std::string find_postintegration_task(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE, "name", "path", missing_msg);
					}


				std::string find_output_task(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, __CPP_TRANSPORT_SQLITE_OUTPUT_TASKS_TABLE, "name", "path", missing_msg);
					}


				std::string find_product(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, __CPP_TRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE, "name", "path", missing_msg);
					}


				template <typename Payload>
				std::string find_group(sqlite3* db, const std::string& name, const std::string& missing_msg);


				template <>
				std::string find_group<integration_payload>(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
						return internal_find(db, name, __CPP_TRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE, "name", "path", missing_msg);
					}


		    template <>
		    std::string find_group<postintegration_payload>(sqlite3* db, const std::string& name, const std::string& missing_msg)
					{
				    return internal_find(db, name, __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE, "name", "path", missing_msg);
					}


		    template <>
		    std::string find_group<output_payload>(sqlite3* db, const std::string& name, const std::string& missing_msg)
			    {
		        return internal_find(db, name, __CPP_TRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE, "name", "path", missing_msg);
			    }


				void internal_enumerate_content_groups(sqlite3* db, const std::string& name, std::list<std::string>& groups, const std::string& table)
					{
				    std::stringstream find_stmt;
						find_stmt << "SELECT name FROM " << table << " WHERE " << table << ".task='" << name << "'";

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
				            msg << __CPP_TRANSPORT_REPO_SEARCH_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
				            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
					        }
					    }

				    check_stmt(db, sqlite3_finalize(stmt));
					}


				template <typename Payload>
				void enumerate_content_groups(sqlite3* db, const std::string& name, std::list<std::string>& groups);


				template <>
				void enumerate_content_groups<integration_payload>(sqlite3* db, const std::string& name, std::list<std::string>& groups)
					{
						internal_enumerate_content_groups(db, name, groups, __CPP_TRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE);
					}


		    template <>
		    void enumerate_content_groups<postintegration_payload>(sqlite3* db, const std::string& name, std::list<std::string>& groups)
			    {
		        internal_enumerate_content_groups(db, name, groups, __CPP_TRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE);
			    }


		    template <>
		    void enumerate_content_groups<output_payload>(sqlite3* db, const std::string& name, std::list<std::string>& groups)
			    {
		        internal_enumerate_content_groups(db, name, groups, __CPP_TRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE);
			    }


			}   // namespace sqlite3_operations


	}   // namespace transport


#endif //__repository_admin_H_
