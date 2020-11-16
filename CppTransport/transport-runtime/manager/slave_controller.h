//
// Created by David Seery on 19/03/15.
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


#ifndef CPPTRANSPORT_SLAVE_CONTROLLER_H
#define CPPTRANSPORT_SLAVE_CONTROLLER_H


#include <list>
#include <vector>
#include <memory>
#include <functional>

#include "transport-runtime/models/model.h"
#include "transport-runtime/manager/model_manager.h"
#include "transport-runtime/tasks/task.h"
#include "transport-runtime/tasks/integration_tasks.h"
#include "transport-runtime/tasks/output_tasks.h"

#include "transport-runtime/manager/mpi_operations.h"

#include "transport-runtime/repository/json_repository.h"
#include "transport-runtime/data/data_manager.h"
#include "transport-runtime/manager/slave_work_handler.h"
#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/message_handlers.h"

#include "transport-runtime/scheduler/context.h"
#include "transport-runtime/scheduler/scheduler.h"
#include "transport-runtime/scheduler/device_queue_manager.h"

#include "transport-runtime/utilities/formatter.h"

#include "transport-runtime/instruments/busyidle_timer_set.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/timer/timer.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


// DECLARE SLAVE CONTROLLER
#include "transport-runtime/manager/detail/slave_controller_decl.h"

// DECLARE SLAVE DISPATCH FUNCTION
#include "transport-runtime/manager/detail/slave_datapipe_dispatch_decl.h"
#include "transport-runtime/manager/detail/slave_container_dispatch_decl.h"

// DEFINE SLAVE CONTROLLER
#include "transport-runtime/manager/detail/slave_controller_impl.h"

// DEFINE SLAVE DISPATCH FUNCTION
#include "transport-runtime/manager/detail/slave_datapipe_dispatch_impl.h"
#include "transport-runtime/manager/detail/slave_container_dispatch_impl.h"


#endif //CPPTRANSPORT_SLAVE_CONTROLLER_H
