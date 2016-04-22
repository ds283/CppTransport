//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_DATA_MANAGER_H
#define CPPTRANSPORT_DATA_MANAGER_H


#include <vector>
#include <list>
#include <functional>
#include <memory>

// forward-declare model if needed
#include "transport-runtime/models/model_forward_declare.h"

// need repository record definitions, and writers
#include "transport-runtime/repository/records/repository_records_forward_declare.h"
#include "transport-runtime/repository/writers/writers.h"

// need batcher definitions
#include "transport-runtime/data/batchers/batchers.h"

// need k-configuration databases
#include "transport-runtime/tasks/configuration-database/twopf_config_database.h"
#include "transport-runtime/tasks/configuration-database/threepf_config_database.h"

// need metadata records (worker information, timing information)
#include "transport-runtime/data/metadata.h"

#include "boost/filesystem/operations.hpp"
#include "boost/timer/timer.hpp"
#include "boost/algorithm/string.hpp"


// DECLARE DATAPIPE
#include "transport-runtime/data/datapipe/datapipe_decl.h"

// DECLARE DATA_MANAGER
#include "transport-runtime/data/detail/data_manager_decl.h"

// DEFINE DATAPIPE
#include "transport-runtime/data/datapipe/datapipe_impl.h"

// DEFINE DATA_MANAGER
#include "transport-runtime/data/detail/data_manager_impl.h"


#endif //CPPTRANSPORT_DATA_MANAGER_H
