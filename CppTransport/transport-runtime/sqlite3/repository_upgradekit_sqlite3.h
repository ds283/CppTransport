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

#ifndef CPPTRANSPORT_REPOSITORY_UPGRADEKIT_SQLITE3_H
#define CPPTRANSPORT_REPOSITORY_UPGRADEKIT_SQLITE3_H


#include "transport-runtime/repository/repository_upgradekit.h"
#include "transport-runtime/sqlite3/upgradekit/update_201801.h"

#include "transport-runtime/transactions/transaction_manager.h"

#include "transport-runtime/sqlite3/operations/sqlite3_utility.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

#include "sqlite3.h"


namespace transport
  {
    
    // name of lock file; should match name used by data manager in order
    // to prevent a data manager from a concurrent process gaining a lock
    constexpr auto CPPTRANSPORT_UPGRADEKIT_LOCKFILE_LEAF = "lockfile";
    
    
    // forward-declare upgradekit_transaction_handler class, so we can friend it
    namespace upgradekit_sqlite3_impl
      {
        
        template <typename number>
        class DataContainerJanitor;
        
        template <typename number>
        class upgradekit_transaction_handler;
        
      }   // namespace upgradekit_sqlite3_impl
    
    
    //! repository_upgradekit_sqlite3_sqlite3 provides a toolset to upgrade each repository data container
    //! to the latest schema
    template <typename number>
    class repository_upgradekit_sqlite3 : public repository_upgradekit<number>
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        repository_upgradekit_sqlite3(repository<number>& r, argument_cache& ac,
                                      error_handler eh, warning_handler wh, message_handler mh)
          : repository_upgradekit<number>(r, ac, eh, wh, mh)
          {
          }
        
        //! destructor is default
        ~repository_upgradekit_sqlite3() = default;
        
        
        // INTERFACE
      
      public:
        
        //! perform upgrade
        void operator()() override;
        
        
        // INTERNAL API
      
      protected:
        
        //! perform upgrade of integration containers
        void upgrade_integration_containers();
        
        //! upgrade an individual integration data container
        void
        upgrade_integration_container(const boost::filesystem::path& ctr, const boost::filesystem::path& lock);
        
        
        // TRANSACTIONS
      
      protected:
        
        //! generate a transaction on a SQLite database
        std::unique_ptr<transaction_manager>
        transaction_factory(sqlite3* db, boost::filesystem::path lockfile);
        
        
        //! Begin a transaction on the database.
        void begin_transaction(sqlite3* db);
        
        //! Commit a transaction to the database.
        void commit_transaction(sqlite3* db);
        
        //! Rollback database to beginning of last transaction
        void abort_transaction(sqlite3* db);
        
        //! Release a transaction on the database
        void release_transaction();
        
        
        friend class upgradekit_sqlite3_impl::upgradekit_transaction_handler<number>;
        friend class upgradekit_sqlite3_impl::DataContainerJanitor<number>;
        
      };
    
    
    namespace upgradekit_sqlite3_impl
      {
        
        template <typename number>
        class upgradekit_transaction_handler: public transaction_handler
          {
            
            // CONSTRUCTOR, DESTRUCTOR
          
          public:
            
            //! constructor captures upgradekit instance
            upgradekit_transaction_handler(repository_upgradekit_sqlite3<number>& k, sqlite3* d)
              : upgradekit(k),
                db(d)
              {
              }
            
            //! destructor is default
            ~upgradekit_transaction_handler() = default;
            
            
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
            
            //! reference to repository_upgradekit_sqlite3_sqlite3 instance
            repository_upgradekit_sqlite3<number>& upgradekit;
            
            //! SQLite3 container handle
            sqlite3* db;
            
          };
        
        
        template <typename number>
        void upgradekit_transaction_handler<number>::open()
          {
            this->upgradekit.begin_transaction(this->db);
          }
        
        
        template <typename number>
        void upgradekit_transaction_handler<number>::commit()
          {
            this->upgradekit.commit_transaction(this->db);
          }
        
        
        template <typename number>
        void upgradekit_transaction_handler<number>::rollback()
          {
            this->upgradekit.abort_transaction(this->db);
          }
        
        
        template <typename number>
        void upgradekit_transaction_handler<number>::release()
          {
            this->upgradekit.release_transaction();
          }
        
        
        template <typename number>
        class DataContainerJanitor
          {
            
            // CONSTRUCTOR, DESTRUCTOR
          
          public:
            
            //! constructor opens SQLite container
            DataContainerJanitor(boost::filesystem::path ctr, boost::filesystem::path lock,
                                 repository_upgradekit_sqlite3<number>& k);
            
            //! destructor closes SQLite handle
            ~DataContainerJanitor();
            
            
            // INTERFACE
          
          public:
            
            //! get transaction manager instance
            transaction_manager& get_manager() { return *this->mgr; }
            
            //! get raw SQLite 3 handle
            sqlite3* get_handle() { return this->db; }
            
            
            // INTERNAL DATA
          
          private:
            
            //! sqlite3 handle
            sqlite3* db;
            
            //! transaction manager instance
            std::unique_ptr< transaction_manager > mgr;
            
          };
        
        
        template <typename number>
        DataContainerJanitor<number>::DataContainerJanitor(boost::filesystem::path ctr, boost::filesystem::path lock,
                                                           repository_upgradekit_sqlite3<number>& k)
          : db(nullptr)
          {
            int status = sqlite3_open_v2(ctr.string().c_str(), &db, SQLITE_OPEN_READWRITE, nullptr);
            
            if(status != SQLITE_OK)
              {
                std::ostringstream msg;
                if(db != nullptr)
                  {
                    msg << CPPTRANSPORT_DATACTR_UPGRADE_OPEN_FAIL_A << " '" << ctr.string() << "' "
                        << CPPTRANSPORT_DATACTR_UPGRADE_OPEN_FAIL_B << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_close(db);
                  }
                else
                  {
                    msg << CPPTRANSPORT_DATACTR_UPGRADE_OPEN_FAIL_A << " '" << ctr.string() << "' "
                        << CPPTRANSPORT_DATACTR_UPGRADE_OPEN_FAIL_B << status << ")";
                  }
                throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
              }
            
            sqlite3_extended_result_codes(db, 1);

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
            sqlite3_operations::consistency_pragmas(db, this->args.get_network_mode());
#else
            // change setting to optimize SQLite performance
            sqlite3_operations::container_write_pragmas(db, k.args.get_network_mode());
#endif
            
            // gain container lock
            mgr = k.transaction_factory(db, lock);
          }
        
        
        template <typename number>
        DataContainerJanitor<number>::~DataContainerJanitor()
          {
            // release transaction manager
            this->mgr.release();
            
            sqlite3_close(this->db);
          }
        
      }
    
    
    template <typename number>
    void repository_upgradekit_sqlite3<number>::operator()()
      {
        // get global lock on the repository
        transaction_manager mgr = this->repo.transaction_factory();
        
        // currently only integration data containers require upgrade work
        this->upgrade_integration_containers();
        
        // release repository lock
        mgr.commit();
      }
    
    
    template <typename number>
    void repository_upgradekit_sqlite3<number>::upgrade_integration_containers()
      {
        // obtain list of integration content groups
        integration_content_db groups = this->repo.enumerate_integration_task_content();
        
        const auto& root = this->repo.get_root_path();
        
        for(auto& item : groups)
          {
            const auto& rec = *item.second;
            
            const auto ctr = root / rec.get_payload().get_container_path();
            const auto lock = rec.get_abs_output_path() / CPPTRANSPORT_UPGRADEKIT_LOCKFILE_LEAF;
            
            this->upgrade_integration_container(ctr, lock);
          }
      }
    
    
    template <typename number>
    void
    repository_upgradekit_sqlite3<number>::upgrade_integration_container(const boost::filesystem::path& ctr,
                                                                         const boost::filesystem::path& lock)
      {
        // open container and gain lock; closes container automatically on exit
        upgradekit_sqlite3_impl::DataContainerJanitor<number> janitor{ctr, lock, *this};
        
        // get transaction manager generated by janitor
        transaction_manager& mgr = janitor.get_manager();
        sqlite3* db = janitor.get_handle();
        
        // construct notification gadget
        upgradekit_impl::NotifyGadget notify{ctr, this->msg};
        
        // perform upgrades introduced in 2018.1
        sqlite3_operations::update_201801 h201801;
        h201801.integration_container(db, mgr, notify);

        // release container clock
        mgr.commit();
      }
    
    
    template <typename number>
    void repository_upgradekit_sqlite3<number>::begin_transaction(sqlite3* db)
      {
        assert(db != nullptr);
        
        sqlite3_operations::exec(db, "BEGIN TRANSACTION;");
      }
    
    
    template <typename number>
    void repository_upgradekit_sqlite3<number>::commit_transaction(sqlite3* db)
      {
        assert(db != nullptr);
        
        sqlite3_operations::exec(db, "COMMIT;");
      }
    
    
    template <typename number>
    void repository_upgradekit_sqlite3<number>::abort_transaction(sqlite3* db)
      {
        assert(db != nullptr);
        
        sqlite3_operations::exec(db, "ROLLBACK;");
      }
    
    
    template <typename number>
    std::unique_ptr<transaction_manager>
    repository_upgradekit_sqlite3<number>::transaction_factory(sqlite3* db, boost::filesystem::path lockfile)
      {
        if(this->transactions > 0) throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_UNDERWAY);
        this->transactions++;
        
        auto handle = std::make_unique< upgradekit_sqlite3_impl::upgradekit_transaction_handler<number> >(*this, db);
        
        return std::make_unique<transaction_manager>(std::move(lockfile), std::move(handle));
      }
    
    
    template <typename number>
    void
    repository_upgradekit_sqlite3<number>::release_transaction()
      {
        if(this->transactions == 0) throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_OVER_RELEASE);
        this->transactions--;
      }
    
  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_UPGRADEKIT_SQLITE3_H
