//
// Created by David Seery on 13/04/15.
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


#ifndef CPPTRANSPORT_DATA_MANAGER_AGGREGATE_H
#define CPPTRANSPORT_DATA_MANAGER_AGGREGATE_H


#include "transport-runtime/sqlite3/operations/data_manager_common.h"
#include "transport-runtime/sqlite3/operations/data_traits.h"

#include "transport-runtime/repository/writers/aggregation_profiler.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        //! ATTACH does not work inside transactions, so aggregations have to be separate
        //! from the normal transaction system
        //! attach_manager is a mini-transaction handler which controls attachment and
        //! transaction management purely for aggregation tasks
        //! We have to rely on failures during aggregations being caught by the integrity check
        class attach_manager
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! attach a temporary database
            attach_manager(sqlite3* db, const boost::filesystem::path& p, boost::optional< aggregation_profile_record& > rec = boost::none);

            //! detach temporary database
            ~attach_manager();


            // INTERFACE

          public:

            //! commit transaction
            void commit();

            //! get embedded database connexion
            sqlite3* get_db_connexion() { return(this->handle); }


            // INTERNAL API

          private:

            //! detach database
            void detach(std::string cmd);


            // INTERNAL DATA

          private:

            //! sqlite3 handle
            sqlite3* handle;

            //! path to database
            const boost::filesystem::path& path;

            //! has attachment succeeded?
            bool attached;

            //! have we been committed?
            bool committed;


            // PROFILING SUPPORT

            //! aggregation profiling record associated with this manager
            boost::optional< aggregation_profile_record& > record;

          };


        attach_manager::attach_manager(sqlite3* db, const boost::filesystem::path& p, boost::optional< aggregation_profile_record& > rec)
          : handle(db),
            path(p),
            attached(false),
            committed(false),
            record(rec)
          {
            assert(db != nullptr);

            boost::timer::cpu_timer timer;

            std::ostringstream attach_stmt;
            attach_stmt
              << "ATTACH DATABASE '" << path.string() << "' AS " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "; BEGIN TRANSACTION;";

            exec(handle, attach_stmt.str(), CPPTRANSPORT_DATACTR_ATTACH_FAIL);
            attached = true;

            timer.stop();
            if(record) record->attach_time = timer.elapsed().wall;
          }


        attach_manager::~attach_manager()
          {
            if(!this->attached || this->committed) return;
            this->detach("ROLLBACK");
          }


        void attach_manager::commit()
          {
            if(this->attached && !this->committed)
              {
                this->detach("COMMIT");
                this->committed = true;
              }
          }


        void attach_manager::detach(std::string cmd)
          {
            boost::timer::cpu_timer timer;
            std::ostringstream detach_stmt;

            detach_stmt << cmd << "; ";
            detach_stmt << "DETACH DATABASE " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

            exec(this->handle, detach_stmt.str(), CPPTRANSPORT_DATACTR_DETACH_FAIL);

            timer.stop();
            if(this->record) this->record->detach_time = timer.elapsed().wall;
          }


        // Aggregate the background value table from a temporary container into a principal container
        template <typename number>
        aggregation_table_data aggregate_backg(attach_manager& mgr, integration_writer<number>& writer)
          {
            boost::timer::cpu_timer timer;
            sqlite3* db = mgr.get_db_connexion();

            // we use INSERT OR IGNORE for the background
            // if there are errors during an integration we roll back all twopf and threepf values associated with
            // the failed integration, but not background values (the user may want these!)

            // if the failed container is later used to seed another integration it's possible that
            // duplicate background entries may be inserted, but we assume this to be harmless
            std::ostringstream copy_stmt;
            copy_stmt
              << "INSERT OR IGNORE INTO " << CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE
              << " SELECT * FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE << ";";

            exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_BACKGROUND_COPY);

            timer.stop();
            return {timer.elapsed().wall, static_cast<size_t>(sqlite3_changes(db))};
          }


		    template <typename number, typename WriterObject, typename ValueType>
		    aggregation_table_data aggregate_table(attach_manager& mgr, WriterObject& writer)
			    {
            boost::timer::cpu_timer timer;
            sqlite3* db = mgr.get_db_connexion();

            std::ostringstream copy_stmt;
				    copy_stmt
				      << "INSERT INTO " << data_traits<number, ValueType>::sqlite_table()
			        << " SELECT * FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << data_traits<number, ValueType>::sqlite_table() << ";";

				    exec(db, copy_stmt.str(), data_traits<number, ValueType>::copy_error_msg());

            timer.stop();
            return {timer.elapsed().wall, static_cast<size_t>(sqlite3_changes(db))};
			    }


        // Aggregate an fNL value table from a temporary container
        // Aggregation of fNL values is slightly different, because if an existing result is present for
        // some time serial tserial, we want to add our new value to it.
        // For that purpose we use COALESCE.
        template <typename number>
        aggregation_table_data aggregate_fNL(attach_manager& mgr, postintegration_writer<number>& writer, derived_data::bispectrum_template type)
          {
            boost::timer::cpu_timer timer;
            sqlite3* db = mgr.get_db_connexion();

            // create a temporary table with updated values
            std::stringstream create_stmt;
            create_stmt
              << "CREATE TEMP TABLE " << CPPTRANSPORT_SQLITE_TEMP_FNL_TABLE << " AS"
              << " SELECT " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type) << ".tserial AS tserial,"
              << " " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type) << ".BB + COALESCE(main." << fNL_table_name(type) << ".BB, 0.0) AS new_BB,"
              << " " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type) << ".BT + COALESCE(main." << fNL_table_name(type) << ".BT, 0.0) AS new_BT,"
              << " " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type) << ".TT + COALESCE(main." << fNL_table_name(type) << ".TT, 0.0) AS new_TT"
              << " FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type)
              << " LEFT JOIN " << fNL_table_name(type)
              << " ON " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << fNL_table_name(type) << ".tserial=main." << fNL_table_name(type) << ".tserial;";

            exec(db, create_stmt.str(), CPPTRANSPORT_DATACTR_FNL_COPY);

            std::ostringstream copy_stmt;
            copy_stmt
              << "INSERT OR REPLACE INTO main." << fNL_table_name(type)
              << " SELECT tserial AS tserial,"
              << " new_BB AS BB,"
              << " new_BT AS BT,"
              << " new_TT AS TT"
              << " FROM temp." << CPPTRANSPORT_SQLITE_TEMP_FNL_TABLE << ";";

            exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_FNL_COPY);
            size_t changes = static_cast<size_t>(sqlite3_changes(db));

            std::stringstream drop_stmt;
            drop_stmt << "DROP TABLE temp." << CPPTRANSPORT_SQLITE_TEMP_FNL_TABLE << ";";
            exec(db, drop_stmt.str(), CPPTRANSPORT_DATACTR_FNL_COPY);

            timer.stop();
            return {timer.elapsed().wall, changes};
          }


        // Aggregate a worker info table from a temporary container into the principal container
        template <typename number>
        aggregation_table_data aggregate_workers(attach_manager& mgr, integration_writer<number>& writer)
	        {
            boost::timer::cpu_timer timer;
            sqlite3* db = mgr.get_db_connexion();

            std::ostringstream copy_stmt;
            copy_stmt
	            << "INSERT OR IGNORE INTO " << CPPTRANSPORT_SQLITE_WORKERS_TABLE
	            << " SELECT * FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << CPPTRANSPORT_SQLITE_WORKERS_TABLE << ";";

            exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_WORKERS_COPY);

            timer.stop();
            return {timer.elapsed().wall, static_cast<size_t>(sqlite3_changes(db))};
	        }


        // Aggregate a statistics value table from a temporary container into the principal container
        template <typename number>
        aggregation_table_data aggregate_statistics(attach_manager& mgr, integration_writer<number>& writer)
	        {
            boost::timer::cpu_timer timer;
            sqlite3* db = mgr.get_db_connexion();

            std::ostringstream copy_stmt;
            copy_stmt
	            << "INSERT INTO " << CPPTRANSPORT_SQLITE_STATS_TABLE
	            << " SELECT * FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << CPPTRANSPORT_SQLITE_STATS_TABLE << ";";

            exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_STATISTICS_COPY);

            timer.stop();
            return {timer.elapsed().wall, static_cast<size_t>(sqlite3_changes(db))};
	        }


        // Aggregate an initial-conditions value table frmo a temporary container into a principal container
        template <typename number, typename ValueType>
        aggregation_table_data aggregate_ics(attach_manager& mgr, integration_writer<number>& writer)
	        {
            boost::timer::cpu_timer timer;
            sqlite3* db = mgr.get_db_connexion();

            std::ostringstream copy_stmt;
            copy_stmt
	            << "INSERT INTO " << data_traits<number, ValueType>::sqlite_table()
	            << " SELECT * FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << data_traits<number, ValueType>::sqlite_table() << ";";

            exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_ICS_COPY);

            timer.stop();
            return {timer.elapsed().wall, static_cast<size_t>(sqlite3_changes(db))};
	        }


      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_AGGREGATE_H
