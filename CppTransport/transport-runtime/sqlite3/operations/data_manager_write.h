//
// Created by David Seery on 08/01/2014.
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


#ifndef CPPTRANSPORT_DATA_MANAGER_WRITE_H
#define CPPTRANSPORT_DATA_MANAGER_WRITE_H


#include "transport-runtime/sqlite3/operations/data_manager_common.h"
#include "transport-runtime/sqlite3/operations/data_traits.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        namespace data_manager_write_impl
          {

            template <typename ValueType>
            class UnpagedPrimaryKeyCompare
              {
              public:
                bool operator()(const std::unique_ptr<ValueType>& A, const std::unique_ptr<ValueType>& B)
                {
                  // for unpaged objects we can just compare unique ID
                  return A->get_unique() < B->get_unique();
                }
              };


            template <typename ValueType>
            class PagedPrimaryKeyCompare
              {
              public:
                bool operator()(const std::unique_ptr<ValueType>& A, const std::unique_ptr<ValueType>& B)
                  {
                    // detect kconfig_serial and time_serial ordering by pretending to be page 0 of a 1 page group
                    // because rows are ordered by page in multipage groups, this will get all rows in
                    // ascending primary key order
                    return A->get_unique(0,1) < B->get_unique(0,1);
                  }
              };


            template <typename number>
            class StatisticsPrimaryKeyCompare
              {
              public:
                bool operator()(const std::unique_ptr<typename integration_items<number>::configuration_statistics>& A, const std::unique_ptr<typename integration_items<number>::configuration_statistics>& B)
                  {
                    return A->serial < B->serial;
                  }
              };

          }

		    // Write host information
		    template <typename number>
		    void write_host_info(transaction_manager& mgr, integration_batcher<number>* batcher)
			    {
				    sqlite3* db = nullptr;
				    batcher->get_manager_handle(&db);

				    const host_information& host = batcher->get_host_information();

		        std::ostringstream insert_stmt;
				    insert_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_WORKERS_TABLE << " VALUES (@workgroup, @worker, @backend, @back_stepper, @pert_stepper, @back_abs_tol, @back_rel_tol, @pert_abs_tol, @pert_rel_tol, @hostname, @os_name, @os_version, @os_release, @architecture, @cpu_vendor_id)";

				    sqlite3_stmt* stmt;
				    check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

				    // to document the different choice of length in these sqlite3_bind_text() statements compared to sqlite3_prepare_v2,
				    // the SQLite3 documentation says:

		        // If the caller knows that the supplied string is nul-terminated, then there is a small performance advantage to be
		        // gained by passing an nByte parameter that is equal to the number of bytes in the input string including the nul-terminator
		        // bytes as this saves SQLite from having to make a copy of the input string.

				    // However, here, we certainly *shouldn't* include the null-terminator byte, because that isn't part of the string

		        check_stmt(db, sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@workgroup"), batcher->get_worker_group()));
		        check_stmt(db, sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@worker"), batcher->get_worker_number()));
		        check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@backend"), batcher->get_backend().c_str(), batcher->get_backend().length(), SQLITE_STATIC));

		        check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@back_stepper"), batcher->get_back_stepper().c_str(), batcher->get_back_stepper().length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@pert_stepper"), batcher->get_pert_stepper().c_str(), batcher->get_pert_stepper().length(), SQLITE_STATIC));

            std::pair< double, double > backg_tol = batcher->get_back_tol();
            check_stmt(db, sqlite3_bind_double(stmt, sqlite3_bind_parameter_index(stmt, "@back_abs_tol"), backg_tol.first));
            check_stmt(db, sqlite3_bind_double(stmt, sqlite3_bind_parameter_index(stmt, "@back_rel_tol"), backg_tol.second));

            std::pair< double, double > pert_tol = batcher->get_pert_tol();
            check_stmt(db, sqlite3_bind_double(stmt, sqlite3_bind_parameter_index(stmt, "@pert_abs_tol"), pert_tol.first));
            check_stmt(db, sqlite3_bind_double(stmt, sqlite3_bind_parameter_index(stmt, "@pert_rel_tol"), pert_tol.second));

		        check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@hostname"), host.get_host_name().c_str(), host.get_host_name().length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@os_name"), host.get_os_name().c_str(), host.get_os_name().length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@os_version"), host.get_os_version().c_str(), host.get_os_version().length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@os_release"), host.get_os_release().c_str(), host.get_os_release().length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@architecture"), host.get_architecture().c_str(), host.get_architecture().length(), SQLITE_STATIC));
		        check_stmt(db, sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@cpu_vendor_id"), host.get_cpu_vendor_id().c_str(), host.get_cpu_vendor_id().length(), SQLITE_STATIC));

				    check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_DATACTR_WORKER_INSERT_FAIL, SQLITE_DONE);

				    check_stmt(db, sqlite3_clear_bindings(stmt));
				    check_stmt(db, sqlite3_finalize(stmt));
			    }


        // Write a batch of per-configuration statistics values
        template <typename number>
        void write_stats(transaction_manager& mgr, integration_batcher<number>* batcher, std::vector< std::unique_ptr< typename integration_items<number>::configuration_statistics > >& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << CPPTRANSPORT_SQLITE_STATS_TABLE << " VALUES (@kserial, @integration_time, @batch_time, @steps, @refinements, @workgroup, @worker);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            const int kserial_id = sqlite3_bind_parameter_index(stmt, "@kserial");
            const int integration_time_id = sqlite3_bind_parameter_index(stmt, "@integration_time");
            const int batch_time_id = sqlite3_bind_parameter_index(stmt, "@batch_time");
            const int steps_id = sqlite3_bind_parameter_index(stmt, "@steps");
            const int refinements_id = sqlite3_bind_parameter_index(stmt, "@refinements");
            const int workgroup_id = sqlite3_bind_parameter_index(stmt, "@workgroup");
            const int worker_id = sqlite3_bind_parameter_index(stmt, "@worker");

            // sort batch into ascending primary key order;
            // sorting is done in-place for performance
            std::sort(batch.begin(), batch.end(), data_manager_write_impl::StatisticsPrimaryKeyCompare<number>());

            for(const std::unique_ptr< typename integration_items<number>::configuration_statistics >& item : batch)
              {
                check_stmt(db, sqlite3_bind_int(stmt, kserial_id, item->serial));
                check_stmt(db, sqlite3_bind_int64(stmt, integration_time_id, item->integration));
                check_stmt(db, sqlite3_bind_int64(stmt, batch_time_id, item->batching));
                check_stmt(db, sqlite3_bind_int(stmt, steps_id, item->steps));
		            check_stmt(db, sqlite3_bind_int(stmt, refinements_id, item->refinements));
		            check_stmt(db, sqlite3_bind_int(stmt, workgroup_id, batcher->get_worker_group()));
                check_stmt(db, sqlite3_bind_int(stmt, worker_id, batcher->get_worker_number()));

                check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_DATACTR_STATS_INSERT_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

            check_stmt(db, sqlite3_finalize(stmt));
          }


        template <typename number, typename ValueType>
        void write_coordinate_output(transaction_manager& mgr, integration_batcher<number>* batcher, std::vector< std::unique_ptr<ValueType> >& batch)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            unsigned int Nfields = batcher->get_number_fields();

            // work out how many columns we have, and how many pages
            // we need to fit in our number of columns.
            // num_pages will be 1 or greater
		        unsigned int num_elements = data_traits<number, ValueType>::number_elements(Nfields);

		        unsigned int num_cols = std::min(2*Nfields, max_columns);
            unsigned int num_pages = (2*Nfields-1)/num_cols + 1;

            std::ostringstream insert_stmt;
            insert_stmt
	            << "INSERT INTO " << data_traits<number, ValueType>::sqlite_table()
	            << " VALUES (@" << data_traits<number, ValueType>::sqlite_unique_column() << ", @" << data_traits<number, ValueType>::sqlite_serial_column() << ", @page";

		        if(data_traits<number, ValueType>::has_texit) insert_stmt << ", @t_exit";

            std::vector<std::string> coord_names(num_cols);
            for(unsigned int i = 0; i < num_cols; ++i)
              {
                std::string id = std::string("@coord") + boost::lexical_cast<std::string>(i);
                coord_names[i] = id;
                insert_stmt << ", " << id;
              }
            insert_stmt << ");";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            const int unique_id = sqlite3_bind_parameter_index(stmt, (std::string("@") + data_traits<number, ValueType>::sqlite_unique_column()).c_str());
            const int serial_id = sqlite3_bind_parameter_index(stmt, (std::string("@") + data_traits<number, ValueType>::sqlite_serial_column()).c_str());
            const int page_id   = sqlite3_bind_parameter_index(stmt, "@page");

            std::vector<int> coord_ids(num_cols);
            for(unsigned int i = 0; i < num_cols; ++i)
              {
                coord_ids[i] = sqlite3_bind_parameter_index(stmt, coord_names[i].c_str());
              }

            // sort batch into ascending primary key order;
            // sorting is done in-place for performance
            std::sort(batch.begin(), batch.end(), data_manager_write_impl::PagedPrimaryKeyCompare<ValueType>());

            for(const std::unique_ptr<ValueType>& item : batch)
              {
                for(unsigned int page = 0; page < num_pages; ++page)
	                {

                    check_stmt(db, sqlite3_bind_int64(stmt, unique_id, item->get_unique(page, num_pages)));
                    check_stmt(db, sqlite3_bind_int(stmt, serial_id, item->get_serial()));
		                check_stmt(db, sqlite3_bind_int(stmt, page_id, page));

		                if(data_traits<number, ValueType>::has_texit)
                      {
                        const int texit_id = sqlite3_bind_parameter_index(stmt, "@t_exit");
                        check_stmt(db, sqlite3_bind_double(stmt, texit_id, item->get_texit()));
                      }

		                for(unsigned int i = 0; i < num_cols; ++i)
			                {
				                unsigned int index = page*num_cols + i;
				                number       value = index < 2*Nfields ? item->coords[index] : 0.0;

		                    check_stmt(db, sqlite3_bind_double(stmt, coord_ids[i], static_cast<double>(value)));    // 'number' must be castable to double
			                }

		                check_stmt(db, sqlite3_step(stmt), data_traits<number, ValueType>::write_error_msg(), SQLITE_DONE);

		                check_stmt(db, sqlite3_clear_bindings(stmt));
		                check_stmt(db, sqlite3_reset(stmt));
	                }
              }

            check_stmt(db, sqlite3_finalize(stmt));
          }


		    template <typename number, typename BatcherType, typename ValueType>
		    void write_paged_output(transaction_manager& mgr, BatcherType* batcher, std::vector< std::unique_ptr<ValueType> >& batch)
			    {
				    sqlite3* db = nullptr;
				    batcher->get_manager_handle(&db);

				    unsigned int Nfields = batcher->get_number_fields();

		        // work out how many columns we have, and how many pages
		        // we need to fit in our number of columns.
		        // num_pages will be 1 or greater
				    unsigned int num_elements = data_traits<number, ValueType>::number_elements(Nfields);

		        unsigned int num_cols = std::min(num_elements, max_columns);
		        unsigned int num_pages = (num_elements - 1)/num_cols + 1;

		        std::ostringstream insert_stmt;
		        insert_stmt << "INSERT INTO " << data_traits<number, ValueType>::sqlite_table()
              << " VALUES (" << "@" << data_traits<number, ValueType>::sqlite_unique_column()
              << ", @tserial, @kserial, @page";

            std::vector<std::string> ele_names(num_cols);
		        for(unsigned int i = 0; i < num_cols; ++i)
			        {
                std::string id = std::string("@ele") + boost::lexical_cast<std::string>(i);
                ele_names[i] = id;
                insert_stmt << ", " << id;
			        }
		        insert_stmt << ");";

		        sqlite3_stmt* stmt;
		        check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            const int unique_id  = sqlite3_bind_parameter_index(stmt, (std::string("@") + data_traits<number, ValueType>::sqlite_unique_column()).c_str());
            const int tserial_id = sqlite3_bind_parameter_index(stmt, "@tserial");
            const int kserial_id = sqlite3_bind_parameter_index(stmt, "@kserial");
            const int page_id    = sqlite3_bind_parameter_index(stmt, "@page");

            std::vector<int> ele_ids(num_cols);
            for(unsigned int i = 0; i < num_cols; ++i)
              {
                ele_ids[i] = sqlite3_bind_parameter_index(stmt, ele_names[i].c_str());
              }

            // sort batch into ascending primary key order;
            // sorting is done in-place for performance
            std::sort(batch.begin(), batch.end(), data_manager_write_impl::PagedPrimaryKeyCompare<ValueType>());

            for(const std::unique_ptr<ValueType>& item : batch)
			        {
		            for(unsigned int page = 0; page < num_pages; ++page)
			            {
                    check_stmt(db, sqlite3_bind_int64(stmt, unique_id, item->get_unique(page, num_pages)));
		                check_stmt(db, sqlite3_bind_int(stmt, tserial_id, item->time_serial));
		                check_stmt(db, sqlite3_bind_int(stmt, kserial_id, item->kconfig_serial));
		                check_stmt(db, sqlite3_bind_int(stmt, page_id, page));

		                for(unsigned int i = 0; i < num_cols; ++i)
			                {
		                    unsigned int index = page*num_cols + i;
		                    number       value = index < num_elements ? item->elements[index] : 0.0;

		                    check_stmt(db, sqlite3_bind_double(stmt, ele_ids[i], static_cast<double>(value)));    // 'number' must be castable to double
			                }

		                check_stmt(db, sqlite3_step(stmt), data_traits<number, ValueType>::write_error_msg(), SQLITE_DONE);

		                check_stmt(db, sqlite3_clear_bindings(stmt));
		                check_stmt(db, sqlite3_reset(stmt));
			            }
			        }

		        check_stmt(db, sqlite3_finalize(stmt));
			    }


        // Write a batch of unpaged values
        template <typename number, typename BatcherType, typename ValueType >
        void write_unpaged(transaction_manager& mgr, BatcherType* batcher, std::vector< std::unique_ptr<ValueType> >& batch)
	        {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO " << data_traits<number, ValueType>::sqlite_table()
              << " VALUES (@" << data_traits<number, ValueType>::sqlite_unique_column()
              << ", @tserial, @kserial, @" << data_traits<number, ValueType>::column_name();

            if(data_traits<number, ValueType>::has_redbsp) insert_stmt << ", @redbsp";
            insert_stmt << ");";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            const int unique_id  = sqlite3_bind_parameter_index(stmt, (std::string("@") + data_traits<number, ValueType>::sqlite_unique_column()).c_str());
            const int tserial_id = sqlite3_bind_parameter_index(stmt, "@tserial");
            const int kserial_id = sqlite3_bind_parameter_index(stmt, "@kserial");
            const int column_id  = sqlite3_bind_parameter_index(stmt, (std::string("@") + data_traits<number, ValueType>::column_name()).c_str());
            const int redbsp_id  = data_traits<number, ValueType>::has_redbsp ? sqlite3_bind_parameter_index(stmt, "@redbsp") : 0;

            // sort batch into ascending primary key order;
            // sorting is done in-place for performance
            std::sort(batch.begin(), batch.end(), data_manager_write_impl::UnpagedPrimaryKeyCompare<ValueType>());

            for(const std::unique_ptr<ValueType>& item : batch)
	            {
                check_stmt(db, sqlite3_bind_int64(stmt, unique_id, item->get_unique()));
                check_stmt(db, sqlite3_bind_int(stmt, tserial_id, item->time_serial));
                check_stmt(db, sqlite3_bind_int(stmt, kserial_id, item->kconfig_serial));
                check_stmt(db, sqlite3_bind_double(stmt, column_id, static_cast<double>(item->value)));
                if(data_traits<number, ValueType>::has_redbsp)
                  {
                    check_stmt(db, sqlite3_bind_double(stmt, redbsp_id, static_cast<double>(item->redbsp)));
                  }

                check_stmt(db, sqlite3_step(stmt), data_traits<number, ValueType>::write_error_msg(), SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
	            }

            check_stmt(db, sqlite3_finalize(stmt));
	        }


        // Write a batch of fNL values.
		    // The semantics here are slightly different. If an existing value is already present for a given
		    // tserial then we want to add our new value to it.
		    // For that purpose we use COALESCE.
        template <typename number>
        void write_fNL(transaction_manager& mgr, postintegration_batcher<number>* batcher,
                       const typename postintegration_items<number>::fNL_cache& batch, derived_data::bispectrum_template type)
          {
            sqlite3* db = nullptr;
            batcher->get_manager_handle(&db);

		        // first, inject all new BT and TT values into a temporary table
            std::stringstream create_stmt;
		        create_stmt << "CREATE TEMP TABLE " << CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE << " ("
			        << "tserial INTEGER, "
              << "BB      DOUBLE, "
		          << "BT      DOUBLE, "
			        << "TT      DOUBLE, "
		          << "PRIMARY KEY (tserial)"
		          << ");";
		        exec(db, create_stmt.str(), CPPTRANSPORT_DATACTR_FNL_DATATAB_FAIL);

            std::ostringstream insert_stmt;
            insert_stmt << "INSERT INTO temp." << CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE << " VALUES (@tserial, @BB, @BT, @TT);";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

            const int tserial_id = sqlite3_bind_parameter_index(stmt, "@tserial");
            const int BB_id      = sqlite3_bind_parameter_index(stmt, "@BB");
            const int BT_id      = sqlite3_bind_parameter_index(stmt, "@BT");
            const int TT_id      = sqlite3_bind_parameter_index(stmt, "@TT");

            // cache will already be sorted in ascending order of primary key = time_serial
            for(const typename postintegration_items<number>::fNL_cache::value_type& item : batch)
              {
                check_stmt(db, sqlite3_bind_int(stmt, tserial_id, item->time_serial));
                check_stmt(db, sqlite3_bind_double(stmt, BB_id, static_cast<double>(item->BB)));
                check_stmt(db, sqlite3_bind_double(stmt, BT_id, static_cast<double>(item->BT)));
                check_stmt(db, sqlite3_bind_double(stmt, TT_id, static_cast<double>(item->TT)));

                check_stmt(db, sqlite3_step(stmt), CPPTRANSPORT_DATACTR_FNL_DATATAB_FAIL, SQLITE_DONE);

                check_stmt(db, sqlite3_clear_bindings(stmt));
                check_stmt(db, sqlite3_reset(stmt));
              }

		        // now create a second temporary table which merges results from the existing fNL table
		        // and the new one we have just constructed
            std::stringstream create_stmt2;
		        create_stmt2 << "CREATE TEMP TABLE " << CPPTRANSPORT_SQLITE_TEMP_FNL_TABLE << " AS"
		          << " SELECT temp." << CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE << ".tserial AS tserial,"
              << " temp." << CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE << ".BB + COALESCE(" << fNL_table_name(type) << ".BB, 0.0) AS new_BB,"
		          << " temp." << CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE << ".BT + COALESCE(" << fNL_table_name(type) << ".BT, 0.0) AS new_BT,"
		          << " temp." << CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE << ".TT + COALESCE(" << fNL_table_name(type) << ".TT, 0.0) AS new_TT"
		          << " FROM temp." << CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE << " LEFT JOIN " << fNL_table_name(type)
		          << " ON temp." << CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE << ".tserial=" << fNL_table_name(type) << ".tserial;";
		        exec(db, create_stmt2.str(), CPPTRANSPORT_DATACTR_FNL_DATATAB_FAIL);

		        // finally, copy or update values back into the main table
            std::stringstream copy_stmt;
		        copy_stmt
		          << " INSERT OR REPLACE INTO " << fNL_table_name(type)
			        << " SELECT tserial AS tserial,"
              << " new_BB AS BB,"
		          << " new_BT AS BT,"
			        << " new_TT AS TT"
		          << " FROM temp." << CPPTRANSPORT_SQLITE_TEMP_FNL_TABLE << ";";
		        exec(db, copy_stmt.str(), CPPTRANSPORT_DATACTR_FNL_DATATAB_FAIL);

            std::stringstream drop_stmt;
		        drop_stmt << "DROP TABLE temp." << CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE << ";"
		          << " DROP TABLE temp." << CPPTRANSPORT_SQLITE_TEMP_FNL_TABLE << ";";
		        exec(db, drop_stmt.str(), CPPTRANSPORT_DATACTR_FNL_DATATAB_FAIL);

            check_stmt(db, sqlite3_finalize(stmt));
          }

	    }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_WRITE_H
