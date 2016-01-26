//
// Created by David Seery on 19/03/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_SLAVE_CONTROLLER_H
#define CPPTRANSPORT_SLAVE_CONTROLLER_H


#include <list>
#include <vector>
#include <memory>
#include <functional>

#include "transport-runtime-api/models/model.h"
#include "transport-runtime-api/manager/model_manager.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"

#include "transport-runtime-api/manager/mpi_operations.h"

#include "transport-runtime-api/repository/json_repository.h"
#include "transport-runtime-api/data/data_manager.h"
#include "transport-runtime-api/manager/slave_work_handler.h"
#include "transport-runtime-api/manager/environment.h"
#include "transport-runtime-api/manager/message_handlers.h"

#include "transport-runtime-api/scheduler/context.h"
#include "transport-runtime-api/scheduler/scheduler.h"
#include "transport-runtime-api/scheduler/work_queue.h"

#include "transport-runtime-api/utilities/formatter.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/timer/timer.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


// DECLARE SLAVE CONTROLLER
#include "transport-runtime-api/manager/detail/slave_controller_decl.h"

// DECLARE SLAVE DISPATCH FUNCTION
#include "transport-runtime-api/manager/detail/slave_dispatch_decl.h"

// DEFINE SLAVE CONTROLLER
#include "transport-runtime-api/manager/detail/slave_controller_impl.h"

// DEFINE SLAVE DISPATCH FUNCTION
#include "transport-runtime-api/manager/detail/slave_dispatch_impl.h"


#endif //CPPTRANSPORT_SLAVE_CONTROLLER_H
