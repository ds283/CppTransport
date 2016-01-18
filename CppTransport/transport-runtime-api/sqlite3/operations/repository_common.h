//
// Created by David Seery on 22/06/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_COMMON_H
#define CPPTRANSPORT_REPOSITORY_COMMON_H


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

#include "boost/lexical_cast.hpp"


#define CPPTRANSPORT_SQLITE_PACKAGE_TABLE                 "packages"
#define CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE       "integration_tasks"
#define CPPTRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE   "postintegration_tasks"
#define CPPTRANSPORT_SQLITE_OUTPUT_TASKS_TABLE            "output_tasks"
#define CPPTRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE        "derived_products"
#define CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE      "integration_groups"
#define CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE  "postintegration_groups"
#define CPPTRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE           "output_groups"
#define CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE  "inflight_content"
#define CPPTRANSPORT_SQLITE_INTEGRATION_WRITERS_TABLE     "integration_writers"
#define CPPTRANSPORT_SQLITE_POSTINTEGRATION_WRITERS_TABLE "postintegration_writers"
#define CPPTRANSPORT_SQLITE_DERIVED_WRITERS_TABLE         "output_writers"


namespace transport
  {

    namespace sqlite3_operations
      {

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
                    msg << CPPTRANSPORT_REPO_COUNT_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));

            if(num_rows != 1)
              {
                std::stringstream msg;
                msg << CPPTRANSPORT_REPO_COUNT_MULTIPLE_RETURN << " (" << num_rows << ")";
                throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            return(result);
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
                    msg << CPPTRANSPORT_REPO_SEARCH_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));

            if(num_rows == 0)
              {
                throw runtime_exception(exception_type::RECORD_NOT_FOUND, name);   // RECORD_NOT_FOUND expects task name in message
              }
            else if(num_rows > 1)
              {
                std::stringstream msg;
                msg << CPPTRANSPORT_REPO_SEARCH_MULTIPLE_RETURN << " (" << num_rows << ")";
                throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            return(result);
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_COMMON_H
