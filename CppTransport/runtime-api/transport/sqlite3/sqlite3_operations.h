//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __sqlite3_operations_H_
#define __sqlite3_operations_H_


#include <set>

#include "transport/tasks/task.h"
#include "transport/scheduler/work_queue.h"

#include "transport/exceptions.h"
#include "transport/messages_en.h"

#include "boost/lexical_cast.hpp"

#include "sqlite3.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        // Create a sample table of times
        template <typename number>
        void create_time_sample_table(sqlite3* db, task<number>* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            std::vector<double> sample_times = tk->get_sample_times();

            // set up a table
            std::stringstream create_stmt;
            create_stmt << "CREATE TABLE time_samples("
                << "serial INTEGER PRIMARY KEY,"
                << "time   DOUBLE"
                << ");";

            char* errmsg = nullptr;

            int status = sqlite3_exec(db, create_stmt.str().c_str(), nullptr, nullptr, &errmsg);
            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_DATACTR_TIMETAB_FAIL << errmsg << ")";
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
              }

            std::stringstream insert_stmt;
            insert_stmt << "INSERT INTO time_samples VALUES (@serial, @time)";

            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr);

            sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, &errmsg);

            for(unsigned int i = 0; i < sample_times.size(); i++)
              {
                sqlite3_bind_int(stmt, 1, i);
                sqlite3_bind_double(stmt, 2, sample_times[i]);

                status = sqlite3_step(stmt);
                if(status != SQLITE_DONE)
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATACTR_TIMETAB_FAIL << sqlite3_errmsg(db) << ")";
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }

                sqlite3_clear_bindings(stmt);
                sqlite3_reset(stmt);
              }

            sqlite3_exec(db, "END TRANSACTION", nullptr, nullptr, &errmsg);
            sqlite3_finalize(stmt);
          }


        // Create a sample table of twopf configurations
        template <typename number>
        void create_twopf_sample_table(sqlite3* db, twopf_list_task<number>* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            std::vector<double> conventional_list = tk->get_k_list();
            std::vector<double> comoving_list = tk->get_k_list_comoving();

            // set up a table
            std::stringstream stmt_text;
            stmt_text << "CREATE TABLE twopf_samples("
              << "serial       INTEGER PRIMARY KEY,"
              << "conventional DOUBLE,"
              << "comoving     DOUBLE"
              << ");";

            char* errmsg = nullptr;
            int status = sqlite3_exec(db, stmt_text.str().c_str(), nullptr, nullptr, &errmsg);

            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_DATACTR_TWOPFTAB_FAIL << errmsg << ")";
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
              }

            std::stringstream insert_stmt;
            insert_stmt << "INSERT INTO twopf_samples VALUES (@serial, @conventional, @comoving)";

            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr);

            sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, &errmsg);

            for(unsigned int i = 0; i < conventional_list.size(); i++)
              {
                sqlite3_bind_int(stmt, 1, i);
                sqlite3_bind_double(stmt, 2, conventional_list[i]);
                sqlite3_bind_double(stmt, 3, comoving_list[i]);

                status = sqlite3_step(stmt);
                if(status != SQLITE_DONE)
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATACTR_TWOPFTAB_FAIL << sqlite3_errmsg(db) << ")";
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }

                sqlite3_clear_bindings(stmt);
                sqlite3_reset(stmt);
              }

            sqlite3_exec(db, "END TRANSACTION", nullptr, nullptr, &errmsg);
            sqlite3_finalize(stmt);
          }


        // Create a sample table of threepf configurations
        template <typename number>
        void create_threepf_sample_table(sqlite3* db, threepf_task<number>* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            std::vector<threepf_kconfig> threepf_sample = tk->get_sample();

            // set up a table
            std::stringstream stmt_text;
            stmt_text << "CREATE TABLE threepf_samples("
              << "serial          INTEGER PRIMARY KEY,"
              << "wavenumber1     INTEGER,"
              << "wavenumber2     INTEGER,"
              << "wavenumber3     INTEGER,"
              << "kt_comoving     DOUBLE,"
              << "kt_conventional DOUBLE,"
              << "alpha           DOUBLE,"
              << "beta            DOUBLE,"
              << "FOREIGN KEY(wavenumber1) REFERENCES twopf_samples(serial),"
              << "FOREIGN KEY(wavenumber2) REFERENCES twopf_samples(serial),"
              << "FOREIGN KEY(wavenumber3) REFERENCES twopf_samples(serial)"
              << ");";

            char* errmsg = nullptr;
            int status = sqlite3_exec(db, stmt_text.str().c_str(), nullptr, nullptr, &errmsg);

            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_DATACTR_THREEPFTAB_FAIL << errmsg << ")";
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
              }

            std::stringstream insert_stmt;
            insert_stmt << "INSERT INTO threepf_samples VALUES (@serial, @wn1, @wn2, @wn3, @kt_com, @kt_conv, @alpha, @beta)";

            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr);

            sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, &errmsg);

            for(unsigned int i = 0; i < threepf_sample.size(); i++)
              {
                sqlite3_bind_int(stmt, 1, i);
                sqlite3_bind_int(stmt, 2, threepf_sample[i].index[0]);
                sqlite3_bind_int(stmt, 3, threepf_sample[i].index[1]);
                sqlite3_bind_int(stmt, 4, threepf_sample[i].index[2]);
                sqlite3_bind_double(stmt, 5, threepf_sample[i].k_t);
                sqlite3_bind_double(stmt, 6, threepf_sample[i].k_t_conventional);
                sqlite3_bind_double(stmt, 7, threepf_sample[i].alpha);
                sqlite3_bind_double(stmt, 8, threepf_sample[i].beta);

                status = sqlite3_step(stmt);
                if(status != SQLITE_DONE)
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATACTR_THREEPFTAB_FAIL << sqlite3_errmsg(db) << ")";
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }

                sqlite3_clear_bindings(stmt);
                sqlite3_reset(stmt);
              }

            sqlite3_exec(db, "END TRANSACTION", nullptr, nullptr, &errmsg);
            sqlite3_finalize(stmt);
          }


        // Write a taskfile
        template <typename WorkItem>
        void create_taskfile(sqlite3* taskfile, const work_queue<WorkItem>& queue)
          {
            assert(taskfile != nullptr);

            // set up a table
            std::stringstream stmt_text;
            stmt_text << "CREATE TABLE tasklist("
              << "serial INTEGER PRIMARY KEY,"
              << "worker INTEGER"
              << ");";

            char* errmsg = nullptr;
            int status = sqlite3_exec(taskfile, stmt_text.str().c_str(), nullptr, nullptr, &errmsg);

            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_DATACTR_TASKLIST_FAIL << errmsg << ")";
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
              }

            std::stringstream insert_stmt;
            insert_stmt << "INSERT INTO tasklist VALUES (@serial, @worker)";

            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(taskfile, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr);

            sqlite3_exec(taskfile, "BEGIN TRANSACTION", nullptr, nullptr, &errmsg);

            // work through all items in the queue, writing them into the taskfile
            for(size_t device = 0; device < queue.size(); device++)
              {
                const typename work_queue<WorkItem>::device_queue& this_device = queue[device];
                for(size_t list = 0; list < this_device.size(); list++)
                  {
                    const typename work_queue<WorkItem>::device_work_list& this_list = this_device[list];
                    for(size_t item = 0; item < this_list.size(); item++)
                      {
                        const WorkItem& this_item = this_list[item];

                        sqlite3_bind_int(stmt, 1, this_item.serial);  // unique id of this item of work
                        sqlite3_bind_int(stmt, 2, device);            // is assigned to this device

                        status = sqlite3_step(stmt);
                        if(status != SQLITE_DONE)
                          {
                            std::ostringstream msg;
                            msg << __CPP_TRANSPORT_DATACTR_TASKLIST_FAIL << sqlite3_errmsg(taskfile) << ")";
                            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                          }

                        sqlite3_clear_bindings(stmt);
                        sqlite3_reset(stmt);
                      }
                  }
              }

            sqlite3_exec(taskfile, "END TRANSACTION", nullptr, nullptr, &errmsg);
            sqlite3_finalize(stmt);
          }


        // Read a taskfile
        std::set<unsigned int> read_taskfile(const std::string& taskfile_name, unsigned int worker)
          {
            sqlite3* taskfile;
            std::set<unsigned int> work_items;

            int status = sqlite3_open_v2(taskfile_name.c_str(), &taskfile, SQLITE_OPEN_READONLY, nullptr);

            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                if(taskfile != nullptr)
                  {
                    msg << __CPP_TRANSPORT_DATACTR_TASKLIST_OPEN_A << " '" << taskfile_name << "' " << __CPP_TRANSPORT_DATACTR_TASKLIST_OPEN_B << status << ": " << sqlite3_errmsg(taskfile) << ")";
                    sqlite3_close(taskfile);
                  }
                else
                  {
                    msg << __CPP_TRANSPORT_DATACTR_TASKLIST_OPEN_A << " '" << taskfile_name << "' " << __CPP_TRANSPORT_DATACTR_TASKLIST_OPEN_B << status << ")";
                  }
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
              }

            // read tasks from the database
            std::ostringstream select_stmt;
            select_stmt << "SELECT serial FROM tasklist WHERE worker=" << worker;

            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(taskfile, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr);

            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    int serial = sqlite3_column_int(stmt, 0);
                    work_items.insert(serial);
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATACTR_TASKLIST_READ_A << " '" << taskfile_name << "' " << __CPP_TRANSPORT_DATACTR_TASKLIST_READ_B << status << ": " << sqlite3_errmsg(taskfile) << ")";
                    sqlite3_finalize(stmt);
                    sqlite3_close(taskfile);
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }
              }

            sqlite3_finalize(stmt);
            sqlite3_close(taskfile);

            return(work_items);
          }


      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //__sqlite3_operations_H_
