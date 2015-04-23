//
// Created by David Seery on 13/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __data_manager_pull_H_
#define __data_manager_pull_H_


#include "transport-runtime-api/sqlite3/operations/data_manager_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {

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

            for(unsigned int i = 0; i < serial_numbers.size(); ++i)
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


        void warn_time_serial_too_few(unsigned int expected, unsigned int received, std::string name)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_TOO_FEW_A << " " << expected << ", "
              << __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_TOO_FEW_B << " " << received
              << " [" << name << "]";
            throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
          }


        void warn_kconfig_serial_too_few(unsigned int expected, unsigned int received, std::string name)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_TOO_FEW_A << " " << expected << " "
              << __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_TOO_FEW_B << " " << received
              << " [" << name << "]";
            throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
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
            if(sample.size() != serial_numbers.size()) warn_time_serial_too_few(serial_numbers.size(), sample.size(), __func__);
          }


        // Pull a set of twopf k-configuration sample points, identified by their serial numbers
        template <typename number>
        void pull_twopf_kconfig_sample(sqlite3* db, const std::vector<unsigned int>& serial_numbers,
                                       std::vector<twopf_kconfig>& sample, unsigned int worker)
          {
            assert(db != nullptr);

            // set up a temporary table representing the serial numbers we want to use
            create_temporary_serial_table(db, serial_numbers, worker);

            // pull out the set of k-sample points matching serial numbers in the temporary table
            std::stringstream select_stmt;
            select_stmt
              << "SELECT " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << ".conventional AS conventional,"
              << " " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << ".comoving AS comoving,"
	            << " " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << ".t_exit AS t_exit"
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
                    twopf_kconfig value;

                    value.serial         = *t;
                    value.k_conventional = sqlite3_column_double(stmt, 0);
                    value.k_comoving     = sqlite3_column_double(stmt, 1);
		                value.t_exit         = sqlite3_column_double(stmt, 2);

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
            if(sample.size() != serial_numbers.size()) warn_kconfig_serial_too_few(serial_numbers.size(), sample.size(), __func__);
          }


        // Pull a set of threepf k-configuration sample points, identified by their serial numbers
        template <typename number>
        void pull_threepf_kconfig_sample(sqlite3* db, const std::vector<unsigned int>& serial_numbers,
                                         std::vector<threepf_kconfig>& sample, unsigned int worker)
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
                    threepf_kconfig value;

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
            if(sample.size() != serial_numbers.size()) warn_kconfig_serial_too_few(serial_numbers.size(), sample.size(), __func__);
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
            if(sample.size() != t_serials.size()) warn_time_serial_too_few(t_serials.size(), sample.size(), __func__);
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
            if(sample.size() != t_serials.size()) warn_time_serial_too_few(t_serials.size(), sample.size(), __func__);
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
            if(sample.size() != t_serials.size()) warn_time_serial_too_few(t_serials.size(), sample.size(), __func__);
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
            if(sample.size() != t_serials.size()) warn_time_serial_too_few(t_serials.size(), sample.size(), __func__);
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
            if(sample.size() != k_serials.size()) warn_kconfig_serial_too_few(k_serials.size(), sample.size(), __func__);
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
            if(sample.size() != k_serials.size()) warn_kconfig_serial_too_few(k_serials.size(), sample.size(), __func__);
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
            if(sample.size() != k_serials.size()) warn_kconfig_serial_too_few(k_serials.size(), sample.size(), __func__);
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
            if(sample.size() != t_serials.size()) warn_time_serial_too_few(t_serials.size(), sample.size(), __func__);
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
            if(sample.size() != t_serials.size()) warn_time_serial_too_few(t_serials.size(), sample.size(), __func__);
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
            if(sample.size() != t_serials.size()) warn_time_serial_too_few(t_serials.size(), sample.size(), __func__);
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
            if(sample.size() != t_serials.size()) warn_time_serial_too_few(t_serials.size(), sample.size(), __func__);
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
            if(sample.size() != t_serials.size()) warn_time_serial_too_few(t_serials.size(), sample.size(), __func__);
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
            if(sample.size() != t_serials.size()) warn_time_serial_too_few(t_serials.size(), sample.size(), __func__);
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
            if(sample.size() != k_serials.size()) warn_kconfig_serial_too_few(k_serials.size(), sample.size(), __func__);
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
            if(sample.size() != k_serials.size()) warn_kconfig_serial_too_few(k_serials.size(), sample.size(), __func__);
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
            if(sample.size() != k_serials.size()) warn_kconfig_serial_too_few(k_serials.size(), sample.size(), __func__);
          }


      }   // namespace sqlite3_operations

  }   // namespace transport

#endif //__data_manager_pull_H_
