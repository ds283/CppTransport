//
// Created by David Seery on 27/01/2016.
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

#ifndef CPPTRANSPORT_REPOSITORY_SQLITE3_TRANSACTION_HANDLER_DECL_H
#define CPPTRANSPORT_REPOSITORY_SQLITE3_TRANSACTION_HANDLER_DECL_H


#include "transport-runtime/transactions/transaction_manager.h"


namespace transport
  {

    // forward-declare repository_sqlite3
    template <typename number> class repository_sqlite3;

    template <typename number>
    class repository_sqlite3_transaction_handler: public transaction_handler
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository instance
        repository_sqlite3_transaction_handler(repository_sqlite3<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~repository_sqlite3_transaction_handler() = default;


        // INTERFACE

      public:

        //! open transaction
        void open() override;

        //! commit transaction
        void commit() override;

        //! rollback transaction
        void rollback() override;

        //! release transaction
        void release() override;


        // INTERNAL DATA

      private:

        //! reference to repository instance
        repository_sqlite3<number>& repo;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_SQLITE3_TRANSACTION_HANDLER_DECL_H
