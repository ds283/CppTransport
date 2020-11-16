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


#ifndef CPPTRANSPORT_DATA_MANAGER_COMMON_H
#define CPPTRANSPORT_DATA_MANAGER_COMMON_H


#include <set>
#include <vector>
#include <string>
#include <sstream>

#include "transport-runtime/tasks/task.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/template_types.h"
#include "transport-runtime/scheduler/device_queue_manager.h"
#include "transport-runtime/models/model.h"

#include "transport-runtime/exceptions.h"
#include "transport-runtime/messages.h"

#include "transport-runtime/repository/writers/writers.h"
#include "transport-runtime/data/datapipe/linecache_specializations.h"
#include "transport-runtime/data/batchers/batchers.h"

#include "boost/lexical_cast.hpp"

#include "transport-runtime/transactions/transaction_manager.h"

#include "sqlite3.h"

#include "transport-runtime/sqlite3/operations/sqlite3_utility.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        constexpr auto CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE                   = "time_samples";
        constexpr auto CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE                  = "twopf_samples";
        constexpr auto CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE                = "threepf_samples";
        constexpr auto CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE                   = "backg";
        constexpr auto CPPTRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE            = "tensor_twopf";
        constexpr auto CPPTRANSPORT_SQLITE_TWOPF_RE_VALUE_TABLE                = "twopf_re";
        constexpr auto CPPTRANSPORT_SQLITE_TWOPF_IM_VALUE_TABLE                = "twopf_im";
        constexpr auto CPPTRANSPORT_SQLITE_THREEPF_MOMENTUM_VALUE_TABLE        = "threepf_momentum";
        constexpr auto CPPTRANSPORT_SQLITE_THREEPF_DERIV_VALUE_TABLE           = "threepf_deriv";
        constexpr auto CPPTRANSPORT_SQLITE_WORKERS_TABLE                       = "worker_data";
        constexpr auto CPPTRANSPORT_SQLITE_STATS_TABLE                         = "integration_statistics";
        constexpr auto CPPTRANSPORT_SQLITE_ICS_TABLE                           = "horizon_exit_values";
        constexpr auto CPPTRANSPORT_SQLITE_KT_ICS_TABLE                        = "kt_horizon_exit_values";
        constexpr auto CPPTRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE              = "zeta_twopf";
        constexpr auto CPPTRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE            = "zeta_threepf";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM1_VALUE_TABLE              = "gauge_xfm1";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM2_123_VALUE_TABLE          = "gauge_xfm2_123";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM2_213_VALUE_TABLE          = "gauge_xfm2_213";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM2_312_VALUE_TABLE          = "gauge_xfm2_312";
        constexpr auto CPPTRANSPORT_SQLITE_FNL_LOCAL_VALUE_TABLE               = "fNL_local";
        constexpr auto CPPTRANSPORT_SQLITE_FNL_EQUI_VALUE_TABLE                = "fNL_equi";
        constexpr auto CPPTRANSPORT_SQLITE_FNL_ORTHO_VALUE_TABLE               = "fNL_ortho";
        constexpr auto CPPTRANSPORT_SQLITE_FNL_DBI_VALUE_TABLE                 = "fNL_DBI";

        constexpr auto CPPTRANSPORT_SQLITE_TEMP_FNL_TABLE                      = "fNL_update";
        constexpr auto CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE                    = "fNL_insert";

        constexpr auto CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME                    = "tempdb";

        constexpr auto CPPTRANSPORT_SQLITE_TWOPF_RE_TIME_INDEX                 = "twopf_re_time";
        constexpr auto CPPTRANSPORT_SQLITE_TWOPF_RE_K_INDEX                    = "twopf_re_k";
        constexpr auto CPPTRANSPORT_SQLITE_TWOPF_IM_TIME_INDEX                 = "twopf_im_time";
        constexpr auto CPPTRANSPORT_SQLITE_TWOPF_IM_K_INDEX                    = "twopf_im_k";
        constexpr auto CPPTRANSPORT_SQLITE_TENSOR_TWOPF_TIME_INDEX             = "tensor_twopf_time";
        constexpr auto CPPTRANSPORT_SQLITE_TENSOR_TWOPF_K_INDEX                = "tensor_twopf_k";
        constexpr auto CPPTRANSPORT_SQLITE_THREEPF_MOMENTUM_TIME_INDEX         = "threepf_momentum_time";
        constexpr auto CPPTRANSPORT_SQLITE_THREEPF_MOMENTUM_K_INDEX            = "threepf_momentum_k";
        constexpr auto CPPTRANSPORT_SQLITE_THREEPF_DERIV_TIME_INDEX            = "threepf_deriv_time";
        constexpr auto CPPTRANSPORT_SQLITE_THREEPF_DERIV_K_INDEX               = "threepf_deriv_k";
        constexpr auto CPPTRANSPORT_SQLITE_ZETA_TWOPF_TIME_INDEX               = "zeta_twopf_time";
        constexpr auto CPPTRANSPORT_SQLITE_ZETA_TWOPF_K_INDEX                  = "zeta_twopf_k";
        constexpr auto CPPTRANSPORT_SQLITE_ZETA_THREEPF_TIME_INDEX             = "zeta_threepf_time";
        constexpr auto CPPTRANSPORT_SQLITE_ZETA_THREEPF_K_INDEX                = "zeta_threepf_k";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM1_TIME_INDEX               = "gauge_xfm1_time";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM1_K_INDEX                  = "gauge_xfm1_k";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM2_123_TIME_INDEX           = "gauge_xfm2_123_time";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM2_123_K_INDEX              = "gauge_xfm2_123_k";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM2_213_TIME_INDEX           = "gauge_xfm2_213_time";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM2_213_K_INDEX              = "gauge_xfm2_213_k";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM2_312_TIME_INDEX           = "gauge_xfm2_312_time";
        constexpr auto CPPTRANSPORT_SQLITE_GAUGE_XFM2_312_K_INDEX              = "gauge_xfm2_312_k";

        // sqlite defaults to a maximum number of columns of 2000
        constexpr unsigned int CPPTRANSPORT_DEFAULT_SQLITE_MAX_COLUMN          = 2000;
        // sqlite defaults to a maximum number of host parameters of 999
        constexpr unsigned int CPPTRANSPORT_DEFAULT_SQLITE_MAX_VARIABLE_NUMBER = 999;

        // overhead on number of columns per page
        // 50 is extremely conservative
        constexpr unsigned int CPPTRANSPORT_DEFAULT_SQLITE_COLUMN_OVERHEAD     = 50;

        enum class foreign_keys_type { foreign_keys, no_foreign_keys };

        enum class kconfiguration_type { twopf_configs, threepf_configs };


        // sqlite has a default maximum number of columns, and a maximum number of
        // host parameters
        // with a large number of fields we can easily exceed those, so need
        // to set a limit on the number of columns per row
        constexpr unsigned int max_columns = (CPPTRANSPORT_DEFAULT_SQLITE_MAX_VARIABLE_NUMBER < CPPTRANSPORT_DEFAULT_SQLITE_MAX_COLUMN ? CPPTRANSPORT_DEFAULT_SQLITE_MAX_VARIABLE_NUMBER : CPPTRANSPORT_DEFAULT_SQLITE_MAX_COLUMN) - CPPTRANSPORT_DEFAULT_SQLITE_COLUMN_OVERHEAD;


        // construct the name of an fNL table
        inline std::string fNL_table_name(derived_data::bispectrum_template type)
          {
            switch(type)
              {
                case derived_data::bispectrum_template::local:
                  return static_cast<std::string>(CPPTRANSPORT_SQLITE_FNL_LOCAL_VALUE_TABLE);

                case derived_data::bispectrum_template::equilateral:
                  return static_cast<std::string>(CPPTRANSPORT_SQLITE_FNL_EQUI_VALUE_TABLE);

                case derived_data::bispectrum_template::orthogonal:
                  return static_cast<std::string>(CPPTRANSPORT_SQLITE_FNL_ORTHO_VALUE_TABLE);

                case derived_data::bispectrum_template::DBI:
                  return static_cast<std::string>(CPPTRANSPORT_SQLITE_FNL_DBI_VALUE_TABLE);
              }
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_COMMON_H
