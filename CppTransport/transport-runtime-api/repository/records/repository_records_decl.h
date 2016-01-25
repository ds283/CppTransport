//
// Created by David Seery on 25/03/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_REPOSITORY_RECORDS_DECL_H
#define CPPTRANSPORT_REPOSITORY_RECORDS_DECL_H


#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/localizations/messages_en.h"

#include "transport-runtime-api/repository/records/record_finder_decl.h"

#include "transport-runtime-api/manager/instance_manager.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare initial conditions class if needed
#include "transport-runtime-api/concepts/initial_conditions_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare derived products if needed
#include "transport-runtime-api/derived-products/derived_product_forward_declare.h"


#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/filesystem/operations.hpp"

#include "transport-runtime-api/repository/records/detail/metadata_decl.h"
#include "transport-runtime-api/repository/records/detail/record_decl.h"
#include "transport-runtime-api/repository/records/detail/package_decl.h"
#include "transport-runtime-api/repository/records/detail/task_decl.h"
#include "transport-runtime-api/repository/records/detail/derived_product_decl.h"
#include "transport-runtime-api/repository/records/detail/output_group_metadata_decl.h"
#include "transport-runtime-api/repository/records/detail/output_group_decl.h"



#endif //CPPTRANSPORT_REPOSITORY_RECORDS_DECL_H
