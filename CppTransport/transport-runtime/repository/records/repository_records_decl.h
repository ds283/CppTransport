//
// Created by David Seery on 25/03/15.
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


#ifndef CPPTRANSPORT_REPOSITORY_RECORDS_DECL_H
#define CPPTRANSPORT_REPOSITORY_RECORDS_DECL_H


#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <set>
#include <list>

#include "transport-runtime/serialization/serializable.h"

#include "transport-runtime/exceptions.h"
#include "transport-runtime/localizations/messages_en.h"

#include "transport-runtime/repository/records/record_finder_decl.h"

#include "transport-runtime/manager/model_manager.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

// forward-declare initial conditions class if needed
#include "transport-runtime/concepts/initial_conditions_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime/tasks/tasks_forward_declare.h"

// forward-declare derived products if needed
#include "transport-runtime/derived-products/derived_product_forward_declare.h"


#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/filesystem/operations.hpp"

#include "transport-runtime/repository/records/detail/metadata_decl.h"
#include "transport-runtime/repository/records/detail/notes.h"
#include "transport-runtime/repository/records/detail/record_decl.h"
#include "transport-runtime/repository/records/detail/package_decl.h"
#include "transport-runtime/repository/records/detail/task_decl.h"
#include "transport-runtime/repository/records/detail/derived_product_decl.h"
#include "transport-runtime/repository/records/detail/content_group_metadata_decl.h"
#include "transport-runtime/repository/records/detail/content_group_decl.h"



#endif //CPPTRANSPORT_REPOSITORY_RECORDS_DECL_H
