//
// Created by David Seery on 30/03/15.
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


#ifndef CPPTRANSPORT_REPOSITORY_SQLITE3_H
#define CPPTRANSPORT_REPOSITORY_SQLITE3_H


#include <assert.h>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <functional>
#include <utility>

#include "transport-runtime/repository/json_repository.h"
#include "transport-runtime/derived-products/data_products.h"
#include "transport-runtime/sqlite3/operations/repository.h"


#include "transport-runtime/version.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"


#include "boost/filesystem/operations.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "sqlite3.h"


// DECLARE SQLITE3_TRANSACTION_HANDLER
#include "transport-runtime/sqlite3/detail/repository_transaction_handler_decl.h"

// DECLARE REPOSITORY_SQLITE3
#include "transport-runtime/sqlite3/detail/repository_sqlite3_decl.h"

// DEFINE SQLITE3_TRANSACTION_HANDLER
#include "transport-runtime/sqlite3/detail/repository_transaction_handler_impl.h"

// DEFINE REPOSITORY SQLITE3
#include "transport-runtime/sqlite3/detail/repository_sqlite3_impl.h"


// FACTORY FUNCTIONS TO BUILD A REPOSITORY


namespace transport
  {

    template <typename number>
    std::unique_ptr<json_repository<number> > repository_factory(const std::string& path, model_manager<number>& finder,
                                                                 repository_mode mode, local_environment& ev,
                                                                 error_handler e, warning_handler w, message_handler m)
      {
        return std::make_unique<repository_sqlite3<number> >(path, finder, mode, ev, e, w, m);
      }

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_SQLITE3_H
