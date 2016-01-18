//
// Created by David Seery on 22/06/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_STORE_H
#define CPPTRANSPORT_REPOSITORY_STORE_H


#include "transport-runtime-api/sqlite3/operations/repository_common.h"


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

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        void store_integration_task(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& pkg)
          {
            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << " VALUES (@name, @package, @path)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@package"), pkg.c_str(), pkg.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@path"), filename.c_str(), filename.length(), SQLITE_STATIC));

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

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

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

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

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

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

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        template <typename Payload>
        void store_group(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& task);


        template <>
        void store_group<integration_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& task)
          {
            unsigned int count = internal_count(db, name, CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE, "name");
            if(count != 1)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_COMMIT_OUTPUT_NOT_RESERVED << " '" << name << "'";
                throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE << " VALUES (@name, @task, @path)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, 2, task.c_str(), task.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, 3, filename.c_str(), filename.length(), SQLITE_STATIC));

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));

            // remove the integration writer associated with this content group from the list of in-flight writers
            std::stringstream drop_writer_stmt;
            drop_writer_stmt << "DELETE FROM " << CPPTRANSPORT_SQLITE_INTEGRATION_WRITERS_TABLE << " WHERE content_group='" << name << "'";
            exec(db, drop_writer_stmt.str());

            // remove this content group from the list of in-flight groups
            std::stringstream drop_stmt;
            drop_stmt << "DELETE FROM " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << " WHERE name='" << name << "'";
            exec(db, drop_stmt.str());
          }


        template <>
        void store_group<postintegration_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& task)
          {
            unsigned int count = internal_count(db, name, CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE, "name");
            if(count != 1)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_COMMIT_OUTPUT_NOT_RESERVED << " '" << name << "'";
                throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE << " VALUES (@name, @task, @path)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@task"), task.c_str(), task.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@path"), filename.c_str(), filename.length(), SQLITE_STATIC));

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));

            // remove the postintegration writer associated with this content group from the list of in-flight writers
            std::stringstream drop_writer_stmt;
            drop_writer_stmt << "DELETE FROM " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_WRITERS_TABLE << " WHERE content_group='" << name << "'";
            exec(db, drop_writer_stmt.str());

            // remove this content group from the list of in-flight groups

            std::stringstream drop_stmt;
            drop_stmt << "DELETE FROM " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << " WHERE name='" << name << "'";
            exec(db, drop_stmt.str());
          }


        template <>
        void store_group<output_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& filename, const std::string& task)
          {
            unsigned int count = internal_count(db, name, CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE, "name");
            if(count != 1)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_COMMIT_OUTPUT_NOT_RESERVED << " '" << name << "'";
                throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE << " VALUES (@name, @task, @path)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@task"), task.c_str(), task.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@path"), filename.c_str(), filename.length(), SQLITE_STATIC));

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));

            // remove the derived content writer associated with this content group from the list of in-flight writers
            std::stringstream drop_writer_stmt;
            drop_writer_stmt << "DELETE FROM " << CPPTRANSPORT_SQLITE_DERIVED_WRITERS_TABLE << " WHERE content_group='" << name << "'";
            exec(db, drop_writer_stmt.str());

            // remove this content group from the list of in-flight groups

            std::stringstream drop_stmt;
            drop_stmt << "DELETE FROM " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << " WHERE name='" << name << "'";
            exec(db, drop_stmt.str());
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_STORE_H
