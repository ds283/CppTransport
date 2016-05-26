//
// Created by David Seery on 25/06/2013.
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

#ifndef CPPTRANSPORT_TRANSPORT_H
#define CPPTRANSPORT_TRANSPORT_H


#ifdef __INTEL_COMPILER
// Intel compiler produces this warning when using boost::escape_dot_string()
#pragma warning disable 2196
#endif


// expect Boost to be linked with dynamic libraries;
// note this macro is essential for Boost.Log
#define BOOST_ALL_DYN_LINK


// enable to force strict checks on referential integrity
// in the database.
// Not enabled by default because it is much slower
// #define CPPTRANSPORT_STRICT_CONSISTENCY


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

// derived_data namespace gets aliased to vis_toolkit
namespace vis_toolkit = transport::derived_data;


#endif // CPPTRANSPORT_TRANSPORT_H
