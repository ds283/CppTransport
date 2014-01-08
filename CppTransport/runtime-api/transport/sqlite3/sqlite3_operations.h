//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __sqlite3_operations_H_
#define __sqlite3_operations_H_


#include "transport/tasks/task.h"

#include "transport/exceptions.h"
#include "transport/messages_en.h"

#include "sqlite3.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        template <typename number>
        void create_time_sample_table(sqlite3* db, task<number>* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            std::vector<double> sample_times = tk->get_sample_times();

            std::stringstream stmt_text;
            stmt_text << "CREATE TABLE time_samples("
                << "serial INTEGER PRIMARY KEY,"
                << "time   DOUBLE"
                << ");";

            char* errmsg = nullptr;
            int status = sqlite3_exec(db, stmt_text.str().c_str(), nullptr, nullptr, &errmsg);

            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_DATACTR_TIMETAB_FAIL << errmsg << ")";
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
              }
          }


        template <typename number>
        void create_twopf_sample_table(sqlite3* db, twopf_list_task<number>* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            std::vector<double> conventional_list = tk->get_k_list();
            std::vector<double> comoving_list = tk->get_k_list_comoving();

            std::stringstream stmt_text;
            stmt_text << "CREATE TABLE twopf_samples("
              << "serial       INTEGER PRIMARY KEY,"
              << "conventional DOUBLE,"
              << "comoving     DOUBLE"
              << ");";

            char* errmsg = nullptr;
            int status = sqlite3_exec(db, stmt_text.str().c_str(), nullptr, nullptr, &errmsg);

            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_DATACTR_TWOPFTAB_FAIL << errmsg << ")";
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
              }
          }


        template <typename number>
        void create_threepf_sample_table(sqlite3* db, task<number>* tk)
          {
            assert(db != nullptr);
            assert(tk != nullptr);

            std::vector<double> sample_times = tk->get_sample_times();

            std::stringstream stmt_text;
            stmt_text << "CREATE TABLE threepf_samples("
              << "serial      INTEGER PRIMARY KEY,"
              << "wavenumber1 INTEGER,"
              << "wavenumber2 INTEGER,"
              << "wavenumber3 INTEGER,"
              << "FOREIGN KEY(wavenumber1) REFERENCES twopf_samples(serial),"
              << "FOREIGN KEY(wavenumber2) REFERENCES twopf_samples(serial),"
              << "FOREIGN KEY(wavenumber3) REFERENCES twopf_samples(serial)"
              << ");";

            char* errmsg = nullptr;
            int status = sqlite3_exec(db, stmt_text.str().c_str(), nullptr, nullptr, &errmsg);

            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_DATACTR_THREEPFTAB_FAIL << errmsg << ")";
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
              }
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //__sqlite3_operations_H_
