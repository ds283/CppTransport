//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_RECORDS_IMPL_H
#define CPPTRANSPORT_REPOSITORY_RECORDS_IMPL_H


#include "boost/timer/timer.hpp"
#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"

#include "sqlite3.h"
#include "transport-runtime/sqlite3/operations/sqlite3_utility.h"


#include "transport-runtime/repository/records/detail/metadata_impl.h"
#include "transport-runtime/repository/records/detail/record_impl.h"
#include "transport-runtime/repository/records/detail/package_impl.h"
#include "transport-runtime/repository/records/detail/task_impl.h"
#include "transport-runtime/repository/records/detail/derived_product_impl.h"
#include "transport-runtime/repository/records/detail/content_group_metadata_impl.h"
#include "transport-runtime/repository/records/detail/content_group_impl.h"



#endif //CPPTRANSPORT_REPOSITORY_RECORDS_IMPL_H
