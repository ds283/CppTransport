//
// Created by David Seery on 10/05/2016.
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

#ifndef CPPTRANSPORT_DATA_MANAGER_FINALIZE_H
#define CPPTRANSPORT_DATA_MANAGER_FINALIZE_H


#include "transport-runtime/sqlite3/operations/data_manager_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        namespace finalize_impl
          {

            void create_tserial_index(transaction_manager& mgr, sqlite3* db, const std::string index_name, const std::string table_name)
              {
                assert(db != nullptr);

                std::ostringstream create_index_stmt;
                create_index_stmt << "CREATE INDEX " << index_name << " ON " << table_name << "(tserial);";
                exec(db, create_index_stmt.str());
              }


            void create_kserial_index(transaction_manager& mgr, sqlite3* db, const std::string index_name, const std::string table_name)
              {
                assert(db != nullptr);

                std::ostringstream create_index_stmt;
                create_index_stmt << "CREATE INDEX " << index_name << " ON " << table_name << "(kserial);";
                exec(db, create_index_stmt.str());
              }


            void analyze(transaction_manager& mgr, sqlite3* db)
              {
                assert(db != nullptr);

                exec(db, "ANALYZE;");
              }

          }


        void finalize_twopf_writer(transaction_manager& mgr, sqlite3* db)
          {
            assert(db != nullptr);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_TWOPF_RE_TIME_INDEX, CPPTRANSPORT_SQLITE_TWOPF_RE_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_TWOPF_RE_K_INDEX, CPPTRANSPORT_SQLITE_TWOPF_RE_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_TENSOR_TWOPF_TIME_INDEX, CPPTRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_TENSOR_TWOPF_K_INDEX, CPPTRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE);

            finalize_impl::analyze(mgr, db);
          }


        void finalize_threepf_writer(transaction_manager& mgr, sqlite3* db)
          {
            assert(db != nullptr);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_TWOPF_RE_TIME_INDEX, CPPTRANSPORT_SQLITE_TWOPF_RE_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_TWOPF_RE_K_INDEX, CPPTRANSPORT_SQLITE_TWOPF_RE_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_TWOPF_IM_TIME_INDEX, CPPTRANSPORT_SQLITE_TWOPF_IM_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_TWOPF_IM_K_INDEX, CPPTRANSPORT_SQLITE_TWOPF_IM_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_TENSOR_TWOPF_TIME_INDEX, CPPTRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_TENSOR_TWOPF_K_INDEX, CPPTRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_THREEPF_MOMENTUM_TIME_INDEX, CPPTRANSPORT_SQLITE_THREEPF_MOMENTUM_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_THREEPF_MOMENTUM_K_INDEX, CPPTRANSPORT_SQLITE_THREEPF_MOMENTUM_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_THREEPF_DERIV_TIME_INDEX, CPPTRANSPORT_SQLITE_THREEPF_DERIV_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_THREEPF_DERIV_K_INDEX, CPPTRANSPORT_SQLITE_THREEPF_DERIV_VALUE_TABLE);

            finalize_impl::analyze(mgr, db);
          }


        void finalize_zeta_twopf_writer(transaction_manager& mgr, sqlite3* db)
          {
            assert(db != nullptr);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_ZETA_TWOPF_TIME_INDEX, CPPTRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_ZETA_TWOPF_K_INDEX, CPPTRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_GAUGE_XFM1_TIME_INDEX, CPPTRANSPORT_SQLITE_GAUGE_XFM1_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_GAUGE_XFM1_K_INDEX, CPPTRANSPORT_SQLITE_GAUGE_XFM1_VALUE_TABLE);

            finalize_impl::analyze(mgr, db);
          }


        void finalize_zeta_threepf_writer(transaction_manager& mgr, sqlite3* db)
          {
            assert(db != nullptr);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_ZETA_TWOPF_TIME_INDEX, CPPTRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_ZETA_TWOPF_K_INDEX, CPPTRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_ZETA_THREEPF_TIME_INDEX, CPPTRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_ZETA_THREEPF_K_INDEX, CPPTRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_GAUGE_XFM1_TIME_INDEX, CPPTRANSPORT_SQLITE_GAUGE_XFM1_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_GAUGE_XFM1_K_INDEX, CPPTRANSPORT_SQLITE_GAUGE_XFM1_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_GAUGE_XFM2_123_TIME_INDEX, CPPTRANSPORT_SQLITE_GAUGE_XFM2_123_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_GAUGE_XFM2_123_K_INDEX, CPPTRANSPORT_SQLITE_GAUGE_XFM2_123_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_GAUGE_XFM2_213_TIME_INDEX, CPPTRANSPORT_SQLITE_GAUGE_XFM2_213_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_GAUGE_XFM2_213_K_INDEX, CPPTRANSPORT_SQLITE_GAUGE_XFM2_213_VALUE_TABLE);

            finalize_impl::create_tserial_index(mgr, db, CPPTRANSPORT_SQLITE_GAUGE_XFM2_312_TIME_INDEX, CPPTRANSPORT_SQLITE_GAUGE_XFM2_312_VALUE_TABLE);
            finalize_impl::create_kserial_index(mgr, db, CPPTRANSPORT_SQLITE_GAUGE_XFM2_312_K_INDEX, CPPTRANSPORT_SQLITE_GAUGE_XFM2_312_VALUE_TABLE);

            finalize_impl::analyze(mgr, db);
          }


        void finalize_fNL_writer(transaction_manager& mgr, sqlite3* db)
          {
            assert(db != nullptr);

            // don't currently create indexes for fNL

            finalize_impl::analyze(mgr, db);
          }

      }   // namespace sqlite3_operations

  }   // namespace transport

#endif //CPPTRANSPORT_DATA_MANAGER_FINALIZE_H
