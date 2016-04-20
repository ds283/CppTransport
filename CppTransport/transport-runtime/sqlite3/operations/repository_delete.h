//
// Created by David Seery on 04/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_DELETE_H
#define CPPTRANSPORT_REPOSITORY_DELETE_H


#include "transport-runtime/sqlite3/operations/repository_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        void generic_delete_group(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& task, const std::string table)
          {
            unsigned int count = internal_count(db, name, table, "name");
            if(count != 1)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_DELETE_OUTPUT_UNSTORED << " '" << name << "'";
                throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            std::ostringstream delete_stmt;
            delete_stmt << "DELETE FROM " << table << " WHERE name=@name AND task=@task;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, delete_stmt.str().c_str(), delete_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@task"), task.c_str(), task.length(), SQLITE_STATIC));

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_DELETE_OUTPUT_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        template <typename Payload>
        void delete_group(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& task);


        template <>
        void delete_group<integration_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& task)
          {
            generic_delete_group(mgr, db, name, task, CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE);
          }


        template <>
        void delete_group<postintegration_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& task)
          {
            generic_delete_group(mgr, db, name, task, CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE);
          }


        template <>
        void delete_group<output_payload>(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& task)
          {
            generic_delete_group(mgr, db, name, task, CPPTRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE);
          }


      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_DELETE_H
