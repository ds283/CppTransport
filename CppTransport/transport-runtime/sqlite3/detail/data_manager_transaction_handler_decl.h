//
// Created by David Seery on 27/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DATA_MANAGER_SQLITE3_TRANSACTION_HANDLER_DECL_H
#define CPPTRANSPORT_DATA_MANAGER_SQLITE3_TRANSACTION_HANDLER_DECL_H


#include "transport-runtime/transactions/transaction_manager.h"

#include "sqlite3.h"


namespace transport
  {

    // forward-declare data_manager_sqlite3
    template <typename number> class data_manager_sqlite3;

    template <typename number>
    class data_manager_sqlite3_transaction_handler: public transaction_handler
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures data manager instance
        data_manager_sqlite3_transaction_handler(data_manager_sqlite3<number>& r, sqlite3* d)
          : manager(r),
            db(d)
          {
          }

        //! destructor is default
        ~data_manager_sqlite3_transaction_handler() = default;


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

        //! reference to data manager instance
        data_manager_sqlite3<number>& manager;

        //! SQLite3 container handle
        sqlite3* db;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_SQLITE3_TRANSACTION_HANDLER_DECL_H
