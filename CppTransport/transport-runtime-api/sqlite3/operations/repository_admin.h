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

			}   // namespace sqlite3_operations

	}   // namespace transport


#endif //__repository_admin_H_
