//
// Created by David Seery on 09/08/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_UPGRADEKIT_UPDATE_201801_H
#define CPPTRANSPORT_UPGRADEKIT_UPDATE_201801_H


#include "transport-runtime/transactions/transaction_manager.h"

#include "transport-runtime/sqlite3/operations/data_manager_common.h"
#include "transport-runtime/sqlite3/operations/sqlite3_utility.h"
#include "sqlite3.h"


namespace transport
  {
    
    namespace sqlite3_operations
      {
    
        class update_201801
          {
        
            // CONSTRUCTOR, DESTRUCTOR
      
          public:
        
            //! constructor is default
            update_201801() = default;
        
            //! destructor
            ~update_201801() = default;
        
        
            // INTERFACE
      
          public:
        
            //! update an integration container
            void integration_container(sqlite3* db, transaction_manager& mgr, upgradekit_impl::NotifyGadget& notify);
        
        
            // INTERNAL API
      
          protected:
        
            //! upgrade an integration container worker table
            void update_worker_table(sqlite3* db, transaction_manager& mgr, upgradekit_impl::NotifyGadget& notify);
        
          };
    
    
        void update_201801::integration_container(sqlite3* db, transaction_manager& mgr, upgradekit_impl::NotifyGadget& notify)
          {
            this->update_worker_table(db, mgr, notify);
          }
    
    
        void update_201801::update_worker_table(sqlite3* db, transaction_manager& mgr, upgradekit_impl::NotifyGadget& notify)
          {
            // determine whether cpu_brand column is present
            bool present = false;
            
            // enumerate columns in worker table
            std::ostringstream enum_stmt;
            enum_stmt << "PRAGMA table_info(" << CPPTRANSPORT_SQLITE_WORKERS_TABLE << ");";
            
            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, enum_stmt.str().c_str(), enum_stmt.str().length()+1, &stmt, nullptr));
            
            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    std::string col_name = std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                                                        static_cast<unsigned int>(sqlite3_column_bytes(stmt, 1)) };
                    
                    if(col_name == "cpu_brand")
                      {
                        present = true;
                        break;
                      }
                  }
              }
            
            // if column was present, nothing to do so return
            if(present) return;
            
            // notify that container is being upgraded
            notify();
            
            // amend schema to add column
            std::ostringstream alter_stmt;
            alter_stmt << "ALTER TABLE " << CPPTRANSPORT_SQLITE_WORKERS_TABLE << " ADD COLUMN cpu_brand TEXT;";
            exec(db, alter_stmt.str());
          }
        
      }   // namespace sqlite3_operations
    
  }   // namespace transport


#endif //CPPTRANSPORT_UPGRADEKIT_UPDATE_201801_H
