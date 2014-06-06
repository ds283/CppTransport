//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __sqlite3_operations_H_
#define __sqlite3_operations_H_


#include <set>

#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/scheduler/work_queue.h"
#include "transport-runtime-api/models/model.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/messages.h"

#include "boost/lexical_cast.hpp"

#include "sqlite3.h"
#include "transport-runtime-api/manager/data_manager.h"


#define __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE      "time_samples"
#define __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE     "twopf_samples"
#define __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE   "threepf_samples"
#define __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE      "backg"
#define __CPP_TRANSPORT_SQLITE_TWOPF_VALUE_TABLE      "twopf"
#define __CPP_TRANSPORT_SQLITE_TWOPF_REAL_TAG         "re"
#define __CPP_TRANSPORT_SQLITE_TWOPF_IMAGINARY_TAG    "im"
#define __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE    "threepf"
#define __CPP_TRANSPORT_SQLITE_GAUGE_XFM_1_TABLE      "gauge_xfm_1"
#define __CPP_TRANSPORT_SQLITE_GAUGE_XFM_2_TABLE      "gauge_xfm_2"
#define __CPP_TRANSPORT_SQLITE_U2_TABLE               "u2"
#define __CPP_TRANSPORT_SQLITE_U3_TABLE               "u3"
#define __CPP_TRANSPORT_SQLITE_A_TABLE                "A"
#define __CPP_TRANSPORT_SQLITE_B_TABLE                "B"
#define __CPP_TRANSPORT_SQLITE_C_TABLE                "C"

#define __CPP_TRANSPORT_SQLITE_TASKLIST_TABLE         "task_list"
#define __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE      "serial_search"
#define __CPP_TRANSPORT_SQLITE_TEMP_THREEPF_TABLE     "threepf_search"

#define __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME       "tempdb"


namespace transport
  {

    namespace sqlite3_operations
      {

        typedef enum { foreign_keys, no_foreign_keys } add_foreign_keys_type;

        typedef enum { real_twopf, imag_twopf } twopf_value_type;

        typedef enum { gauge_xfm_1, gauge_xfm_2 } gauge_xfm_type;

        // Utility functions
        namespace
          {

            // error-check an exec statement
            inline void exec(sqlite3* db, const std::string& stmt, const std::string& err)
              {
                char* errmsg;

                int status = sqlite3_exec(db, stmt.c_str(), nullptr, nullptr, &errmsg);

                if(status != SQLITE_OK)
                  {
                    std::ostringstream msg;
                    msg << err << errmsg << ") [status=" << status << "]";
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }
              }

            // error-check an exec statement
            inline void exec(sqlite3* db, const std::string& stmt)
              {
                char* errmsg;

                int status = sqlite3_exec(db, stmt.c_str(), nullptr, nullptr, &errmsg);

                if(status != SQLITE_OK)
                  {
                    std::ostringstream msg;
                    msg << errmsg << " [status=" << status << "]";
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }
              }

            // error-check a non-exec statement
            inline void check_stmt(sqlite3* db, int status, const std::string& err, int check_code=SQLITE_OK)
              {
                if(status != check_code)
                  {
                    std::ostringstream msg;
                    msg << err << sqlite3_errmsg(db) << ") [status=" << status << "]";
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }
              }

            // error-check a non-exec statement
            inline void check_stmt(sqlite3* db, int status, int check_code=SQLITE_OK)
              {
                if(status != check_code)
                  {
                    std::ostringstream msg;
                    msg << sqlite3_errmsg(db) << " [status=" << status << "]";
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }
              }

            // construct the name of a twopf table
            inline std::string twopf_table_name(twopf_value_type type)
              {
                return(static_cast<std::string>(__CPP_TRANSPORT_SQLITE_TWOPF_VALUE_TABLE) + "_"
                  + (type == real_twopf ? __CPP_TRANSPORT_SQLITE_TWOPF_REAL_TAG : __CPP_TRANSPORT_SQLITE_TWOPF_IMAGINARY_TAG));
              }

          }   // unnamed namespace


        // Create a sample table of times
        template <typename number>
        void create_time_sample_table(sqlite3* db, integration_task<number>* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            std::vector<time_config> sample_times = tk->get_time_config_list();

            // set up a table
            std::stringstream create_stmt;
            create_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "("
                << "serial INTEGER PRIMARY KEY,"
                << "time   DOUBLE"
                << ");";

            exec(db, create_stmt.str(), __CPP_TRANSPORT_DATACTR_TIMETAB_FAIL);

            std::stringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << " VALUES (@serial, @time)";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(std::vector<time_config>::const_iterator t = sample_times.begin(); t != sample_times.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, (*t).serial));
                check_stmt(db, sqlite3_bind_double(stmt, 2, (*t).t));

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_TIMETAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Create a sample table of twopf configurations
        template <typename number>
        void create_twopf_sample_table(sqlite3* db, twopf_list_task<number>* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            std::vector<twopf_kconfig> kconfig_list = tk->get_twopf_kconfig_list();

            // set up a table
            std::stringstream stmt_text;
            stmt_text << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << "("
              << "serial       INTEGER PRIMARY KEY,"
              << "conventional DOUBLE,"
              << "comoving     DOUBLE"
              << ");";

            exec(db, stmt_text.str(), __CPP_TRANSPORT_DATACTR_TWOPFTAB_FAIL);

            std::stringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " VALUES (@serial, @conventional, @comoving);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(std::vector<twopf_kconfig>::const_iterator t = kconfig_list.begin(); t != kconfig_list.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, (*t).serial));
                check_stmt(db, sqlite3_bind_double(stmt, 2, (*t).k_conventional));
                check_stmt(db, sqlite3_bind_double(stmt, 3, (*t).k));

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_TWOPFTAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Create a sample table of threepf configurations
        template <typename number>
        void create_threepf_sample_table(sqlite3* db, threepf_task<number>* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            std::vector<threepf_kconfig> threepf_sample = tk->get_threepf_kconfig_list();

            // set up a table
            std::stringstream stmt_text;
            stmt_text << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << "("
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

            exec(db, stmt_text.str());

            std::stringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << " VALUES (@serial, @wn1, @wn2, @wn3, @kt_com, @kt_conv, @alpha, @beta);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(unsigned int i = 0; i < threepf_sample.size(); i++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, i));
                check_stmt(db, sqlite3_bind_int(stmt, 2, threepf_sample[i].index[0]));
                check_stmt(db, sqlite3_bind_int(stmt, 3, threepf_sample[i].index[1]));
                check_stmt(db, sqlite3_bind_int(stmt, 4, threepf_sample[i].index[2]));
                check_stmt(db, sqlite3_bind_double(stmt, 5, threepf_sample[i].k_t));
                check_stmt(db, sqlite3_bind_double(stmt, 6, threepf_sample[i].k_t_conventional));
                check_stmt(db, sqlite3_bind_double(stmt, 7, threepf_sample[i].alpha));
                check_stmt(db, sqlite3_bind_double(stmt, 8, threepf_sample[i].beta));

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_THREEPFTAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a taskfile
        template <typename WorkItem>
        void create_taskfile(sqlite3* taskfile, const work_queue<WorkItem>& queue)
          {
            assert(taskfile != nullptr);

            // set up a table
            std::stringstream stmt_text;
            stmt_text << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_TASKLIST_TABLE << "("
                << "serial INTEGER PRIMARY KEY,"
                << "worker INTEGER"
              << ");";

            exec(taskfile, stmt_text.str(), __CPP_TRANSPORT_DATACTR_TASKLIST_FAIL);

            std::stringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_TASKLIST_TABLE << " VALUES (@serial, @worker);";

            sqlite3_stmt* stmt;
            check_stmt(taskfile, sqlite3_prepare_v2(taskfile, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(taskfile, "BEGIN TRANSACTION;");

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

                        check_stmt(taskfile, sqlite3_bind_int(stmt, 1, this_item.get_serial()));  // unique id of this item of work
                        check_stmt(taskfile, sqlite3_bind_int(stmt, 2, device));                  // is assigned to this device

                        check_stmt(taskfile, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_TASKLIST_FAIL, SQLITE_DONE);

                        check_stmt(taskfile, sqlite3_clear_bindings(stmt));
                        check_stmt(taskfile, sqlite3_reset(stmt));
                      }
                  }
              }

            exec(taskfile, "END TRANSACTION;");
            check_stmt(taskfile, sqlite3_finalize(stmt));
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
            select_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_TASKLIST_TABLE << " WHERE worker=" << worker << ";";

            sqlite3_stmt* stmt;
            check_stmt(taskfile, sqlite3_prepare_v2(taskfile, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

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
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(taskfile, sqlite3_finalize(stmt));
            check_stmt(taskfile, sqlite3_close(taskfile));

            return(work_items);
          }


        // Create table for background values
        void create_backg_table(sqlite3* db, unsigned int Nfields, add_foreign_keys_type keys=no_foreign_keys)
          {
            std::ostringstream create_stmt;
            create_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << "("
              << "tserial INTEGER PRIMARY KEY";

            for(unsigned int i = 0; i < 2*Nfields; i++)
              {
                create_stmt << ", coord" << i << " DOUBLE";
              }
            if(keys == foreign_keys) create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)";
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Create table for twopf values
        void create_twopf_table(sqlite3* db, unsigned int Nfields, twopf_value_type type=real_twopf, add_foreign_keys_type keys=no_foreign_keys)
          {
            std::ostringstream create_stmt;
            create_stmt << "CREATE TABLE " << twopf_table_name(type) << "("
              << "tserial INTEGER,"
              << "kserial INTEGER";

            for(unsigned int i = 0; i < 2*Nfields * 2*Nfields; i++)
              {
                create_stmt << ", ele" << i << " DOUBLE";
              }

            create_stmt << ", PRIMARY KEY (tserial, kserial)";
            if(keys == foreign_keys)
              {
                create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)"
                  << ", FOREIGN KEY(kserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << "(serial)";
              }
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Create table for threepf values
        void create_threepf_table(sqlite3* db, unsigned int Nfields, add_foreign_keys_type keys=no_foreign_keys)
          {
            std::ostringstream create_stmt;
            create_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << "("
              << "tserial INTEGER,"
              << "kserial INTEGER";

            for(unsigned int i = 0; i < 2*Nfields * 2*Nfields * 2*Nfields; i++)
              {
                create_stmt << ", ele" << i << " DOUBLE";
              }

            create_stmt << ", PRIMARY KEY (tserial, kserial)";
            if(keys == foreign_keys)
              {
                create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)"
                  << ", FOREIGN KEY(kserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << "(serial)";
              }
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Create table for 1st-order gauge xfm values
        void create_dN_table(sqlite3* db, unsigned int Nfields, add_foreign_keys_type keys=no_foreign_keys)
          {
            std::ostringstream create_stmt;
            create_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_GAUGE_XFM_1_TABLE << "("
              << "tserial INTEGER PRIMARY KEY";

            for(unsigned int i = 0; i < 2*Nfields; i++)
              {
                create_stmt << ", ele" << i << " DOUBLE";
              }

            if(keys == foreign_keys) create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)";
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Create table for 2nd-order gauge xfm values
        void create_ddN_table(sqlite3* db, unsigned int Nfields, add_foreign_keys_type keys=no_foreign_keys)
          {
            std::ostringstream create_stmt;
            create_stmt << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_GAUGE_XFM_2_TABLE << "("
              << "tserial INTEGER PRIMARY KEY";

            for(unsigned int i = 0; i < 2*Nfields*2*Nfields; i++)
              {
                create_stmt << ", ele" << i << " DOUBLE";
              }

            if(keys == foreign_keys) create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)";
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Write a batch of background values
        template <typename number>
        void write_backg(typename data_manager<number>::generic_batcher* batcher,
                         const std::vector<typename data_manager<number>::backg_item>& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            unsigned int Nfields = batcher->get_number_fields();

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << " VALUES (@tserial";

            for(unsigned int i = 0; i < 2*Nfields; i++)
              {
                insert_stmt << ", @coord" << i;
              }
            insert_stmt << ");";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector<typename data_manager<number>::backg_item>::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, (*t).time_serial));
                for(unsigned int i = 0; i < 2*Nfields; i++)
                  {
                    check_stmt(db, sqlite3_bind_double(stmt, i+2, static_cast<double>((*t).coords[i])));    // 'number' must be castable to double
                  }

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_BACKG_DATATAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a batch of twopf values
        template <typename number>
        void write_twopf(twopf_value_type type, typename data_manager<number>::generic_batcher* batcher,
                         const std::vector<typename data_manager<number>::twopf_item>& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            unsigned int Nfields = batcher->get_number_fields();

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << twopf_table_name(type) << " VALUES (@tserial, @kserial";

            for(unsigned int i = 0; i < 2*Nfields*2*Nfields; i++)
              {
                insert_stmt << ", @ele" << i;
              }
            insert_stmt << ");";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector<typename data_manager<number>::twopf_item>::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, (*t).time_serial));
                check_stmt(db, sqlite3_bind_int(stmt, 2, (*t).kconfig_serial));
                for(unsigned int i = 0; i < 2*Nfields*2*Nfields; i++)
                  {
                    check_stmt(db, sqlite3_bind_double(stmt, i+3, static_cast<double>((*t).elements[i])));    // 'number' must be castable to double
                  }

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_TWOPF_DATATAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a batch of threepf values
        template <typename number>
        void write_threepf(typename data_manager<number>::generic_batcher* batcher,
                           const std::vector<typename data_manager<number>::threepf_item>& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            unsigned int Nfields = batcher->get_number_fields();

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << " VALUES (@tserial, @kserial";

            for(unsigned int i = 0; i < 2*Nfields*2*Nfields*2*Nfields; i++)
              {
                insert_stmt << ", @ele" << i;
              }
            insert_stmt << ");";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector<typename data_manager<number>::threepf_item>::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, (*t).time_serial));
                check_stmt(db, sqlite3_bind_int(stmt, 2, (*t).kconfig_serial));
                for(unsigned int i = 0; i < 2*Nfields*2*Nfields*2*Nfields; i++)
                  {
                    check_stmt(db, sqlite3_bind_double(stmt, i+3, static_cast<double>((*t).elements[i])));    // 'number' must be castable to double
                  }

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_THREEPF_DATATAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Create a temporary container for a twopf integration
        sqlite3* create_temp_twopf_container(const boost::filesystem::path& container, unsigned int Nfields)
          {
            sqlite3* db = nullptr;

            int status = sqlite3_open_v2(container.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                if(db != nullptr)
                  {
                    msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
                      << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_close(db);
                  }
                else
                  {
                    msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
                      << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ")";
                  }
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
              }

            // create the necessary tables
            create_backg_table(db, Nfields, no_foreign_keys);
            create_twopf_table(db, Nfields, real_twopf, no_foreign_keys);

            return(db);
          }


        // Create a temporary container for a threepf integration
        sqlite3* create_temp_threepf_container(const boost::filesystem::path& container, unsigned int Nfields)
          {
            sqlite3* db = nullptr;

            int status = sqlite3_open_v2(container.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                if(db != nullptr)
                  {
                    msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
                      << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_close(db);
                  }
                else
                  {
                    msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
                      << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ")";
                  }
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
              }

            // create the necessary tables
            create_backg_table(db, Nfields, no_foreign_keys);
            create_twopf_table(db, Nfields, real_twopf, no_foreign_keys);
            create_twopf_table(db, Nfields, imag_twopf, no_foreign_keys);
            create_threepf_table(db, Nfields, no_foreign_keys);

            return(db);
          }


        // Aggregate the background value table from a temporary container into a principal container
        template <typename number>
        void aggregate_backg(sqlite3* db, typename repository<number>::integration_writer& ctr,
                             const std::string& temp_ctr, model<number>* m, integration_task<number>* tk, gauge_xfm_type gauge_xfm)
          {
            char* errmsg;

            BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "   && Aggregating background values";

            std::ostringstream attach_stmt;
            attach_stmt << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

            exec(db, attach_stmt.str(), __CPP_TRANSPORT_DATACTR_BACKGATTACH);
            exec(db, "BEGIN TRANSACTION;");

            std::ostringstream read_stmt_text;
            read_stmt_text << "SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << ";";
            sqlite3_stmt* read_stmt;
            check_stmt(db, sqlite3_prepare_v2(db, read_stmt_text.str().c_str(), read_stmt_text.str().length()+1, &read_stmt, nullptr));

            std::ostringstream write_stmt_text;
            write_stmt_text << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << " VALUES (@tserial";
            for(unsigned int i = 0; i < 2*m->get_N_fields(); i++)
              {
                write_stmt_text << ", @coord" << i;
              }
            write_stmt_text << ");";
            sqlite3_stmt* write_stmt;
            check_stmt(db, sqlite3_prepare_v2(db, write_stmt_text.str().c_str(), write_stmt_text.str().length()+1, &write_stmt, nullptr));

//            std::ostringstream xfm1_stmt_text;
//            xfm1_stmt_text << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_GAUGE_XFM_1_TABLE << " VALUES (@tserial";
//            for(unsigned int i = 0; i < 2*m->get_N_fields(); i++)
//              {
//                xfm1_stmt_text << ", @ele" << i;
//              }
//            xfm1_stmt_text << ");";
//            sqlite3_stmt* xfm1_stmt;
//            check_stmt(db, sqlite3_prepare_v2(db, xfm1_stmt_text.str().c_str(), xfm1_stmt_text.str().length()+1, &xfm1_stmt, nullptr));
//
//            std::ostringstream xfm2_stmt_text;
//            xfm2_stmt_text << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_GAUGE_XFM_2_TABLE << " VALUES (@tserial";
//            for(unsigned int i = 0; i < 2*m->get_N_fields()*2*m->get_N_fields(); i++)
//              {
//                xfm2_stmt_text << ", @ele" << i;
//              }
//            xfm2_stmt_text << ");";
//            sqlite3_stmt* xfm2_stmt;
//            check_stmt(db, sqlite3_prepare_v2(db, xfm2_stmt_text.str().c_str(), xfm2_stmt_text.str().length()+1, &xfm2_stmt, nullptr));

            // read rows from the temporary container, then write them into the principal database
            int status;
            while((status = sqlite3_step(read_stmt)) != SQLITE_DONE)
              {
                check_stmt(db, status, __CPP_TRANSPORT_DATACTR_BACKGREAD, SQLITE_ROW);

                int serial = sqlite3_column_int(read_stmt, 0);

                check_stmt(db, sqlite3_bind_int(write_stmt, 1, serial));

                std::vector<number> coords(2*m->get_N_fields());
                for(unsigned int i = 0; i < 2*m->get_N_fields(); i++)
                  {
                    double value = sqlite3_column_double(read_stmt, i+1);
                    check_stmt(db, sqlite3_bind_double(write_stmt, i+2, value));
                    coords[i] = static_cast<number>(value);
                  }

                check_stmt(db, sqlite3_step(write_stmt), __CPP_TRANSPORT_DATACTR_BACKGWRITE, SQLITE_DONE);

//                const parameters<number>& params = tk->get_params();
//                std::vector<number> xfm1;
//
//                m->compute_gauge_xfm_1(params, coords, xfm1);
//
//                check_stmt(db, sqlite3_bind_int(xfm1_stmt, 1, serial));
//                for(unsigned int i = 0; i < 2*m->get_N_fields(); i++)
//                  {
//                    check_stmt(db, sqlite3_bind_double(xfm1_stmt, 2 + m->flatten(i), static_cast<double>(xfm1[i])));
//                  }
//
//                check_stmt(db, sqlite3_step(xfm1_stmt), __CPP_TRANSPORT_DATACTR_BACKGXFM, SQLITE_DONE);
//
//                if(gauge_xfm == gauge_xfm_2)
//                  {
//                    std::vector< std::vector<number> > xfm2;
//                    m->compute_gauge_xfm_2(params, coords, xfm2);
//
//                    check_stmt(db, sqlite3_bind_int(xfm2_stmt, 1, serial));
//                    for(unsigned int i = 0; i < 2*m->get_N_fields(); i++)
//                      {
//                        for(unsigned int j = 0; j < 2*m->get_N_fields(); j++)
//                          {
//                            check_stmt(db, sqlite3_bind_double(xfm2_stmt, 2 + m->flatten(i, j), static_cast<double>(xfm2[i][j])));
//                          }
//
//                      }
//
//                    check_stmt(db, sqlite3_step(xfm2_stmt), __CPP_TRANSPORT_DATACTR_BACKGXFM, SQLITE_DONE);
//
//                    check_stmt(db, sqlite3_clear_bindings(xfm2_stmt));
//                    check_stmt(db, sqlite3_reset(xfm2_stmt));
//                  }

                check_stmt(db, sqlite3_clear_bindings(write_stmt));
//                check_stmt(db, sqlite3_clear_bindings(xfm1_stmt));
                check_stmt(db, sqlite3_reset(write_stmt));
//                check_stmt(db, sqlite3_reset(xfm1_stmt));
              }

            exec(db, "END TRANSACTION;");

            check_stmt(db, sqlite3_finalize(read_stmt));
            check_stmt(db, sqlite3_finalize(write_stmt));
//            check_stmt(db, sqlite3_finalize(xfm1_stmt));
//            check_stmt(db, sqlite3_finalize(xfm2_stmt));

            exec(db, static_cast<std::string>("DETACH DATABASE ") + __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME + ";", __CPP_TRANSPORT_DATACTR_BACKGDETACH);
          }


        // Aggregate a twopf value table from a temporary container into the principal container
        template <typename number>
        void aggregate_twopf(sqlite3* db, typename repository<number>::integration_writer& ctr,
                             const std::string& temp_ctr, twopf_value_type type)
          {
            BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "   && Aggregating twopf values";

            std::ostringstream copy_stmt;
            copy_stmt << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "; "
              << "INSERT INTO " << twopf_table_name(type)
                << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << twopf_table_name(type) << "; "
              << "DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

            BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "   && Executing SQL statement: " << copy_stmt.str();

            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_TWOPFCOPY);
          }


        // Aggregate a twopf value table from a temporary container into the principal container
        template <typename number>
        void aggregate_threepf(sqlite3* db, typename repository<number>::integration_writer& ctr,
                               const std::string& temp_ctr)
          {
            BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "   && Aggregating threepf values";

            std::ostringstream copy_stmt;
            copy_stmt << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "; "
              << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE
                << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << "; "
              << "DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

            BOOST_LOG_SEV(ctr.get_log(), repository<number>::normal) << "   && Executing SQL statement: " << copy_stmt.str();

            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_THREEPFCOPY);
          }


		    // set up a temporary table representing the serial numbers we want to use
		    void create_temporary_serial_table(sqlite3* db, const std::vector<unsigned int>& serial_numbers, unsigned int worker)
			    {
				    assert(db != nullptr);

            std::stringstream stmt_text;
            stmt_text << "CREATE TEMP TABLE " << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << "("
              << "serial INTEGER"
              << ");";

            exec(db, stmt_text.str(), __CPP_TRANSPORT_DATAMGR_TEMP_SERIAL_CREATE_FAIL);

            std::stringstream insert_stmt;
            insert_stmt << "INSERT INTO temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << " VALUES (@serial);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(unsigned int i = 0; i < serial_numbers.size(); i++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, serial_numbers[i]));

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATAMGR_TEMP_SERIAL_INSERT_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION");
            check_stmt(db, sqlite3_finalize(stmt));
			    }


		    // drop a temporary table of time serial numbers
		    void drop_temporary_timeserial_table(sqlite3* db, unsigned int worker)
			    {
		        assert(db != nullptr);

		        std::stringstream stmt_text;
		        stmt_text << "DROP TABLE temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ";";

			      exec(db, stmt_text.str(), __CPP_TRANSPORT_DATAMGR_TEMP_SERIAL_DROP_FAIL);
			    }


		    // Pull a set of time sample points, identified by their serial numbers
		    void pull_time_sample(sqlite3* db, const std::vector<unsigned int>& serial_numbers, std::vector<double>& sample, unsigned int worker)
			    {
				    assert(db != nullptr);

						// set up a temporary table representing the serial numbers we want to use
		        create_temporary_serial_table(db, serial_numbers, worker);

				    // pull out the set of time-sample points matching serial numbers in the temporary table
		        std::stringstream select_stmt;
				    select_stmt << "SELECT " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << ".time"
		          << " FROM " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE
					    << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
					    << " ON " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << ".serial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
					    << " ORDER BY temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".ROWID;";

				    sqlite3_stmt* stmt;
				    check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

				    sample.clear();
		        sample.reserve(serial_numbers.size());

				    int status;
				    while((status = sqlite3_step(stmt)) != SQLITE_DONE)
					    {
						    if(status == SQLITE_ROW)
							    {
								    double time = sqlite3_column_double(stmt, 0);
								    sample.push_back(time);
							    }
						    else
							    {
						        std::ostringstream msg;
								    msg << __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
								    sqlite3_finalize(stmt);
								    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
							    }
					    }

				    check_stmt(db, sqlite3_finalize(stmt));

				    // drop temporary table of serial numbers
				    drop_temporary_timeserial_table(db, worker);

				    // check that we have as many values as we expect
				    if(sample.size() != serial_numbers.size()) throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_TOO_FEW);
			    }


        // Pull a set of twopf k-configuration sample points, identified by their serial numbers
        template <typename number>
        void pull_twopf_kconfig_sample(sqlite3* db, const std::vector<unsigned int>& serial_numbers,
                                       std::vector<typename data_manager<number>::twopf_configuration>& sample,
                                       unsigned int worker)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, serial_numbers, worker);

            // pull out the set of k-sample points matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt << "SELECT " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << ".conventional AS conventional,"
	            << " " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << ".comoving AS comoving"
	            << " FROM " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << ".serial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " ORDER BY temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".ROWID;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

            sample.clear();
            sample.reserve(serial_numbers.size());

            std::vector<unsigned int>::const_iterator t = serial_numbers.begin();
            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE && t != serial_numbers.end())
	            {
                if(status == SQLITE_ROW)
	                {
		                typename data_manager<number>::twopf_configuration value;

		                value.serial         = *t;
		                value.k_conventional = sqlite3_column_double(stmt, 0);
		                value.k_comoving     = sqlite3_column_double(stmt, 1);

                    sample.push_back(value);
		                t++;
	                }
                else
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
	                }
	            }

            check_stmt(db, sqlite3_finalize(stmt));

            // drop temporary table of serial numbers
            drop_temporary_timeserial_table(db, worker);

            // check that we have as many values as we expect
            if(sample.size() != serial_numbers.size()) throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_TOO_FEW);
	        }


        // Pull a set of threepf k-configuration sample points, identified by their serial numbers
        template <typename number>
        void pull_threepf_kconfig_sample(sqlite3* db, const std::vector<unsigned int>& serial_numbers,
                                         std::vector<typename data_manager<number>::threepf_configuration>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, serial_numbers, worker);

            // create a temporary table which contains the set of configuration-sample points
            // matching serial numbers in the temporary table, together with the looked-up
		        // values of the corresponding 2pf k-configurations
            std::stringstream create_stmt;
            create_stmt << "CREATE TEMP TABLE " << __CPP_TRANSPORT_SQLITE_TEMP_THREEPF_TABLE << "_" << worker << " AS"
	            << " SELECT " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".kt_conventional AS kt_conventional,"
	            << " " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".kt_comoving AS kt_comoving,"
	            << " " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".alpha AS alpha,"
              << " " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".beta AS beta,"
	            << " " << "tpf1.conventional AS conventional_k1,"
	            << " " << "tpf1.comoving AS comoving_k1,"
              << " " << "tpf2.conventional AS conventional_k2,"
	            << " " << "tpf2.comoving AS comoving_k2,"
              << " " << "tpf3.conventional AS conventional_k3,"
	            << " " << "tpf3.comoving AS comoving_k3,"
              << " " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".wavenumber1,"
							<< " " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".wavenumber2,"
	            << " " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".wavenumber3"
	            << " FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".serial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " INNER JOIN " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " AS tpf1"
	            << " ON " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".wavenumber1=tpf1.serial"
	            << " INNER JOIN " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " AS tpf2"
	            << " ON " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".wavenumber2=tpf2.serial"
	            << " INNER JOIN " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " AS tpf3"
	            << " ON " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << ".wavenumber3=tpf3.serial"
	            << " ORDER BY temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".ROWID;";

            exec(db, create_stmt.str(), __CPP_TRANSPORT_DATAMGR_TEMP_THREEPF_CREATE_FAIL);

		        // now select the individual components of the threepf configuration out of this temporary table
            std::stringstream select_stmt;
		        select_stmt << "SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMP_THREEPF_TABLE << "_" << worker << ";";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

            sample.clear();
            sample.reserve(serial_numbers.size());

            std::vector<unsigned int>::const_iterator t = serial_numbers.begin();
            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE && t != serial_numbers.end())
	            {
                if(status == SQLITE_ROW)
	                {
                    typename data_manager<number>::threepf_configuration value;

		                value.serial          = *t;
                    value.kt_conventional = sqlite3_column_double(stmt, 0);
		                value.kt_comoving     = sqlite3_column_double(stmt, 1);
		                value.alpha           = sqlite3_column_double(stmt, 2);
		                value.beta            = sqlite3_column_double(stmt, 3);
		                value.k1_conventional = sqlite3_column_double(stmt, 4);
		                value.k1_comoving     = sqlite3_column_double(stmt, 5);
		                value.k1_serial       = static_cast<unsigned int>(sqlite3_column_int(stmt, 10));
			              value.k2_conventional = sqlite3_column_double(stmt, 6);
				            value.k2_comoving     = sqlite3_column_double(stmt, 7);
		                value.k2_serial       = static_cast<unsigned int>(sqlite3_column_int(stmt, 11));
		                value.k3_conventional = sqlite3_column_double(stmt, 8);
		                value.k3_comoving     = sqlite3_column_double(stmt, 9);
		                value.k3_serial       = static_cast<unsigned int>(sqlite3_column_int(stmt, 12));

                    sample.push_back(value);
		                t++;
	                }
                else
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
	                }
	            }

            check_stmt(db, sqlite3_finalize(stmt));

            // drop temporary table of serial numbers
            drop_temporary_timeserial_table(db, worker);

		        // drop temporary threepf query tables
            std::stringstream drop_stmt;
		        drop_stmt << "DROP TABLE temp." << __CPP_TRANSPORT_SQLITE_TEMP_THREEPF_TABLE << "_" << worker<< ";";

            exec(db, drop_stmt.str(), __CPP_TRANSPORT_DATAMGR_TEMP_THREEPF_DROP_FAIL);

            // check that we have as many values as we expect
            if(sample.size()     != serial_numbers.size()) throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_TOO_FEW);
	        }


		    // Pull a sample of the background field evolution, for a specific field, for a specific set of time serial numbers
		    template <typename number>
		    void pull_background_time_sample(sqlite3* db, unsigned int id, const std::vector<unsigned int>& t_serials,
                                         std::vector<number>& sample, unsigned int worker)
			    {
						assert(db != nullptr);

				    // set up a temporary table representing the serial numbers we want to use
		        create_temporary_serial_table(db, t_serials, worker);

				    // pull out the set of background fields (labelled by 'id')
				    // matching serial numbers in the temporary table
		        std::stringstream select_stmt;
				    select_stmt << "SELECT " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << ".coord" << id
					    << " FROM " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE
					    << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
					    << " ON " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
					    << " ORDER BY temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".ROWID;";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

		        sample.clear();
		        sample.reserve(t_serials.size());

		        int status;
		        while((status = sqlite3_step(stmt)) != SQLITE_DONE)
			        {
		            if(status == SQLITE_ROW)
			            {
		                double value = sqlite3_column_double(stmt, 0);
		                sample.push_back(static_cast<number>(value));
			            }
		            else
			            {
		                std::ostringstream msg;
		                msg << __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
		                sqlite3_finalize(stmt);
		                throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
			            }
			        }

		        check_stmt(db, sqlite3_finalize(stmt));

		        // drop temporary table of serial numbers
		        drop_temporary_timeserial_table(db, worker);

		        // check that we have as many values as we expect
		        if(sample.size() != t_serials.size()) throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_TOO_FEW);
			    }


		    // Pull a sample of a twopf component evolution, for a specific k-configuration and a specific set of time serial numbers
		    template <typename number>
		    void pull_twopf_time_sample(sqlite3* db, unsigned int id, const std::vector<unsigned int>& t_serials,
		                                unsigned int k_serial, std::vector<number>& sample, twopf_value_type type, unsigned int worker)
			    {
				    assert(db != nullptr);

				    // set up a temporary table representing the serial numbers we want to use
		        create_temporary_serial_table(db, t_serials, worker);

				    // pull out the twopf component (labelled by 'id') matching serial numbers
				    // in the temporary table
		        std::stringstream select_stmt;
				    select_stmt << "SELECT " << twopf_table_name(type) << ".ele" << id << ", " << twopf_table_name(type) << ".kserial"
					    << " FROM " << twopf_table_name(type)
					    << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
					    << " ON " << twopf_table_name(type) << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
					    << " WHERE kserial=" << k_serial
					    << " ORDER BY temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".ROWID;";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

		        sample.clear();
		        sample.reserve(t_serials.size());

		        int status;
		        while((status = sqlite3_step(stmt)) != SQLITE_DONE)
			        {
		            if(status == SQLITE_ROW)
			            {
		                double value = sqlite3_column_double(stmt, 0);
		                sample.push_back(static_cast<number>(value));
			            }
		            else
			            {
		                std::ostringstream msg;
		                msg << __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
		                sqlite3_finalize(stmt);
		                throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
			            }
			        }

		        check_stmt(db, sqlite3_finalize(stmt));

		        // drop temporary table of serial numbers
		        drop_temporary_timeserial_table(db, worker);

		        // check that we have as many values as we expect
		        if(sample.size() != t_serials.size()) throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_TOO_FEW);
			    }


        // Pull a sample of a threepf component evolution, for a specific k-configuration and a specific set of time serial numbers
        template <typename number>
        void pull_threepf_time_sample(sqlite3* db, unsigned int id, const std::vector<unsigned int>& t_serials,
                                      unsigned int k_serial, std::vector<number>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, t_serials, worker);

            // pull out the threepf component (labelled by 'id') matching serial numbers
            // in the temporary table
            std::stringstream select_stmt;
            select_stmt << "SELECT " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".ele" << id << ", " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".kserial"
	            << " FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " WHERE kserial=" << k_serial
	            << " ORDER BY temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".ROWID;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

            sample.clear();
            sample.reserve(t_serials.size());

            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
	            {
                if(status == SQLITE_ROW)
	                {
                    double value = sqlite3_column_double(stmt, 0);
                    sample.push_back(static_cast<number>(value));
	                }
                else
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
	                }
	            }

            check_stmt(db, sqlite3_finalize(stmt));

            // drop temporary table of serial numbers
            drop_temporary_timeserial_table(db, worker);

            // check that we have as many values as we expect
            if(sample.size() != t_serials.size()) throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_TOO_FEW);
	        }


        // Pull a sample of a twopf component, for a specific time sample point and a specific set of k-configuration serial numbers
        template <typename number>
        void pull_twopf_kconfig_sample(sqlite3* db, unsigned int id, const std::vector<unsigned int>& k_serials,
                                       unsigned int t_serial, std::vector<number>& sample, twopf_value_type type, unsigned int worker)
	        {
            assert(db != nullptr);

		        // set up a temporary table representing the serial numbers we want to use
		        create_temporary_serial_table(db, k_serials, worker);

		        // pull out the twopf component (labelled by 'id') matching serial numbers
		        // in the temporary table
            std::stringstream select_stmt;
		        select_stmt << "SELECT " << twopf_table_name(type) << ".ele" << id << ", " << twopf_table_name(type) << ".tserial"
			        << " FROM " << twopf_table_name(type)
			        << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
			        << " ON " << twopf_table_name(type) << ".kserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
			        << " WHERE tserial=" << t_serial
			        << " ORDER BY temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".ROWID;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

            sample.clear();
		        sample.reserve(k_serials.size());

            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
	            {
                if(status == SQLITE_ROW)
	                {
                    double value = sqlite3_column_double(stmt, 0);
                    sample.push_back(static_cast<number>(value));
	                }
                else
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
	                }
	            }

            check_stmt(db, sqlite3_finalize(stmt));

            // drop temporary table of serial numbers
            drop_temporary_timeserial_table(db, worker);

            // check that we have as many values as we expect
            if(sample.size() != k_serials.size()) throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_TOO_FEW);
	        }


        // Pull a sample of a threepf component, for a specific time sample point and a specific set of k-configuration serial numbers
        template <typename number>
        void pull_threepf_kconfig_sample(sqlite3* db, unsigned int id, const std::vector<unsigned int>& k_serials,
                                         unsigned int t_serial, std::vector<number>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

		        // set up a temporary table representing the serial numbers we want to use
		        create_temporary_serial_table(db, k_serials, worker);

		        // pull out the threepf component (labelled by 'id') matching serial numbers
		        // in the temporary table
            std::stringstream select_stmt;
		        select_stmt << "SELECT " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".ele" << id << ", " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".tserial"
			        << " FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE
			        << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
			        << " ON " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".kserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
			        << " WHERE tserial=" << t_serial
			        << " ORDER BY temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".ROWID;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

            sample.clear();
            sample.reserve(k_serials.size());

            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
	            {
                if(status == SQLITE_ROW)
	                {
                    double value = sqlite3_column_double(stmt, 0);
                    sample.push_back(static_cast<number>(value));
	                }
                else
	                {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
	                }
	            }

            check_stmt(db, sqlite3_finalize(stmt));

            // drop temporary table of serial numbers
            drop_temporary_timeserial_table(db, worker);

            // check that we have as many values as we expect
            if(sample.size() != k_serials.size()) throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_TOO_FEW);
	        }


      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //__sqlite3_operations_H_
