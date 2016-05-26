//
// Created by David Seery on 27/04/15.
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


#ifndef CPPTRANSPORT_DATA_MANAGER_CREATE_H
#define CPPTRANSPORT_DATA_MANAGER_CREATE_H


#include "transport-runtime/sqlite3/operations/data_manager_common.h"
#include "transport-runtime/sqlite3/operations/data_traits.h"


namespace transport
	{

		namespace sqlite3_operations
			{

		    // Create a sample table of times
		    template <typename number>
		    void create_time_sample_table(transaction_manager& mgr, sqlite3* db, derivable_task<number>* tk)
			    {
		        assert(db != nullptr);
		        assert(tk != nullptr);

		        const time_config_database& time_db = tk->get_stored_time_config_database();

		        // set up a table
		        std::stringstream create_stmt;
		        create_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "("
			        << "serial INTEGER PRIMARY KEY,"
			        << "time   DOUBLE"
			        << ");";

		        exec(db, create_stmt.str(), CPPTRANSPORT_DATACTR_TIMETAB_FAIL);

		        std::stringstream insert_stmt;
		        insert_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE << " VALUES (@serial, @time)";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

		        for(time_config_database::const_config_iterator t = time_db.config_begin(); t != time_db.config_end(); ++t)
			        {
		            check_stmt(db, sqlite3_bind_int(stmt, 1, t->serial));
		            check_stmt(db, sqlite3_bind_double(stmt, 2, t->t));

		            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_DATACTR_TIMETAB_FAIL, SQLITE_DONE);

		            check_stmt(db, sqlite3_clear_bindings(stmt));
		            check_stmt(db, sqlite3_reset(stmt));
			        }

		        check_stmt(db, sqlite3_finalize(stmt));
			    }


		    // Create a sample table of twopf configurations
		    template <typename TaskType>
		    void create_twopf_sample_table(transaction_manager& mgr, sqlite3* db, TaskType* tk)
			    {
		        assert(db != nullptr);
		        assert(tk != nullptr);

		        const twopf_kconfig_database& twopf_db = tk->get_twopf_database();

		        // set up a table
		        std::stringstream stmt_text;
		        stmt_text
			        << "CREATE TABLE " << CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << "("
			        << "serial       INTEGER PRIMARY KEY, "
			        << "conventional DOUBLE, "
			        << "comoving     DOUBLE, "
			        << "t_exit       DOUBLE, "
              << "t_massless   DOUBLE"
			        << ");";

		        exec(db, stmt_text.str(), CPPTRANSPORT_DATACTR_TWOPFTAB_FAIL);

		        std::stringstream insert_stmt;
		        insert_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " VALUES (@serial, @conventional, @comoving, @t_exit, @t_massless);";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

		        for(twopf_kconfig_database::const_config_iterator t = twopf_db.config_begin(); t != twopf_db.config_end(); ++t)
			        {
		            check_stmt(db, sqlite3_bind_int(stmt, 1, t->serial));
		            check_stmt(db, sqlite3_bind_double(stmt, 2, t->k_conventional));
		            check_stmt(db, sqlite3_bind_double(stmt, 3, t->k_comoving));
		            check_stmt(db, sqlite3_bind_double(stmt, 4, t->t_exit));
                check_stmt(db, sqlite3_bind_double(stmt, 5, t->t_massless));

		            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_DATACTR_TWOPFTAB_FAIL, SQLITE_DONE);

		            check_stmt(db, sqlite3_clear_bindings(stmt));
		            check_stmt(db, sqlite3_reset(stmt));
			        }

		        check_stmt(db, sqlite3_finalize(stmt));
			    }


		    // Create a sample table of threepf configurations
		    template <typename TaskType>
		    void create_threepf_sample_table(transaction_manager& mgr, sqlite3* db, TaskType* tk)
			    {
		        assert(db != nullptr);
		        assert(tk != nullptr);

		        const threepf_kconfig_database& threepf_db = tk->get_threepf_database();

		        // set up a table
		        std::stringstream stmt_text;
		        stmt_text
			        << "CREATE TABLE " << CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << "("
			        << "serial          INTEGER PRIMARY KEY, "
			        << "wavenumber1     INTEGER, "
			        << "wavenumber2     INTEGER, "
			        << "wavenumber3     INTEGER, "
			        << "kt_comoving     DOUBLE, "
			        << "kt_conventional DOUBLE, "
			        << "alpha           DOUBLE, "
			        << "beta            DOUBLE, "
			        << "t_exit_kt       DOUBLE, "
              << "t_massless      DOUBLE, "
			        << "FOREIGN KEY(wavenumber1) REFERENCES twopf_samples(serial), "
			        << "FOREIGN KEY(wavenumber2) REFERENCES twopf_samples(serial), "
			        << "FOREIGN KEY(wavenumber3) REFERENCES twopf_samples(serial)"
			        << ");";

		        exec(db, stmt_text.str());

		        std::stringstream insert_stmt;
		        insert_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << " VALUES (@serial, @wn1, @wn2, @wn3, @kt_com, @kt_conv, @alpha, @beta, @t_exit_kt, @t_massless);";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

		        for(threepf_kconfig_database::const_config_iterator t = threepf_db.config_begin(); t != threepf_db.config_end(); ++t)
			        {
		            check_stmt(db, sqlite3_bind_int(stmt, 1, t->serial));
		            check_stmt(db, sqlite3_bind_int(stmt, 2, t->k1_serial));
		            check_stmt(db, sqlite3_bind_int(stmt, 3, t->k2_serial));
		            check_stmt(db, sqlite3_bind_int(stmt, 4, t->k3_serial));
		            check_stmt(db, sqlite3_bind_double(stmt, 5, t->kt_comoving));
		            check_stmt(db, sqlite3_bind_double(stmt, 6, t->kt_conventional));
		            check_stmt(db, sqlite3_bind_double(stmt, 7, t->alpha));
		            check_stmt(db, sqlite3_bind_double(stmt, 8, t->beta));
		            check_stmt(db, sqlite3_bind_double(stmt, 9, t->t_exit));
                check_stmt(db, sqlite3_bind_double(stmt, 10, t->t_massless));

		            check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_DATACTR_THREEPFTAB_FAIL, SQLITE_DONE);

		            check_stmt(db, sqlite3_clear_bindings(stmt));
		            check_stmt(db, sqlite3_reset(stmt));
			        }

		        check_stmt(db, sqlite3_finalize(stmt));
			    }


		    // Create table documenting workers
		    void create_worker_info_table(transaction_manager& mgr, sqlite3* db, foreign_keys_type keys=foreign_keys_type::no_foreign_keys)
			    {
		        std::ostringstream create_stmt;
		        create_stmt
			        << "CREATE TABLE " << CPPTRANSPORT_SQLITE_WORKERS_TABLE << "("
			        << "workgroup     INTEGER, "
			        << "worker        INTEGER, "
			        << "backend       TEXT, "
			        << "back_stepper  TEXT, "
			        << "pert_stepper  TEXT, "
              << "back_abs_tol  DOUBLE, "
              << "back_rel_tol  DOUBLE, "
              << "pert_abs_tol  DOUBLE, "
              << "pert_rel_tol  DOUBLE, "
			        << "hostname      TEXT, "
			        << "os_name       TEXT, "
			        << "os_version    TEXT, "
			        << "os_release    TEXT, "
			        << "architecture  TEXT, "
			        << "cpu_vendor_id TEXT";

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
            create_stmt << ", PRIMARY KEY (workgroup, worker)"
#endif

            create_stmt << ");";

		        exec(db, create_stmt.str());
			    }


		    // Create table for statistics, if they are being collected
		    void create_stats_table(transaction_manager& mgr, sqlite3* db, foreign_keys_type keys=foreign_keys_type::no_foreign_keys, kconfiguration_type type= kconfiguration_type::twopf_configs)
			    {
		        std::ostringstream create_stmt;
		        create_stmt
			        << "CREATE TABLE " << CPPTRANSPORT_SQLITE_STATS_TABLE << "("
			        << "kserial           INTEGER PRIMARY KEY, "
			        << "integration_time  INTEGER, "
			        << "batch_time        INTEGER, "
			        << "steps             INTEGER, "
			        << "refinements       INTEGER, "
			        << "workgroup         INTEGER, "
			        << "worker            INTEGER";

		        if(keys == foreign_keys_type::foreign_keys)
			        {
		            create_stmt << ", FOREIGN KEY(kserial) REFERENCES ";
		            switch(type)
			            {
		                case kconfiguration_type::twopf_configs:
			                create_stmt << CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE;
		                  break;

		                case kconfiguration_type::threepf_configs:
			                create_stmt << CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE;
		                  break;
			            }
		            create_stmt << "(serial)";

		            create_stmt << ", FOREIGN KEY(workgroup, worker) REFERENCES " << CPPTRANSPORT_SQLITE_WORKERS_TABLE << "(workgroup, worker)";
			        }
		        create_stmt << ");";

		        exec(db, create_stmt.str());
			    }


		    // Create table for initial conditions, if they are being collected
				template <typename number, typename ValueType>
		    void create_ics_table(transaction_manager& mgr, sqlite3* db, unsigned int Nfields, foreign_keys_type keys=foreign_keys_type::no_foreign_keys,
		                          kconfiguration_type type= kconfiguration_type::twopf_configs)
			    {
		        unsigned int num_cols = std::min(2*Nfields, max_columns);

		        std::ostringstream create_stmt;
		        create_stmt
			        << "CREATE TABLE " << data_traits<number, ValueType>::sqlite_table() << "("
			        << "kserial INTEGER, "
			        << "page    INTEGER, "
			        << "t_exit  DOUBLE";

		        for(unsigned int i = 0; i < num_cols; ++i)
			        {
		            create_stmt << ", coord" << i << " DOUBLE";
			        }

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
		        create_stmt << ", PRIMARY KEY (kserial, page)";
#endif

		        if(keys == foreign_keys_type::foreign_keys)
			        {
		            create_stmt << ", FOREIGN KEY(kserial) REFERENCES ";
		            switch(type)
			            {
		                case kconfiguration_type::twopf_configs:
			                create_stmt << CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE;
		                  break;

		                case kconfiguration_type::threepf_configs:
			                create_stmt << CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE;
		                  break;
			            }
		            create_stmt << "(serial)";
			        }
		        create_stmt << ");";

		        exec(db, create_stmt.str());
			    }


		    // Create table for background values
        template <typename number, typename ValueType>
		    void create_backg_table(transaction_manager& mgr, sqlite3* db, unsigned int Nfields, foreign_keys_type keys=foreign_keys_type::no_foreign_keys)
			    {
            unsigned int num_elements = data_traits<number, ValueType>::number_elements(Nfields);
            unsigned int num_cols = std::min(num_elements, max_columns);

		        std::ostringstream create_stmt;
		        create_stmt
			        << "CREATE TABLE " << data_traits<number, ValueType>::sqlite_table() << "("
			        << "tserial INTEGER, "
			        << "page    INTEGER";

		        for(unsigned int i = 0; i < num_cols; ++i)
			        {
		            create_stmt << ", coord" << i << " DOUBLE";
			        }

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
		        create_stmt << ", PRIMARY KEY (tserial, page)";
#endif

		        if(keys == foreign_keys_type::foreign_keys) create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)";
		        create_stmt << ");";

		        exec(db, create_stmt.str());
			    }


        // Create table for paged values
        template <typename number, typename ValueType>
        void create_paged_table(transaction_manager& mgr, sqlite3* db, unsigned int Nfields, foreign_keys_type keys=foreign_keys_type::no_foreign_keys,
                                kconfiguration_type type=kconfiguration_type::twopf_configs)
          {
            unsigned int num_elements = data_traits<number, ValueType>::number_elements(Nfields);
            unsigned int num_cols = std::min(num_elements, max_columns);

            std::ostringstream create_stmt;
            create_stmt
              << "CREATE TABLE " << data_traits<number, ValueType>::sqlite_table() << "("
              << "tserial INTEGER, "
              << "kserial INTEGER, "
              << "page    INTEGER";

            for(unsigned int i = 0; i < num_cols; ++i)
              {
                create_stmt << ", ele" << i << " DOUBLE";
              }

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
            create_stmt << ", PRIMARY KEY (tserial, kserial, page)";
#endif

            if(keys == foreign_keys_type::foreign_keys)
              {
                create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)";

                create_stmt << ", FOREIGN KEY(kserial) REFERENCES ";
                switch(type)
                  {
                    case kconfiguration_type::twopf_configs:
                      create_stmt << CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE;
                    break;

                    case kconfiguration_type::threepf_configs:
                      create_stmt << CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE;
                    break;
                  }
                create_stmt << "(serial)";
              }
            create_stmt << ");";

            exec(db, create_stmt.str());
          }


		    // Create table for zeta twopf values
		    void create_zeta_twopf_table(transaction_manager& mgr, sqlite3* db, foreign_keys_type keys=foreign_keys_type::no_foreign_keys)
			    {
		        std::ostringstream create_stmt;
		        create_stmt
			        << "CREATE TABLE " << CPPTRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << "("
			        << "tserial       INTEGER, "
			        << "kserial       INTEGER, "
			        << "twopf         DOUBLE";

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
            create_stmt << ", PRIMARY KEY (tserial, kserial)";
#endif

		        if(keys == foreign_keys_type::foreign_keys)
			        {
		            create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)"
			            << ", FOREIGN KEY(kserial) REFERENCES " << CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << "(serial)";
			        }
		        create_stmt << ");";

		        exec(db, create_stmt.str());
			    }


		    // Create table for zeta threepf values
		    void create_zeta_threepf_table(transaction_manager& mgr, sqlite3* db, foreign_keys_type keys=foreign_keys_type::no_foreign_keys)
			    {
		        std::ostringstream create_stmt;
		        create_stmt
			        << "CREATE TABLE " << CPPTRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << "("
			        << "tserial       INTEGER, "
			        << "kserial       INTEGER, "
			        << "threepf       DOUBLE, "
              << "redbsp        DOUBLE";

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
            create_stmt << "PRIMARY KEY (tserial, kserial)";
#endif

		        if(keys == foreign_keys_type::foreign_keys)
			        {
		            create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)"
			            << ", FOREIGN KEY(kserial) REFERENCES " << CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << "(serial)";
			        }
		        create_stmt << ");";

		        exec(db, create_stmt.str());
			    }


		    // Create table for fNL values
		    void create_fNL_table(transaction_manager& mgr, sqlite3* db, derived_data::bispectrum_template type, foreign_keys_type keys=foreign_keys_type::no_foreign_keys)
			    {
		        std::ostringstream create_stmt;
		        create_stmt
			        << "CREATE TABLE " << fNL_table_name(type) << "("
			        << "tserial INTEGER, "
              << "BB      DOUBLE, "
			        << "BT      DOUBLE, "
			        << "TT      DOUBLE";

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
            create_stmt << "PRIMARY KEY (tserial)";
#endif

		        if(keys == foreign_keys_type::foreign_keys)
			        {
		            create_stmt << ", FOREIGN KEY(tserial) REFERENCES " << CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE << "(serial)";
			        }
		        create_stmt << ");";

		        exec(db, create_stmt.str());
			    }

			}   // namespace sqlite3_operations

	}   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_CREATE_H
