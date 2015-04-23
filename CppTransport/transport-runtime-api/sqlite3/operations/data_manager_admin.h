//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __data_manager_admin_H_
#define __data_manager_admin_H_


#include "transport-runtime-api/sqlite3/operations/data_manager_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        // Create a sample table of times
        template <typename number>
        void create_time_sample_table(sqlite3* db, derivable_task<number>* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            const time_config_database& time_db = tk->get_stored_time_config_database();

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

            for(time_config_database::const_config_iterator t = time_db.config_begin(); t != time_db.config_end(); t++)
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
        template <typename TaskType>
        void create_twopf_sample_table(sqlite3* db, TaskType* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            const twopf_kconfig_database& twopf_db = tk->get_twopf_database();

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

            for(twopf_kconfig_database::const_config_iterator t = twopf_db.config_begin(); t != twopf_db.config_end(); t++)
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
        template <typename TaskType>
        void create_threepf_sample_table(sqlite3* db, TaskType* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            const threepf_kconfig_database& threepf_db = tk->get_threepf_database();

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

            for(threepf_kconfig_database::const_config_iterator t = threepf_db.config_begin(); t != threepf_db.config_end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, t->serial));
                check_stmt(db, sqlite3_bind_int(stmt, 2, t->k1_serial));
                check_stmt(db, sqlite3_bind_int(stmt, 3, t->k2_serial));
                check_stmt(db, sqlite3_bind_int(stmt, 4, t->k3_serial));
                check_stmt(db, sqlite3_bind_double(stmt, 5, t->kt_comoving));
                check_stmt(db, sqlite3_bind_double(stmt, 6, t->kt_conventional));
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
					    << "workgroup     INTEGER, "
				      << "worker        INTEGER, "
					    << "backend       TEXT, "
              << "back_stepper  TEXT, "
              << "pert_stepper  TEXT, "
				      << "hostname      TEXT, "
				      << "os_name       TEXT, "
				      << "os_version    TEXT, "
				      << "os_release    TEXT, "
				      << "architecture  TEXT, "
				      << "cpu_vendor_id TEXT, "
		          << "PRIMARY KEY (workgroup, worker)"
					    << ");";

				    exec(db, create_stmt.str());
			    }


        // Create table for statistics, if they are being collected
        void create_stats_table(sqlite3* db, add_foreign_keys_type keys=no_foreign_keys, metadata_configuration_type type=twopf_configs)
          {
            std::ostringstream create_stmt;
            create_stmt
              << "CREATE TABLE " << __CPP_TRANSPORT_SQLITE_STATS_TABLE << "("
              << "kserial           INTEGER PRIMARY KEY, "
              << "integration_time  DOUBLE, "
              << "batch_time        DOUBLE, "
              << "steps             INTEGER, "
	            << "refinements       INTEGER, "
              << "workgroup         INTEGER, "
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

		            create_stmt << ", FOREIGN KEY(workgroup, worker) REFERENCES " << __CPP_TRANSPORT_SQLITE_WORKERS_TABLE << "(workgroup, worker)";
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
		    void write_host_info(integration_batcher<number>* batcher)
			    {
				    sqlite3* db = nullptr;
				    batcher->get_manager_handle(&db);

				    const host_information& host = batcher->get_host_information();

		        std::ostringstream insert_stmt;
				    insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_WORKERS_TABLE << " VALUES (@workgroup, @worker, @backend, @back_stepper, @pert_stepper, @hostname, @os_name, @os_version, @os_release, @architecture, @cpu_vendor_id)";

				    sqlite3_stmt* stmt;
				    check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

				    exec(db, "BEGIN TRANSACTION");

				    // to document the different choice of length in these sqlite3_bind_text() statements compared to sqlite3_prepare_v2,
				    // the SQLite3 documentation says:

		        // If the caller knows that the supplied string is nul-terminated, then there is a small performance advantage to be
		        // gained by passing an nByte parameter that is equal to the number of bytes in the input string including the nul-terminator
		        // bytes as this saves SQLite from having to make a copy of the input string.

				    // However, here, we certainly *shouldn't* include the nul-terminator byte, because that isn't part of the string

				    check_stmt(db, sqlite3_bind_int(stmt, 1, batcher->get_worker_group()));
				    check_stmt(db, sqlite3_bind_int(stmt, 2, batcher->get_worker_number()));
				    check_stmt(db, sqlite3_bind_text(stmt, 3, batcher->get_backend().c_str(), batcher->get_backend().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, 4, batcher->get_back_stepper().c_str(), batcher->get_back_stepper().length(), SQLITE_STATIC));
            check_stmt(db, sqlite3_bind_text(stmt, 5, batcher->get_pert_stepper().c_str(), batcher->get_pert_stepper().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 6, host.get_host_name().c_str(), host.get_host_name().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 7, host.get_os_name().c_str(), host.get_os_name().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 8, host.get_os_version().c_str(), host.get_os_version().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 9, host.get_os_release().c_str(), host.get_os_release().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 10, host.get_architecture().c_str(), host.get_architecture().length(), SQLITE_STATIC));
				    check_stmt(db, sqlite3_bind_text(stmt, 11, host.get_cpu_vendor_id().c_str(), host.get_cpu_vendor_id().length(), SQLITE_STATIC));

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
            insert_stmt << "INSERT INTO " << __CPP_TRANSPORT_SQLITE_STATS_TABLE << " VALUES (@kserial, @integration_time, @batch_time, @steps, @refinements, @workgroup, @worker);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            exec(db, "BEGIN TRANSACTION;");

            for(typename std::vector<typename integration_items<number>::configuration_statistics >::const_iterator t = batch.begin(); t != batch.end(); t++)
              {
                check_stmt(db, sqlite3_bind_int(stmt, 1, t->serial));
                check_stmt(db, sqlite3_bind_double(stmt, 2, t->integration));
                check_stmt(db, sqlite3_bind_double(stmt, 3, t->batching));
                check_stmt(db, sqlite3_bind_int(stmt, 4, t->steps));
		            check_stmt(db, sqlite3_bind_int(stmt, 5, t->refinements));
		            check_stmt(db, sqlite3_bind_int(stmt, 6, batcher->get_worker_group()));
                check_stmt(db, sqlite3_bind_int(stmt, 7, batcher->get_worker_number()));

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


	    }   // namespace sqlite3_operations

  }   // namespace transport


#endif //__data_manager_admin_H_
