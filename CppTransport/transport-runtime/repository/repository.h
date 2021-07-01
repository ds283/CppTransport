//
// Created by David Seery on 30/12/2013.
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


#ifndef CPPTRANSPORT_REPOSITORY_H
#define CPPTRANSPORT_REPOSITORY_H


#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <map>
#include <unordered_map>


#include "transport-runtime/version.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"
#include "transport-runtime/defaults.h"
#include "transport-runtime/common.h"

#include "transport-runtime/serialization/serializable.h"
#include "transport-runtime/manager/message_handlers.h"
#include "transport-runtime/manager/environment.h"

#include "boost/date_time.hpp"


// include repository_forward_declare.h to get correct declaration of template default argument
// (it can only be given *once*)
#include "transport-runtime/repository/repository_forward_declare.h"

// need model_manager for model_finder object
#include "transport-runtime/manager/model_manager.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

// forward-declare initial conditions class if needed
#include "transport-runtime/concepts/initial_conditions_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime/tasks/tasks_forward_declare.h"

// forward-declare repository records; this automatically pulls in repository_record_databases.h also
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

// forward-declare derived_product if needed
#include "transport-runtime/derived-products/derived_product_forward_declare.h"

#include "transport-runtime/repository/writers/writers.h"
#include "transport-runtime/transactions/transaction_manager.h"

// need data_manager definition
#include "transport-runtime/data/data_manager.h"


// DECLARE REPOSITORY RECORD FINDERS
#include "transport-runtime/repository/records/record_finder_decl.h"

// DECLARE REPOSITORY RECORDS
#include "transport-runtime/repository/records/repository_records_decl.h"

// DECLARE INFLIGHT CONTENT RECORDS
#include "transport-runtime/repository/inflight.h"

// DECLARE WRITER HANDLERS
#include "transport-runtime/repository/detail/writer_repo_decl.h"

// DECLARE MAIN REPOSITORY CLASS
#include "transport-runtime/repository/detail/repository_decl.h"

// DECLARE CONTENT GROUP FINDER
#include "transport-runtime/repository/records/content_finder_decl.h"


// DEFINITIONS FOR REPOSITORY RECORD FINDERS
#include "transport-runtime/repository/records/record_finder_impl.h"

// DEFINITIONS FOR REPOSITORY RECORDS
#include "transport-runtime/repository/records/repository_records_impl.h"

// DEFINE WRITER HANDLERS
#include "transport-runtime/repository/detail/writer_repo_impl.h"

// DEFINITIONS FOR MAIN REPOSITORY CLASS METHODS
#include "transport-runtime/repository/detail/repository_impl.h"

// DEFINITIONS FOR CONTENT GROUP FINDER
#include "transport-runtime/repository/records/content_finder_impl.h"


#endif //CPPTRANSPORT_REPOSITORY_H
