//
// Created by David Seery on 22/06/15.
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

#ifndef CPPTRANSPORT_REPOSITORY_STORE_H
#define CPPTRANSPORT_REPOSITORY_STORE_H


#include "transport-runtime/sqlite3/operations/repository_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        void store_package(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename)
          {
            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_PACKAGE_TABLE << " VALUES (@name, @path)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@path"), filename.c_str(), filename.length(), SQLITE_STATIC));

            std::ostringstream fail_msg;
            fail_msg << CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL << " '" << name << "' " << CPPTRANSPORT_REPO_BACKEND_FAIL;
            check_stmt(db, sqlite3_step(stmt), fail_msg.str(), SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        void store_integration_task(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& pkg)
          {
            std::ostringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << " VALUES (@name, @package, @path)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@package"), pkg.c_str(), pkg.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@path"), filename.c_str(), filename.length(), SQLITE_STATIC));

            std::ostringstream fail_msg;
            fail_msg << CPPTRANSPORT_REPO_STORE_TASK_FAIL << " '" << name << "' " << CPPTRANSPORT_REPO_BACKEND_FAIL;
            check_stmt(db, sqlite3_step(stmt), fail_msg.str(), SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        void store_postintegration_task(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& parent)
          {
            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE << " VALUES (@name, @parent, @path)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@parent"), parent.c_str(), parent.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@path"), filename.c_str(), filename.length(), SQLITE_STATIC));

            std::ostringstream fail_msg;
            fail_msg << CPPTRANSPORT_REPO_STORE_TASK_FAIL << " '" << name << "' " << CPPTRANSPORT_REPO_BACKEND_FAIL;
            check_stmt(db, sqlite3_step(stmt), fail_msg.str(), SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        void store_output_task(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename)
          {
            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_OUTPUT_TASKS_TABLE << " VALUES (@name, @path)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@path"), filename.c_str(), filename.length(), SQLITE_STATIC));

            std::ostringstream fail_msg;
            fail_msg << CPPTRANSPORT_REPO_STORE_TASK_FAIL << " '" << name << "' " << CPPTRANSPORT_REPO_BACKEND_FAIL;
            check_stmt(db, sqlite3_step(stmt), fail_msg.str(), SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        void store_product(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename)
          {
            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE << " VALUES (@name, @path)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@path"), filename.c_str(), filename.length(), SQLITE_STATIC));

            std::ostringstream fail_msg;
            fail_msg << CPPTRANSPORT_REPO_STORE_PRODUCT_FAIL << " '" << name << "' " << CPPTRANSPORT_REPO_BACKEND_FAIL;
            check_stmt(db, sqlite3_step(stmt), fail_msg.str(), SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        void generic_store_group(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename,
                                 const std::string& task, const boost::posix_time::ptime& posix_time, const std::string table)
          {
            unsigned int count = internal_count(db, name, CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE, "name");
            if(count != 1)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_COMMIT_OUTPUT_NOT_RESERVED << " '" << name << "'";
                throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            std::ostringstream store_stmt;
            store_stmt << "INSERT INTO " << table << " VALUES (@name, @task, @path, @posix_time)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@task"), task.c_str(), task.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@path"), filename.c_str(), filename.length(), SQLITE_STATIC));

            std::string posix_time_string = boost::posix_time::to_iso_string(posix_time);
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@posix_time"), posix_time_string.c_str(), posix_time_string.length(), SQLITE_STATIC));

            std::ostringstream fail_msg;
            fail_msg << CPPTRANSPORT_REPO_STORE_OUTPUT_FAIL << " '" << name << "' " << CPPTRANSPORT_REPO_BACKEND_FAIL;
            check_stmt(db, sqlite3_step(stmt), fail_msg.str(), SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        template <typename Payload>
        void store_group(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename,
                         const std::string& task, const boost::posix_time::ptime& posix_time);


        template <>
        void store_group<integration_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename,
                                              const std::string& task, const boost::posix_time::ptime& posix_time)
          {
            generic_store_group(mgr, db, name, filename, task, posix_time, CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE);
          }


        template <>
        void store_group<postintegration_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename,
                                                  const std::string& task, const boost::posix_time::ptime& posix_time)
          {
            generic_store_group(mgr, db, name, filename, task, posix_time, CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE);
          }


        template <>
        void store_group<output_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename,
                                         const std::string& task, const boost::posix_time::ptime& posix_time)
          {
            generic_store_group(mgr, db, name, filename, task, posix_time, CPPTRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE);
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_STORE_H
