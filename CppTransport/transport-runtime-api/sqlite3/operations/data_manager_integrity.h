//
// Created by David Seery on 13/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __data_manager_integrity_H_
#define __data_manager_integrity_H_


#include "transport-runtime-api/sqlite3/operations/data_manager_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        std::list<unsigned int> get_missing_twopf_serials(sqlite3* db, twopf_value_type type=real_twopf)
          {
            std::ostringstream find_stmt;
            find_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << twopf_table_name(type) << ") ORDER BY serial;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

            std::list<unsigned int> serials;
            int status;
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
                    msg << __CPP_TRANSPORT_DATAMGR_INTEGRITY_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ") [" << __func__ << "]";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));

            return(serials);
          }


        std::list<unsigned int> get_missing_threepf_serials(sqlite3* db)
          {
            std::ostringstream find_stmt;
            find_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ") ORDER BY serial;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

            std::list<unsigned int> serials;
            int status;
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
                    msg << __CPP_TRANSPORT_DATAMGR_INTEGRITY_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ") [" << __func__ << "]";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));

            return(serials);
          }


        std::list<unsigned int> get_missing_zeta_twopf_serials(sqlite3* db)
          {
            std::ostringstream find_stmt;
            find_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << ") ORDER BY serial;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

            std::list<unsigned int> serials;
            int status;
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
                    msg << __CPP_TRANSPORT_DATAMGR_INTEGRITY_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ") [" << __func__ << "]";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));

            return(serials);
          }


        std::list<unsigned int> get_missing_zeta_threepf_serials(sqlite3* db)
          {
            std::ostringstream find_stmt;
            find_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << ") ORDER BY serial;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

            std::list<unsigned int> serials;
            int status;
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
                    msg << __CPP_TRANSPORT_DATAMGR_INTEGRITY_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ") [" << __func__ << "]";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));

            return(serials);
          }


        std::list<unsigned int> get_missing_zeta_redbsp_serials(sqlite3* db)
          {
            std::ostringstream find_stmt;
            find_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << ") ORDER BY serial;";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, find_stmt.str().c_str(), find_stmt.str().length()+1, &stmt, nullptr));

            std::list<unsigned int> serials;
            int status;
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
                    msg << __CPP_TRANSPORT_DATAMGR_INTEGRITY_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ") [" << __func__ << "]";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(runtime_exception::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));

            return(serials);
          }


        template <typename ConfigurationData>
        class ConfigurationFinder
          {
          public:
            ConfigurationFinder(unsigned int s)
              : serial(s)
              {
              }

            bool operator()(const ConfigurationData& a) { return(a.serial == this->serial); }

          private:
            unsigned int serial;
          };


        template <typename Database>
        void drop_statistics(sqlite3* db, const std::list<unsigned int>& drop_list, const Database& dbase)
          {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                typename Database::const_config_iterator u = std::find_if(dbase.config_begin(), dbase.config_end(),
                                                                          ConfigurationFinder<typename Database::const_config_iterator::type>(*t));

                if(u != dbase.config_end())
                  {
                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << __CPP_TRANSPORT_SQLITE_STATS_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }


        template <typename WriterObject>
        void drop_twopf_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const twopf_kconfig_database& twopf_db,
                                       twopf_value_type type=real_twopf, bool silent=false)
          {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                twopf_kconfig_database::const_config_iterator u = std::find_if(twopf_db.config_begin(), twopf_db.config_end(),
                                                                               ConfigurationFinder<twopf_kconfig_database::const_config_iterator::type>(*t));

                if(u != twopf_db.config_end())
                  {
                    if(!silent) BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** " << *u;

                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << twopf_table_name(type) << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }


        template <typename WriterObject>
        void drop_threepf_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const threepf_kconfig_database& threepf_db,
                                         bool silent = false)
          {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                threepf_kconfig_database::const_config_iterator u = std::find_if(threepf_db.config_begin(), threepf_db.config_end(),
                                                                                ConfigurationFinder<threepf_kconfig_database::const_config_iterator::type>(*t));

                if(u != threepf_db.config_end())
                  {
                    if(!silent) BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** " << *u;

                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }


        template <typename WriterObject>
        void drop_zeta_twopf_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const twopf_kconfig_database& twopf_db,
                                            bool silent=false)
          {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                twopf_kconfig_database::const_config_iterator u = std::find_if(twopf_db.config_begin(), twopf_db.config_end(),
                                                                               ConfigurationFinder<twopf_kconfig_database::const_config_iterator::type>(*t));

                if(u != twopf_db.config_end())
                  {
                    if(!silent) BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** " << *u;

                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }


        template <typename WriterObject>
        void drop_zeta_threepf_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const threepf_kconfig_database& threepf_db,
                                              bool silent = false)
          {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                threepf_kconfig_database::const_config_iterator u = std::find_if(threepf_db.config_begin(), threepf_db.config_end(),
                                                                                 ConfigurationFinder<threepf_kconfig_database::const_config_iterator::type>(*t));

                if(u != threepf_db.config_end())
                  {
                    if(!silent) BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** " << *u;

                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }


        template <typename WriterObject>
        void drop_zeta_redbsp_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const threepf_kconfig_database& threepf_db,
                                             bool silent=false)
          {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                threepf_kconfig_database::const_config_iterator u = std::find_if(threepf_db.config_begin(), threepf_db.config_end(),
                                                                                 ConfigurationFinder<threepf_kconfig_database::const_config_iterator::type>(*t));

                if(u != threepf_db.config_end())
                  {
                    if(!silent) BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** " << *u;

                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //__data_manager_integrity_H_
