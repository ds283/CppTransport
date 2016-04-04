//
// Created by David Seery on 22/06/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_CREATE_H
#define CPPTRANSPORT_REPOSITORY_CREATE_H


#include "transport-runtime-api/sqlite3/operations/repository_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        void create_repository_tables(sqlite3* db)
          {
            std::ostringstream packages_stmt;
            packages_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_PACKAGE_TABLE << "("
              << "name TEXT PRIMARY KEY, "
              << "path TEXT)";
            exec(db, packages_stmt.str());

            std::ostringstream i_tasks_stmt;
            i_tasks_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << "("
              << "name    TEXT PRIMARY KEY, "
              << "package TEXT, "
              << "path    TEXT, "
              << "FOREIGN KEY(package) REFERENCES " << CPPTRANSPORT_SQLITE_PACKAGE_TABLE << "(name));";
            exec(db, i_tasks_stmt.str());

            std::ostringstream p_tasks_stmt;
            p_tasks_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE << "("
              << "name   TEXT PRIMARY KEY, "
              << "parent TEXT, "
              << "path   TEXT, "
              << "FOREIGN KEY(parent) REFERENCES " << CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << "(name));";
            exec(db, p_tasks_stmt.str());

            std::ostringstream o_tasks_stmt;
            o_tasks_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_OUTPUT_TASKS_TABLE << "("
              << "name   TEXT PRIMARY KEY, "
              << "path   TEXT);";
            exec(db, o_tasks_stmt.str());

            std::ostringstream products_stmt;
            products_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE << "("
              << "name TEXT PRIMARY KEY, "
              << "path TEXT);";
            exec(db, products_stmt.str());

            std::ostringstream i_groups_stmt;
            i_groups_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE << "("
              << "name TEXT PRIMARY KEY, "
              << "task TEXT, "
              << "path TEXT, "
              << "FOREIGN KEY(task) REFERENCES " << CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << "(name));";
            exec(db, i_groups_stmt.str());

            std::ostringstream p_groups_stmt;
            p_groups_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE << "("
              << "name TEXT PRIMARY KEY, "
              << "task TEXT, "
              << "path TEXT, "
              << "FOREIGN KEY(task) REFERENCES " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE << "(name));";
            exec(db, p_groups_stmt.str());

            std::ostringstream o_groups_stmt;
            o_groups_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE << "("
              << "name TEXT PRIMARY KEY, "
              << "task TEXT, "
              << "path TEXT, "
              << "FOREIGN KEY(task) REFERENCES " << CPPTRANSPORT_SQLITE_OUTPUT_TASKS_TABLE << "(name));";
            exec(db, o_groups_stmt.str());

            std::ostringstream o_reserved_stmt;
            o_reserved_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << "("
              << "name       TEXT PRIMARY KEY, "
              << "task       TEXT, "
              << "path       TEXT, "
              << "posix_time TEXT, "
              << "cores      INTEGER, "
              << "completion TEXT);";   // no obvious way to implement constraint on task names
            exec(db, o_reserved_stmt.str());

            std::ostringstream o_int_writer_stmt;
            o_int_writer_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_INTEGRATION_WRITERS_TABLE << "("
              << "content_group    TEXT PRIMARY KEY, "
              << "task             TEXT, "
              << "output           TEXT, "
              << "container        TEXT, "
              << "logdir           TEXT, "
              << "tempdir          TEXT, "
              << "workgroup_number INTEGER, "
              << "seeded           INTEGER, "
              << "seed_group       TEXT, "
              << "collect_stats    INTEGER, "
              << "collect_ics      INTEGER, "
              << "FOREIGN KEY(content_group) REFERENCES " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << "(name), "
              << "FOREIGN KEY(task) REFERENCES " << CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << "(name), "
              << "FOREIGN KEY(seed_group) REFERENCES " << CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE << "(name));";
            exec(db, o_int_writer_stmt.str());

            std::ostringstream o_pint_writer_stmt;
            o_pint_writer_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_WRITERS_TABLE << "("
              << "content_group    TEXT PRIMARY KEY, "
              << "task             TEXT, "
              << "output           TEXT, "
              << "container        TEXT, "
              << "logdir           TEXT, "
              << "tempdir          TEXT, "
              << "paired           INTEGER, "
              << "parent           TEXT, "        // parent integration group, or paired integration group if this is a paired writer
              << "seeded           INTEGER, "
              << "seed_group       TEXT, "
              << "FOREIGN KEY(content_group) REFERENCES " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << "(name), "
              << "FOREIGN KEY(task) REFERENCES " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE << "(name), "
              << "FOREIGN KEY(seed_group) REFERENCES " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE << "(name));";
            exec(db, o_pint_writer_stmt.str());

            std::ostringstream o_out_writer_stmt;
            o_out_writer_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_DERIVED_WRITERS_TABLE << "("
              << "content_group TEXT PRIMARY KEY, "
              << "task          TEXT, "
              << "output        TEXT, "
              << "logdir        TEXT, "
              << "tempdir       TEXT, "
              << "FOREIGN KEY(content_group) REFERENCES " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << "(name), "
              << "FOREIGN KEY(task) REFERENCES " << CPPTRANSPORT_SQLITE_OUTPUT_TASKS_TABLE << "(name));";
            exec(db, o_out_writer_stmt.str());
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_CREATE_H
