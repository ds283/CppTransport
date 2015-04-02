//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __data_manager_operations_H_
#define __data_manager_operations_H_


#include <set>
#include <vector>
#include <string>
#include <sstream>

#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/derived-products/template_types.h"
#include "transport-runtime-api/scheduler/work_queue.h"
#include "transport-runtime-api/models/model.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/repository/writers/writers.h"
#include "transport-runtime-api/data/datapipe/configurations.h"
#include "transport-runtime-api/data/batchers/batchers.h"

#include "boost/lexical_cast.hpp"

#include "sqlite3.h"

#include "transport-runtime-api/sqlite3/operations/sqlite3_utility.h"


#define __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE                   "time_samples"
#define __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE                  "twopf_samples"
#define __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE                "threepf_samples"
#define __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE                   "backg"
#define __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE            "tensor_twopf"
#define __CPP_TRANSPORT_SQLITE_TWOPF_VALUE_TABLE                   "twopf"
#define __CPP_TRANSPORT_SQLITE_TWOPF_REAL_TAG                      "re"
#define __CPP_TRANSPORT_SQLITE_TWOPF_IMAGINARY_TAG                 "im"
#define __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE                 "threepf"
#define __CPP_TRANSPORT_SQLITE_WORKERS_TABLE                       "worker_data"
#define __CPP_TRANSPORT_SQLITE_STATS_TABLE                         "integration_statistics"
#define __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE              "zeta_twopf"
#define __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE            "zeta_threepf"
#define __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE "zeta_redbsp"
#define __CPP_TRANSPORT_SQLITE_FNL_LOCAL_VALUE_TABLE               "fNL_local"
#define __CPP_TRANSPORT_SQLITE_FNL_EQUI_VALUE_TABLE                "fNL_equi"
#define __CPP_TRANSPORT_SQLITE_FNL_ORTHO_VALUE_TABLE               "fNL_ortho"
#define __CPP_TRANSPORT_SQLITE_FNL_DBI_VALUE_TABLE                 "fNL_DBI"

#define __CPP_TRANSPORT_SQLITE_TASKLIST_TABLE                      "task_list"
#define __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE                   "serial_search"
#define __CPP_TRANSPORT_SQLITE_TEMP_THREEPF_TABLE                  "threepf_search"
#define __CPP_TRANSPORT_SQLITE_TEMP_FNL_TABLE                      "fNL_update"
#define __CPP_TRANSPORT_SQLITE_INSERT_FNL_TABLE                    "fNL_insert"

#define __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME                    "tempdb"

// sqlite defaults to a maximum number of columns of 2000
#define __CPP_TRANSPORT_DEFAULT_SQLITE_MAX_COLUMN                  2000
// sqlite defaults to a maximum number of host parameters of 999
#define __CPP_TRANSPORT_DEFAULT_SQLITE_MAX_VARIABLE_NUMBER         999

// overhead on number of columns per page
// 50 is extremely conservative
#define __CPP_TRANSPORT_DEFAULT_SQLITE_COLUMN_OVERHEAD             50

namespace transport
  {

    namespace sqlite3_operations
      {

        typedef enum { foreign_keys, no_foreign_keys } add_foreign_keys_type;

        typedef enum { real_twopf, imag_twopf } twopf_value_type;

        typedef enum { twopf_configs, threepf_configs } statistics_configuration_type;


		    // sqlite has a default maximum number of columns, and a maximum number of
        // host parameters
        // with a large number of fields we can easily exceed those, so need
        // to set a limit on the number of columns per row
        constexpr unsigned int max_columns = (__CPP_TRANSPORT_DEFAULT_SQLITE_MAX_VARIABLE_NUMBER < __CPP_TRANSPORT_DEFAULT_SQLITE_MAX_COLUMN ? __CPP_TRANSPORT_DEFAULT_SQLITE_MAX_VARIABLE_NUMBER : __CPP_TRANSPORT_DEFAULT_SQLITE_MAX_COLUMN) - __CPP_TRANSPORT_DEFAULT_SQLITE_COLUMN_OVERHEAD;


        // Utility functions
        namespace
          {

            // construct the name of a twopf table
            inline std::string twopf_table_name(twopf_value_type type)
              {
                return(static_cast<std::string>(__CPP_TRANSPORT_SQLITE_TWOPF_VALUE_TABLE) + "_"
                  + (type == real_twopf ? __CPP_TRANSPORT_SQLITE_TWOPF_REAL_TAG : __CPP_TRANSPORT_SQLITE_TWOPF_IMAGINARY_TAG));
              }
            
            // construct the name of an fNL table
            inline std::string fNL_table_name(derived_data::template_type type)
              {
                switch(type)
                  {
                    case derived_data::fNL_local_template:
                      return static_cast<std::string>(__CPP_TRANSPORT_SQLITE_FNL_LOCAL_VALUE_TABLE);
                      
                    case derived_data::fNL_equi_template:
                      return static_cast<std::string>(__CPP_TRANSPORT_SQLITE_FNL_EQUI_VALUE_TABLE);
                    
                    case derived_data::fNL_ortho_template:
                      return static_cast<std::string>(__CPP_TRANSPORT_SQLITE_FNL_ORTHO_VALUE_TABLE);
                    
                    case derived_data::fNL_DBI_template:
                      return static_cast<std::string>(__CPP_TRANSPORT_SQLITE_FNL_DBI_VALUE_TABLE);
                      
                    default:
                      assert(false);
                      throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, __CPP_TRANSPORT_DATAMGR_UNKNOWN_FNL_TEMPLATE);
                  }
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
                check_stmt(db, sqlite3_bind_int(stmt, 1, t->serial));
                check_stmt(db, sqlite3_bind_double(stmt, 2, t->t));

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
            stmt_text
              << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << "("
              << "serial       INTEGER PRIMARY KEY, "
              << "conventional DOUBLE, "
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
                check_stmt(db, sqlite3_bind_int(stmt, 1, t->serial));
                check_stmt(db, sqlite3_bind_double(stmt, 2, t->k_conventional));
                check_stmt(db, sqlite3_bind_double(stmt, 3, t->k_comoving));

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
            stmt_text
              << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << "("
              << "serial          INTEGER PRIMARY KEY, "
              << "wavenumber1     INTEGER, "
              << "wavenumber2     INTEGER, "
              << "wavenumber3     INTEGER, "
              << "kt_comoving     DOUBLE, "
              << "kt_conventional DOUBLE, "
              << "alpha           DOUBLE, "
              << "beta            DOUBLE, "
              << "FOREIGN KEY(wavenumber1) REFERENCES twopf_samples(serial), "
              << "FOREIGN KEY(wavenumber2) REFERENCES twopf_samples(serial), "
              << "FOREIGN KEY(wavenumber3) REFERENCES twopf_samples(serial)"
              << ");";

            exec(db, stmt_text.str());

            std::stringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << " VALUES (@serial, @wn1, @wn2, @wn3, @kt_com, @kt_conv, @alpha, @beta);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(std::vector<threepf_kconfig>::const_iterator t = threepf_sample.begin(); t != threepf_sample.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, t->serial));
                check_stmt(db, sqlite3_bind_int(stmt, 2, t->index[0]));
                check_stmt(db, sqlite3_bind_int(stmt, 3, t->index[1]));
                check_stmt(db, sqlite3_bind_int(stmt, 4, t->index[2]));
                check_stmt(db, sqlite3_bind_double(stmt, 5, t->k_t_comoving));
                check_stmt(db, sqlite3_bind_double(stmt, 6, t->k_t_conventional));
                check_stmt(db, sqlite3_bind_double(stmt, 7, t->alpha));
                check_stmt(db, sqlite3_bind_double(stmt, 8, t->beta));

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_THREEPFTAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


		    // Create table documenting workers
		    void create_worker_info_table(sqlite3* db, add_foreign_keys_type keys=no_foreign_keys)
			    {
		        std::ostringstream create_stmt;
				    create_stmt
				      << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_WORKERS_TABLE << "("
				      << "worker        INTEGER PRIMARY KEY, "
				      << "hostname      TEXT, "
				      << "os_name       TEXT, "
				      << "os_version    TEXT, "
				      << "os_release    TEXT, "
				      << "architecture  TEXT, "
				      << "cpu_vendor_id TEXT)";

				    exec(db, create_stmt.str());
			    }


        // Create table for statistics, if they are being collected
        void create_stats_table(sqlite3* db, add_foreign_keys_type keys=no_foreign_keys, statistics_configuration_type type=twopf_configs)
          {
            std::ostringstream create_stmt;
            create_stmt
              << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_STATS_TABLE << "("
              << "kserial           INTEGER PRIMARY KEY, "
              << "integration_time  DOUBLE, "
              << "batch_time        DOUBLE, "
	            << "refinements       INTEGER, "
              << "worker            INTEGER";

            if(keys == foreign_keys)
              {
                create_stmt << ", FOREIGN KEY(kserial) REFERENCES ";
                switch(type)
                  {
                    case twopf_configs:
                      create_stmt << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE;
                      break;

                    case threepf_configs:
                      create_stmt << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE;
                      break;

                    default:
                      assert(false);
                  }
                create_stmt << "(serial)";

		            create_stmt << ", FOREIGN KEY(worker) REFERENCES " << __CPP_TRANSPORT_SQLITE_WORKERS_TABLE << "(worker)";
              }
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Create table for background values
        void create_backg_table(sqlite3* db, unsigned int Nfields, add_foreign_keys_type keys=no_foreign_keys)
          {
		        unsigned int num_cols = std::min(2*Nfields, max_columns);

            std::ostringstream create_stmt;
            create_stmt
	            << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << "("
              << "tserial INTEGER PRIMARY KEY, "
	            << "page    INTEGER";

            for(unsigned int i = 0; i < num_cols; i++)
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
		        unsigned int num_cols = std::min(2*Nfields * 2*Nfields, max_columns);

            std::ostringstream create_stmt;
            create_stmt
	            << "CREATE TABLE " << twopf_table_name(type) << "("
              << "tserial INTEGER, "
              << "kserial INTEGER, "
	            << "page    INTEGER";

            for(unsigned int i = 0; i < num_cols; i++)
              {
                create_stmt << ", ele" << i << " DOUBLE";
              }

            create_stmt << ", PRIMARY KEY (tserial, kserial, page)";
            if(keys == foreign_keys)
              {
                create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)"
                  << ", FOREIGN KEY(kserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << "(serial)";
              }
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Create table for tensor twopf values
        void create_tensor_twopf_table(sqlite3* db, add_foreign_keys_type keys=no_foreign_keys)
          {
            unsigned int num_cols = std::min(static_cast<unsigned int>(4), max_columns);

            std::ostringstream create_stmt;
            create_stmt
              << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << "("
              << "tserial INTEGER, "
              << "kserial INTEGER, "
              << "page    INTEGER";

            for(unsigned int i = 0; i < num_cols; i++)
              {
                create_stmt << ", ele" << i << " DOUBLE";
              }

            create_stmt << ", PRIMARY KEY (tserial, kserial, page)";
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
		        unsigned int num_cols = std::min(2*Nfields * 2*Nfields * 2*Nfields, max_columns);

            std::ostringstream create_stmt;
            create_stmt
	            << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << "("
              << "tserial INTEGER, "
              << "kserial INTEGER, "
	            << "page    INTEGER";

            for(unsigned int i = 0; i < num_cols; i++)
              {
                create_stmt << ", ele" << i << " DOUBLE";
              }

            create_stmt << ", PRIMARY KEY (tserial, kserial, page)";
            if(keys == foreign_keys)
              {
                create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)"
                  << ", FOREIGN KEY(kserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << "(serial)";
              }
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Create table for zeta twopf values
        void create_zeta_twopf_table(sqlite3* db, add_foreign_keys_type keys=no_foreign_keys)
          {
            std::ostringstream create_stmt;
            create_stmt
              << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << "("
              << "tserial INTEGER, "
              << "kserial INTEGER, "
              << "ele     DOUBLE, "
              << "PRIMARY KEY (tserial, kserial)";

            if(keys == foreign_keys)
              {
                create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)"
                  << ", FOREIGN KEY(kserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << "(serial)";
              }
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Create table for zeta twopf values
        void create_zeta_threepf_table(sqlite3* db, add_foreign_keys_type keys=no_foreign_keys)
          {
            std::ostringstream create_stmt;
            create_stmt
              << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << "("
              << "tserial INTEGER, "
              << "kserial INTEGER, "
              << "ele     DOUBLE, "
              << "PRIMARY KEY (tserial, kserial)";

            if(keys == foreign_keys)
              {
                create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)"
                  << ", FOREIGN KEY(kserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << "(serial)";
              }
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Create table for zeta twopf values
        void create_zeta_reduced_bispectrum_table(sqlite3* db, add_foreign_keys_type keys=no_foreign_keys)
          {
            std::ostringstream create_stmt;
            create_stmt
              << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << "("
              << "tserial INTEGER, "
              << "kserial INTEGER, "
              << "ele     DOUBLE, "
              << "PRIMARY KEY (tserial, kserial)";

            if(keys == foreign_keys)
              {
                create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)"
                  << ", FOREIGN KEY(kserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << "(serial)";
              }
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


        // Create table for zeta twopf values
        void create_fNL_table(sqlite3* db, derived_data::template_type type, add_foreign_keys_type keys=no_foreign_keys)
          {
            std::ostringstream create_stmt;
            create_stmt
              << "CREATE TABLE " << fNL_table_name(type) << "("
              << "tserial INTEGER, "
              << "BT      DOUBLE, "
              << "TT      DOUBLE, "
              << "PRIMARY KEY (tserial)";

            if(keys == foreign_keys)
              {
                create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)";
              }
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


		    // Write host information
		    template <typename number>
		    void write_host_info(generic_batcher* batcher)
			    {
				    sqlite3* db = nullptr;
				    batcher->get_manager_handle(&db);

				    const host_information& host = batcher->get_host_information();

		        std::ostringstream insert_stmt;
				    insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_WORKERS_TABLE << " VALUES (@worker, @hostname, @os_name, @os_version, @os_release, @architecture, @cpu_vendor_id)";

				    sqlite3_stmt* stmt;
				    check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

				    exec(db, "BEGIN TRANSACTION");

				    // to document the different choice of length in these sqlite3_bind_text() statements compared to sqlite3_prepare_v2,
				    // the SQLite3 documentation says:

		        // If the caller knows that the supplied string is nul-terminated, then there is a small performance advantage to be
		        // gained by passing an nByte parameter that is equal to the number of bytes in the input string including the nul-terminator
		        // bytes as this saves SQLite from having to make a copy of the input string.

				    // However, here, we certainly *shouldn't* include the nul-terminator byte, because that isn't part of the string

				    check_stmt(db, sqlite3_bind_int(stmt, 1, batcher->get_worker_number()));
				    check_stmt(db, sqlite3_bind_text(stmt, 2, host.get_host_name().c_str(), host.get_host_name().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 3, host.get_os_name().c_str(), host.get_os_name().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 4, host.get_os_version().c_str(), host.get_os_version().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 5, host.get_os_release().c_str(), host.get_os_release().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 6, host.get_architecture().c_str(), host.get_architecture().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 7, host.get_cpu_vendor_id().c_str(), host.get_cpu_vendor_id().length(), SQLITE_STATIC));

				    check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_WORKER_INSERT_FAIL, SQLITE_DONE);

				    exec(db, "END TRANSACTION");

				    check_stmt(db, sqlite3_clear_bindings(stmt));
				    check_stmt(db, sqlite3_finalize(stmt));
			    }

        // Write a batch of per-configuration statistics values
        template <typename number>
        void write_stats(generic_batcher* batcher,
                         const std::vector< typename integration_items<number>::configuration_statistics >& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_STATS_TABLE << " VALUES (@kserial, @integration_time, @batch_time, @refinements, @worker);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector<typename integration_items<number>::configuration_statistics >::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, t->serial));
                check_stmt(db, sqlite3_bind_double(stmt, 2, t->integration));
                check_stmt(db, sqlite3_bind_double(stmt, 3, t->batching));
		            check_stmt(db, sqlite3_bind_int(stmt, 4, t->refinements));
                check_stmt(db, sqlite3_bind_int(stmt, 5, batcher->get_worker_number()));

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_STATS_INSERT_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a batch of background values
        template <typename number>
        void write_backg(integration_batcher<number>* batcher, const std::vector< typename integration_items<number>::backg_item >& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            unsigned int Nfields = batcher->get_number_fields();

            // work out how many columns we have, and how many pages
            // we need to fit in our number of columns.
            // num_pages will be 1 or greater
		        unsigned int num_cols = std::min(2*Nfields, max_columns);
            unsigned int num_pages = (2*Nfields-1)/num_cols + 1;

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << " VALUES (@tserial, @page";

            for(unsigned int i = 0; i < num_cols; i++)
              {
                insert_stmt << ", @coord" << i;
              }
            insert_stmt << ");";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector< typename integration_items<number>::backg_item >::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                for(unsigned int page = 0; page < num_pages; page++)
	                {
                    check_stmt(db, sqlite3_bind_int(stmt, 1, t->time_serial));
		                check_stmt(db, sqlite3_bind_int(stmt, 2, page));

		                for(unsigned int i = 0; i < num_cols; i++)
			                {
				                unsigned int index = page*num_cols + i;
				                number       value = index < 2*Nfields ? t->coords[index] : 0.0;

		                    check_stmt(db, sqlite3_bind_double(stmt, i+3, static_cast<double>(value)));    // 'number' must be castable to double
			                }

		                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_BACKG_DATATAB_FAIL, SQLITE_DONE);

		                check_stmt(db, sqlite3_clear_bindings(stmt));
		                check_stmt(db, sqlite3_reset(stmt));
	                }
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a batch of twopf values
        template <typename number>
        void write_twopf(twopf_value_type type, integration_batcher<number>* batcher, const std::vector< typename integration_items<number>::twopf_item >& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            unsigned int Nfields = batcher->get_number_fields();

            // work out how many columns we have, and how many pages
            // we need to fit in our number of columns.
            // num_pages will be 1 or greater
            unsigned int num_cols = std::min(2*Nfields * 2*Nfields, max_columns);
            unsigned int num_pages = (2*Nfields * 2*Nfields - 1)/num_cols + 1;

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << twopf_table_name(type) << " VALUES (@tserial, @kserial, @page";

            for(unsigned int i = 0; i < num_cols; i++)
              {
                insert_stmt << ", @ele" << i;
              }
            insert_stmt << ");";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector< typename integration_items<number>::twopf_item >::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
		            for(unsigned int page = 0; page < num_pages; page++)
			            {
		                check_stmt(db, sqlite3_bind_int(stmt, 1, t->time_serial));
		                check_stmt(db, sqlite3_bind_int(stmt, 2, t->kconfig_serial));
				            check_stmt(db, sqlite3_bind_int(stmt, 3, page));

		                for(unsigned int i = 0; i < num_cols; i++)
			                {
				                unsigned int index = page*num_cols + i;
				                number       value = index < 2*Nfields * 2*Nfields ? t->elements[index] : 0.0;

		                    check_stmt(db, sqlite3_bind_double(stmt, i+4, static_cast<double>(value)));    // 'number' must be castable to double
			                }

		                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_TWOPF_DATATAB_FAIL, SQLITE_DONE);

		                check_stmt(db, sqlite3_clear_bindings(stmt));
		                check_stmt(db, sqlite3_reset(stmt));
			            }
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a batch of tensor twopf values
        template <typename number>
        void write_tensor_twopf(integration_batcher<number>* batcher, const std::vector< typename integration_items<number>::tensor_twopf_item >& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            // work out how many columns we have, and how many pages
            // we need to fit in our number of columns.
            // num_pages will be 1 or greater
            unsigned int num_cols = std::min(static_cast<unsigned int>(4), max_columns);
            unsigned int num_pages = (4 - 1)/num_cols + 1;

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << " VALUES (@tserial, @kserial, @page";

            for(unsigned int i = 0; i < num_cols; i++)
              {
                insert_stmt << ", @ele" << i;
              }
            insert_stmt << ");";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector< typename integration_items<number>::tensor_twopf_item >::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                for(unsigned int page = 0; page < num_pages; page++)
                  {
                    check_stmt(db, sqlite3_bind_int(stmt, 1, t->time_serial));
                    check_stmt(db, sqlite3_bind_int(stmt, 2, t->kconfig_serial));
                    check_stmt(db, sqlite3_bind_int(stmt, 3, page));

                    for(unsigned int i = 0; i < num_cols; i++)
                      {
                        unsigned int index = page*num_cols + i;
                        number       value = index < 4 ? t->elements[index] : 0.0;

                        check_stmt(db, sqlite3_bind_double(stmt, i+4, static_cast<double>(value)));    // 'number' must be castable to double
                      }

                    check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_TENSOR_TWOPF_DATATAB_FAIL, SQLITE_DONE);

                    check_stmt(db, sqlite3_clear_bindings(stmt));
                    check_stmt(db, sqlite3_reset(stmt));
                  }
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a batch of threepf values
        template <typename number>
        void write_threepf(integration_batcher<number>* batcher, const std::vector<typename integration_items<number>::threepf_item >& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            unsigned int Nfields = batcher->get_number_fields();

            // work out how many columns we have, and how many pages
            // we need to fit in our number of columns.
            // num_pages will be 1 or greater
            unsigned int num_cols = std::min(2*Nfields * 2*Nfields * 2*Nfields, max_columns);
            unsigned int num_pages = (2*Nfields * 2*Nfields * 2*Nfields - 1)/num_cols + 1;

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << " VALUES (@tserial, @kserial, @page";

            for(unsigned int i = 0; i < num_cols; i++)
              {
                insert_stmt << ", @ele" << i;
              }
            insert_stmt << ");";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector<typename integration_items<number>::threepf_item >::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
		            for(unsigned int page  = 0; page < num_pages; page++)
			            {
		                check_stmt(db, sqlite3_bind_int(stmt, 1, t->time_serial));
		                check_stmt(db, sqlite3_bind_int(stmt, 2, t->kconfig_serial));
				            check_stmt(db, sqlite3_bind_int(stmt, 3, page));

		                for(unsigned int i = 0; i < num_cols; i++)
			                {
				                unsigned int index = page*num_cols + i;
				                number       value = index < 2*Nfields * 2*Nfields * 2*Nfields ? t->elements[index] : 0.0;

		                    check_stmt(db, sqlite3_bind_double(stmt, i+4, static_cast<double>(value)));    // 'number' must be castable to double
			                }

		                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_THREEPF_DATATAB_FAIL, SQLITE_DONE);

		                check_stmt(db, sqlite3_clear_bindings(stmt));
		                check_stmt(db, sqlite3_reset(stmt));
			            }
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a batch of zeta twopf values
        template <typename number>
        void write_zeta_twopf(postintegration_batcher* batcher, const std::vector< typename postintegration_items<number>::zeta_twopf_item >& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << " VALUES (@tserial, @kserial, @ele);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector< typename postintegration_items<number>::zeta_twopf_item >::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, t->time_serial));
                check_stmt(db, sqlite3_bind_int(stmt, 2, t->kconfig_serial));
                check_stmt(db, sqlite3_bind_double(stmt, 3, static_cast<double>(t->value)));

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_ZETA_TWOPF_DATATAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a batch of zeta threepf values
        template <typename number>
        void write_zeta_threepf(postintegration_batcher* batcher, const std::vector< typename postintegration_items<number>::zeta_threepf_item >& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << " VALUES (@tserial, @kserial, @ele);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector< typename postintegration_items<number>::zeta_threepf_item >::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, t->time_serial));
                check_stmt(db, sqlite3_bind_int(stmt, 2, t->kconfig_serial));
                check_stmt(db, sqlite3_bind_double(stmt, 3, static_cast<double>(t->value)));

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_ZETA_THREEPF_DATATAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a batch of zeta threepf values
        template <typename number>
        void write_zeta_redbsp(postintegration_batcher* batcher, const std::vector< typename postintegration_items<number>::zeta_threepf_item >& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << " VALUES (@tserial, @kserial, @ele);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector<typename postintegration_items<number>::zeta_threepf_item >::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, t->time_serial));
                check_stmt(db, sqlite3_bind_int(stmt, 2, t->kconfig_serial));
                check_stmt(db, sqlite3_bind_double(stmt, 3, static_cast<double>(t->value)));

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_ZETA_REDBSP_DATATAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Write a batch of fNL values.
		    // The semantics here are slightly different. If an existing value is already present for a given
		    // tserial then we want to add our new value to it.
		    // For that purpose we use COALESCE.
        template <typename number>
        void write_fNL(postintegration_batcher* batcher, const std::set< typename postintegration_items<number>::fNL_item, typename postintegration_items<number>::fNL_item_comparator >& batch,
                       derived_data::template_type type)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            exec(db, "BEGIN TRANSACTION;");

		        // first, inject all new BT and TT values into a temporary table
            std::stringstream create_stmt;
		        create_stmt << "CREATE TEMP TABLE " << __CPP_TRANSPORT_SQLITE_INSERT_FNL_TABLE << " ("
			        << "tserial INTEGER, "
		          << "BT      DOUBLE, "
			        << "TT      DOUBLE, "
		          << "PRIMARY KEY (tserial)"
		          << ");";
		        exec(db, create_stmt.str(), __CPP_TRANSPORT_DATACTR_FNL_DATATAB_FAIL);

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO temp." << __CPP_TRANSPORT_SQLITE_INSERT_FNL_TABLE << " VALUES (@tserial, @BT, @TT);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            for(typename std::set< typename postintegration_items<number>::fNL_item, typename postintegration_items<number>::fNL_item_comparator >::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, t->time_serial));
                check_stmt(db, sqlite3_bind_double(stmt, 2, static_cast<double>(t->BT)));
                check_stmt(db, sqlite3_bind_double(stmt, 3, static_cast<double>(t->TT)));

                check_stmt(db, sqlite3_step(stmt), __CPP_TRANSPORT_DATACTR_FNL_DATATAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

		        // now create a second temporary table which merges results from the existing fNL table
		        // and the new one we have just constructed
            std::stringstream create_stmt2;
		        create_stmt2 << "CREATE TEMP TABLE " << __CPP_TRANSPORT_SQLITE_TEMP_FNL_TABLE << " AS"
		          << " SELECT temp." << __CPP_TRANSPORT_SQLITE_INSERT_FNL_TABLE << ".tserial AS tserial,"
		          << " temp." << __CPP_TRANSPORT_SQLITE_INSERT_FNL_TABLE << ".BT + COALESCE(" << fNL_table_name(type) << ".BT, 0.0) AS new_BT,"
		          << " temp." << __CPP_TRANSPORT_SQLITE_INSERT_FNL_TABLE << ".TT + COALESCE(" << fNL_table_name(type) << ".TT, 0.0) AS new_TT"
		          << " FROM temp." << __CPP_TRANSPORT_SQLITE_INSERT_FNL_TABLE << " LEFT JOIN " << fNL_table_name(type)
		          << " ON temp." << __CPP_TRANSPORT_SQLITE_INSERT_FNL_TABLE << ".tserial=" << fNL_table_name(type) << ".tserial;";
		        exec(db, create_stmt2.str(), __CPP_TRANSPORT_DATACTR_FNL_DATATAB_FAIL);

		        // finally, copy or update values back into the main table
            std::stringstream copy_stmt;
		        copy_stmt
		          << " INSERT OR REPLACE INTO " << fNL_table_name(type)
			        << " SELECT tserial AS tserial,"
		          << " new_BT AS BT,"
			        << " new_TT AS TT"
		          << " FROM temp." << __CPP_TRANSPORT_SQLITE_TEMP_FNL_TABLE << ";";
		        exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_FNL_DATATAB_FAIL);

            std::stringstream drop_stmt;
		        drop_stmt << "DROP TABLE temp." << __CPP_TRANSPORT_SQLITE_INSERT_FNL_TABLE << ";"
		          << " DROP TABLE temp." << __CPP_TRANSPORT_SQLITE_TEMP_FNL_TABLE << ";";
		        exec(db, drop_stmt.str(), __CPP_TRANSPORT_DATACTR_FNL_DATATAB_FAIL);

            exec(db, "END TRANSACTION;");
            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Create a temporary container for a twopf integration
        sqlite3* create_temp_twopf_container(const boost::filesystem::path& container, unsigned int Nfields, bool collect_stats)
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
		        create_worker_info_table(db, no_foreign_keys);
            if(collect_stats) create_stats_table(db, no_foreign_keys);
            create_backg_table(db, Nfields, no_foreign_keys);
            create_twopf_table(db, Nfields, real_twopf, no_foreign_keys);
            create_tensor_twopf_table(db, no_foreign_keys);

            return(db);
          }


        // Create a temporary container for a threepf integration
        sqlite3* create_temp_threepf_container(const boost::filesystem::path& container, unsigned int Nfields, bool collect_stats)
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
		        create_worker_info_table(db, no_foreign_keys);
            if(collect_stats) create_stats_table(db, no_foreign_keys);
            create_backg_table(db, Nfields, no_foreign_keys);
            create_twopf_table(db, Nfields, real_twopf, no_foreign_keys);
            create_twopf_table(db, Nfields, imag_twopf, no_foreign_keys);
            create_tensor_twopf_table(db, no_foreign_keys);
            create_threepf_table(db, Nfields, no_foreign_keys);

            return(db);
          }


        // Create a temporary container for a zeta twopf
        sqlite3* create_temp_zeta_twopf_container(const boost::filesystem::path& container)
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
            create_zeta_twopf_table(db, no_foreign_keys);

            return(db);
          }


        // Create a temporary container for a zeta threepf
        sqlite3* create_temp_zeta_threepf_container(const boost::filesystem::path& container)
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
            create_zeta_twopf_table(db, no_foreign_keys);
            create_zeta_threepf_table(db, no_foreign_keys);
            create_zeta_reduced_bispectrum_table(db, no_foreign_keys);

            return(db);
          }


        // Create a temporary container for a zeta threepf
        sqlite3* create_temp_fNL_container(const boost::filesystem::path& container, derived_data::template_type type)
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
            create_fNL_table(db, type, no_foreign_keys);

            return(db);
          }


        // Aggregate the background value table from a temporary container into a principal container
        template <typename number>
        void aggregate_backg(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
          {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating background values";

            std::ostringstream copy_stmt;
		        copy_stmt
		          << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
			        << " INSERT INTO " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE
			        << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << ";"
			        << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//		        BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();

		        exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_BACKGROUND_COPY);
          }


        // Aggregate a twopf value table from a temporary container into the principal container
        template <typename number>
        void aggregate_twopf(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr, twopf_value_type type)
          {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating twopf values";

            std::ostringstream copy_stmt;
            copy_stmt
	            << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
              << " INSERT INTO " << twopf_table_name(type)
              << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << twopf_table_name(type) << ";"
              << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();

            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_TWOPF_COPY);
          }


        // Aggregate a tensor twopf value table from a temporary container into the principal container
        template <typename number>
        void aggregate_tensor_twopf(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
          {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating twopf values";

            std::ostringstream copy_stmt;
            copy_stmt
              << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
              << " INSERT INTO " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE
              << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << ";"
              << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();

            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_TENSOR_TWOPF_COPY);
          }


        // Aggregate a threepf value table from a temporary container into the principal container
        template <typename number>
        void aggregate_threepf(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
          {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating threepf values";

            std::ostringstream copy_stmt;
            copy_stmt
	            << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
              << " INSERT INTO " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE
              << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ";"
              << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();

            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_THREEPF_COPY);
          }


		    // Aggregate a worker info table from a temporary container into the principal container
		    template <typename number>
		    void aggregate_workers(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
			    {
//				    BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating worker information";

		        std::ostringstream copy_stmt;
				    copy_stmt
				      << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
				      << " INSERT OR IGNORE INTO " << __CPP_TRANSPORT_SQLITE_WORKERS_TABLE
				      << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_WORKERS_TABLE << ";"
				      << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//				    BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();

				    exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_WORKERS_COPY);
			    }


        // Aggregate a statistics value table from a temporary container into the principal container
        template <typename number>
        void aggregate_statistics(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
          {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating per-configuration statistics";

            std::ostringstream copy_stmt;
            copy_stmt
	            << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
              << " INSERT INTO " << __CPP_TRANSPORT_SQLITE_STATS_TABLE
              << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_STATS_TABLE << ";"
              << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();

            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_STATISTICS_COPY);
          }


        // Aggregate a zeta twopf value table from a temporary container
        template <typename number>
        void aggregate_zeta_twopf(sqlite3* db, postintegration_writer<number>& writer, const std::string& temp_ctr)
          {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating zeta twopf values";

            std::ostringstream copy_stmt;
            copy_stmt
              << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
              << " INSERT INTO " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE
              << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << ";"
              << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();

            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_ZETA_TWOPF_COPY);
          }


        // Aggregate a zeta threepf value table from a temporary container
        template <typename number>
        void aggregate_zeta_threepf(sqlite3* db, postintegration_writer<number>& writer, const std::string& temp_ctr)
          {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating zeta threepf values";

            std::ostringstream copy_stmt;
            copy_stmt
              << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
              << " INSERT INTO " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE
              << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << ";"
              << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();

            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_ZETA_THREEPF_COPY);
          }


        // Aggregate a zeta reduced bispectrum value table from a temporary container
        template <typename number>
        void aggregate_zeta_reduced_bispectrum(sqlite3* db, postintegration_writer<number>& writer, const std::string& temp_ctr)
          {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating zeta reduced bispectrum values";

            std::ostringstream copy_stmt;
            copy_stmt
              << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
              << " INSERT INTO " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE
              << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << ";"
              << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();

            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_ZETA_REDUCED_BISPECTRUM_COPY);
          }


        // Aggregate an fNL value table from a temporary container
        // Aggregation of fNL values is slightly different, because if an existing result is present for for
		    // some time serial tserial, we want to add our new value to it.
		    // For that purpose we use COALESCE.
        template <typename number>
        void aggregate_fNL(sqlite3* db, postintegration_writer<number>& writer, const std::string& temp_ctr, derived_data::template_type type)
          {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating " << derived_data::template_name(type) << " values";

		        // create a temporary table with updated values
            std::stringstream create_stmt;
            create_stmt
              << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
              << " CREATE TEMP TABLE " << __CPP_TRANSPORT_SQLITE_TEMP_FNL_TABLE << " AS"
              << " SELECT " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type) << ".tserial AS tserial,"
              << " " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type) << ".BT + COALESCE(main." << fNL_table_name(type) << ".BT, 0.0) AS new_BT,"
              << " " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type) << ".TT + COALESCE(main." << fNL_table_name(type) << ".TT, 0.0) AS new_TT"
              << " FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type)
              << " LEFT JOIN " << fNL_table_name(type)
              << " ON " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type) << ".tserial=main." << fNL_table_name(type) << ".tserial;";

//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << create_stmt.str();
            exec(db, create_stmt.str(), __CPP_TRANSPORT_DATACTR_FNL_COPY);

            std::ostringstream copy_stmt;
            copy_stmt
              << " INSERT OR REPLACE INTO main." << fNL_table_name(type)
              << " SELECT tserial AS tserial,"
              << " new_BT AS BT,"
              << " new_TT AS TT"
              << " FROM temp." << __CPP_TRANSPORT_SQLITE_TEMP_FNL_TABLE << ";"
              << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();
            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_FNL_COPY);

            std::stringstream drop_stmt;
            drop_stmt << "DROP TABLE temp." << __CPP_TRANSPORT_SQLITE_TEMP_FNL_TABLE << ";";
            exec(db, drop_stmt.str(), __CPP_TRANSPORT_DATACTR_FNL_COPY);
          }


		    // set up a temporary table representing the serial numbers we want to use
		    void create_temporary_serial_table(sqlite3* db, const std::vector<unsigned int>& serial_numbers, unsigned int worker)
			    {
				    assert(db != nullptr);

            std::stringstream stmt_text;
            stmt_text
	            << "CREATE TEMP TABLE " << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << "("
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
				    select_stmt
					    << "SELECT " << __CPP_TRANSPORT_SQLITE_TIME_SAMPLE_TABLE << ".time"
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
                                       std::vector<twopf_configuration>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, serial_numbers, worker);

            // pull out the set of k-sample points matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << ".conventional AS conventional,"
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
		                twopf_configuration value;

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
                                         std::vector<threepf_configuration>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, serial_numbers, worker);

            // create a temporary table which contains the set of configuration-sample points
            // matching serial numbers in the temporary table, together with the looked-up
		        // values of the corresponding 2pf k-configurations
            std::stringstream create_stmt;
            create_stmt
	            << "CREATE TEMP TABLE " << __CPP_TRANSPORT_SQLITE_TEMP_THREEPF_TABLE << "_" << worker << " AS"
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
                    threepf_configuration value;

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
		        drop_stmt << "DROP TABLE temp." << __CPP_TRANSPORT_SQLITE_TEMP_THREEPF_TABLE << "_" << worker << ";";
            exec(db, drop_stmt.str(), __CPP_TRANSPORT_DATAMGR_TEMP_THREEPF_DROP_FAIL);

            // check that we have as many values as we expect
            if(sample.size() != serial_numbers.size()) throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_TOO_FEW);
	        }


		    // Pull a sample of the background field evolution, for a specific field, for a specific set of time serial numbers
		    template <typename number>
		    void pull_background_time_sample(sqlite3* db, unsigned int id, const std::vector<unsigned int>& t_serials,
                                         std::vector<number>& sample, unsigned int worker, unsigned int Nfields)
			    {
						assert(db != nullptr);

				    unsigned int num_cols = std::min(2*Nfields, max_columns);
				    unsigned int page = id / num_cols;
				    unsigned int col  = id % num_cols;

				    // set up a temporary table representing the serial numbers we want to use
		        create_temporary_serial_table(db, t_serials, worker);

				    // pull out the set of background fields (labelled by 'id')
				    // matching serial numbers in the temporary table
		        std::stringstream select_stmt;
				    select_stmt
					    << "SELECT " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << ".coord" << col
					    << " FROM " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE
					    << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
					    << " ON " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
					    << " WHERE " __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE << ".page=" << page
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
		                                unsigned int k_serial, std::vector<number>& sample, twopf_value_type type, unsigned int worker, unsigned int Nfields)
			    {
				    assert(db != nullptr);

		        unsigned int num_cols = std::min(2*Nfields * 2*Nfields, max_columns);
		        unsigned int page = id / num_cols;
		        unsigned int col  = id % num_cols;

				    // set up a temporary table representing the serial numbers we want to use
		        create_temporary_serial_table(db, t_serials, worker);

				    // pull out the twopf component (labelled by 'id') matching serial numbers
				    // in the temporary table
		        std::stringstream select_stmt;
				    select_stmt
					    << "SELECT " << twopf_table_name(type) << ".ele" << col
					    << " FROM " << twopf_table_name(type)
					    << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
					    << " ON " << twopf_table_name(type) << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
					    << " WHERE " << twopf_table_name(type) << ".kserial=" << k_serial << " AND " << twopf_table_name(type) << ".page=" << page
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
                                      unsigned int k_serial, std::vector<number>& sample, unsigned int worker, unsigned int Nfields)
	        {
            assert(db != nullptr);

            unsigned int num_cols = std::min(2*Nfields * 2*Nfields * 2*Nfields, max_columns);
            unsigned int page = id / num_cols;
            unsigned int col  = id % num_cols;

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, t_serials, worker);

            // pull out the threepf component (labelled by 'id') matching serial numbers
            // in the temporary table
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".ele" << col
	            << " FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " WHERE " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".kserial=" << k_serial << " AND " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".page=" << page
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


        // Pull a sample of a tensor twopf component evolution, for a specific k-configuration and a specific set of time serial numbers
        template <typename number>
        void pull_tensor_twopf_time_sample(sqlite3* db, unsigned int id, const std::vector<unsigned int>& t_serials,
                                           unsigned int k_serial, std::vector<number>& sample, unsigned int worker)
          {
            assert(db != nullptr);

            unsigned int num_cols = std::min(static_cast<unsigned int>(4), max_columns);
            unsigned int page = id / num_cols;
            unsigned int col  = id % num_cols;

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, t_serials, worker);

            // pull out the tensor twopf component (labelled by 'id') matching serial numbers
            // in the temporary table
            std::stringstream select_stmt;
            select_stmt
              << "SELECT " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << ".ele" << col
              << " FROM " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE
              << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
              << " ON " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
              << " WHERE " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << ".kserial=" << k_serial << " AND " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << ".page=" << page
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
                                       unsigned int t_serial, std::vector<number>& sample, twopf_value_type type, unsigned int worker, unsigned int Nfields)
	        {
            assert(db != nullptr);

            unsigned int num_cols = std::min(2*Nfields * 2*Nfields, max_columns);
            unsigned int page = id / num_cols;
            unsigned int col  = id % num_cols;

		        // set up a temporary table representing the serial numbers we want to use
		        create_temporary_serial_table(db, k_serials, worker);

		        // pull out the twopf component (labelled by 'id') matching serial numbers
		        // in the temporary table
            std::stringstream select_stmt;
		        select_stmt
			        << "SELECT " << twopf_table_name(type) << ".ele" << col
			        << " FROM " << twopf_table_name(type)
			        << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
			        << " ON " << twopf_table_name(type) << ".kserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
			        << " WHERE " << twopf_table_name(type) << ".tserial=" << t_serial << " AND " << twopf_table_name(type) << ".page=" << page
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
                                         unsigned int t_serial, std::vector<number>& sample, unsigned int worker, unsigned int Nfields)
	        {
            assert(db != nullptr);

            unsigned int num_cols = std::min(2*Nfields * 2*Nfields * 2*Nfields, max_columns);
            unsigned int page = id / num_cols;
            unsigned int col  = id % num_cols;

		        // set up a temporary table representing the serial numbers we want to use
		        create_temporary_serial_table(db, k_serials, worker);

		        // pull out the threepf component (labelled by 'id') matching serial numbers
		        // in the temporary table
            std::stringstream select_stmt;
		        select_stmt
			        << "SELECT " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".ele" << col
			        << " FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE
			        << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
			        << " ON " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".kserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
			        << " WHERE " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".tserial=" << t_serial << " AND " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ".page=" << page
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


        // Pull a sample of a tensor twopf component, for a specific time sample point and a specific set of k-configuration serial numbers
        template <typename number>
        void pull_tensor_twopf_kconfig_sample(sqlite3* db, unsigned int id, const std::vector<unsigned int>& k_serials,
                                              unsigned int t_serial, std::vector<number>& sample, unsigned int worker)
          {
            assert(db != nullptr);

            unsigned int num_cols = std::min(static_cast<unsigned int>(4), max_columns);
            unsigned int page = id / num_cols;
            unsigned int col  = id % num_cols;

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, k_serials, worker);

            // pull out the threepf component (labelled by 'id') matching serial numbers
            // in the temporary table
            std::stringstream select_stmt;
            select_stmt
              << "SELECT " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << ".ele" << col
              << " FROM " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE
              << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
              << " ON " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << ".kserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
              << " WHERE " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << ".tserial=" << t_serial << " AND " << __CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE << ".page=" << page
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


		    // Pull a sample of a zeta twopf, for a specific k-configuration and a specific set of time serial numbers
		    template <typename number>
		    void pull_zeta_twopf_time_sample(sqlite3* db, const std::vector<unsigned int>& t_serials,
		                                     unsigned int k_serial, std::vector<number>& sample, unsigned int worker)
			    {
		        assert(db != nullptr);

				    // set up a temporary table representing the serial numbers we want to use
		        create_temporary_serial_table(db, t_serials, worker);

				    // pull out the zeta twopf component matching serial numbers in the temporary table
		        std::stringstream select_stmt;
				    select_stmt
				      << "SELECT " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << ".ele"
					    << " FROM " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE
					    << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
					    << " ON " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
					    << " WHERE " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << ".kserial=" << k_serial
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


        // Pull a sample of a zeta threepf, for a specific k-configuration and a specific set of time serial numbers
        template <typename number>
        void pull_zeta_threepf_time_sample(sqlite3* db, const std::vector<unsigned int>& t_serials,
                                           unsigned int k_serial, std::vector<number>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, t_serials, worker);

            // pull out the zeta twopf component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << ".ele"
	            << " FROM " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " WHERE " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << ".kserial=" << k_serial
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


        // Pull a sample of a zeta reduced bispectrum, for a specific k-configuration and a specific set of time serial numbers
        template <typename number>
        void pull_zeta_redbsp_time_sample(sqlite3* db, const std::vector<unsigned int>& t_serials,
                                          unsigned int k_serial, std::vector<number>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, t_serials, worker);

            // pull out the zeta twopf component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << ".ele"
	            << " FROM " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " WHERE " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << ".kserial=" << k_serial
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


        // Pull a sample of an fNL, for a specific set of time serial numbers
        template <typename number>
        void pull_fNL_time_sample(sqlite3* db, const std::vector<unsigned int>& t_serials,
                                  std::vector<number>& sample, unsigned int worker, derived_data::template_type type)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, t_serials, worker);

            // pull out the zeta twopf component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT (5.0/3.0)*(" << fNL_table_name(type) << ".BT/" << fNL_table_name(type) << ".TT)"
	            << " FROM " << fNL_table_name(type)
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << fNL_table_name(type) << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
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


        // Pull a sample of bispectrum.template, for a specific set of time serial numbers
        template <typename number>
        void pull_BT_time_sample(sqlite3* db, const std::vector<unsigned int>& t_serials,
                                 std::vector<number>& sample, unsigned int worker, derived_data::template_type type)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, t_serials, worker);

            // pull out the zeta twopf component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT " << fNL_table_name(type) << ".BT"
	            << " FROM " << fNL_table_name(type)
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << fNL_table_name(type) << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " ORDER BY temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".ROWID;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length() + 1, &stmt, nullptr));

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


        // Pull a sample of template.template, for a specific set of time serial numbers
        template <typename number>
        void pull_TT_time_sample(sqlite3* db, const std::vector<unsigned int>& t_serials,
                                 std::vector<number>& sample, unsigned int worker, derived_data::template_type type)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, t_serials, worker);

            // pull out the zeta twopf component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT " << fNL_table_name(type) << ".TT"
	            << " FROM " << fNL_table_name(type)
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << fNL_table_name(type) << ".tserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " ORDER BY temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".ROWID;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length() + 1, &stmt, nullptr));

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


        // Pull a sample of a zeta twopf, for a specific k-configuration and a specific set of time serial numbers
        template <typename number>
        void pull_zeta_twopf_kconfig_sample(sqlite3* db, const std::vector<unsigned int>& k_serials,
                                            unsigned int t_serial, std::vector<number>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, k_serials, worker);

            // pull out the zeta twopf component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << ".ele"
	            << " FROM " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << ".kserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " WHERE " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << ".tserial=" << t_serial
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
                    msg << __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
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


        // Pull a sample of a zeta threepf, for a specific k-configuration and a specific set of time serial numbers
        template <typename number>
        void pull_zeta_threepf_kconfig_sample(sqlite3* db, const std::vector<unsigned int>& k_serials,
                                              unsigned int t_serial, std::vector<number>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, k_serials, worker);

            // pull out the zeta twopf component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << ".ele"
	            << " FROM " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << ".kserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " WHERE " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << ".tserial=" << t_serial
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
                    msg << __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
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


        // Pull a sample of a zeta reduced bispectrum, for a specific k-configuration and a specific set of time serial numbers
        template <typename number>
        void pull_zeta_redbsp_kconfig_sample(sqlite3* db, const std::vector<unsigned int>& k_serials,
                                             unsigned int t_serial, std::vector<number>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, k_serials, worker);

            // pull out the zeta twopf component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << ".ele"
	            << " FROM " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE
	            << " INNER JOIN temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker
	            << " ON " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << ".kserial=" << "temp." << __CPP_TRANSPORT_SQLITE_TEMP_SERIAL_TABLE << "_" << worker << ".serial"
	            << " WHERE " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << ".tserial=" << t_serial
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
                    msg << __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
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


#endif //__data_manager_operations_H_
