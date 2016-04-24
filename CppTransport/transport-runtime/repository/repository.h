//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_REPOSITORY_H
#define CPPTRANSPORT_REPOSITORY_H


#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <map>

#include "transport-runtime/version.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

#include "transport-runtime/serialization/serializable.h"
#include "transport-runtime/manager/message_handlers.h"
#include "transport-runtime/manager/environment.h"


#include "boost/date_time.hpp"

// need model_manager for model_finder object
#include "transport-runtime/manager/model_manager.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

// forward-declare initial conditions class if needed
#include "transport-runtime/concepts/initial_conditions_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime/tasks/tasks_forward_declare.h"

// forward-declare repository records
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

// forward-declare derived_product if needed
#include "transport-runtime/derived-products/derived_product_forward_declare.h"

#include "transport-runtime/repository/writers/writers.h"
#include "transport-runtime/transactions/transaction_manager.h"

// need data_manager definition
#include "transport-runtime/data/data_manager.h"


// DECLARE REPOSITORY RECORD FINDERS
#include "transport-runtime/repository/records/record_finder_decl.h"
#include "transport-runtime/repository/records/content_finder_decl.h"

// DECLARE REPOSITORY RECORDS
#include "transport-runtime/repository/records/repository_records_decl.h"

// DECLARE INFLIGHT CONTENT RECORDS
#include "transport-runtime/repository/inflight.h"

// DECLARE WRITER HANDLERS
#include "transport-runtime/repository/detail/writer_repo_decl.h"

// DECLARE MAIN REPOSITORY CLASS
#include "transport-runtime/repository/detail/repository_decl.h"

// DEFINITIONS FOR REPOSITORY RECORD FINDERS
#include "transport-runtime/repository/records/record_finder_impl.h"
#include "transport-runtime/repository/records/content_finder_impl.h"

// DEFINITIONS FOR REPOSITORY RECORDS
#include "transport-runtime/repository/records/repository_records_impl.h"

// DEFINE WRITER HANDLERS
#include "transport-runtime/repository/detail/writer_repo_impl.h"

// DEFINITIONS FOR MAIN REPOSITORY CLASS METHODS
#include "transport-runtime/repository/detail/repository_impl.h"


#endif //CPPTRANSPORT_REPOSITORY_H
