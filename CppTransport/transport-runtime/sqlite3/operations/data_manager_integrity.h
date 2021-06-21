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


#ifndef CPPTRANSPORT_DATA_MANAGER_INTEGRITY_H
#define CPPTRANSPORT_DATA_MANAGER_INTEGRITY_H


#include "transport-runtime/sqlite3/operations/data_manager_common.h"
#include "transport-runtime/sqlite3/operations/data_traits.h"


namespace transport
	{

    namespace sqlite3_operations
	    {

        namespace integrity_detail
	        {

            serial_number_list get_serials(sqlite3* db, std::string sql_query)
	            {
                sqlite3_stmt* stmt;
                check_stmt(db, sqlite3_prepare_v2(db, sql_query.c_str(), sql_query.length() + 1, &stmt, nullptr));

                serial_number_list serials;
                int                    status;
                while((status = sqlite3_step(stmt)) != SQLITE_DONE)
	                {
                    if(status == SQLITE_ROW)
	                    {
                        unsigned int serial = sqlite3_column_int(stmt, 0);
                        serials.insert(serial);
	                    }
                    else
	                    {
                        std::ostringstream msg;
                        msg << CPPTRANSPORT_DATAMGR_INTEGRITY_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ") [" << __func__ << "]";
                        sqlite3_finalize(stmt);
                        throw runtime_exception(exception_type::DATA_MANAGER_BACKEND_ERROR, msg.str());
	                    }
	                }

                check_stmt(db, sqlite3_finalize(stmt));

                return (serials);
	            }

	        }   // namespace integrity_detail


        // at the moment we only check for missing k-configuration serial numbers
        // we don't check that k-configuration serial numbers which are *present* have their full complement
        // of time samples
		    template <typename number, typename ValueType>
		    serial_number_list get_missing_serials(sqlite3* db)
			    {
		        std::ostringstream find_stmt;
				    find_stmt
				      << "SELECT serial FROM " << data_traits<number, ValueType>::sqlite_sample_table()
					    << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << data_traits<number, ValueType>::sqlite_table() << ")"
		          << " ORDER BY serial;";

				    return integrity_detail::get_serials(db, find_stmt.str());
			    }


        template <typename number, typename ValueType>
        serial_number_list get_stored_serials(sqlite3* db)
          {
            std::ostringstream find_stmt;

            find_stmt << "SELECT DISTINCT(kserial) FROM " << data_traits<number, ValueType>::sqlite_table() << " ORDER BY kserial;";
            return integrity_detail::get_serials(db, find_stmt.str());
          };


        namespace integrity_detail
	        {

            template <typename ConfigurationData>
            class ConfigurationFinder
	            {
              public:
                ConfigurationFinder(unsigned int s)
	                : serial(s)
	                {
	                }


                bool operator()(const ConfigurationData& a)
	                {
                    return (a.serial == this->serial);
	                }


              private:
                unsigned int serial;
	            };

	        }   // namespace integrity_detail


        template <typename Database>
        void drop_statistics(transaction_manager& mgr, sqlite3* db, const serial_number_list& drop_list, const Database& dbase)
	        {
            for(unsigned int drop_serial : drop_list)
	            {
                typename Database::const_config_iterator u = std::find_if(dbase.config_begin(), dbase.config_end(),
                                                                          integrity_detail::ConfigurationFinder<typename Database::const_config_iterator::type>(drop_serial));

                if(u != dbase.config_end())
	                {
                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << CPPTRANSPORT_SQLITE_STATS_TABLE << " WHERE kserial=" << drop_serial << ";";
                    exec(db, drop_stmt.str());
	                }
	            }
          }


        template <typename number, typename ValueType, typename Database>
        void drop_ics(transaction_manager& mgr, sqlite3* db, const serial_number_list& drop_list, const Database& dbase)
	        {
            for(unsigned int drop_serial : drop_list)
	            {
                typename Database::const_config_iterator u = std::find_if(dbase.config_begin(), dbase.config_end(),
                                                                          integrity_detail::ConfigurationFinder<typename Database::const_config_iterator::type>(drop_serial));

                if(u != dbase.config_end())
	                {
                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << data_traits<number, ValueType>::sqlite_table() << " WHERE kserial=" << drop_serial << ";";
                    exec(db, drop_stmt.str());
	                }
	            }
	        }


        // should be wrapped in an outer transaction
		    template <typename WriterObject, typename Database>
		    void drop_k_configurations(transaction_manager& mgr, sqlite3* db, WriterObject& writer, const serial_number_list& drop_list, const Database& dbase,
		                               std::string table, bool silent=false)
			    {
            for(unsigned int drop_serial : drop_list)
					    {
				        typename Database::const_config_iterator u = std::find_if(dbase.config_begin(), dbase.config_end(),
				                                                                  integrity_detail::ConfigurationFinder<typename Database::const_config_iterator::type>(drop_serial));

				        if(u != dbase.config_end())
					        {
//				            if(!silent) BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** " << *u;

				            std::ostringstream drop_stmt;
				            drop_stmt << "DELETE FROM " << table << " WHERE kserial=" << drop_serial << ";";
				            exec(db, drop_stmt.str());
					        }
					    }
			    }


	    }   // namespace sqlite3_operations

	}   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_INTEGRITY_H
