//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_TRANSPORT_H_
#define CPPTRANSPORT_TRANSPORT_H_

#define BOOST_ALL_DYN_LINK
#define FUSION_MAX_VECTOR_SIZE 20

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/defaults.h"

#include "transport-runtime-api/models/observers.h"
#include "transport-runtime-api/models/canonical_model.h"

#include "transport-runtime-api/tasks/task_helper.h"

// current implementation uses SQLite/libjsoncpp as the repository database
#include "transport-runtime-api/sqlite3/repository_sqlite3.h"

// current implementation uses sqlite3 as the data container database
#include "transport-runtime-api/sqlite3/data_manager_sqlite3.h"

// derived data products
#include "transport-runtime-api/derived-products/data_products.h"

#include "transport-runtime-api/manager/task_manager.h"


#endif // CPPTRANSPORT_TRANSPORT_H_
