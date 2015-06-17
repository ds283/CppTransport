//
// Created by David Seery on 13/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __data_manager_common_H_
#define __data_manager_common_H_


#include <set>
#include <vector>
#include <string>
#include <sstream>

#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/derived-products/derived-content/correlation-functions/template_types.h"
#include "transport-runtime-api/scheduler/work_queue.h"
#include "transport-runtime-api/models/model.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/repository/writers/writers.h"
#include "transport-runtime-api/data/datapipe/linecache_specializations.h"
#include "transport-runtime-api/data/batchers/batchers.h"

#include "boost/lexical_cast.hpp"

#include "sqlite3.h"

#include "transport-runtime-api/sqlite3/operations/sqlite3_utility.h"


#define CPPTRANSPORT_SQLITE_TIME_SAMPLE_TABLE                   "time_samples"
#define CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE                  "twopf_samples"
#define CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE                "threepf_samples"
#define CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE                   "backg"
#define CPPTRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE            "tensor_twopf"
#define CPPTRANSPORT_SQLITE_TWOPF_RE_VALUE_TABLE                "twopf_re"
#define CPPTRANSPORT_SQLITE_TWOPF_IM_VALUE_TABLE                "twopf_im"
#define CPPTRANSPORT_SQLITE_THREEPF_VALUE_TABLE                 "threepf"
#define CPPTRANSPORT_SQLITE_WORKERS_TABLE                       "worker_data"
#define CPPTRANSPORT_SQLITE_STATS_TABLE                         "integration_statistics"
#define CPPTRANSPORT_SQLITE_ICS_TABLE                           "horizon_exit_values"
#define CPPTRANSPORT_SQLITE_KT_ICS_TABLE                        "kt_horizon_exit_values"
#define CPPTRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE              "zeta_twopf"
#define CPPTRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE            "zeta_threepf"
#define CPPTRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE "zeta_redbsp"
#define CPPTRANSPORT_SQLITE_FNL_LOCAL_VALUE_TABLE               "fNL_local"
#define CPPTRANSPORT_SQLITE_FNL_EQUI_VALUE_TABLE                "fNL_equi"
#define CPPTRANSPORT_SQLITE_FNL_ORTHO_VALUE_TABLE               "fNL_ortho"
#define CPPTRANSPORT_SQLITE_FNL_DBI_VALUE_TABLE                 "fNL_DBI"

#define CPPTRANSPORT_SQLITE_TEMP_SERIAL_TABLE                   "serial_search"
#define CPPTRANSPORT_SQLITE_TEMP_FNL_TABLE                      "fNL_update"
#define CPPTRANSPORT_SQLITE_INSERT_FNL_TABLE                    "fNL_insert"

#define CPPTRANSPORT_SQLITE_TEMPORARY_DBNAME                    "tempdb"

// sqlite defaults to a maximum number of columns of 2000
#define CPPTRANSPORT_DEFAULT_SQLITE_MAX_COLUMN                  2000
// sqlite defaults to a maximum number of host parameters of 999
#define CPPTRANSPORT_DEFAULT_SQLITE_MAX_VARIABLE_NUMBER         999

// overhead on number of columns per page
// 50 is extremely conservative
#define CPPTRANSPORT_DEFAULT_SQLITE_COLUMN_OVERHEAD             50


namespace transport
  {

    namespace sqlite3_operations
      {

        typedef enum { foreign_keys, no_foreign_keys } add_foreign_keys_type;

        typedef enum { twopf_configs, threepf_configs } metadata_configuration_type;


        // sqlite has a default maximum number of columns, and a maximum number of
        // host parameters
        // with a large number of fields we can easily exceed those, so need
        // to set a limit on the number of columns per row
        constexpr unsigned int max_columns = (CPPTRANSPORT_DEFAULT_SQLITE_MAX_VARIABLE_NUMBER < CPPTRANSPORT_DEFAULT_SQLITE_MAX_COLUMN ? CPPTRANSPORT_DEFAULT_SQLITE_MAX_VARIABLE_NUMBER : CPPTRANSPORT_DEFAULT_SQLITE_MAX_COLUMN) - CPPTRANSPORT_DEFAULT_SQLITE_COLUMN_OVERHEAD;


        // construct the name of an fNL table
        inline std::string fNL_table_name(derived_data::template_type type)
          {
            switch(type)
              {
                case derived_data::fNL_local_template:
                  return static_cast<std::string>(CPPTRANSPORT_SQLITE_FNL_LOCAL_VALUE_TABLE);

                case derived_data::fNL_equi_template:
                  return static_cast<std::string>(CPPTRANSPORT_SQLITE_FNL_EQUI_VALUE_TABLE);

                case derived_data::fNL_ortho_template:
                  return static_cast<std::string>(CPPTRANSPORT_SQLITE_FNL_ORTHO_VALUE_TABLE);

                case derived_data::fNL_DBI_template:
                  return static_cast<std::string>(CPPTRANSPORT_SQLITE_FNL_DBI_VALUE_TABLE);

                default:
                  assert(false);
                throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, CPPTRANSPORT_DATAMGR_UNKNOWN_FNL_TEMPLATE);
              }
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //__data_manager_common_H_
