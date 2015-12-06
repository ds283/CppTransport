//
// Created by David Seery on 13/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __data_manager_pull_H_
#define __data_manager_pull_H_


#include "transport-runtime-api/sqlite3/operations/data_manager_common.h"
#include "transport-runtime-api/sqlite3/operations/data_traits.h"
#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"


namespace transport
  {

    namespace sqlite3_operations
      {

		    namespace pull_implementation
			    {

				    // sample should be empty and have a suitable size reserved before calling
				    template <typename TargetType>
				    void pull_number_list(sqlite3* db, std::vector<TargetType>& sample, std::string sql_query, std::string error_msg)
					    {
				        sqlite3_stmt* stmt;
				        check_stmt(db, sqlite3_prepare_v2(db, sql_query.c_str(), sql_query.length()+1, &stmt, nullptr));

				        int status;
				        while((status = sqlite3_step(stmt)) != SQLITE_DONE)
					        {
				            if(status == SQLITE_ROW)
					            {
				                double time = static_cast<TargetType>(sqlite3_column_double(stmt, 0));
				                sample.push_back(time);
					            }
				            else
					            {
				                std::ostringstream msg;
				                msg << error_msg << status << ": " << sqlite3_errmsg(db) << ")";
				                sqlite3_finalize(stmt);
				                throw runtime_exception(exception_type::DATA_MANAGER_BACKEND_ERROR, msg.str());
					            }
					        }

				        check_stmt(db, sqlite3_finalize(stmt));
					    }

			    }


        // Pull a set of time sample points, identified by their serial numbers
        void pull_time_config_sample(sqlite3* db, const derived_data::SQL_time_config_query& query,
                                     std::vector<time_config>& sample, unsigned int worker)
          {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            // pull out the set of time-sample points matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
              << "SELECT"
	            << " temp.serial AS serial,"
	            << " temp.time AS time"
              << " FROM (" << query.make_query(policy, false) << ") temp"
              << " ORDER BY temp.serial;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

            sample.clear();

            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    time_config value;

                    value.serial = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));
                    value.t      = sqlite3_column_double(stmt, 1);

                    sample.push_back(value);
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(exception_type::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Pull a set of twopf k-configuration sample points, identified by their serial numbers
        template <typename number>
        void pull_twopf_kconfig_sample(sqlite3* db, const derived_data::SQL_twopf_kconfig_query& query,
                                       std::vector<twopf_kconfig>& sample, unsigned int worker)
          {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            // pull out the set of k-sample points matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
              << "SELECT"
	            << " temp.serial AS serial,"
	            << " temp.conventional AS conventional,"
              << " temp.comoving AS comoving,"
	            << " temp.t_exit AS t_exit"
              << " FROM (" << query.make_query(policy, false) << ") temp"
              << " ORDER BY temp.serial";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

            sample.clear();

            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    twopf_kconfig value;

                    value.serial         = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));
                    value.k_conventional = sqlite3_column_double(stmt, 1);
                    value.k_comoving     = sqlite3_column_double(stmt, 2);
		                value.t_exit         = sqlite3_column_double(stmt, 3);

                    sample.push_back(value);
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_DATAMGR_KCONFIG_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(exception_type::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Pull a set of threepf k-configuration sample points, identified by their serial numbers
        template <typename number>
        void pull_threepf_kconfig_sample(sqlite3* db, const derived_data::SQL_threepf_kconfig_query& query,
                                         std::vector<threepf_kconfig>& sample, unsigned int worker)
          {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            // create a temporary table which contains the set of configuration-sample points
            // matching serial numbers in the temporary table, together with the looked-up
            // values of the corresponding 2pf k-configurations
            std::stringstream select_stmt;
            select_stmt
              << " SELECT"
              << " temp.serial AS serial,"
	            << " temp.kt_conventional AS kt_conventional,"
              << " temp.kt_comoving AS kt_comoving,"
              << " temp.alpha AS alpha,"
              << " temp.beta AS beta,"
              << " tpf1.conventional AS conventional_k1,"
              << " tpf1.comoving AS comoving_k1,"
              << " tpf2.conventional AS conventional_k2,"
              << " tpf2.comoving AS comoving_k2,"
              << " tpf3.conventional AS conventional_k3,"
              << " tpf3.comoving AS comoving_k3,"
              << " temp.wavenumber1 AS wavenumber1,"
              << " temp.wavenumber2 AS wavenumber1,"
              << " temp.wavenumber3 AS wavenumber1,"
	            << " temp.t_exit AS t_exit"
              << " FROM (" << query.make_query(policy, false) << ") temp"
              << " INNER JOIN " << CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " AS tpf1"
              << " ON temp.wavenumber1=tpf1.serial"
              << " INNER JOIN " << CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " AS tpf2"
              << " ON temp.wavenumber2=tpf2.serial"
              << " INNER JOIN " << CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " AS tpf3"
              << " ON temp.wavenumber3=tpf3.serial"
              << " ORDER BY temp.serial;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length()+1, &stmt, nullptr));

            sample.clear();

            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    threepf_kconfig value;

                    value.serial          = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));
                    value.kt_conventional = sqlite3_column_double(stmt, 1);
                    value.kt_comoving     = sqlite3_column_double(stmt, 2);
                    value.alpha           = sqlite3_column_double(stmt, 3);
                    value.beta            = sqlite3_column_double(stmt, 4);
                    value.k1_conventional = sqlite3_column_double(stmt, 5);
                    value.k1_comoving     = sqlite3_column_double(stmt, 6);
                    value.k1_serial       = static_cast<unsigned int>(sqlite3_column_int(stmt, 11));
                    value.k2_conventional = sqlite3_column_double(stmt, 7);
                    value.k2_comoving     = sqlite3_column_double(stmt, 8);
                    value.k2_serial       = static_cast<unsigned int>(sqlite3_column_int(stmt, 12));
                    value.k3_conventional = sqlite3_column_double(stmt, 9);
                    value.k3_comoving     = sqlite3_column_double(stmt, 10);
                    value.k3_serial       = static_cast<unsigned int>(sqlite3_column_int(stmt, 13));
		                value.t_exit          = sqlite3_column_double(stmt, 14);

                    sample.push_back(value);
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_DATAMGR_KCONFIG_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(exception_type::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));
          }


        // Pull k-configuration statistics data for a set of k-configuration serial numbers
        void pull_k_statistics_sample(sqlite3* db, const derived_data::SQL_query& query,
                                      std::vector<kconfiguration_statistics>& data, unsigned int worker)
	        {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            // pull out matching k-statistics data
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT"
	            << " temp.kserial AS kserial,"
	            << " temp.integration_time AS integration_time,"
	            << " temp.batch_time AS batch_time,"
	            << " temp.steps AS steps,"
	            << " temp.refinements AS refinements,"
	            << " temp.workgroup AS workgroup,"
	            << " temp.worker AS worker,"
	            << " workers.backend AS backend,"
	            << " workers.back_stepper AS back_stepper,"
	            << " workers.pert_stepper AS pert_stepper,"
	            << " workers.hostname AS hostname"
	            << " FROM (SELECT * FROM " << CPPTRANSPORT_SQLITE_STATS_TABLE
	            << " INNER JOIN (" << query.make_query(policy, true) << ") tpf"
	            << " ON " << CPPTRANSPORT_SQLITE_STATS_TABLE << ".kserial=tpf.serial) temp"
	            << " INNER JOIN " << CPPTRANSPORT_SQLITE_WORKERS_TABLE << " workers ON workers.workgroup=temp.workgroup"
	            << " WHERE temp.worker=workers.worker"
	            << " ORDER BY temp.kserial;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, select_stmt.str().c_str(), select_stmt.str().length() + 1, &stmt, nullptr));

            data.clear();

            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
	            {
                if(status == SQLITE_ROW)
	                {
                    kconfiguration_statistics value;

                    value.serial               = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));
                    value.integration          = sqlite3_column_int64(stmt, 1);
                    value.batching             = sqlite3_column_int64(stmt, 2);
                    value.steps                = static_cast<unsigned int>(sqlite3_column_int(stmt, 3));
                    value.refinements          = static_cast<unsigned int>(sqlite3_column_int(stmt, 4));
                    value.workgroup            = static_cast<unsigned int>(sqlite3_column_int(stmt, 5));
                    value.worker               = static_cast<unsigned int>(sqlite3_column_int(stmt, 6));
                    value.backend              = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)),  static_cast<unsigned int>(sqlite3_column_bytes(stmt, 7)));
                    value.background_stepper   = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)),  static_cast<unsigned int>(sqlite3_column_bytes(stmt, 8)));
                    value.perturbation_stepper = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)),  static_cast<unsigned int>(sqlite3_column_bytes(stmt, 9)));
                    value.hostname             = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10)), static_cast<unsigned int>(sqlite3_column_bytes(stmt, 10)));

                    data.push_back(value);
	                }
                else
	                {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_DATAMGR_KCONFIG_SERIAL_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(exception_type::DATA_MANAGER_BACKEND_ERROR, msg.str());
	                }
	            }

            check_stmt(db, sqlite3_finalize(stmt));
	        }


        // Pull a sample of the background field evolution, for a specific field, for a specific set of time serial numbers
        template <typename number>
        void pull_background_time_sample(sqlite3* db, unsigned int id, const derived_data::SQL_query& tquery,
                                         std::vector<number>& sample, unsigned int worker, unsigned int Nfields)
          {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            unsigned int num_cols = std::min(2*Nfields, max_columns);
            unsigned int page = id / num_cols;
            unsigned int col  = id % num_cols;

            // pull out the set of background fields (labelled by 'id')
            // matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
              << "SELECT"
	            << " " << CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE << ".coord" << col
              << " FROM " << CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE
              << " INNER JOIN (" << tquery.make_query(policy, true) << ") tsample"
	            << " ON " << CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE << ".tserial=tsample.serial"
              << " WHERE " CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE << ".page=" << page
              << " ORDER BY tsample.serial;";

            sample.clear();
            pull_implementation::pull_number_list(db, sample, select_stmt.str(), CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL);
          }


		    template <typename number, typename ValueType>
		    void pull_paged_time_sample(sqlite3* db, unsigned int id, const derived_data::SQL_query& tquery,
		                                unsigned int k_serial, std::vector<number>& sample, unsigned int worker, unsigned int Nfields)
			    {
				    assert(db != nullptr);

		        derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
		                                        CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
		                                        CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
		                                        "wavenumber1", "wavenumber2", "wavenumber3");

		        unsigned int num_elements = data_traits<number, ValueType>::number_elements(Nfields);

		        unsigned int num_cols = std::min(num_elements, max_columns);
		        unsigned int page = id / num_cols;
		        unsigned int col  = id % num_cols;

		        std::string table_name = data_traits<number, ValueType>::sqlite_table();

				    // construct SQL query to pull relevant data
		        std::stringstream select_stmt;
		        select_stmt
			        << "SELECT"
			        << " " << table_name << ".ele" << col
			        << " FROM " << table_name
			        << " INNER JOIN (" << tquery.make_query(policy, true) << ") tsample"
			        << " ON " << table_name << ".tserial=tsample.serial"
			        << " WHERE " << table_name << ".kserial=" << k_serial << " AND " << table_name << ".page=" << page
			        << " ORDER BY tsample.serial;";

						sample.clear();
		        pull_implementation::pull_number_list(db, sample, select_stmt.str(), CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL);
			    }


        template <typename number, typename ValueType>
        void pull_paged_kconfig_sample(sqlite3* db, unsigned int id, const derived_data::SQL_query& kquery,
                                       unsigned int t_serial, std::vector<number>& sample, unsigned int worker, unsigned int Nfields)
	        {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            unsigned int num_elements = data_traits<number, ValueType>::number_elements(Nfields);

            unsigned int num_cols = std::min(num_elements, max_columns);
            unsigned int page = id / num_cols;
            unsigned int col  = id % num_cols;

            std::string table_name = data_traits<number, ValueType>::sqlite_table();

            // construct SQL query to pull relevant data
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT"
	            << " " << table_name << ".ele" << col
	            << " FROM " << table_name
	            << " INNER JOIN (" << kquery.make_query(policy, true) << ") ksample"
	            << " ON " << table_name << ".kserial=ksample.serial"
	            << " WHERE " << table_name << ".tserial=" << t_serial << " AND " << table_name << ".page=" << page
	            << " ORDER BY ksample.serial;";

            sample.clear();
            pull_implementation::pull_number_list(db, sample, select_stmt.str(), CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL);
	        }


        template <typename number, typename ValueType>
        void pull_unpaged_time_sample(sqlite3* db, const derived_data::SQL_query& tquery,
                                      unsigned int k_serial, std::vector<number>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            std::string table_name = data_traits<number, ValueType>::sqlite_table();

            // construct SQL query to pull relevant data
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT"
	            << " " << table_name << "." << data_traits<number, ValueType>::column_name()
	            << " FROM " << table_name
	            << " INNER JOIN (" << tquery.make_query(policy, true) << ") tsample"
	            << " ON " << table_name << ".tserial=tsample.serial"
	            << " WHERE " << table_name << ".kserial=" << k_serial
	            << " ORDER BY tsample.serial;";

            sample.clear();
            pull_implementation::pull_number_list(db, sample, select_stmt.str(), CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL);
	        }


        template <typename number, typename ValueType>
        void pull_unpaged_kconfig_sample(sqlite3* db, const derived_data::SQL_query& kquery,
                                         unsigned int t_serial, std::vector<number>& sample, unsigned int worker)
	        {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            std::string table_name = data_traits<number, ValueType>::sqlite_table();

            // construct SQL query to pull relevant data
            std::stringstream select_stmt;
            select_stmt
	            << "SELECT"
	            << " " << table_name << "." << data_traits<number, ValueType>::column_name()
	            << " FROM " << table_name
	            << " INNER JOIN (" << kquery.make_query(policy, true) << ") ksample"
	            << " ON " << table_name << ".kserial=ksample.serial"
	            << " WHERE " << table_name << ".tserial=" << t_serial
	            << " ORDER BY ksample.serial;";

            sample.clear();
            pull_implementation::pull_number_list(db, sample, select_stmt.str(), CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL);
	        }


        // Pull a sample of an fNL, for a specific set of time serial numbers
        template <typename number>
        void pull_fNL_time_sample(sqlite3* db, const derived_data::SQL_query& tquery,
                                  std::vector<number>& sample, unsigned int worker, derived_data::template_type type)
          {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            // pull out the fNL component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
              << "SELECT (5.0/3.0)*(" << fNL_table_name(type) << ".BT/" << fNL_table_name(type) << ".TT)"
              << " FROM " << fNL_table_name(type)
	            << " INNER JOIN (" << tquery.make_query(policy, true) << ") tsample"
	            << " ON " << fNL_table_name(type) << ".tserial=tsample.serial"
              << " ORDER BY tsample.serial;";

            sample.clear();
            pull_implementation::pull_number_list(db, sample, select_stmt.str(), CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL);
          }


        // Pull a sample of bispectrum.template, for a specific set of time serial numbers
        template <typename number>
        void pull_BT_time_sample(sqlite3* db, const derived_data::SQL_query& tquery,
                                 std::vector<number>& sample, unsigned int worker, derived_data::template_type type)
          {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            // pull out the bispectrum.template component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
              << "SELECT " << fNL_table_name(type) << ".BT"
              << " FROM " << fNL_table_name(type)
	            << " INNER JOIN (" << tquery.make_query(policy, true) << ") tsample"
	            << " ON " << fNL_table_name(type) << ".tserial=tsample.serial"
	            << " ORDER BY tsample.serial;";

            sample.clear();
            pull_implementation::pull_number_list(db, sample, select_stmt.str(), CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL);
          }


        // Pull a sample of template.template, for a specific set of time serial numbers
        template <typename number>
        void pull_TT_time_sample(sqlite3* db, const derived_data::SQL_query& tquery,
                                 std::vector<number>& sample, unsigned int worker, derived_data::template_type type)
          {
            assert(db != nullptr);

            derived_data::SQL_policy policy(CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE, "serial",
                                            CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE, "serial",
                                            "wavenumber1", "wavenumber2", "wavenumber3");

            // pull out the template.template component matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
              << "SELECT " << fNL_table_name(type) << ".TT"
              << " FROM " << fNL_table_name(type)
	            << " INNER JOIN (" << tquery.make_query(policy, true) << ") tsample"
	            << " ON " << fNL_table_name(type) << ".tserial=tsample.serial"
	            << " ORDER BY tsample.serial;";

            sample.clear();
            pull_implementation::pull_number_list(db, sample, select_stmt.str(), CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL);
          }


      }   // namespace sqlite3_operations

  }   // namespace transport

#endif //__data_manager_pull_H_
