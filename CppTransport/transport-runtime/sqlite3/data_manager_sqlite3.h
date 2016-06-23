//
// Created by David Seery on 08/01/2014.
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


#ifndef CPPTRANSPORT_DATA_MANAGER_SQLITE3_H
#define CPPTRANSPORT_DATA_MANAGER_SQLITE3_H


#include <sstream>
#include <list>
#include <string>
#include <vector>
#include <memory>

#include "transport-runtime/defaults.h"
#include "transport-runtime/data/data_manager.h"

// need definitions of repository records
#include "transport-runtime/repository/records/repository_records_decl.h"

#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

#include "boost/filesystem/operations.hpp"

#include "sqlite3.h"
#include "transport-runtime/sqlite3/operations/data_manager.h"


// DECLARE CONTAINER REPLACEMENT FUNCTION
#include "transport-runtime/sqlite3/detail/container_replace_decl.h"

// DECLARE *_WRITER HANDLERS
#include "transport-runtime/sqlite3/detail/writer_data_mgr_decl.h"

// DECLARE SQLITE3_TRANSACTION_HANDLER
#include "transport-runtime/sqlite3/detail/data_manager_transaction_handler_decl.h"

// DECLARE DATA_MANAGER_SQLITE3
#include "transport-runtime/sqlite3/detail/data_manager_sqlite3_decl.h"

// DEFINE CONTAINER REPLACEMENT FUNCTION
#include "transport-runtime/sqlite3/detail/container_replace_impl.h"

// DEFINE *_WRITER HANDLERS
#include "transport-runtime/sqlite3/detail/writer_data_mgr_impl.h"

// DEFINE SQLITE3_TRANSACTION_HANDLER
#include "transport-runtime/sqlite3/detail/data_manager_transaction_handler_impl.h"

// DEFINE DATA_MANAGER_SQLITE3
#include "transport-runtime/sqlite3/detail/data_manager_sqlite3_impl.h"


namespace transport
  {

    // FACTORY FUNCTIONS TO BUILD A DATA_MANAGER


    template <typename number>
    std::unique_ptr< data_manager<number> > data_manager_factory(local_environment& e, argument_cache& a)
      {
        return std::make_unique< data_manager_sqlite3<number> >(e, a);
      }

  };   // namespace transport



#endif //CPPTRANSPORT_DATA_MANAGER_SQLITE3_H
