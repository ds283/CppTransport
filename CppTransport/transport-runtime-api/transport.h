//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_TRANSPORT_H_
#define __CPP_TRANSPORT_TRANSPORT_H_

#define BOOST_ALL_DYN_LINK

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/defaults.h"

#include "transport-runtime-api/models/observers.h"
#include "transport-runtime-api/models/canonical_model.h"

#include "transport-runtime-api/tasks/task_helper.h"

// current implementation uses unqlite as the repository database
#include "transport-runtime-api/unqlite/repository_unqlite.h"

// current implementation uses sqlite3 as the data container database
#include "transport-runtime-api/sqlite3/data_manager_sqlite3.h"

#include "transport-runtime-api/derived-products/derived-content/zeta_timeseries_compute.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_kseries_compute.h"
#include "transport-runtime-api/derived-products/derived-content/fNL_timeseries_compute.h"

// derived data products
#include "transport-runtime-api/derived-products/data_products.h"

#include "transport-runtime-api/manager/task_manager.h"


#endif // __CPP_TRANSPORT_TRANSPORT_H_
