//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_TRANSPORT_H
#define CPPTRANSPORT_TRANSPORT_H


// expect Boost to be linked with dynamic libraries;
// note this macro is essential for Boost.Log
#define BOOST_ALL_DYN_LINK

#include "transport-runtime/messages.h"
#include "transport-runtime/defaults.h"

#include "transport-runtime/models/observers.h"
#include "transport-runtime/models/canonical_model.h"

#include "transport-runtime/tasks/task_helper.h"

// current implementation uses SQLite/libjsoncpp as the repository database
#include "transport-runtime/sqlite3/repository_sqlite3.h"

// current implementation uses sqlite3 as the data container database
#include "transport-runtime/sqlite3/data_manager_sqlite3.h"

// derived data products
#include "transport-runtime/derived-products/data_products.h"

#include "transport-runtime/manager/task_manager.h"


#endif // CPPTRANSPORT_TRANSPORT_H
