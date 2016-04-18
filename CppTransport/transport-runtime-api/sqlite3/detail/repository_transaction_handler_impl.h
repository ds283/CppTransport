//
// Created by David Seery on 27/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_TRANSACTION_HANDLER_IMPL_H
#define CPPTRANSPORT_TRANSACTION_HANDLER_IMPL_H


#include "transport-runtime-api/sqlite3/detail/repository_transaction_handler_decl.h"


namespace transport
  {

    template <typename number>
    void repository_sqlite3_transaction_handler<number>::open()
      {
        this->repo.begin_transaction();
      }


    template <typename number>
    void repository_sqlite3_transaction_handler<number>::commit()
      {
        this->repo.commit_transaction();
      }


    template <typename number>
    void repository_sqlite3_transaction_handler<number>::rollback()
      {
        this->repo.abort_transaction();
      }


    template <typename number>
    void repository_sqlite3_transaction_handler<number>::release()
      {
        this->repo.release_transaction();
      }

  }   // namespace transport


#endif //CPPTRANSPORT_TRANSACTION_HANDLER_IMPL_H
