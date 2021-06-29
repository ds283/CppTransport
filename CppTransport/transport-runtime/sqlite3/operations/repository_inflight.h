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

#ifndef CPPTRANSPORT_REPOSITORY_INFLIGHT_H
#define CPPTRANSPORT_REPOSITORY_INFLIGHT_H


#include "transport-runtime/sqlite3/operations/repository_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        std::string reserve_content_name(transaction_manager& mgr, sqlite3* db, const std::string& tk,
                                         boost::filesystem::path& parent_path,
                                         const std::string& posix_time_string, const std::string& suffix,
                                         unsigned int num_cores)
          {
            std::string filename = posix_time_string;
            if(suffix.length() > 0) filename += "-" + suffix;

            // check if a content group with this POSIX timestamp already exists
            // if so, add a -N disambiguating string to the end of the proposed filename
            unsigned int count_inflight        = internal_count(db, filename, CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE, "posix_time");
            unsigned int count_integration     = internal_count(db, filename, CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE, "posix_time");
            unsigned int count_postintegration = internal_count(db, filename, CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE, "posix_time");
            unsigned int count_output          = internal_count(db, filename, CPPTRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE, "posix_time");

            unsigned int count = count_inflight + count_integration + count_postintegration + count_output;

            if(count > 0)
              {
                filename = posix_time_string;
                filename += "-" + boost::lexical_cast<std::string>(count);
                if(suffix.length() > 0) filename += "-" + suffix;
              }

            boost::filesystem::path output_path = parent_path / filename;

            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << " VALUES (@name, @task, @path, @posix_time, @cores, @completion)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), filename.c_str(), filename.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@task"), tk.c_str(), tk.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@path"), output_path.string().c_str(), output_path.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@posix_time"), posix_time_string.c_str(), posix_time_string.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@cores"), num_cores));

            std::string unset(CPPTRANSPORT_DEFAULT_COMPLETION_UNSET);
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@completion"), unset.c_str(), unset.length(), SQLITE_STATIC));

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_RESERVE_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));

            return(filename);
          }


        void deregister_content_group(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string writer_table)
          {
            sqlite3_stmt* stmt;

            // remove the writer associated with this content group from the list of in-flight writers
            std::stringstream drop_writer_stmt;
            drop_writer_stmt << "DELETE FROM " << writer_table << " WHERE content_group=@group;";

            check_stmt(db, sqlite3_prepare_v2(db, drop_writer_stmt.str().c_str(), drop_writer_stmt.str().length()+1, &stmt, nullptr));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@group"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_DEREGISTER_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));

            // remove this content group from the list of in-flight groups
            std::stringstream drop_stmt;
            drop_stmt << "DELETE FROM " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << " WHERE name=@group;";

            check_stmt(db, sqlite3_prepare_v2(db, drop_stmt.str().c_str(), drop_stmt.str().length()+1, &stmt, nullptr));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@group"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_DEREGISTER_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        void advise_completion_time(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& time)
          {
            std::stringstream update_stmt;
            update_stmt << "UPDATE " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << " SET completion=@completion WHERE name=@name;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, update_stmt.str().c_str(), update_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@completion"), time.c_str(), time.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@name"), name.c_str(), name.length(), SQLITE_STATIC));

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_UPDATE_COMPLETION_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        inflight_db enumerate_inflight_groups(sqlite3* db)
          {
            std::stringstream find_stmt;
            find_stmt << "SELECT name, task, path, posix_time, cores, completion FROM " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << ";";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

            int status;
            inflight_db groups;

            groups.clear();
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    std::unique_ptr<inflight_group> gp = std::make_unique<inflight_group>();

                    const char* sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    gp->name = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                    gp->task_name = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                    gp->path = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                    gp->posix_time = std::string(sqlite_str);

                    gp->cores = static_cast<unsigned int>(sqlite3_column_int(stmt, 4));

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
                    gp->completion = std::string(sqlite_str);

                    groups.insert(std::make_pair(gp->name, std::move(gp)));
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));
            return groups;
          }


        void register_integration_writer(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& task,
                                         const boost::filesystem::path& output_path, const boost::filesystem::path& sql_path,
                                         const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                         unsigned int workgroup_number, bool is_seeded, const std::string& seed_group,
                                         bool is_collecting_stats, bool is_collecting_ics)
          {
            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_INTEGRATION_WRITERS_TABLE << " VALUES (@content_group, @task, @output, @container, @logdir, @tempdir, @workgroup_number, @seeded, @seed_group, @collect_stats, @collect_ics)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@content_group"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@task"), task.c_str(), task.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@output"), output_path.string().c_str(), output_path.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@container"), sql_path.string().c_str(), sql_path.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@logdir"), logdir_path.string().c_str(), logdir_path.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@tempdir"), tempdir_path.string().c_str(), tempdir_path.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@workgroup_number"), workgroup_number));
            check_stmt(db, sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@seeded"), static_cast<int>(is_seeded)));
            if(is_seeded)
              {
                // don't set seed_group field unless seeded; SQLite allows NULL to satisfy the foreign key constraint, but not an empty string
                check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@seed_group"), seed_group.c_str(), seed_group.length(), SQLITE_STATIC));
              }
            check_stmt(db, sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@collect_stats"), static_cast<int>(is_collecting_stats)));
            check_stmt(db, sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@collect_ics"), static_cast<int>(is_collecting_ics)));

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_INTEGRATION_WRITER_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        void register_postintegration_writer(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& task,
                                             const boost::filesystem::path& output_path, const boost::filesystem::path& container,
                                             const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                             bool is_paired, const std::string& parent_group,
                                             bool is_seeded, const std::string& seed_group, bool has_spectral)
          {
            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_WRITERS_TABLE << " VALUES (@content_group, @task, @output, @container, @logdir, @tempdir, @paired, @parent, @seeded, @seed_group, @has_spectral)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@content_group"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@task"), task.c_str(), task.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@output"), output_path.string().c_str(), output_path.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@container"), container.string().c_str(), container.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@logdir"), logdir_path.string().c_str(), logdir_path.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@tempdir"), tempdir_path.string().c_str(), tempdir_path.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@paired"), static_cast<int>(is_paired)));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@parent"), parent_group.c_str(), parent_group.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@seeded"), static_cast<int>(is_seeded)));
            check_stmt(db, sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@has_spectral"), static_cast<int>(has_spectral)));
            if(is_seeded)
              {
                // don't set seed_group field unless seeded; SQLite allows NULL to satisfy the foreign key constraint, but not an empty string
                check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@seed_group"), seed_group.c_str(), seed_group.length(), SQLITE_STATIC));
              }

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_POSTINTEGRATION_WRITER_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        void register_derived_content_writer(transaction_manager& mgr, sqlite3* db, const std::string& name, const std::string& task,
                                             const boost::filesystem::path& output_path, const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path)
          {
            std::stringstream store_stmt;
            store_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_DERIVED_WRITERS_TABLE << " VALUES (@content_group, @task, @output, @logdir, @tempdir)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, store_stmt.str().c_str(), store_stmt.str().length()+1, &stmt, nullptr));

            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@content_group"), name.c_str(), name.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@task"), task.c_str(), task.length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@output"), output_path.string().c_str(), output_path.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@logdir"), logdir_path.string().c_str(), logdir_path.string().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@tempdir"), tempdir_path.string().c_str(), tempdir_path.string().length(), SQLITE_STATIC));

            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_REPO_STORE_DERIVED_CONTENT_WRITER_FAIL, SQLITE_DONE);

            check_stmt(db, sqlite3_clear_bindings(stmt));
            check_stmt(db, sqlite3_finalize(stmt));
          }


        inflight_integration_db enumerate_inflight_integrations(sqlite3* db)
          {
            std::stringstream find_stmt;
            find_stmt << "SELECT content_group, task, output, container, logdir, tempdir, workgroup_number, seeded, seed_group, collect_stats, collect_ics FROM " << CPPTRANSPORT_SQLITE_INTEGRATION_WRITERS_TABLE << ";";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

            constexpr auto COL_CONTENT_GROUP = 0;
            constexpr auto COL_TASK = 1;
            constexpr auto COL_OUTPUT = 2;
            constexpr auto COL_CONTAINER = 3;
            constexpr auto COL_LOGDIR = 4;
            constexpr auto COL_TEMPDIR = 5;
            constexpr auto COL_WORKGROUP = 6;
            constexpr auto COL_SEEDED = 7;
            constexpr auto COL_SEED_GROUP = 8;
            constexpr auto COL_HAS_STATS = 9;
            constexpr auto COL_HAS_ICS = 10;

            int status;
            inflight_integration_db groups;

            groups.clear();
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    std::unique_ptr<inflight_integration> gp = std::make_unique<inflight_integration>();

                    const char* sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_CONTENT_GROUP));
                    gp->content_group       = std::string(sqlite_str);

                    sqlite_str   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_TASK));
                    gp->task_name = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_OUTPUT));
                    gp->output  = std::string(sqlite_str);

                    sqlite_str   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_CONTAINER));
                    gp->container = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_LOGDIR));
                    gp->logdir  = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_TEMPDIR));
                    gp->tempdir = std::string(sqlite_str);

                    gp->workgroup_number = static_cast<unsigned int>(sqlite3_column_int(stmt, COL_WORKGROUP));

                    gp->is_seeded = static_cast<bool>(sqlite3_column_int(stmt, COL_SEEDED));

                    if(gp->is_seeded)   // seed_group column will be NULL if not seeded, so don't try to use it
                      {
                        sqlite_str    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_SEED_GROUP));
                        if(sqlite_str != nullptr) gp->seed_group = std::string(sqlite_str);
                      }

                    gp->is_collecting_stats = static_cast<bool>(sqlite3_column_int(stmt, COL_HAS_STATS));
                    gp->is_collecting_ics   = static_cast<bool>(sqlite3_column_int(stmt, COL_HAS_ICS));

                    groups.insert(std::make_pair(gp->content_group, std::move(gp)));
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));
            return groups;
          }


        inflight_postintegration_db enumerate_inflight_postintegrations(sqlite3* db)
          {
            std::stringstream find_stmt;
            find_stmt << "SELECT content_group, task, output, container, logdir, tempdir, paired, parent, seeded, seed_group, has_spectral FROM " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_WRITERS_TABLE << ";";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

            constexpr auto COL_CONTENT_GROUP = 0;
            constexpr auto COL_TASK = 1;
            constexpr auto COL_OUTPUT = 2;
            constexpr auto COL_CONTAINER = 3;
            constexpr auto COL_LOGDIR = 4;
            constexpr auto COL_TEMPDIR = 5;
            constexpr auto COL_PAIRED = 6;
            constexpr auto COL_PARENT = 7;
            constexpr auto COL_SEEDED = 8;
            constexpr auto COL_SEED_GROUP = 9;
            constexpr auto COL_HAS_SPECTRAL = 10;

            int status;
            inflight_postintegration_db groups;

            groups.clear();
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    std::unique_ptr<inflight_postintegration> gp = std::make_unique<inflight_postintegration>();

                    const char* sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_CONTENT_GROUP));
                    gp->content_group = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_TASK));
                    gp->task_name = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_OUTPUT));
                    gp->output = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_CONTAINER));
                    gp->container = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_LOGDIR));
                    gp->logdir = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_TEMPDIR));
                    gp->tempdir = std::string(sqlite_str);

                    gp->is_paired = static_cast<bool>(sqlite3_column_int(stmt, COL_PAIRED));

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_PARENT));
                    gp->parent_group = std::string(sqlite_str);

                    gp->is_seeded = static_cast<bool>(sqlite3_column_int(stmt, COL_SEEDED));

                    if(gp->is_seeded)   // seed_group column will be NULL if not seeded, so don't try to use it
                      {
                        sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_SEED_GROUP));
                        if(sqlite_str != nullptr) gp->seed_group = std::string(sqlite_str);
                      }

                    gp->has_spectral = static_cast<bool>(sqlite3_column_int(stmt, COL_HAS_SPECTRAL));

                    groups.insert(std::make_pair(gp->content_group, std::move(gp)));
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));
            return groups;
          }


        inflight_derived_content_db enumerate_inflight_derived_content(sqlite3* db)
          {
            std::stringstream find_stmt;
            find_stmt << "SELECT content_group, task, output, logdir, tempdir FROM " << CPPTRANSPORT_SQLITE_DERIVED_WRITERS_TABLE << ";";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

            constexpr auto COL_CONTENT_GROUP = 0;
            constexpr auto COL_TASK = 1;
            constexpr auto COL_OUTPUT = 2;
            constexpr auto COL_LOGDIR = 3;
            constexpr auto COL_TEMPDIR = 4;

            int status;
            inflight_derived_content_db groups;

            groups.clear();
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    std::unique_ptr<inflight_derived_content> gp = std::make_unique<inflight_derived_content>();

                    const char* sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_CONTENT_GROUP));
                    gp->content_group       = std::string(sqlite_str);

                    sqlite_str   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_TASK));
                    gp->task_name = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_OUTPUT));
                    gp->output  = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_LOGDIR));
                    gp->logdir  = std::string(sqlite_str);

                    sqlite_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, COL_TEMPDIR));
                    gp->tempdir = std::string(sqlite_str);

                    groups.insert(std::make_pair(gp->content_group, std::move(gp)));
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));
            return groups;
          }


      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_INFLIGHT_H
