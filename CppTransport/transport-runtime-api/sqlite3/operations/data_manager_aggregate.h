//
// Created by David Seery on 13/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_DATA_MANAGER_AGGREGATE_H
#define CPPTRANSPORT_DATA_MANAGER_AGGREGATE_H


#include "transport-runtime-api/sqlite3/operations/data_manager_common.h"
#include "transport-runtime-api/sqlite3/operations/data_traits.h"


namespace transport
  {

    namespace sqlite3_operations
      {


        namespace aggregation_impl
          {

            //! RAII-type manager for attaching/detaching temporary databases
            class attach_manager
              {

                // CONSTRUCTOR, DESTRUCTOR

              public:

                //! attach a temporary database
                attach_manager(sqlite3* db, const std::string& p);

                //! detach temporary database
                ~attach_manager();


                // INTERFACE

              public:

                void commit();


                // INTERNAL DATA

              private:

                //! sqlite3 handle
                sqlite3* handle;

                //! path to database
                const std::string& path;

                //! has attachment succeeded?
                bool attached;

                //! have we been committed?
                bool committed;

              };


            attach_manager::attach_manager(sqlite3* db, const std::string& p)
              : handle(db),
                path(p),
                attached(false),
                committed(false)
              {
                assert(db != nullptr);

                std::ostringstream attach_stmt;
                attach_stmt
                  << "ATTACH DATABASE '" << path << "' AS " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "; BEGIN TRANSACTION;";

                exec(handle, attach_stmt.str(), CPPTRANSPORT_DATACTR_ATTACH_FAIL);
                attached = true;
              }


            attach_manager::~attach_manager()
              {
                if(!this->attached) return;

                std::ostringstream detach_stmt;

                if(this->committed) detach_stmt << "COMMIT; ";
                else                detach_stmt << "ROLLBACK; ";

                detach_stmt << "DETACH DATABASE " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << ";";

                exec(this->handle, detach_stmt.str(), CPPTRANSPORT_DATACTR_DETACH_FAIL);
              }


            void attach_manager::commit()
              {
                if(this->attached) this->committed = true;
              }

          }


        // Aggregate the background value table from a temporary container into a principal container
        template <typename number>
        void aggregate_backg(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
          {
            // we use INSERT OR IGNORE for the background
            // if there are errors during an integration we roll back all twopf and threepf values associated with
            // the failed integration, but not background values (the user may want these!)

            // if the failed container is later used to seed another integration it's possible that
            // duplicate background entries may be inserted, but we assume this to be harmless
            aggregation_impl::attach_manager mgr(db, temp_ctr);

            std::ostringstream copy_stmt;
            copy_stmt
              << "INSERT OR IGNORE INTO " << CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE
              << " SELECT * FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE << ";";

            exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_BACKGROUND_COPY);
            mgr.commit();
          }


		    template <typename number, typename WriterObject, typename ValueType>
		    void aggregate_table(sqlite3* db, WriterObject& writer, const std::string& temp_ctr)
			    {
            aggregation_impl::attach_manager mgr(db, temp_ctr);

		        std::ostringstream copy_stmt;
				    copy_stmt
				      << "INSERT INTO " << data_traits<number, ValueType>::sqlite_table()
			        << " SELECT * FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << data_traits<number, ValueType>::sqlite_table() << ";";

				    exec(db, copy_stmt.str(), data_traits<number, ValueType>::copy_error_msg());
            mgr.commit();
			    }


        // Aggregate an fNL value table from a temporary container
        // Aggregation of fNL values is slightly different, because if an existing result is present for for
        // some time serial tserial, we want to add our new value to it.
        // For that purpose we use COALESCE.
        template <typename number>
        void aggregate_fNL(sqlite3* db, postintegration_writer<number>& writer, const std::string& temp_ctr, derived_data::template_type type)
          {
            aggregation_impl::attach_manager mgr(db, temp_ctr);

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
              << " INSERT OR REPLACE INTO main." << fNL_table_name(type)
              << " SELECT tserial AS tserial,"
              << " new_BB AS BB,"
              << " new_BT AS BT,"
              << " new_TT AS TT"
              << " FROM temp." << CPPTRANSPORT_SQLITE_TEMP_FNL_TABLE << ";";

            exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_FNL_COPY);

            std::stringstream drop_stmt;
            drop_stmt << "DROP TABLE temp." << CPPTRANSPORT_SQLITE_TEMP_FNL_TABLE << ";";
            exec(db, drop_stmt.str(), CPPTRANSPORT_DATACTR_FNL_COPY);

            mgr.commit();
          }


        // Aggregate a worker info table from a temporary container into the principal container
        template <typename number>
        void aggregate_workers(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
	        {
            aggregation_impl::attach_manager mgr(db, temp_ctr);

            std::ostringstream copy_stmt;
            copy_stmt
	            << "INSERT OR IGNORE INTO " << CPPTRANSPORT_SQLITE_WORKERS_TABLE
	            << " SELECT * FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << CPPTRANSPORT_SQLITE_WORKERS_TABLE << ";";

            exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_WORKERS_COPY);
            mgr.commit();
	        }


        // Aggregate a statistics value table from a temporary container into the principal container
        template <typename number>
        void aggregate_statistics(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
	        {
            aggregation_impl::attach_manager mgr(db, temp_ctr);

            std::ostringstream copy_stmt;
            copy_stmt
	            << "INSERT INTO " << CPPTRANSPORT_SQLITE_STATS_TABLE
	            << " SELECT * FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << CPPTRANSPORT_SQLITE_STATS_TABLE << ";";

            exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_STATISTICS_COPY);
            mgr.commit();
	        }


        // Aggregate an initial-conditions value table frmo a temporary container into a principal container
        template <typename number, typename ValueType>
        void aggregate_ics(sqlite3* db, integration_writer<number>& writer, const std::string& temp_ctr)
	        {
            aggregation_impl::attach_manager mgr(db, temp_ctr);

            std::ostringstream copy_stmt;
            copy_stmt
	            << "INSERT INTO " << data_traits<number, ValueType>::sqlite_table()
	            << " SELECT * FROM " << CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME << "." << data_traits<number, ValueType>::sqlite_table() << ";";

            exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_ICS_COPY);
            mgr.commit();
	        }


      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_AGGREGATE_H
