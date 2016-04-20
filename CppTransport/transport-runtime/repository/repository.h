//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_REPOSITORY_H
#define CPPTRANSPORT_REPOSITORY_H


#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <map>

#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/manager/message_handlers.h"
#include "transport-runtime-api/manager/environment.h"


#include "boost/date_time.hpp"

// need model_manager for model_finder object
#include "transport-runtime-api/manager/model_manager.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare initial conditions class if needed
#include "transport-runtime-api/concepts/initial_conditions_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

// forward-declare derived_product if needed
#include "transport-runtime-api/derived-products/derived_product_forward_declare.h"

#include "transport-runtime-api/repository/writers/writers.h"
#include "transport-runtime-api/transactions/transaction_manager.h"

// need data_manager definition
#include "transport-runtime-api/data/data_manager.h"


// DECLARE REPOSITORY RECORD FINDERS
#include "transport-runtime-api/repository/records/record_finder_decl.h"
#include "transport-runtime-api/repository/records/content_finder_decl.h"

// DECLARE REPOSITORY RECORDS
#include "transport-runtime-api/repository/records/repository_records_decl.h"

// DECLARE INFLIGHT CONTENT RECORDS
#include "transport-runtime-api/repository/inflight.h"

// DECLARE WRITER HANDLERS
#include "transport-runtime-api/repository/detail/writer_repo_decl.h"

// DECLARE MAIN REPOSITORY CLASS
#include "transport-runtime-api/repository/detail/repository_decl.h"

// DEFINITIONS FOR REPOSITORY RECORD FINDERS
#include "transport-runtime-api/repository/records/record_finder_impl.h"
#include "transport-runtime-api/repository/records/content_finder_impl.h"

// DEFINITIONS FOR REPOSITORY RECORDS
#include "transport-runtime-api/repository/records/repository_records_impl.h"

// DEFINE WRITER HANDLERS
#include "transport-runtime-api/repository/detail/writer_repo_impl.h"

// DEFINITIONS FOR MAIN REPOSITORY CLASS METHODS
#include "transport-runtime-api/repository/detail/repository_impl.h"


#endif //CPPTRANSPORT_REPOSITORY_H
