//
// Created by David Seery on 22/06/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_COUNT_H
#define CPPTRANSPORT_REPOSITORY_COUNT_H


#include "transport-runtime/sqlite3/operations/repository_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        unsigned int count_packages(sqlite3* db, const std::string& name)
          {
            return internal_count(db, name, CPPTRANSPORT_SQLITE_PACKAGE_TABLE, "name");
          }


        unsigned int count_integration_tasks(sqlite3* db, const std::string& name)
          {
            return internal_count(db, name, CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE, "name");
          }


        unsigned int count_postintegration_tasks(sqlite3* db, const std::string& name)
          {
            return internal_count(db, name, CPPTRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE, "name");
          }


        unsigned int count_output_tasks(sqlite3* db, const std::string& name)
          {
            return internal_count(db, name, CPPTRANSPORT_SQLITE_OUTPUT_TASKS_TABLE, "name");
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
            return internal_count(db, name, CPPTRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE, "name");
          }


        unsigned int count_integration_groups(sqlite3* db, const std::string& name)
          {
            return internal_count(db, name, CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE, "name");
          }


        unsigned int count_postintegration_groups(sqlite3* db, const std::string& name)
          {
            return internal_count(db, name, CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE, "name");
          }


        unsigned int count_content_groups(sqlite3* db, const std::string& name)
          {
            return internal_count(db, name, CPPTRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE, "name");
          }


        unsigned int count_groups(sqlite3* db, const std::string& name)
          {
            unsigned int integration_groups     = count_integration_groups(db, name);
            unsigned int postintegration_groups = count_postintegration_groups(db, name);
            unsigned int content_groups          = count_content_groups(db, name);

            return(integration_groups + postintegration_groups + content_groups);
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_COUNT_H
