//
// Created by David Seery on 13/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __data_manager_integrity_H_
#define __data_manager_integrity_H_


#include "transport-runtime-api/sqlite3/operations/data_manager_common.h"
#include "transport-runtime-api/sqlite3/operations/data_traits.h"


namespace transport
	{

    namespace sqlite3_operations
	    {

        namespace integrity_detail
	        {

            std::list<unsigned int> get_serials(sqlite3* db, std::string sql_query)
	            {
                sqlite3_stmt* stmt;
                check_stmt(db, sqlite3_prepare_v2(db, sql_query.c_str(), sql_query.length() + 1, &stmt, nullptr));

                std::list<unsigned int> serials;
                int                     status;
                while((status = sqlite3_step(stmt)) != SQLITE_DONE)
	                {
                    if(status == SQLITE_ROW)
	                    {
                        unsigned int serial = sqlite3_column_int(stmt, 0);
                        serials.push_back(serial);
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
		    std::list<unsigned int> get_missing_serials(sqlite3* db)
			    {
		        std::ostringstream find_stmt;
				    find_stmt
				      << "SELECT serial FROM " << data_traits<number, ValueType>::sqlite_sample_table()
					    << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << data_traits<number, ValueType>::sqlite_table() << ")"
		          << " ORDER BY serial;";

				    return integrity_detail::get_serials(db, find_stmt.str());
			    }


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
        void drop_statistics(sqlite3* db, const std::list<unsigned int>& drop_list, const Database& dbase)
	        {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); ++t)
	            {
                typename Database::const_config_iterator u = std::find_if(dbase.config_begin(), dbase.config_end(),
                                                                          integrity_detail::ConfigurationFinder<typename Database::const_config_iterator::type>(*t));

                if(u != dbase.config_end())
	                {
                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << CPPTRANSPORT_SQLITE_STATS_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
	                }
	            }
	        }


        template <typename number, typename ValueType, typename Database>
        void drop_ics(sqlite3* db, const std::list<unsigned int>& drop_list, const Database& dbase)
	        {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); ++t)
	            {
                typename Database::const_config_iterator u = std::find_if(dbase.config_begin(), dbase.config_end(),
                                                                          integrity_detail::ConfigurationFinder<typename Database::const_config_iterator::type>(*t));

                if(u != dbase.config_end())
	                {
                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << data_traits<number, ValueType>::sqlite_table() << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
	                }
	            }
	        }


		    template <typename WriterObject, typename Database>
		    void drop_k_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const Database& dbase,
		                               std::string table, bool silent=false)
			    {
				    for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); ++t)
					    {
				        typename Database::const_config_iterator u = std::find_if(dbase.config_begin(), dbase.config_end(),
				                                                                  integrity_detail::ConfigurationFinder<typename Database::const_config_iterator::type>(*t));

				        if(u != dbase.config_end())
					        {
				            if(!silent) BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** " << *u;

				            std::ostringstream drop_stmt;
				            drop_stmt << "DELETE FROM " << table << " WHERE kserial=" << *t << ";";
				            exec(db, drop_stmt.str());
					        }
					    }
			    }


	    }   // namespace sqlite3_operations

	}   // namespace transport


#endif //__data_manager_integrity_H_
