//
// Created by David Seery on 27/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_SQLITE3_TRANSACTION_HANDLER_DECL_H
#define CPPTRANSPORT_REPOSITORY_SQLITE3_TRANSACTION_HANDLER_DECL_H


#include "transport-runtime-api/transactions/transaction_manager.h"


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
