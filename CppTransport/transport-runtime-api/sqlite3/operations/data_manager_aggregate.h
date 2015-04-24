//
// Created by David Seery on 13/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __data_manager_aggregate_H_
#define __data_manager_aggregate_H_


#include "transport-runtime-api/sqlite3/operations/data_manager_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {


        // Aggregate the background value table from a temporary container into a principal container
        template <typename number>
        void aggregate_backg(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
          {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating background values";

            // we use INSERT OR IGNORE for the background
            // if there are errors during an integration we roll back all twopf and threepf values associated with
            // the failed integration, but not background values.
            // if the failed container is later used to seed another integration it's possible that
            // duplicate background entries may be inserted, but we assume this to be harmless
            std::ostringstream copy_stmt;
            copy_stmt
              << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
              << " INSERT OR IGNORE INTO " << __CPP_TRANSPORT_SQLITE_BACKG_VALUE_TABLE
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


        // Aggregate an initial-conditions value table frmo a temporary container into a principal container
        template <typename number>
        void aggregate_ics(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
	        {
//            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Aggregating background values";

            std::ostringstream copy_stmt;
            copy_stmt
	            << "ATTACH DATABASE '" << temp_ctr << "' AS " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";"
	            << " INSERT INTO " << __CPP_TRANSPORT_SQLITE_ICS_TABLE
	            << " SELECT * FROM " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << __CPP_TRANSPORT_SQLITE_ICS_TABLE << ";"
	            << " DETACH DATABASE " << __CPP_TRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

//		        BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "   && Executing SQL statement: " << copy_stmt.str();

            exec(db, copy_stmt.str(), __CPP_TRANSPORT_DATACTR_ICS_COPY);
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


      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //__data_manager_aggregate_H_
