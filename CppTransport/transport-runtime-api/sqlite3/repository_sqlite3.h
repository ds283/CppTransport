//
// Created by David Seery on 30/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __repository_sqlite3_H_
#define __repository_sqlite3_H_


#include <assert.h>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <functional>
#include <utility>

#include "transport-runtime-api/repository/json_repository.h"

#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "transport-runtime-api/derived-products/data_products.h"

#include "boost/filesystem/operations.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "sqlite3.h"
#include "transport-runtime-api/sqlite3/operations/repository.h"


#define CPPTRANSPORT_REPO_REPOSITORY_LEAF "database.sqlite"
#define CPPTRANSPORT_REPO_STORE_LEAF      "repository"
#define CPPTRANSPORT_REPO_PACKAGES_LEAF   "packages"
#define CPPTRANSPORT_REPO_TASKS_LEAF      "tasks"
#define CPPTRANSPORT_REPO_PRODUCTS_LEAF   "products"
#define CPPTRANSPORT_REPO_OUTPUT_LEAF     "output"


namespace transport
  {

    // forward-declare 'key' class used to create a repository.
    // the complete declaration is in a separate file,
    // which must be included to allow creation of repositories
    class repository_creation_key;


    //! repository_sqlite3<> implements the repository interface using
    //! libjsoncpp and sqlite3 as the database backend.
    //! This implementation replaces two previous ones, the first
    //! using Oracle DBXML and the second using UnQLite.
    template <typename number>
    class repository_sqlite3: public json_repository<number>
      {

      private:

        class default_error_handler
          {
          public:
            void operator()(const std::string& msg)
              {
                std::cerr << msg << '\n';
              }
          };


        class default_warning_handler
          {
          public:
            void operator()(const std::string& msg)
              {
                std::cout << msg << '\n';
              }
          };


        class default_message_handler
          {
          public:
            void operator()(const std::string& msg)
              {
                std::cout << msg << '\n';
              }
          };


        //! record count callback, used when committing to the database
        typedef std::function <unsigned int(sqlite3*, const std::string&)>                                   count_function;

        //! record store callback, used when committing to the database
        typedef std::function <void(transaction_manager&, sqlite3*, const std::string&, const std::string&)> store_function;

        //! record find function, used when replacing records in the database
        typedef std::function<std::string(sqlite3*, const std::string&)>                                     find_function;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Open a repository with a specified pathname, and specified warning and error handlers
        repository_sqlite3(const std::string& path,
                           typename repository<number>::access_type mode = repository<number>::access_type::readwrite,
                           typename repository<number>::error_callback e = default_error_handler(),
                           typename repository<number>::warning_callback w = default_warning_handler(),
                           typename repository<number>::message_callback m = default_message_handler());

        //! Create a repository with a specific pathname
        repository_sqlite3(const std::string& path, const repository_creation_key& key);

        //! Close a repository, including any corresponding containers
        virtual ~repository_sqlite3();



        // TRANSACTIONS

      protected:

        //! Generate a transaction management object
        transaction_manager transaction_factory();

        //! Begin a transaction on the database.
        void begin_transaction();

        //! Commit a transaction to the database.
        void commit_transaction();

        //! Rollback database to beginning of last transaction
        void abort_transaction();


        // CREATE RECORDS -- implements a 'repository' interface

      public:

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        virtual void commit_package(const initial_conditions<number>& ics) override;

        //! Write an integration task to the database.
        virtual void commit_task(const integration_task<number>& tk) override;

        //! Write an output task to the database
        virtual void commit_task(const output_task<number>& tk) override;

        //! Write a postintegration task to the database
        virtual void commit_task(const postintegration_task<number>& tk) override;

        //! Write a derived product specification
        virtual void commit_derived_product(const derived_data::derived_product<number>& d) override;


        // READ RECORDS FROM THE DATABASE -- implements a 'repository' interface

      public:

        //! Read a package record from the database
        virtual package_record<number>* query_package(const std::string& name) override;

        //! Read a task record from the database
        virtual task_record<number>* query_task(const std::string& name) override;

        //! Read a derived product specification from the database
        virtual derived_product_record<number>* query_derived_product(const std::string& name) override;

        //! Enumerate the output groups available from a named integration task
        virtual std::list<std::shared_ptr < output_group_record<integration_payload> > > enumerate_integration_task_content(const std::string& name) override;

        //! Enumerate the output groups available for a named postintegration task
        virtual std::list<std::shared_ptr < output_group_record<postintegration_payload> > > enumerate_postintegration_task_content(const std::string& name) override;

        //! Enumerate the output groups available from a named output task
        virtual std::list<std::shared_ptr < output_group_record<output_payload> > > enumerate_output_task_content(const std::string& name) override;


        // JSON INTERFACE


        // PULL RECORDS FROM THE REPOSITORY IN JSON FORMAT -- implements a 'json_repository_interface' interface

      public:

        //! export a package record in JSON format
        virtual std::string export_JSON_package_record(const std::string& name) override;

        //! export a task record in JSON format
        virtual std::string export_JSON_task_record(const std::string& name) override;

        //! export a derived product record in JSON format
        virtual std::string export_JSON_product_record(const std::string& name) override;

        //! export a content record in JSON format
        virtual std::string export_JSON_content_record(const std::string& name) override;


        // REPOSITORY RECORD FACTORIES -- USED TO OBTAIN REPOSITORY RECORD CLASSES FROM OTHER REPRESENTATIONS

        // 1. Factories based on the class hierarhcy: these are needed as part of the 'repository<>' interface

      protected:

        //! Create a new package record from an explicit object
        virtual package_record<number>* package_record_factory(const initial_conditions<number>& ics) override;

        //! Create a new integration task record from an explicit object
        virtual integration_task_record<number>* integration_task_record_factory(const integration_task<number>& tk) override;

        //! Create a new output task record from an explicit object
        virtual output_task_record<number>* output_task_record_factory(const output_task<number>& tk) override;

        //! Create a postintegration task record from an explicit object
        virtual postintegration_task_record<number>* postintegration_task_record_factory(const postintegration_task<number>& tk) override;

        //! Create a new derived product record from explicit object
        virtual derived_product_record<number>* derived_product_record_factory(const derived_data::derived_product<number>& prod) override;

        //! Create a new content group for an integration task
        virtual output_group_record<integration_payload>* integration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                   bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) override;

        //! Create a new content group for a postintegration task
        virtual output_group_record<postintegration_payload>* postintegration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                           bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) override;

        //! Create a new content group for an output task
        virtual output_group_record<output_payload>* output_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                         bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) override;

        //! Implementation -- Create a new content group record
        template <typename Payload>
        output_group_record<Payload>* content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                   bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg);

        // 2. Factories from JSON representations: these are needed as part of the 'json_repository<>' interface

      protected:

        //! Create a package record from a JSON value
        virtual package_record<number>* package_record_factory(Json::Value& reader) override;

        //! Create an integration task record from a JSON value
        virtual integration_task_record<number>* integration_task_record_factory(Json::Value& reader) override;

        //! Create an output task record from a JSON value
        virtual output_task_record<number>* output_task_record_factory(Json::Value& reader) override;

        //! Create a postintegration task record from a JSON value
        virtual postintegration_task_record<number>* postintegration_task_record_factory(Json::Value& reader) override;

        //! create a new derived product record from a JSON value
        virtual derived_product_record<number>* derived_product_record_factory(Json::Value& reader) override;

        //! Create a new content group for an integration task from a JSON value
        virtual output_group_record<integration_payload>* integration_content_group_record_factory(Json::Value& reader) override;

        //! Create a new content group for a postintegration task from a JSON value
        virtual output_group_record<postintegration_payload>* postintegration_content_group_record_factory(Json::Value& reader) override;

        //! Create a new content group for an output task from a JSON value
        virtual output_group_record<output_payload>* output_content_group_record_factory(Json::Value& reader) override;

        //! Implementation -- Create a new content group record
        template <typename Payload>
        output_group_record<Payload>* content_group_record_factory(Json::Value& reader);


        // UTILITY FUNCTIONS

      protected:

        //! Check whether a package already exists in the database. Throws an exception if so.
        void check_package_duplicate(const std::string& name);

        //! Check whether a task already exists in the database. Throws an exception if so.
        void check_task_duplicate(const std::string& name);

        //! Check whether a derived product already exists in the database. Throws an exception if so.
        void check_product_duplicate(const std::string& name);

        //! Enumerate content groups
        template <typename Payload>
        void enumerate_content_groups(const std::string& name, std::list< std::shared_ptr < output_group_record<Payload> > >& list, find_function finder);


        // COMMIT CALLBACK METHODS FOR REPOSITORY RECORDS

      protected:

        //! Commit callback: commit to database for the first time
        void commit_first(repository_record& record, count_function counter, store_function storer, std::string store_root, std::string exists_err);

        //! Commit callback: replacement commit to database
        void commit_replace(repository_record& record, find_function finder);

        //! Commit callback: commit integration task to database for the first time (need special treatment)
        void commit_integration_first(repository_record& record, count_function counter, store_function storer, std::string store_root, std::string exists_err);

        //! Commit callback: replacement commit to database for integration tasks (need special treatment)
        void commit_integration_replace(repository_record& record, find_function finder);

        //! Commit k-configuration databases
        void commit_kconfig_database(transaction_manager& mgr, integration_task_record<number>& record);


        // CONTENT GROUP MANAGEMENT

      protected:

        virtual std::string reserve_content_name(const std::string& tk, boost::filesystem::path& parent_path, boost::posix_time::ptime& now, const std::string& suffix) override;


        // REGISTER AND RECOVER WRITERS

      public:

        //! perform recovery on the repository (this is a global operation -- it affects all tasks)
        virtual void perform_recovery(std::shared_ptr< data_manager<number> > data_mgr, unsigned int worker) override;

        //! register an integration writer
        virtual void register_writer(std::shared_ptr <integration_writer<number>>& writer) override;

        //! register a postintegration writer
        virtual void register_writer(std::shared_ptr <postintegration_writer<number>>& writer) override;

        //! register a derived-content writer
        virtual void register_writer(std::shared_ptr <derived_content_writer<number>>& writer) override;

      protected:

        //! recover a list of hot integrations
        void recover_integrations(std::shared_ptr< data_manager<number> > data_mgr, std::list<sqlite3_operations::inflight_integration>& list, unsigned int worker);

        //! recover a list of hot postintegrations
        void recover_postintegrations(std::shared_ptr< data_manager<number> > data_mgr,
                                      std::list<sqlite3_operations::inflight_postintegration>& p_list,
                                      std::list<sqlite3_operations::inflight_integration>& i_list, unsigned int worker);

        //! recover a list of hot derived-content writers
        void recover_derived_content(std::shared_ptr< data_manager<number> > data_mgr, std::list<sqlite3_operations::inflight_derived_content>& list, unsigned int worker);

      protected:

        //! construct an integration_writer in recovery mode
        std::shared_ptr< integration_writer<number> > get_integration_recovery_writer(const sqlite3_operations::inflight_integration& data,
                                                                                      std::shared_ptr< data_manager<number> > data_mgr,
                                                                                      integration_task_record<number>* rec, unsigned int worker);

        //! construct a postintegration_writer in recovery mode
        std::shared_ptr< postintegration_writer<number> > get_postintegration_recovery_writer(const sqlite3_operations::inflight_postintegration& data,
                                                                                              std::shared_ptr< data_manager<number> > data_mgr,
                                                                                              postintegration_task_record<number>* rec, unsigned int worker);

        void recover_unpaired_postintegration(const sqlite3_operations::inflight_postintegration& data, std::shared_ptr< data_manager<number> > data_mgr,
                                              postintegration_task_record<number>* rec, unsigned int worker);

        void recover_paired_postintegration(const sqlite3_operations::inflight_postintegration& data, std::shared_ptr< data_manager<number> > data_mgr,
                                            postintegration_task_record<number>* p_rec,
                                            std::list<sqlite3_operations::inflight_integration>& i_list, unsigned int worker);


        // INTERNAL DATA

      private:

        //! BOOST path to database
        boost::filesystem::path db_path;

        //! relative BOOST path to package store
        boost::filesystem::path package_store;

        //! relative BOOST path to task store
        boost::filesystem::path task_store;

        //! relative BOOST path to derived product store
        boost::filesystem::path product_store;

        //! relative BOOST path to output group store
        boost::filesystem::path output_store;


        // SQLITE3 DATABASE HANDLES

      private:

        //! SQLite database connexion
        sqlite3* db;

      };


    // IMPLEMENTATION


    // Create a repository object associated with a pathname
    template <typename number>
    repository_sqlite3<number>::repository_sqlite3(const std::string& path, typename repository<number>::access_type mode,
                                                   typename repository<number>::error_callback e,
                                                   typename repository<number>::warning_callback w,
                                                   typename repository<number>::message_callback m)
      : json_repository<number>(path, mode, e, w, m,
                                std::bind(&repository_sqlite3<number>::query_package, this, std::placeholders::_1),
                                std::bind(&repository_sqlite3<number>::query_task, this, std::placeholders::_1),
                                std::bind(&repository_sqlite3<number>::query_derived_product, this, std::placeholders::_1)),
        db(nullptr)
      {
        // supplied path should be a directory which exists
        if(!boost::filesystem::is_directory(path))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_ROOT << " '" << path << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // database should be present in an existing directory
        db_path = this->get_root_path() / CPPTRANSPORT_REPO_REPOSITORY_LEAF;
        if(!boost::filesystem::is_regular_file(db_path))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_DATABASE << " '" << path << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // package store should be present
        package_store = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_PACKAGES_LEAF;
        if(!boost::filesystem::is_directory(this->get_root_path() / package_store))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_PACKAGE_STORE << " '" << path << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // task store should be present
        task_store = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_TASKS_LEAF;
        if(!boost::filesystem::is_directory(this->get_root_path() / task_store))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_TASK_STORE << " '" << path << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // product store should be present
        product_store = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_PRODUCTS_LEAF;
        if(!boost::filesystem::is_directory(this->get_root_path() / product_store))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_PRODUCT_STORE << " '" << path << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // output store should be present
        output_store = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_OUTPUT_LEAF;
        if(!boost::filesystem::is_directory(this->get_root_path() / output_store))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_OUTPUT_STORE << " '" << path << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        unsigned int sqlite_mode = (mode == repository<number>::access_type::readonly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE);
        if(sqlite3_open_v2(db_path.string().c_str(), &db, sqlite_mode, nullptr) != SQLITE_OK)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_FAIL_DATABASE_OPEN << " " << db_path;
            throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
          }

        // TODO: consider checking whether required tables are present
      }


    // Create a named repository
    template <typename number>
    repository_sqlite3<number>::repository_sqlite3(const std::string& path, const repository_creation_key& key)
      : json_repository<number>(path, repository<number>::access_type::readwrite,
                                typename repository<number>::error_callback(repository_sqlite3 < number > ::default_error_handler()),
                                typename repository<number>::warning_callback(repository_sqlite3 < number > ::default_warning_handler()),
                                typename repository<number>::message_callback(repository_sqlite3 < number > ::default_message_handler()),
                                std::bind(&repository_sqlite3 < number > ::query_package, this, std::placeholders::_1),
                                std::bind(&repository_sqlite3 < number > ::query_task, this, std::placeholders::_1),
                                std::bind(&repository_sqlite3 < number > ::query_derived_product, this, std::placeholders::_1)),
        db(nullptr)
      {
        // check whether root directory for the repository already exists -- it shouldn't
        if(boost::filesystem::exists(path))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_ROOT_EXISTS << " '" << path << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        db_path       = this->get_root_path() / CPPTRANSPORT_REPO_REPOSITORY_LEAF;
        package_store = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_PACKAGES_LEAF;
        task_store    = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_TASKS_LEAF;
        product_store = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_PRODUCTS_LEAF;
        output_store  = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_OUTPUT_LEAF;

        // create directory structure
        boost::filesystem::create_directories(this->get_root_path());
        boost::filesystem::create_directories(this->get_root_path() / package_store);
        boost::filesystem::create_directories(this->get_root_path() / task_store);
        boost::filesystem::create_directories(this->get_root_path() / product_store);
        boost::filesystem::create_directories(this->get_root_path() / output_store);

        // create container
        if(sqlite3_open_v2(db_path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_FAIL_DATABASE_OPEN << " " << db_path;
            throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
          }

        sqlite3_operations::create_repository_tables(db);
      }


    // Destroy a repository object, closing the associated container
    template <typename number>
    repository_sqlite3<number>::~repository_sqlite3()
      {
        // close open handles if it exists
        if(this->db != nullptr)
          {
            // perform routine maintenance
            if(this->access_mode == repository<number>::access_type::readwrite) sqlite3_operations::exec(this->db, "VACUUM;");

            sqlite3_close(this->db);
          }
      }


    // TRANSACTION MANAGEMENT


    // Begin transaction
    template <typename number>
    void repository_sqlite3<number>::begin_transaction()
      {
        assert(this->db != nullptr);

        sqlite3_operations::exec(this->db, "BEGIN TRANSACTION");
      }


    // End transaction
    template <typename number>
    void repository_sqlite3<number>::commit_transaction()
      {
        assert(this->db != nullptr);

        sqlite3_operations::exec(this->db, "COMMIT");
      }


    // Abort transaction
    template <typename number>
    void repository_sqlite3<number>::abort_transaction()
      {
        assert(this->db != nullptr);

        sqlite3_operations::exec(this->db, "ROLLBACK");
      }


    template <typename number>
    transaction_manager repository_sqlite3<number>::transaction_factory()
      {
        transaction_manager::open_handler     opener   = std::bind(&repository_sqlite3<number>::begin_transaction, this);
        transaction_manager::commit_handler   closer   = std::bind(&repository_sqlite3<number>::commit_transaction, this);
        transaction_manager::rollback_handler rollback = std::bind(&repository_sqlite3<number>::abort_transaction, this);

        return this->repository<number>::transaction_factory(opener, closer, rollback);
      }


    // COMMIT CALLBACKS FOR REPOSITORY RECORDS


    template <typename number>
    void repository_sqlite3<number>::commit_first(repository_record& record,
                                                  typename repository_sqlite3<number>::count_function counter,
                                                  typename repository_sqlite3<number>::store_function storer,
                                                  std::string store_root, std::string exists_err)
      {
        // check that no package with this name already exists
        unsigned int count = counter(this->db, record.get_name());
        if(count > 0)
          {
            std::ostringstream msg;
            msg << exists_err << " '" << record.get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        // commit entry to the database
        boost::filesystem::path document_path = boost::filesystem::path(store_root) / record.get_name();
        document_path += ".json";   // add json extension

        // obtain a lock on the database
        // the transaction manager will roll back any changes if it is not committed
        transaction_manager transaction = this->transaction_factory();

        // store record in database
        storer(transaction, this->db, record.get_name(), document_path.string());

        // store package on disk
        this->commit_JSON_document(transaction, document_path, record);

        // commit
        transaction.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit_integration_first(repository_record& record,
                                                              typename repository_sqlite3<number>::count_function counter,
                                                              typename repository_sqlite3<number>::store_function storer,
                                                              std::string store_root, std::string exists_err)
      {
        integration_task_record<number>& task_record = dynamic_cast<integration_task_record<number>&>(record);

        // check that no package with this name already exists
        unsigned int count = counter(this->db, task_record.get_name());
        if(count > 0)
          {
            std::ostringstream msg;
            msg << exists_err << " '" << task_record.get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        // commit entry to the database

        // set up path to repository record
        boost::filesystem::path document_path = boost::filesystem::path(store_root) / task_record.get_name();
        document_path += ".json";   // add json extension

        // set up path to kconfig-database
        boost::filesystem::path kconfig_database_path = boost::filesystem::path(store_root) / task_record.get_name();
        kconfig_database_path += ".kconfig-db.sqlite";    // add extension
        task_record.set_relative_kconfig_database_path(kconfig_database_path);

        // obtain a lock on the database
        // the transaction manager will roll back any changes if it is not committed
        transaction_manager transaction = this->transaction_factory();

        // store record in database
        storer(transaction, this->db, task_record.get_name(), document_path.string());

        // store package on disk
        this->commit_JSON_document(transaction, document_path, task_record);

        // store kconfiguration database on disk
        this->commit_kconfig_database(transaction, task_record);

        // commit
        transaction.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit_replace(repository_record& record, typename repository_sqlite3<number>::find_function finder)
      {
        // find existing record in the
        boost::filesystem::path document_path = finder(this->db, record.get_name());

        // obtain a lock on the database
        // the transaction manager will roll back any changes if it is not committed
        transaction_manager transaction = this->transaction_factory();

        // replace package on disk
        this->commit_JSON_document(transaction, document_path, record);

        // commit
        transaction.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit_integration_replace(repository_record& record, typename repository_sqlite3<number>::find_function finder)
      {
        integration_task_record<number>& task_record = dynamic_cast<integration_task_record<number>&>(record);

        // find existing record in the database
        boost::filesystem::path document_path = finder(this->db, task_record.get_name());

        // obtain a lock on the database
        // the transaction manager will roll back any changes if it is not committed
        transaction_manager transaction = this->transaction_factory();

        // replace package on disk
        this->commit_JSON_document(transaction, document_path, task_record);

        // replace kconfiguration database on disk
        this->commit_kconfig_database(transaction, task_record);

        // commit
        transaction.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit_kconfig_database(transaction_manager& mgr, integration_task_record<number>& record)
      {
        // write out database to a temporary file
        boost::filesystem::path filename = record.get_relative_kconfig_database_path().filename();
        boost::filesystem::path parent   = record.get_relative_kconfig_database_path();
        parent.remove_filename();
        boost::filesystem::path temp_filename = boost::filesystem::path(filename.string() + "-temp");

        boost::filesystem::path abs_database  = this->get_root_path() / record.get_relative_kconfig_database_path();
        boost::filesystem::path abs_temporary = this->get_root_path() / (parent / temp_filename);

        if(record.get_task()->is_kconfig_database_modified())   // only write out the full database if it is needed
          {
            record.write_kconfig_database(abs_temporary);

            // if this succeeded, add this record to the transaction journal
            mgr.journal_deposit(abs_temporary, abs_database);
          }
      }


    // REPOSITORY RECORD FACTORIES -- USED TO OBTAIN REPOSITORY RECORD CLASSES FROM OTHER REPRESENTATIONS

    // These factories are provided as part of the repository<> or json_repository<> interface
    // In either case, our job is to package up a constructor with appropriate callbacks to commit
    // the record to the database


    template <typename number>
    package_record<number>* repository_sqlite3<number>::package_record_factory(const initial_conditions<number>& ics)
      {
        repository_record::handler_package pkg;
        count_function                     counter = std::bind(&sqlite3_operations::count_packages, std::placeholders::_1, std::placeholders::_2);
        store_function                     storer  = std::bind(&sqlite3_operations::store_package, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->package_store.string(), CPPTRANSPORT_REPO_PACKAGE_EXISTS);

        return new package_record<number>(ics, pkg);
      }


    template <typename number>
    package_record<number>* repository_sqlite3<number>::package_record_factory(Json::Value& reader)
      {
        repository_record::handler_package pkg;
        find_function                      finder = std::bind(&sqlite3_operations::find_package, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_PACKAGE_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new package_record<number>(reader, this->model_finder, pkg);
      }


    template <typename number>
    integration_task_record<number>* repository_sqlite3<number>::integration_task_record_factory(const integration_task<number>& tk)
      {
        repository_record::handler_package pkg;
        count_function                     counter = std::bind(&sqlite3_operations::count_tasks, std::placeholders::_1, std::placeholders::_2);
        store_function                     storer  = std::bind(&sqlite3_operations::store_integration_task, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, tk.get_ics().get_name());
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_integration_first, this, std::placeholders::_1, counter, storer, this->task_store.string(), CPPTRANSPORT_REPO_TASK_EXISTS);

        return new integration_task_record<number>(tk, pkg);
      }


    template <typename number>
    integration_task_record<number>* repository_sqlite3<number>::integration_task_record_factory(Json::Value& reader)
      {
        repository_record::handler_package pkg;
        find_function                      finder = std::bind(&sqlite3_operations::find_integration_task, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_TASK_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_integration_replace, this, std::placeholders::_1, finder);

        return new integration_task_record<number>(reader, this->root_path, this->pkg_finder, pkg);
      }


    template <typename number>
    output_task_record<number>* repository_sqlite3<number>::output_task_record_factory(const output_task<number>& tk)
      {
        repository_record::handler_package pkg;
        count_function                     counter = std::bind(&sqlite3_operations::count_tasks, std::placeholders::_1, std::placeholders::_2);
        store_function                     storer  = std::bind(&sqlite3_operations::store_output_task, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->task_store.string(), CPPTRANSPORT_REPO_TASK_EXISTS);

        return new output_task_record<number>(tk, pkg);
      }


    template <typename number>
    output_task_record<number>* repository_sqlite3<number>::output_task_record_factory(Json::Value& reader)
      {
        repository_record::handler_package pkg;
        find_function                      finder = std::bind(&sqlite3_operations::find_output_task, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_TASK_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new output_task_record<number>(reader, this->dprod_finder, pkg);
      }


    template <typename number>
    postintegration_task_record<number>* repository_sqlite3<number>::postintegration_task_record_factory(const postintegration_task<number>& tk)
      {
        repository_record::handler_package pkg;
        count_function                     counter = std::bind(&sqlite3_operations::count_tasks, std::placeholders::_1, std::placeholders::_2);
        store_function                     storer  = std::bind(&sqlite3_operations::store_postintegration_task, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, tk.get_parent_task()->get_name());
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->task_store.string(), CPPTRANSPORT_REPO_TASK_EXISTS);

        return new postintegration_task_record<number>(tk, pkg);
      }


    template <typename number>
    postintegration_task_record<number>* repository_sqlite3<number>::postintegration_task_record_factory(Json::Value& reader)
      {
        repository_record::handler_package pkg;
        find_function                      finder = std::bind(&sqlite3_operations::find_postintegration_task, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_TASK_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new postintegration_task_record<number>(reader, this->tk_finder, pkg);
      }


    template <typename number>
    derived_product_record<number>* repository_sqlite3<number>::derived_product_record_factory(const derived_data::derived_product<number>& prod)
      {
        repository_record::handler_package pkg;
        count_function                     counter = std::bind(&sqlite3_operations::count_products, std::placeholders::_1, std::placeholders::_2);
        store_function                     storer  = std::bind(&sqlite3_operations::store_product, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->product_store.string(), CPPTRANSPORT_REPO_PRODUCT_EXISTS);

        return new derived_product_record<number>(prod, pkg);
      }


    template <typename number>
    derived_product_record<number>* repository_sqlite3<number>::derived_product_record_factory(Json::Value& reader)
      {
        repository_record::handler_package pkg;
        find_function                      finder = std::bind(&sqlite3_operations::find_product, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_PRODUCT_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new derived_product_record<number>(reader, this->tk_finder, pkg);
      }


    template <typename number>
    output_group_record<integration_payload>* repository_sqlite3<number>::integration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                                   bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg)
      {
        return this->content_group_record_factory<integration_payload>(tn, path, lock, nt, tg);
      }


    template <typename number>
    output_group_record<integration_payload>* repository_sqlite3<number>::integration_content_group_record_factory(Json::Value& reader)
      {
        return this->content_group_record_factory<integration_payload>(reader);
      }


    template <typename number>
    output_group_record<postintegration_payload>* repository_sqlite3<number>::postintegration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                                           bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg)
      {
        return this->content_group_record_factory<postintegration_payload>(tn, path, lock, nt, tg);
      }


    template <typename number>
    output_group_record<postintegration_payload>* repository_sqlite3<number>::postintegration_content_group_record_factory(Json::Value& reader)
      {
        return this->content_group_record_factory<postintegration_payload>(reader);
      }


    template <typename number>
    output_group_record<output_payload>* repository_sqlite3<number>::output_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                         bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg)
      {
        return this->content_group_record_factory<output_payload>(tn, path, lock, nt, tg);
      }


    template <typename number>
    output_group_record<output_payload>* repository_sqlite3<number>::output_content_group_record_factory(Json::Value& reader)
      {
        return this->content_group_record_factory<output_payload>(reader);
      }


    template <typename number>
    template <typename Payload>
    output_group_record<Payload>* repository_sqlite3<number>::content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                           bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg)
      {
        repository_record::handler_package pkg;
        count_function                     counter = std::bind(&sqlite3_operations::count_groups, std::placeholders::_1, std::placeholders::_2);
        store_function                     storer  = std::bind(&sqlite3_operations::store_group<Payload>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, tn);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->output_store.string(), CPPTRANSPORT_REPO_OUTPUT_EXISTS);

        typename output_group_record<Payload>::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = path;

        return new output_group_record<Payload>(tn, paths, lock, nt, tg, pkg);
      }


    template <typename number>
    template <typename Payload>
    output_group_record<Payload>* repository_sqlite3<number>::content_group_record_factory(Json::Value& reader)
      {
        repository_record::handler_package pkg;
        find_function                      finder = std::bind(&sqlite3_operations::find_group<Payload>, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_OUTPUT_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new output_group_record<Payload>(reader, this->root_path, pkg);
      }


    // CREATE RECORDS


    // Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
    template <typename number>
    void repository_sqlite3<number>::commit_package(const initial_conditions<number>& ics)
      {
        // check for a package with a duplicate name
        this->check_package_duplicate(ics.get_name());

        std::unique_ptr <package_record<number>> record(package_record_factory(ics));
        record->commit();
      }


    // Write an integration task to the database.
    template <typename number>
    void repository_sqlite3<number>::commit_task(const integration_task<number>& tk)
      {
        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        std::unique_ptr <integration_task_record<number>> record(integration_task_record_factory(tk));
        record->commit();

        // check whether the initial conditions package for this task is already present; if not, insert it
        unsigned int count = sqlite3_operations::count_packages(this->db, tk.get_ics().get_name());
        if(count == 0)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_AUTOCOMMIT_INTEGRATION_A << " '" << tk.get_name() << "' "
              << CPPTRANSPORT_REPO_AUTOCOMMIT_INTEGRATION_B << " '" << tk.get_ics().get_name() << "'";
            this->message(msg.str());
            this->commit_package(tk.get_ics());
          }
      }


    // Write an output task to the database
    template <typename number>
    void repository_sqlite3<number>::commit_task(const output_task<number>& tk)
      {
        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        std::unique_ptr <output_task_record<number>> record(output_task_record_factory(tk));
        record->commit();

        // check whether derived products on which this task depends have already been committed to the database
        const typename std::vector<output_task_element<number> >               elements = tk.get_elements();
        for(typename std::vector<output_task_element<number> >::const_iterator t        = elements.begin(); t != elements.end(); ++t)
          {
            derived_data::derived_product<number>* product = (*t).get_product();
            unsigned int count = sqlite3_operations::count_products(this->db, product->get_name());
            if(count == 0)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_AUTOCOMMIT_OUTPUT_A << " '" << tk.get_name() << "' "
                  << CPPTRANSPORT_REPO_AUTOCOMMIT_OUTPUT_B << " '" << product->get_name() << "'";
                this->message(msg.str());
                this->commit_derived_product(*product);
              }
          }
      }


    // Write a postintegration task to the database
    template <typename number>
    void repository_sqlite3<number>::commit_task(const postintegration_task<number>& tk)
      {
        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        std::unique_ptr <postintegration_task_record<number>> record(postintegration_task_record_factory(tk));
        record->commit();

        // check whether parent task is already committed to the database
        unsigned int count = sqlite3_operations::count_tasks(this->db, tk.get_parent_task()->get_name());
        if(count == 0)
          {
            derivable_task<number>* ptk = tk.get_parent_task();

            integration_task<number>    * Itk = nullptr;
            postintegration_task<number>* Ptk = nullptr;

            if((Itk = dynamic_cast< integration_task<number>* >(ptk)) != nullptr)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_A << " '" << tk.get_name() << "' "
                  << CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_B << " '" << tk.get_parent_task()->get_name() << "'";
                this->message(msg.str());
                this->commit_task(*Itk);
              }
            else if((Ptk = dynamic_cast< postintegration_task<number>* >(ptk)) != nullptr)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_C << " '" << tk.get_name() << "' "
                  << CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_D << " '" << tk.get_parent_task()->get_name() << "'";
                this->message(msg.str());
                this->commit_task(*Ptk);
              }
          }
      }


    // Write a derived product specification
    template <typename number>
    void repository_sqlite3<number>::commit_derived_product(const derived_data::derived_product<number>& d)
      {
        // check for a derived product with a duplicate name
        this->check_product_duplicate(d.get_name());

        std::unique_ptr <derived_product_record<number>> record(derived_product_record_factory(d));
        record->commit();

        // check whether all tasks on which this derived product depends are already in the database
        typename std::vector<derivable_task<number>*> task_list;
        d.get_task_list(task_list);

        for(typename std::vector<derivable_task<number>*>::iterator t = task_list.begin(); t != task_list.end(); ++t)
          {
            unsigned int count = sqlite3_operations::count_tasks(this->db, (*t)->get_name());
            if(count == 0)
              {
                integration_task<number>    * Itk = nullptr;
                postintegration_task<number>* Ptk = nullptr;

                if((Itk = dynamic_cast< integration_task<number>* >(*t)) != nullptr)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_A << " '" << d.get_name() << "' "
                      << CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_B << " '" << Itk->get_name() << "'";
                    this->message(msg.str());
                    this->commit_task(*Itk);
                  }
                else if((Ptk = dynamic_cast< postintegration_task<number>* >(*t)) != nullptr)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_C << " '" << d.get_name() << "' "
                      << CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_D << " '" << Ptk->get_name() << "'";
                    this->message(msg.str());
                    this->commit_task(*Ptk);
                  }
              }
          }
      }


    // READ RECORDS FROM THE DATABASE


    //! Read a package record from the database
    template <typename number>
    package_record<number>* repository_sqlite3<number>::query_package(const std::string& name)
      {
        boost::filesystem::path filename = sqlite3_operations::find_package(this->db, name, CPPTRANSPORT_REPO_PACKAGE_MISSING);
        Json::Value             root     = this->deserialize_JSON_document(filename);
        return this->package_record_factory(root);
      }


    //! Read a task record from the database
    template <typename number>
    task_record<number>* repository_sqlite3<number>::query_task(const std::string& name)
      {
        if(sqlite3_operations::count_integration_tasks(this->db, name) > 0)
          {
            boost::filesystem::path filename = sqlite3_operations::find_integration_task(this->db, name, CPPTRANSPORT_REPO_TASK_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);
            return this->integration_task_record_factory(root);
          }
        else if(sqlite3_operations::count_postintegration_tasks(this->db, name) > 0)
          {
            boost::filesystem::path filename = sqlite3_operations::find_postintegration_task(this->db, name, CPPTRANSPORT_REPO_TASK_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);
            return this->postintegration_task_record_factory(root);
          }
        else if(sqlite3_operations::count_output_tasks(this->db, name) > 0)
          {
            boost::filesystem::path filename = sqlite3_operations::find_output_task(this->db, name, CPPTRANSPORT_REPO_TASK_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);
            return this->output_task_record_factory(root);
          }

        throw runtime_exception(exception_type::RECORD_NOT_FOUND, name);   // RECORD_NOT_FOUND expects task name in message
      }


    // Read a derived product from the database
    template <typename number>
    derived_product_record<number>* repository_sqlite3<number>::query_derived_product(const std::string& name)
      {
        boost::filesystem::path filename = sqlite3_operations::find_product(this->db, name, CPPTRANSPORT_REPO_PRODUCT_MISSING);
        Json::Value             root     = this->deserialize_JSON_document(filename);
        return this->derived_product_record_factory(root);
      }


    // Enumerate the output groups available from a named integration task
    template <typename number>
    std::list<std::shared_ptr < output_group_record<integration_payload> > >


    repository_sqlite3<number>::enumerate_integration_task_content(const std::string& name)
      {
        std::unique_ptr <task_record<number>> record(this->query_task(name));

        if(record->get_type() != task_record<number>::task_type::integration)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        std::list<std::shared_ptr < output_group_record<integration_payload> > > list;
        find_function finder = std::bind(sqlite3_operations::find_integration_task, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_TASK_MISSING);
        this->enumerate_content_groups<integration_payload>(name, list, finder);

        list.sort(&output_group_helper::comparator<integration_payload>);
        return (list);
      }


    // Enumerate the output groups available from a named postintegration task
    template <typename number>
    std::list<std::shared_ptr < output_group_record<postintegration_payload> > >


    repository_sqlite3<number>::enumerate_postintegration_task_content(const std::string& name)
      {
        std::unique_ptr <task_record<number>> record(this->query_task(name));

        if(record->get_type() != task_record<number>::task_type::postintegration)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_POSTINT << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        std::list<std::shared_ptr < output_group_record<postintegration_payload> > > list;
        find_function finder = std::bind(sqlite3_operations::find_postintegration_task, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_TASK_MISSING);
        this->enumerate_content_groups<postintegration_payload>(name, list, finder);

        list.sort(&output_group_helper::comparator<postintegration_payload>);
        return (list);
      }


    // Enumerate the output groups available from a named output task
    template <typename number>
    std::list<std::shared_ptr < output_group_record<output_payload> > >


    repository_sqlite3<number>::enumerate_output_task_content(const std::string& name)
      {
        std::unique_ptr <task_record<number>> record(this->query_task(name));

        if(record->get_type() != task_record<number>::task_type::output)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_OUTPUT << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        std::list<std::shared_ptr < output_group_record<output_payload> > > list;
        find_function finder = std::bind(sqlite3_operations::find_output_task, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_OUTPUT_MISSING);
        this->enumerate_content_groups<output_payload>(name, list, finder);

        list.sort(&output_group_helper::comparator<output_payload>);
        return (list);
      }


    // JSON INTERFACE


    template <typename number>
    std::string repository_sqlite3<number>::export_JSON_package_record(const std::string& name)
      {
        boost::filesystem::path filename = sqlite3_operations::find_package(this->db, name, CPPTRANSPORT_REPO_PACKAGE_MISSING);
        Json::Value             root     = this->deserialize_JSON_document(filename);

        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, root);
      }


    template <typename number>
    std::string repository_sqlite3<number>::export_JSON_task_record(const std::string& name)
      {
        if(sqlite3_operations::count_integration_tasks(this->db, name) > 0)
          {
            boost::filesystem::path filename = sqlite3_operations::find_integration_task(this->db, name, CPPTRANSPORT_REPO_TASK_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);

            Json::StreamWriterBuilder builder;
            return Json::writeString(builder, root);
          }
        else if(sqlite3_operations::count_postintegration_tasks(this->db, name) > 0)
          {
            boost::filesystem::path filename = sqlite3_operations::find_postintegration_task(this->db, name, CPPTRANSPORT_REPO_TASK_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);

            Json::StreamWriterBuilder builder;
            return Json::writeString(builder, root);
          }
        else if(sqlite3_operations::count_output_tasks(this->db, name) > 0)
          {
            boost::filesystem::path filename = sqlite3_operations::find_output_task(this->db, name, CPPTRANSPORT_REPO_TASK_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);

            Json::StreamWriterBuilder builder;
            return Json::writeString(builder, root);
          }

        std::stringstream msg;
        msg << CPPTRANSPORT_REPO_TASK_MISSING << " '" << name << "'";
        throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
      }


    template <typename number>
    std::string repository_sqlite3<number>::export_JSON_product_record(const std::string& name)
      {
        boost::filesystem::path filename = sqlite3_operations::find_product(this->db, name, CPPTRANSPORT_REPO_PRODUCT_MISSING);
        Json::Value             root     = this->deserialize_JSON_document(filename);

        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, root);
      }


    template <typename number>
    std::string repository_sqlite3<number>::export_JSON_content_record(const std::string& name)
      {
        if(sqlite3_operations::count_integration_groups(this->db, name) > 0)
          {
            boost::filesystem::path filename = sqlite3_operations::find_group<integration_payload>(this->db, name, CPPTRANSPORT_REPO_OUTPUT_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);

            Json::StreamWriterBuilder builder;
            return Json::writeString(builder, root);
          }
        else if(sqlite3_operations::count_postintegration_groups(this->db, name) > 0)
          {
            boost::filesystem::path filename = sqlite3_operations::find_group<postintegration_payload>(this->db, name, CPPTRANSPORT_REPO_OUTPUT_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);

            Json::StreamWriterBuilder builder;
            return Json::writeString(builder, root);
          }
        else if(sqlite3_operations::count_output_groups(this->db, name) > 0)
          {
            boost::filesystem::path filename = sqlite3_operations::find_group<output_payload>(this->db, name, CPPTRANSPORT_REPO_OUTPUT_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);

            Json::StreamWriterBuilder builder;
            return Json::writeString(builder, root);
          }

        std::stringstream msg;
        msg << CPPTRANSPORT_REPO_OUTPUT_MISSING << " '" << name << "'";
        throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
      }


    // DATABASE UTILITY FUNCTIONS


    // Check for a duplicate package name
    template <typename number>
    void repository_sqlite3<number>::check_package_duplicate(const std::string& name)
      {
        if(sqlite3_operations::count_packages(this->db, name) > 0) // should always =1, because primary key constraints in the database prevent duplicates
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_PACKAGE_DUPLICATE << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
      }


    // Check for a duplicate task name
    template <typename number>
    void repository_sqlite3<number>::check_task_duplicate(const std::string& name)
      {
        if(sqlite3_operations::count_tasks(this->db, name) > 0) // should always =1, because primary key constraints in the database prevent duplicates
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TASK_DUPLICATE << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
      }


    // Check for a duplicate task name
    template <typename number>
    void repository_sqlite3<number>::check_product_duplicate(const std::string& name)
      {
        if(sqlite3_operations::count_products(this->db, name) > 0) // should always =1, because primary key constraints in the database prevent duplicates
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_PRODUCT_DUPLICATE << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
      }


    template <typename number>
    template <typename Payload>
    void repository_sqlite3<number>::enumerate_content_groups(const std::string& name, std::list< std::shared_ptr< output_group_record<Payload> > >& list,
                                                              find_function finder)
      {
        std::list<std::string> group_names;

        // get list of group names associated with the task 'name'
        sqlite3_operations::enumerate_content_groups<Payload>(this->db, name, group_names);

        for(std::list<std::string>::iterator t = group_names.begin(); t != group_names.end(); ++t)
          {
            boost::filesystem::path filename = sqlite3_operations::find_group<Payload>(this->db, *t, CPPTRANSPORT_REPO_OUTPUT_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);
            list.push_back(std::shared_ptr<output_group_record<Payload> >(this->template content_group_record_factory<Payload>(root)));
          }
      }


// CONTENT GROUP MANAGEMENT


template <typename number>
std::string repository_sqlite3<number>::reserve_content_name(const std::string& tk, boost::filesystem::path& parent_path, boost::posix_time::ptime& now, const std::string& suffix)
  {
    std::string posix_time_string = boost::posix_time::to_iso_string(now);

    transaction_manager transaction = this->transaction_factory();
    std::string         name        = sqlite3_operations::reserve_content_name(transaction, this->db, tk, parent_path, posix_time_string, suffix);
    transaction.commit();

    return(name);
  }


template <typename number>
void repository_sqlite3<number>::perform_recovery(std::shared_ptr< data_manager<number> > data_mgr, unsigned int worker)
  {
    // get SQLite layer to enumerate hot writers
    std::list<sqlite3_operations::inflight_integration>     hot_integrations;
    std::list<sqlite3_operations::inflight_postintegration> hot_postintegrations;
    std::list<sqlite3_operations::inflight_derived_content> hot_derived_content;

    sqlite3_operations::enumerate_inflight_integrations(this->db, hot_integrations);
    sqlite3_operations::enumerate_inflight_postintegrations(this->db, hot_postintegrations);
    sqlite3_operations::enumerate_inflight_derived_content(this->db, hot_derived_content);

    // recover these writers
    // do postintegrations first to pick up any paired integrations
    this->recover_postintegrations(data_mgr, hot_postintegrations, hot_integrations, worker);

    // recover any integrations left over after handling paired integrations
    this->recover_integrations(data_mgr, hot_integrations, worker);

    this->recover_derived_content(data_mgr, hot_derived_content, worker);
  }


template <typename number>
void repository_sqlite3<number>::recover_integrations(std::shared_ptr< data_manager<number> > data_mgr, std::list<sqlite3_operations::inflight_integration>& list, unsigned int worker)
  {
    // loop through hot integrations; for each one, set up a new integration_writer which is populated
    // with the configuration of the hot writer

    // then, carry out an integrity check and commit the writer

    for(std::list< sqlite3_operations::inflight_integration >::const_iterator t = list.begin(); t != list.end(); ++t)
      {
        // get task record
        std::unique_ptr< task_record<number> > pre_rec(this->query_task(t->task_name));
        integration_task_record<number>* rec = dynamic_cast< integration_task_record<number>* >(pre_rec.get());

        assert(rec != nullptr);
        if(rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

        std::shared_ptr< integration_writer<number> > writer = this->get_integration_recovery_writer(*t, data_mgr, rec, worker);

        // carry out an integrity check; this updates the writer with all missing serial numbers
        // if any are missing, the writer will be marked as failed
        writer->check_integrity(rec->get_task());

        // close writer
        data_mgr->close_writer(writer);

        // commit output
        writer->commit();
      }
  }


template <typename number>
std::shared_ptr< integration_writer<number> >
repository_sqlite3<number>::get_integration_recovery_writer(const sqlite3_operations::inflight_integration& data, std::shared_ptr< data_manager<number> > data_mgr,
                                                            integration_task_record<number>* rec, unsigned int worker)
  {
    // set up a new writer instance for this content group
    std::shared_ptr< integration_writer<number> > writer = this->recover_integration_task_content(data.content_group, rec, data.output, data.container, data.logdir, data.tempdir, worker, data.workgroup_number);

    // initialize writer in recovery mode
    data_mgr->initialize_writer(writer, true);

    // was this container seeded?
    if(data.is_seeded) writer->set_seed(data.seed_group);
    writer->set_collecting_statistics(data.is_collecting_stats);
    writer->set_collecting_initial_conditions(data.is_collecting_ics);

    return writer;
  }


template <typename number>
void repository_sqlite3<number>::recover_postintegrations(std::shared_ptr< data_manager<number> > data_mgr,
                                                          std::list<sqlite3_operations::inflight_postintegration>& p_list,
                                                          std::list<sqlite3_operations::inflight_integration>& i_list,
                                                          unsigned int worker)
  {
    for(std::list<sqlite3_operations::inflight_postintegration>::const_iterator t = p_list.begin(); t != p_list.end(); ++t)
      {
        // get task record
        std::unique_ptr< task_record<number> > pre_rec(this->query_task(t->task_name));
        postintegration_task_record<number>* rec = dynamic_cast< postintegration_task_record<number>* >(pre_rec.get());

        assert(rec != nullptr);
        if(rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

        if(t->is_paired) this->recover_paired_postintegration(*t, data_mgr, rec, i_list, worker);
        else             this->recover_unpaired_postintegration(*t, data_mgr, rec, worker);
      }
  }


template <typename number>
void repository_sqlite3<number>::recover_unpaired_postintegration(const sqlite3_operations::inflight_postintegration& data, std::shared_ptr< data_manager<number> > data_mgr,
                                                                  postintegration_task_record<number>* rec, unsigned int worker)
  {
    std::shared_ptr< postintegration_writer<number> > writer = this->get_postintegration_recovery_writer(data, data_mgr, rec, worker);

    // carry out an integrity check; this updates the writer with all missing serial numbers
    // if any are missing, the writer will be marked as failed
    writer->check_integrity(rec->get_task());

    // close writer
    data_mgr->close_writer(writer);

    // commit output
    writer->commit();
  }


namespace repository_sqlite3_impl
  {

    template <typename InFlightObject>
    class FindInFlight
      {
      public:
        FindInFlight(const std::string& n)
          : name(n)
          {
          }

        bool operator()(const InFlightObject& obj)
          {
            return(obj.content_group == name);
          }

      protected:
        const std::string name;
      };

  }   // namespace repository_sqlite3_impl


template <typename number>
void repository_sqlite3<number>::recover_paired_postintegration(const sqlite3_operations::inflight_postintegration& data, std::shared_ptr< data_manager<number> > data_mgr,
                                                                postintegration_task_record<number>* p_rec,
                                                                std::list<sqlite3_operations::inflight_integration>& i_list, unsigned int worker)
  {
    // try to find paired integration in i_list
    std::list<sqlite3_operations::inflight_integration>::iterator t = std::find_if(i_list.begin(), i_list.end(),
                                                                                   repository_sqlite3_impl::FindInFlight<sqlite3_operations::inflight_integration>(data.parent_group));

    if(t == i_list.end()) this->recover_unpaired_postintegration(data, data_mgr, p_rec, worker);
    else
      {
        // get task record
        std::unique_ptr< task_record<number> > pre_rec(this->query_task(t->task_name));
        integration_task_record<number>* i_rec = dynamic_cast< integration_task_record<number>* >(pre_rec.get());

        assert(i_rec != nullptr);
        if(i_rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

        std::shared_ptr< integration_writer<number> >     i_writer = this->get_integration_recovery_writer(*t, data_mgr, i_rec, worker);
        std::shared_ptr< postintegration_writer<number> > p_writer = this->get_postintegration_recovery_writer(data, data_mgr, p_rec, worker);

        // carry out an integrity check; this updates the writers with all missing serial numbers
        // if any are missing, the writer will be marked as failed
        i_writer->check_integrity(i_rec->get_task());
        p_writer->check_integrity(p_rec->get_task());

        data_mgr->synchronize_missing_serials(i_writer, p_writer, i_rec->get_task(), p_rec->get_task());

        // close writers
        data_mgr->close_writer(i_writer);
        data_mgr->close_writer(p_writer);

        // commit output
        i_writer->commit();
        p_writer->commit();

        // remove interation from list
        i_list.erase(t);
      }
  }


template <typename number>
std::shared_ptr< postintegration_writer<number> >
repository_sqlite3<number>::get_postintegration_recovery_writer(const sqlite3_operations::inflight_postintegration& data, std::shared_ptr< data_manager<number> > data_mgr,
                                                                postintegration_task_record<number>* rec, unsigned int worker)
  {
    // set up a new writer instance for this content group
    std::shared_ptr< postintegration_writer<number> > writer = this->recover_postintegration_task_content(data.content_group, rec, data.output, data.container, data.logdir, data.tempdir, worker);

    // initialize writer in recovery mode
    data_mgr->initialize_writer(writer, true);

    writer->set_pair(data.is_paired);
    writer->set_parent_group(data.parent_group);

    // was this container seeded?
    if(data.is_seeded) writer->set_seed(data.seed_group);

    return writer;
  }


template <typename number>
void repository_sqlite3<number>::recover_derived_content(std::shared_ptr< data_manager<number> > data_mgr, std::list<sqlite3_operations::inflight_derived_content>& list, unsigned int worker)
  {
    // TODO: not implemented
  }


// separate in-flight tables are needed for the writers because when the global
// in-flight entries are written (in reserve_content_name() above) not all information is
// available -- eg. seeding or pairing of the writers.
// In order to recover a content group if a crash occurs we need to be able to
// reconstuct these details, so we need the separate tables


template <typename number>
void repository_sqlite3<number>::register_writer(std::shared_ptr <integration_writer<number>>& writer)
  {
    transaction_manager transaction = this->transaction_factory();

    sqlite3_operations::register_integration_writer(transaction, this->db,
                                                    writer->get_name(),
                                                    writer->get_record()->get_task()->get_name(),
                                                    writer->get_relative_output_path(),
                                                    writer->get_relative_container_path(),
                                                    writer->get_relative_logdir_path(),
                                                    writer->get_relative_tempdir_path(),
                                                    writer->get_workgroup_number(),
                                                    writer->is_seeded(),
                                                    writer->get_seed_group(),
                                                    writer->is_collecting_statistics(),
                                                    writer->is_collecting_initial_conditions());

    transaction.commit();
  }


template <typename number>
void repository_sqlite3<number>::register_writer(std::shared_ptr <postintegration_writer<number>>& writer)
  {
    transaction_manager transaction = this->transaction_factory();

    sqlite3_operations::register_postintegration_writer(transaction, this->db,
                                                        writer->get_name(),
                                                        writer->get_record()->get_task()->get_name(),
                                                        writer->get_relative_output_path(),
                                                        writer->get_relative_container_path(),
                                                        writer->get_relative_logdir_path(),
                                                        writer->get_relative_tempdir_path(),
                                                        writer->is_paired(),
                                                        writer->get_parent_group(),
                                                        writer->is_seeded(),
                                                        writer->get_seed_group());

    transaction.commit();
  }


template <typename number>
void repository_sqlite3<number>::register_writer(std::shared_ptr <derived_content_writer<number>>& writer)
  {
    transaction_manager transaction = this->transaction_factory();

    sqlite3_operations::register_derived_content_writer(transaction, this->db,
                                                        writer->get_name(),
                                                        writer->get_record()->get_task()->get_name(),
                                                        writer->get_relative_output_path(),
                                                        writer->get_relative_logdir_path(),
                                                        writer->get_relative_tempdir_path());

    transaction.commit();
  }


// FACTORY FUNCTIONS TO BUILD A REPOSITORY


template <typename number>
std::shared_ptr< json_repository<number> > repository_factory(const std::string& path,
                                                              typename repository<number>::access_type mode = repository<number>::access_type::readwrite)
  {
    return std::make_shared< repository_sqlite3<number> >(path, mode);
  }


template <typename number>
std::shared_ptr< json_repository<number> > repository_factory(const std::string& path,
                                                              typename repository<number>::access_type mode,
                                                              typename repository<number>::error_callback e,
                                                              typename repository<number>::warning_callback w,
                                                              typename repository<number>::message_callback m)
  {
    return std::make_shared< repository_sqlite3<number> >(path, mode, e, w, m);
  }


template <typename number>
std::shared_ptr< json_repository<number> > repository_factory(const std::string& path, const repository_creation_key& key)
  {
    return std::make_shared< repository_sqlite3<number> >(path, key);
  }


}   // namespace transport


#endif //__repository_sqlite3_H_
