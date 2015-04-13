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

        std::list<unsigned int> get_missing_twopf_serials(sqlite3* db)
          {
            std::ostringstream find_stmt;
            find_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << __CPP_TRANSPORT_SQLITE_TWOPF_VALUE_TABLE << ");";

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
            find_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << ");";

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
            find_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << ");";

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
            find_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << ");";

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
            find_stmt << "SELECT serial FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE << " WHERE serial NOT IN (SELECT DISTINCT kserial FROM " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << ");";

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


        template <typename WriterObject>
        void drop_twopf_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const std::vector<twopf_kconfig>& configs)
          {

            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                std::vector<twopf_kconfig>::const_iterator u = std::find_if(configs.begin(), configs.end(), ConfigurationFinder<twopf_kconfig>(*t));

                if(u != configs.end())
                  {
                    BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** " << *u;

                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << __CPP_TRANSPORT_SQLITE_TWOPF_VALUE_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }


        template <typename WriterObject>
        void drop_threepf_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const std::vector<threepf_kconfig>& configs)
          {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                std::vector<threepf_kconfig>::const_iterator u = std::find_if(configs.begin(), configs.end(), ConfigurationFinder<threepf_kconfig>(*t));

                if(u != configs.end())
                  {
                    BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** " << *u;

                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << __CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }


        template <typename WriterObject>
        void drop_zeta_twopf_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const std::vector<twopf_kconfig>& configs)
          {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                std::vector<twopf_kconfig>::const_iterator u = std::find_if(configs.begin(), configs.end(), ConfigurationFinder<twopf_kconfig>(*t));

                if(u != configs.end())
                  {
                    BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** " << *u;

                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << __CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }


        template <typename WriterObject>
        void drop_zeta_threepf_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const std::vector<threepf_kconfig>& configs)
          {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                std::vector<threepf_kconfig>::const_iterator u = std::find_if(configs.begin(), configs.end(), ConfigurationFinder<threepf_kconfig>(*t));

                if(u != configs.end())
                  {
                    BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** " << *u;

                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << __CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }


        template <typename WriterObject>
        void drop_zeta_redbsp_configurations(sqlite3* db, WriterObject& writer, const std::list<unsigned int>& drop_list, const std::vector<threepf_kconfig>& configs)
          {
            for(std::list<unsigned int>::const_iterator t = drop_list.begin(); t != drop_list.end(); t++)
              {
                std::vector<threepf_kconfig>::const_iterator u = std::find_if(configs.begin(), configs.end(), ConfigurationFinder<threepf_kconfig>(*t));

                if(u != configs.end())
                  {
                    BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** " << *u;

                    std::ostringstream drop_stmt;
                    drop_stmt << "DELETE FROM " << __CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE << " WHERE kserial=" << *t << ";";
                    exec(db, drop_stmt.str());
                  }
              }
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //__data_manager_integrity_H_
