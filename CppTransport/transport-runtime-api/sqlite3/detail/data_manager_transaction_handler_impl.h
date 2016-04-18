//
// Created by David Seery on 27/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DATA_MANAGER_TRANSACTION_HANDLER_IMPL_H
#define CPPTRANSPORT_DATA_MANAGER_TRANSACTION_HANDLER_IMPL_H


#include "transport-runtime-api/sqlite3/detail/data_manager_transaction_handler_decl.h"


namespace transport
  {

    template <typename number>
    void data_manager_sqlite3_transaction_handler<number>::open()
      {
        this->manager.begin_transaction(this->db);
      }


    template <typename number>
    void data_manager_sqlite3_transaction_handler<number>::commit()
      {
        this->manager.commit_transaction(this->db);
      }


    template <typename number>
    void data_manager_sqlite3_transaction_handler<number>::rollback()
      {
        this->manager.abort_transaction(this->db);
      }


    template <typename number>
    void data_manager_sqlite3_transaction_handler<number>::release()
      {
        this->manager.release_transaction();
      }

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_TRANSACTION_HANDLER_IMPL_H
