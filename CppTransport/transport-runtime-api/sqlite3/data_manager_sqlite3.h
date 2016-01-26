//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_DATA_MANAGER_SQLITE3_H
#define CPPTRANSPORT_DATA_MANAGER_SQLITE3_H


#include <sstream>
#include <list>
#include <string>
#include <vector>
#include <memory>

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/data/data_manager.h"

// need definitions of repository records
#include "transport-runtime-api/repository/records/repository_records_decl.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"

#include "sqlite3.h"
#include "transport-runtime-api/sqlite3/operations/data_manager.h"


// DECLARE DATA_MANAGER_SQLITE3
#include "transport-runtime-api/sqlite3/detail/data_manager_sqlite3_decl.h"

// DECLARE CONTAINER REPLACEMENT FUNCTION
#include "transport-runtime-api/sqlite3/detail/container_replace_decl.h"

// DEFINE DATA_MANAGER_SQLITE3
#include "transport-runtime-api/sqlite3/detail/data_manager_sqlite3_impl.h"

// DEFINE CONTAINER REPLACEMENT FUNCTION
#include "transport-runtime-api/sqlite3/detail/container_replace_impl.h"


namespace transport
  {

    // FACTORY FUNCTIONS TO BUILD A DATA_MANAGER


    template <typename number>
    std::unique_ptr< data_manager<number> > data_manager_factory(unsigned int bcap, unsigned int dcap, unsigned int ckp)
      {
        return std::make_unique< data_manager_sqlite3<number> >(bcap, dcap, ckp);
      }

  };   // namespace transport



#endif //CPPTRANSPORT_DATA_MANAGER_SQLITE3_H
