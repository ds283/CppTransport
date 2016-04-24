//
// Created by David Seery on 25/01/2016.
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
