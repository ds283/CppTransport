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

#ifndef CPPTRANSPORT_REPOSITORY_SQLITE3_IMPL_H
#define CPPTRANSPORT_REPOSITORY_SQLITE3_IMPL_H


namespace transport
  {

    // IMPLEMENTATION


    // Create a repository object associated with a pathname
    template <typename number>
    repository_sqlite3<number>::repository_sqlite3(const boost::filesystem::path path, model_manager<number>& f, repository_mode mode,
                                                   local_environment& ev, argument_cache& ar)
      : json_repository<number>(path, f, mode, ev, ar, package_finder<number>(*this), task_finder<number>(*this), derived_product_finder<number>(*this)),
        db(nullptr)
      {
        // check whether object exists in filesystem at the specified path; if not, we create it
        if(!boost::filesystem::exists(this->get_root_path()))
          {
            if(mode == repository_mode::readwrite)
              {
                this->create_repository();
              }
          }
        else
          {
            this->validate_repository();

            unsigned int sqlite_mode = (mode == repository_mode::readonly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE);
            if(sqlite3_open_v2(db_path.string().c_str(), &db, sqlite_mode, nullptr) != SQLITE_OK)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_FAIL_DATABASE_OPEN << " " << db_path;
                throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            // set performance-related options
            sqlite3_operations::reading_pragmas(db, this->args.get_network_mode());
          }
      }


    template <typename number>
    void repository_sqlite3<number>::validate_repository()
      {
        boost::filesystem::path path = this->get_root_path();

        // supplied path should be a directory which exists
        if(!boost::filesystem::is_directory(path))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_ROOT << " '" << path.string() << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // database should be present in an existing directory
        db_path = this->get_root_path() / CPPTRANSPORT_REPO_REPOSITORY_LEAF;
        if(!boost::filesystem::is_regular_file(db_path))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_DATABASE << " '" << path.string() << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // package store should be present
        package_store = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_PACKAGES_LEAF;
        if(!boost::filesystem::is_directory(this->get_root_path() / package_store))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_PACKAGE_STORE << " '" << path.string() << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // task store should be present
        task_store = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_TASKS_LEAF;
        if(!boost::filesystem::is_directory(this->get_root_path() / task_store))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_TASK_STORE << " '" << path.string() << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // product store should be present
        product_store = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_PRODUCTS_LEAF;
        if(!boost::filesystem::is_directory(this->get_root_path() / product_store))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_PRODUCT_STORE << " '" << path.string() << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // output store should be present
        output_store = boost::filesystem::path(CPPTRANSPORT_REPO_STORE_LEAF) / CPPTRANSPORT_REPO_OUTPUT_LEAF;
        if(!boost::filesystem::is_directory(this->get_root_path() / output_store))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_MISSING_OUTPUT_STORE << " '" << path.string() << "'";
            throw runtime_exception(exception_type::REPO_NOT_FOUND, msg.str());
          }

        // TODO: consider checking whether required tables are present
      }


    // Create a named repository
    template <typename number>
    void repository_sqlite3<number>::create_repository()
      {
        // guaranteed that nothing stored in filesystem at path
        boost::filesystem::path path = this->get_root_path();

        db_path       = path / CPPTRANSPORT_REPO_REPOSITORY_LEAF;
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

        // enable foreign key constraints
        char* errmsg;
        sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errmsg);

        sqlite3_operations::create_repository_tables(db);
      }


    // Destroy a repository object, closing the associated container
    template <typename number>
    repository_sqlite3<number>::~repository_sqlite3()
      {
        // close open handles if it exists
        if(this->db != nullptr)
          {
            // perform routine maintenance if in read/write mode
            if(this->access_mode == repository_mode::readwrite) sqlite3_operations::exec(this->db, "VACUUM;");

            sqlite3_close(this->db);
          }
      }


    // TRANSACTION MANAGEMENT


    // Begin transaction on the underlying SQLite3 database
    template <typename number>
    void repository_sqlite3<number>::begin_transaction()
      {
        assert(this->db != nullptr);

        sqlite3_operations::exec(this->db, "BEGIN TRANSACTION;");

        // forward to abstract repository instance
        this->repository<number>::begin_transaction();
      }


    // End transaction on the underlying SQLite3 database
    template <typename number>
    void repository_sqlite3<number>::commit_transaction()
      {
        assert(this->db != nullptr);

        sqlite3_operations::exec(this->db, "COMMIT");

        // forward to abstract repository instance
        this->repository<number>::commit_transaction();
      }


    // Abort transaction on the underlying SQLite3 database
    template <typename number>
    void repository_sqlite3<number>::abort_transaction()
      {
        assert(this->db != nullptr);

        sqlite3_operations::exec(this->db, "ROLLBACK;");

        // forward to abstract repository instance
        this->repository<number>::abort_transaction();
      }


    // Release transaction; there's nothing specific for us to do here except pass control to
    // the lower-lying repository<> method
    template <typename number>
    void repository_sqlite3<number>::release_transaction()
      {
        assert(this->db != nullptr);

        this->repository<number>::release_transaction();
      }


    template <typename number>
    transaction_manager repository_sqlite3<number>::transaction_factory()
      {
        // generate a transaction handler bundle
        std::unique_ptr< repository_sqlite3_transaction_handler<number> > handle = std::make_unique< repository_sqlite3_transaction_handler<number> >(*this);

        // construct transaction manager with the SQLite handler
        return this->repository<number>::generate_transaction_manager(std::move(handle));
      }


    // COMMIT CALLBACKS FOR REPOSITORY RECORDS


    template <typename number>
    void repository_sqlite3<number>::commit_first(repository_record& record, transaction_manager& mgr,
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

        // store record in database
        storer(mgr, this->db, record.get_name(), document_path.string());

        // store package on disk
        this->commit_JSON_document(mgr, document_path, record);
      }


    template <typename number>
    void repository_sqlite3<number>::commit_integration_first(repository_record& record, transaction_manager& mgr,
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

        // store record in database
        storer(mgr, this->db, task_record.get_name(), document_path.string());

        // store package on disk
        this->commit_JSON_document(mgr, document_path, task_record);

        // store kconfiguration database on disk
        this->commit_kconfig_database(mgr, task_record);
      }


    template <typename number>
    void repository_sqlite3<number>::commit_replace(repository_record& record, transaction_manager& mgr,
                                                    typename repository_sqlite3<number>::find_function finder)
      {
        // find existing record in the
        boost::filesystem::path document_path = finder(this->db, record.get_name());

        // replace package on disk
        this->commit_JSON_document(mgr, document_path, record);
      }


    template <typename number>
    void repository_sqlite3<number>::commit_integration_replace(repository_record& record, transaction_manager& mgr,
                                                                typename repository_sqlite3<number>::find_function finder)
      {
        integration_task_record<number>& task_record = dynamic_cast<integration_task_record<number>&>(record);

        // find existing record in the database
        boost::filesystem::path document_path = finder(this->db, task_record.get_name());

        // replace package on disk
        this->commit_JSON_document(mgr, document_path, task_record);

        // replace kconfiguration database on disk
        this->commit_kconfig_database(mgr, task_record);
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
    std::unique_ptr< package_record<number> > repository_sqlite3<number>::package_record_factory(const initial_conditions<number>& ics, transaction_manager& mgr)
      {
        count_function counter = std::bind(&sqlite3_operations::count_packages, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_package, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        // for newly-created records, access mode is inherited from the repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, std::placeholders::_2, counter, storer, this->package_store.string(), CPPTRANSPORT_REPO_PACKAGE_EXISTS),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< package_record<number> >(ics, pkg);
      }


    template <typename number>
    std::unique_ptr< package_record<number> > repository_sqlite3<number>::package_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr)
      {
        find_function finder = std::bind(&sqlite3_operations::find_package, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_PACKAGE_MISSING);

        // for deserialized records, access mode must be specified explicitly -- but is respected only if we are ourselves a read/write repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, std::placeholders::_2, finder),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< package_record<number> >(reader, this->m_finder, pkg);
      }


    template <typename number>
    std::unique_ptr< integration_task_record<number> > repository_sqlite3<number>::integration_task_record_factory(const twopf_task<number>& tk, transaction_manager& mgr)
      {
        return this->integration_task_record_factory_generic(tk, mgr);
      }


    template <typename number>
    std::unique_ptr< integration_task_record<number> > repository_sqlite3<number>::integration_task_record_factory(const threepf_task<number>& tk, transaction_manager& mgr)
      {
        return this->integration_task_record_factory_generic(tk, mgr);
      }


    template <typename number>
    template <typename TaskType>
    std::unique_ptr< integration_task_record<number> > repository_sqlite3<number>::integration_task_record_factory_generic(const TaskType& tk, transaction_manager& mgr)
      {
        count_function counter = std::bind(&sqlite3_operations::count_tasks, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_integration_task, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, tk.get_ics().get_name());

        // for newly-created records, access mode is inherited from the repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_integration_first, this, std::placeholders::_1, std::placeholders::_2, counter, storer, this->task_store.string(), CPPTRANSPORT_REPO_TASK_EXISTS),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< integration_task_record<number> >(tk, pkg);
      }


    template <typename number>
    std::unique_ptr< integration_task_record<number> > repository_sqlite3<number>::integration_task_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr)
      {
        find_function finder = std::bind(&sqlite3_operations::find_integration_task, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_TASK_MISSING);

        // for deserialized records, access mode must be specified explicitly -- but is respected only if we are ourselves a read/write repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_integration_replace, this, std::placeholders::_1, std::placeholders::_2, finder),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< integration_task_record<number> >(reader, this->root_path, this->pkg_finder, pkg);
      }


    template <typename number>
    std::unique_ptr< output_task_record<number> > repository_sqlite3<number>::output_task_record_factory(const output_task<number>& tk, transaction_manager& mgr)
      {
        count_function counter = std::bind(&sqlite3_operations::count_tasks, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_output_task, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        // for newly-created records, access mode is inherited from the repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, std::placeholders::_2, counter, storer, this->task_store.string(), CPPTRANSPORT_REPO_TASK_EXISTS),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< output_task_record<number> >(tk, pkg);
      }


    template <typename number>
    std::unique_ptr< output_task_record<number> > repository_sqlite3<number>::output_task_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr)
      {
        find_function finder = std::bind(&sqlite3_operations::find_output_task, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_TASK_MISSING);

        // for deserialized records, access mode must be specified explicitly -- but is respected only if we are ourselves a read/write repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, std::placeholders::_2, finder),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< output_task_record<number> >(reader, this->dprod_finder, pkg);
      }


    template <typename number>
    std::unique_ptr< postintegration_task_record<number> > repository_sqlite3<number>::postintegration_task_record_factory(const zeta_twopf_task<number>& tk, transaction_manager& mgr)
      {
        return this->postintegration_task_record_factory_generic(tk, mgr);
      }


    template <typename number>
    std::unique_ptr< postintegration_task_record<number> > repository_sqlite3<number>::postintegration_task_record_factory(const zeta_threepf_task<number>& tk, transaction_manager& mgr)
      {
        return this->postintegration_task_record_factory_generic(tk, mgr);
      }


    template <typename number>
    std::unique_ptr< postintegration_task_record<number> > repository_sqlite3<number>::postintegration_task_record_factory(const fNL_task<number>& tk, transaction_manager& mgr)
      {
        return this->postintegration_task_record_factory_generic(tk, mgr);
      }


    template <typename number>
    template <typename TaskType>
    std::unique_ptr< postintegration_task_record<number> > repository_sqlite3<number>::postintegration_task_record_factory_generic(const TaskType& tk, transaction_manager& mgr)
      {
        count_function counter = std::bind(&sqlite3_operations::count_tasks, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_postintegration_task, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, tk.get_parent_task()->get_name());

        // for newly-created records, access mode is inherited from the repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, std::placeholders::_2, counter, storer, this->task_store.string(), CPPTRANSPORT_REPO_TASK_EXISTS),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< postintegration_task_record<number> >(tk, pkg);
      }


    template <typename number>
    std::unique_ptr< postintegration_task_record<number> > repository_sqlite3<number>::postintegration_task_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr)
      {
        find_function finder = std::bind(&sqlite3_operations::find_postintegration_task, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_TASK_MISSING);

        // for deserialized records, access mode must be specified explicitly -- but is respected only if we are ourselves a read/write repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, std::placeholders::_2, finder),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< postintegration_task_record<number> >(reader, this->tk_finder, pkg);
      }


    template <typename number>
    std::unique_ptr< derived_product_record<number> > repository_sqlite3<number>::derived_product_record_factory(const derived_data::derived_product<number>& prod, transaction_manager& mgr)
      {
        count_function counter = std::bind(&sqlite3_operations::count_products, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_product, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        // for newly-created records, access mode is inherited from the repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, std::placeholders::_2, counter, storer, this->product_store.string(), CPPTRANSPORT_REPO_PRODUCT_EXISTS),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< derived_product_record<number> >(prod, pkg);
      }


    template <typename number>
    std::unique_ptr< derived_product_record<number> > repository_sqlite3<number>::derived_product_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr)
      {
        find_function finder = std::bind(&sqlite3_operations::find_product, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_PRODUCT_MISSING);

        // for deserialized records, access mode must be specified explicitly -- but is respected only if we are ourselves a read/write repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, std::placeholders::_2, finder),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< derived_product_record<number> >(reader, this->tk_finder, pkg);
      }


    template <typename number>
    std::unique_ptr< content_group_record<integration_payload> > repository_sqlite3<number>::integration_content_group_record_factory(integration_writer<number>& writer,
                                                                                                                                      bool lock, const std::list<note>& nt,
                                                                                                                                      transaction_manager& mgr)
      {
        return this->content_group_record_factory<integration_payload>(writer, lock, nt, mgr);
      }


    template <typename number>
    std::unique_ptr< content_group_record<integration_payload> > repository_sqlite3<number>::integration_content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr)
      {
        return this->content_group_record_factory<integration_payload>(reader, mgr);
      }


    template <typename number>
    std::unique_ptr< content_group_record<postintegration_payload> > repository_sqlite3<number>::postintegration_content_group_record_factory(postintegration_writer<number>& writer,
                                                                                                                                              bool lock, const std::list<note>& nt,
                                                                                                                                              transaction_manager& mgr)
      {
        return this->content_group_record_factory<postintegration_payload>(writer, lock, nt, mgr);
      }


    template <typename number>
    std::unique_ptr< content_group_record<postintegration_payload> > repository_sqlite3<number>::postintegration_content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr)
      {
        return this->content_group_record_factory<postintegration_payload>(reader, mgr);
      }


    template <typename number>
    std::unique_ptr< content_group_record<output_payload> > repository_sqlite3<number>::output_content_group_record_factory(derived_content_writer<number>& writer,
                                                                                                                            bool lock, const std::list<note>& nt,
                                                                                                                            transaction_manager& mgr)
      {
        return this->content_group_record_factory<output_payload>(writer, lock, nt, mgr);
      }


    template <typename number>
    std::unique_ptr< content_group_record<output_payload> > repository_sqlite3<number>::output_content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr)
      {
        return this->content_group_record_factory<output_payload>(reader, mgr);
      }


    template <typename number>
    template <typename Payload, typename WriterObject>
    std::unique_ptr< content_group_record<Payload> > repository_sqlite3<number>::content_group_record_factory(WriterObject& writer,
                                                                                                              bool lock, const std::list<note>& notes,
                                                                                                              transaction_manager& mgr)
      {
        const std::string& task_name = writer.get_task_name();
        const boost::filesystem::path& relative_output_path = writer.get_relative_output_path();
        const std::list<std::string> tags = writer.get_tags();
        const boost::posix_time::ptime& creation_time = writer.get_creation_time();

        count_function counter = std::bind(&sqlite3_operations::count_groups, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_group<Payload>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                                           task_name, creation_time);

        // for created records, read/write status is inherited from repository
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, std::placeholders::_2, counter, storer, this->output_store.string(), CPPTRANSPORT_REPO_OUTPUT_EXISTS),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        typename content_group_record<Payload>::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = relative_output_path;

        return std::make_unique< content_group_record<Payload> >(task_name, paths, lock, notes, tags, pkg);
      }


    template <typename number>
    template <typename Payload>
    std::unique_ptr< content_group_record<Payload> > repository_sqlite3<number>::content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr)
      {
        find_function finder = std::bind(&sqlite3_operations::find_group<Payload>, std::placeholders::_1, std::placeholders::_2, CPPTRANSPORT_REPO_OUTPUT_MISSING);

        // for deserialized records, read/write status must be explicitly specified
        repository_record::handler_package pkg(std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, std::placeholders::_2, finder),
                                               this->env, this->access_mode == repository_mode::readwrite ? mgr : boost::optional<transaction_manager&>());

        return std::make_unique< content_group_record<Payload> >(reader, this->root_path, pkg);
      }


    // CREATE RECORDS


    // Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.

    template <typename number>
    void repository_sqlite3<number>::commit(const initial_conditions<number>& ics)
      {
        transaction_manager mgr = this->transaction_factory();
        this->commit(mgr, ics);
        mgr.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit(transaction_manager& mgr, const initial_conditions<number>& ics)
      {
        // check for a package with a duplicate name
        this->check_package_duplicate(ics.get_name());

        // create a record and commit it; the commit will throw an exception if the repository is not in a read/write mode
        std::unique_ptr< package_record<number> > record = package_record_factory(ics, mgr);
        record->commit();
      }


    // Write an integration task to the database.

    template <typename number>
    void repository_sqlite3<number>::commit(const twopf_task<number>& tk)
      {
        transaction_manager mgr = this->transaction_factory();
        this->commit_integration_task(mgr, tk);
        mgr.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit(transaction_manager& mgr, const twopf_task<number>& tk)
      {
        this->commit_integration_task(mgr, tk);
      }


    template <typename number>
    void repository_sqlite3<number>::commit(const threepf_task<number>& tk)
      {
        transaction_manager mgr = this->transaction_factory();
        this->commit_integration_task(mgr, tk);
        mgr.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit(transaction_manager& mgr, const threepf_task<number>& tk)
      {
        this->commit_integration_task(mgr, tk);
      }


    template <typename number>
    template <typename TaskType>
    void repository_sqlite3<number>::commit_integration_task(transaction_manager& mgr, const TaskType& tk)
      {
        if(!tk.is_serializable())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_NOT_SERIALIZABLE << " '" << tk.get_name() << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        // check whether the initial conditions package for this task is already present; if not, insert it;
        // this has to be done first in order to enforce foreign key constraints in the repository database
        unsigned int count = sqlite3_operations::count_packages(this->db, tk.get_ics().get_name());
        if(count == 0) this->commit(mgr, tk.get_ics());

        // create a record and commit it; the commit will throw an exception if the repository is not in a read/write mode
        std::unique_ptr< integration_task_record<number> > record = integration_task_record_factory(tk, mgr);
        record->commit();
      }


    // Write an output task to the database

    template <typename number>
    void repository_sqlite3<number>::commit(const output_task<number>& tk)
      {
        transaction_manager mgr = this->transaction_factory();
        this->commit(mgr, tk);
        mgr.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit(transaction_manager& mgr, const output_task<number>& tk)
      {
        if(!tk.is_serializable())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_NOT_SERIALIZABLE << " '" << tk.get_name() << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        // check whether derived products on which this task depends have already been committed to the database
        // if not, commit them; we need their records to be available in order to enforce foreign key constraints
        // in the database
        const typename std::vector< output_task_element<number> > elements = tk.get_elements();

        for(const output_task_element<number>& elt : elements)
          {
            derived_data::derived_product<number>& product = elt.get_product();
            unsigned int count = sqlite3_operations::count_products(this->db, product.get_name());
            if(count == 0) this->commit(mgr, product);
          }

        // create a record and commit it; the commit will throw an exception if the repository is not in a read/write mode
        std::unique_ptr< output_task_record<number> > record = output_task_record_factory(tk, mgr);
        record->commit();
      }


    // Write a postintegration task to the database

    template <typename number>
    void repository_sqlite3<number>::commit(const zeta_twopf_task<number>& tk)
      {
        transaction_manager mgr = this->transaction_factory();
        this->commit_postintegration_task(mgr, tk);
        mgr.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit(transaction_manager& mgr, const zeta_twopf_task<number>& tk)
      {
        this->commit_postintegration_task(mgr, tk);
      }


    template <typename number>
    void repository_sqlite3<number>::commit(const zeta_threepf_task<number>& tk)
      {
        transaction_manager mgr = this->transaction_factory();
        this->commit_postintegration_task(mgr, tk);
        mgr.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit(transaction_manager& mgr, const zeta_threepf_task<number>& tk)
      {
        this->commit_postintegration_task(mgr, tk);
      }


    template <typename number>
    void repository_sqlite3<number>::commit(const fNL_task<number>& tk)
      {
        transaction_manager mgr = this->transaction_factory();
        this->commit_postintegration_task(mgr, tk);
        mgr.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::commit(transaction_manager& mgr, const fNL_task<number>& tk)
      {
        this->commit_postintegration_task(mgr, tk);
      }


    template <typename number>
    template <typename TaskType>
    void repository_sqlite3<number>::commit_postintegration_task(transaction_manager& mgr, const TaskType& tk)
      {
        if(!tk.is_serializable())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_NOT_SERIALIZABLE << " '" << tk.get_name() << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        // check whether parent task is already committed to the database;
        // if not, commit it; we need it to be available to enforce foreign key constraints in the database
        unsigned int count = sqlite3_operations::count_tasks(this->db, tk.get_parent_task()->get_name());
        if(count == 0)
          {
            this->autocommit(mgr, *tk.get_parent_task(), tk.get_name(),
                             CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_A, CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_B,
                             CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_C, CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_D);
          }

        // create a record and commit it; the commit will throw an exception if the repository is not in a read/write mode
        std::unique_ptr<postintegration_task_record < number> > record(postintegration_task_record_factory(tk, mgr));
        record->commit();
      }


    // Write a derived product specification

    template <typename number>
    void repository_sqlite3<number>::commit(const derived_data::derived_product<number>& d)
      {
        transaction_manager mgr = this->transaction_factory();
        this->commit(mgr, d);
        mgr.commit();
      }

    template <typename number>
    void repository_sqlite3<number>::commit(transaction_manager& mgr, const derived_data::derived_product<number>& d)
      {
        // check for a derived product with a duplicate name
        this->check_product_duplicate(d.get_name());

        // check whether all tasks on which this derived product depends are already in the database;
        // if not, commit them: we need their records to be available to enforce foreign key constraints
        typename std::list<derivable_task<number>*> task_list;
        d.get_task_list(task_list);

        for(derivable_task<number>* tk : task_list)
          {
            unsigned int count = sqlite3_operations::count_tasks(this->db, tk->get_name());

            if(count == 0)
              {
                this->autocommit(mgr, *tk, d.get_name(),
                                 CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_A, CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_B,
                                 CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_C, CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_D);
              }
          }

        // create a record and commit it; the commit will throw an exception if the repository is not in a read/write mode
        std::unique_ptr< derived_product_record<number> > record = derived_product_record_factory(d, mgr);
        record->commit();
      }


    template <typename number>
    void repository_sqlite3<number>::autocommit(transaction_manager& mgr, derivable_task<number>& tk, std::string parent,
                                                std::string commit_int_A, std::string commit_int_B,
                                                std::string commit_pint_A, std::string commit_pint_B)
      {
        switch(tk.get_type())
          {
            case task_type::integration:
              {
                integration_task<number>& rtk = dynamic_cast< integration_task<number>& >(tk);

                switch(rtk.get_task_type())
                  {
                    case integration_task_type::twopf:
                      {
                        this->commit(mgr, dynamic_cast< twopf_task<number>& >(rtk));
                        break;
                      }

                    case integration_task_type::threepf:
                      {
                        this->commit(mgr, dynamic_cast< threepf_task<number>& >(rtk));
                        break;
                      }
                  }

                break;
              }

            case task_type::postintegration:
              {
                postintegration_task<number>& rtk = dynamic_cast< postintegration_task<number>& >(tk);

                switch(rtk.get_task_type())
                  {
                    case postintegration_task_type::twopf:
                      {
                        this->commit(mgr, dynamic_cast< zeta_twopf_task<number>& >(rtk));
                        break;
                      }

                    case postintegration_task_type::threepf:
                      {
                        this->commit(mgr, dynamic_cast< zeta_threepf_task<number>& >(rtk));
                        break;
                      }

                    case postintegration_task_type::fNL:
                      {
                        this->commit(mgr, dynamic_cast< fNL_task<number>& >(rtk));
                        break;
                      }
                  }

                break;
              }

            case task_type::output:
              assert(false);
          }
      }


    // READ RECORDS FROM THE DATABASE


    //! Read a package record from the database
    template <typename number>
    std::unique_ptr< package_record<number> > repository_sqlite3<number>::query_package(const std::string& name)
      {
        // if no transaction in progress, query whether we already have this record cached
        if(this->transactions == 0)
          {
            typename repository<number>::package_record_cache::const_iterator t = this->pkg_cache.find(name);
            if(t != this->pkg_cache.end())
              {
                // found a match, so build a copy and return that
                return std::unique_ptr< package_record<number> >(dynamic_cast< package_record<number>* >(t->second->clone()));
              }
          }

        // build a read-only copy and store it in the cache, again if no transaction is in progress
        std::unique_ptr< package_record<number> > record = this->query_package(name, boost::optional<transaction_manager&>());
        if(this->transactions == 0) this->pkg_cache.insert( std::make_pair(name, std::unique_ptr< package_record<number> >(dynamic_cast< package_record<number>* >(record->clone()))) );

        return std::move(record);
      }

    template <typename number>
    std::unique_ptr< package_record<number> > repository_sqlite3<number>::query_package(const std::string& name, transaction_manager& mgr)
      {
        // for read-write copies we don't search the cache but always build a new copy from the database
        return this->query_package(name, boost::optional<transaction_manager&>(mgr));
      }

    template <typename number>
    std::unique_ptr< package_record<number> > repository_sqlite3<number>::query_package(const std::string& name, boost::optional<transaction_manager&> mgr)
      {
        boost::filesystem::path filename = sqlite3_operations::find_package(this->db, name, CPPTRANSPORT_REPO_PACKAGE_MISSING);
        Json::Value             root     = this->deserialize_JSON_document(filename);
        return this->package_record_factory(root, mgr);
      }


    //! Read a task record from the database
    template <typename number>
    std::unique_ptr< task_record<number> > repository_sqlite3<number>::query_task(const std::string& name, query_task_hint hint)
      {
        // if no transaction in progress, query whether we already have this record in the cache
        if(this->transactions == 0)
          {
            typename repository<number>::task_record_cache::const_iterator t = this->task_cache.find(name);
            if(t != this->task_cache.end())
              {
                // found a match, so build a copy and return that
                return std::unique_ptr< task_record<number> >(dynamic_cast< task_record<number>* >(t->second->clone()));
              }
          }

        // build a read-only copy and store it in the cache, again if no transaction is in progress
        std::unique_ptr< task_record<number> > record = this->query_task(name, boost::optional<transaction_manager&>(), hint);
        if(this->transactions == 0) this->task_cache.insert( std::make_pair(name, std::unique_ptr< task_record<number> >(dynamic_cast< task_record<number>* >(record->clone()))) );

        return std::move(record);
      }

    template <typename number>
    std::unique_ptr< task_record<number> > repository_sqlite3<number>::query_task(const std::string& name, transaction_manager& mgr, query_task_hint hint)
      {
        // for read-write copies we don't search the cache, but always build a new copy from the database
        return this->query_task(name, boost::optional<transaction_manager&>(mgr), hint);
      }

    template <typename number>
    std::unique_ptr< task_record<number> > repository_sqlite3<number>::query_task(const std::string& name, boost::optional<transaction_manager&> mgr, query_task_hint hint)
      {
        if(hint == query_task_hint::integration || (hint == query_task_hint::no_hint && sqlite3_operations::count_integration_tasks(this->db, name) > 0))
          {
            boost::filesystem::path filename = sqlite3_operations::find_integration_task(this->db, name, CPPTRANSPORT_REPO_TASK_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);
            return this->integration_task_record_factory(root, mgr);
          }
        else if(hint == query_task_hint::postintegration || (hint == query_task_hint::no_hint && sqlite3_operations::count_postintegration_tasks(this->db, name) > 0))
          {
            boost::filesystem::path filename = sqlite3_operations::find_postintegration_task(this->db, name, CPPTRANSPORT_REPO_TASK_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);
            return this->postintegration_task_record_factory(root, mgr);
          }
        else if(hint == query_task_hint::output || (hint == query_task_hint::no_hint && sqlite3_operations::count_output_tasks(this->db, name) > 0))
          {
            boost::filesystem::path filename = sqlite3_operations::find_output_task(this->db, name, CPPTRANSPORT_REPO_TASK_MISSING);
            Json::Value             root     = this->deserialize_JSON_document(filename);
            return this->output_task_record_factory(root, mgr);
          }

        throw runtime_exception(exception_type::RECORD_NOT_FOUND, name);   // RECORD_NOT_FOUND expects task name in message
      }


    // Read a derived product from the database
    template <typename number>
    std::unique_ptr< derived_product_record<number> > repository_sqlite3<number>::query_derived_product(const std::string& name)
      {
        // if no transaction in progress, query whether we already have this record in the cache
        if(this->transactions == 0)
          {
            typename repository<number>::derived_record_cache::const_iterator t = this->derived_cache.find(name);
            if(t != this->derived_cache.end())
              {
                // found a match, so build a copy and return that
                return std::unique_ptr< derived_product_record<number> >(dynamic_cast< derived_product_record<number>* >(t->second->clone()));
              }
          }

        // build a read-only copy and store it in the cache, again if no transaction is in progress
        std::unique_ptr< derived_product_record<number> > record = this->query_derived_product(name, boost::optional<transaction_manager&>());
        if(this->transactions == 0) this->derived_cache.insert( std::make_pair(name, std::unique_ptr< derived_product_record<number> >(dynamic_cast< derived_product_record<number>* >(record->clone()))) );

        return std::move(record);
      }

    template <typename number>
    std::unique_ptr< derived_product_record<number> > repository_sqlite3<number>::query_derived_product(const std::string& name, transaction_manager& mgr)
      {
        // for read-write copies we don't search the cache, but always build a new copy from the database
        return this->query_derived_product(name, boost::optional<transaction_manager&>(mgr));
      }

    template <typename number>
    std::unique_ptr< derived_product_record<number> > repository_sqlite3<number>::query_derived_product(const std::string& name, boost::optional<transaction_manager&> mgr)
      {
        boost::filesystem::path filename = sqlite3_operations::find_product(this->db, name, CPPTRANSPORT_REPO_PRODUCT_MISSING);
        Json::Value             root     = this->deserialize_JSON_document(filename);
        return this->derived_product_record_factory(root, mgr);
      }


    // Read a named integration content group from the database
    template <typename number>
    std::unique_ptr< content_group_record<integration_payload> > repository_sqlite3<number>::query_integration_content(const std::string& name)
      {
        // if no transaction in progress, query whether we already have this record in the cache
        if(this->transactions == 0)
          {
            typename repository<number>::integration_content_cache::const_iterator t = this->int_cache.find(name);
            if(t != this->int_cache.end())
              {
                // found a match, so build a copy and return that
                return std::unique_ptr< content_group_record<integration_payload> >(dynamic_cast< content_group_record<integration_payload>* >(t->second->clone()));
              }
          }

        // build a read-only copy and store it in the cache, again if no transaction is in progress
        std::unique_ptr< content_group_record<integration_payload> > record = this->query_content_group<integration_payload>(name, boost::optional<transaction_manager&>());
        if(this->transactions == 0) this->int_cache.insert( std::make_pair(name, std::unique_ptr< content_group_record<integration_payload> >(dynamic_cast< content_group_record<integration_payload>* >(record->clone()))) );

        return std::move(record);
      }

    template <typename number>
    std::unique_ptr< content_group_record<integration_payload> > repository_sqlite3<number>::query_integration_content(const std::string& name, transaction_manager& mgr)
      {
        // for read-write copies we don't search the cache, but always build a new copy from the database
        return this->query_content_group<integration_payload>(name, boost::optional<transaction_manager&>(mgr));
      }


    // Read a named postintegration content group from the database
    template <typename number>
    std::unique_ptr< content_group_record<postintegration_payload> > repository_sqlite3<number>::query_postintegration_content(const std::string& name)
      {
        // if no transaction in progress, query whether we already have this record in the cache
        if(this->transactions == 0)
          {
            typename repository<number>::postintegration_content_cache::const_iterator t = this->pint_cache.find(name);
            if(t != this->pint_cache.end())
              {
                // found a match, so build a copy and return that
                return std::unique_ptr< content_group_record<postintegration_payload> >(dynamic_cast< content_group_record<postintegration_payload>* >(t->second->clone()));
              }
          }

        // build a read-only copy and store it in the cache, again if no transaction is in progress
        std::unique_ptr< content_group_record<postintegration_payload> > record = this->query_content_group<postintegration_payload>(name, boost::optional<transaction_manager&>());
        if(this->transactions == 0) this->pint_cache.insert( std::make_pair(name, std::unique_ptr< content_group_record<postintegration_payload> >(dynamic_cast< content_group_record<postintegration_payload>* >(record->clone()))) );

        return std::move(record);
      }

    template <typename number>
    std::unique_ptr< content_group_record<postintegration_payload> > repository_sqlite3<number>::query_postintegration_content(const std::string& name, transaction_manager& mgr)
      {
        // for read-write copies we don't search the cache, but always build a new copy from the database
        return this->query_content_group<postintegration_payload>(name, boost::optional<transaction_manager&>(mgr));
      }


    // Read a named output content group from the database
    template <typename number>
    std::unique_ptr< content_group_record<output_payload> > repository_sqlite3<number>::query_output_content(const std::string& name)
      {
        // if no transaction in progress, query whether we already have this record in the cache
        if(this->transactions == 0)
          {
            typename repository<number>::output_content_cache::const_iterator t = this->out_cache.find(name);
            if(t != this->out_cache.end())
              {
                // found a match, so build a copy and return that
                return std::unique_ptr< content_group_record<output_payload> >(dynamic_cast< content_group_record<output_payload>* >(t->second->clone()));
              }
          }

        // build a read-only copy and store it in the cache, again if no transaction is in progress
        std::unique_ptr< content_group_record<output_payload> > record = this->query_content_group<output_payload>(name, boost::optional<transaction_manager&>());
        if(this->transactions == 0) this->out_cache.insert( std::make_pair(name, std::unique_ptr< content_group_record<output_payload> >(dynamic_cast< content_group_record<output_payload>* >(record->clone()))) );

        return std::move(record);
      }

    template <typename number>
    std::unique_ptr< content_group_record<output_payload> > repository_sqlite3<number>::query_output_content(const std::string& name, transaction_manager& mgr)
      {
        // for read-write copies we don't search the cache, but always build a new copy from the database
        return this->query_content_group<output_payload>(name, boost::optional<transaction_manager&>(mgr));
      }


    // Read a named content group from the database
    template <typename number>
    template <typename Payload>
    std::unique_ptr< content_group_record<Payload> > repository_sqlite3<number>::query_content_group(const std::string& name, boost::optional<transaction_manager&> mgr)
      {
        boost::filesystem::path filename = sqlite3_operations::find_group<Payload>(this->db, name, CPPTRANSPORT_REPO_OUTPUT_MISSING);
        Json::Value             root     = this->deserialize_JSON_document(filename);

        return this->content_group_record_factory<Payload>(root, mgr);
      }


    // Enumerate package records
    template <typename number>
    typename package_db<number>::type repository_sqlite3<number>::enumerate_packages()
      {
        std::list<std::string> package_names;

        // get list of package names
        sqlite3_operations::enumerate_packages(this->db, package_names);

        typename package_db<number>::type db;

        for(const std::string& name : package_names)
          {
            std::unique_ptr< package_record<number> > pkg = this->query_package(name);
            db.insert( std::make_pair(name, std::move(pkg)) );
          }

        return(std::move(db));
      }


    // Enumerate task records
    template <typename number>
    typename task_db<number>::type repository_sqlite3<number>::enumerate_tasks()
      {
        std::list<std::string> integration_names;
        std::list<std::string> postintegration_names;
        std::list<std::string> output_names;

        // get list of task names
        sqlite3_operations::enumerate_tasks(this->db, integration_names, postintegration_names, output_names);

        typename task_db<number>::type db;

        for(const std::string& name : integration_names)
          {
            std::unique_ptr< task_record<number> > task = this->query_task(name, query_task_hint::integration);
            db.insert( std::make_pair(name, std::move(task)) );
          }

        for(const std::string& name : postintegration_names)
          {
            std::unique_ptr< task_record<number> > task = this->query_task(name, query_task_hint::postintegration);
            db.insert( std::make_pair(name, std::move(task)) );
          }

        for(const std::string& name : output_names)
          {
            std::unique_ptr< task_record<number> > task = this->query_task(name, query_task_hint::output);
            db.insert( std::make_pair(name, std::move(task)) );
          }

        return(std::move(db));
      }


    // Enumerate derived product records
    template <typename number>
    typename derived_product_db<number>::type repository_sqlite3<number>::enumerate_derived_products()
      {
        std::list<std::string> derived_product_names;

        // get list of derived products
        sqlite3_operations::enumerate_derived_products(this->db, derived_product_names);

        typename derived_product_db<number>::type db;

        for(const std::string& name : derived_product_names)
          {
            std::unique_ptr< derived_product_record<number> > prod = this->query_derived_product(name);
            db.insert( std::make_pair(name, std::move(prod)) );
          }

        return(std::move(db));
      }


    // Enumerate the content groups available from a named integration task
    template <typename number>
    integration_content_db repository_sqlite3<number>::enumerate_integration_task_content(const std::string& name)
      {
        std::unique_ptr< task_record<number> > record = this->query_task(name);

        if(record->get_type() != task_type::integration)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        integration_content_db db;
        this->enumerate_content_groups<integration_payload>(db, name);

        return(std::move(db));   // std::move required by GCC 5.2 although standard implies that copy elision should occur
      }


    template <typename number>
    integration_content_db repository_sqlite3<number>::enumerate_integration_task_content()
      {
        integration_content_db db;
        this->enumerate_content_groups<integration_payload>(db);
        return(std::move(db));
      }


    // Enumerate the content groups available from a named postintegration task
    template <typename number>
    postintegration_content_db repository_sqlite3<number>::enumerate_postintegration_task_content(const std::string& name)
      {
        std::unique_ptr< task_record<number>> record = this->query_task(name);

        if(record->get_type() != task_type::postintegration)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_POSTINT << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        postintegration_content_db db;
        this->enumerate_content_groups<postintegration_payload>(db, name);

        return(std::move(db));   // std::move required by GCC 5.2 although standard implies that copy elision should occur
      }


    template <typename number>
    postintegration_content_db repository_sqlite3<number>::enumerate_postintegration_task_content()
      {
        postintegration_content_db db;
        this->enumerate_content_groups<postintegration_payload>(db);
        return(std::move(db));
      }


    // Enumerate the content groups available from a named output task
    template <typename number>
    output_content_db repository_sqlite3<number>::enumerate_output_task_content(const std::string& name)
      {
        std::unique_ptr< task_record<number>> record = this->query_task(name);

        if(record->get_type() != task_type::output)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_OUTPUT << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        output_content_db db;
        this->enumerate_content_groups<output_payload>(db, name);

        return(std::move(db));   // std::move required by GCC 5.2 although standard implies that copy elision should occur
      }


    template <typename number>
    output_content_db repository_sqlite3<number>::enumerate_output_task_content()
      {
        output_content_db db;
        this->enumerate_content_groups<output_payload>(db);
        return(std::move(db));
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
        else if(sqlite3_operations::count_content_groups(this->db, name) > 0)
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
    void repository_sqlite3<number>::enumerate_content_groups(std::map< std::string, std::unique_ptr< content_group_record<Payload> > >& db, const std::string name)
      {
        std::list<std::string> group_names;

        // get list of group names associated with the task 'name', or all tasks of a specified payload if 'name' is empty.
        // Overwrites existing content of the list group_names

        // The same information is cached in each task record, but there it is on a task-by-task basis.
        // Rather than read in all task records and search them sequentially, here we just read the information straight from the SQL table.
        // There are benefits and drawbacks to both approaches; this implementation could possibly be revisted later.
        sqlite3_operations::enumerate_content_groups<Payload>(this->db, group_names, name);

        // convert this list of names into a database of records
        this->content_groups_from_list(group_names, db);
      }


    template <typename number>
    template <typename Payload>
    void repository_sqlite3<number>::content_groups_from_list(const std::list<std::string>& list, std::map< std::string, std::unique_ptr< content_group_record<Payload> > >& db)
      {
        for(const std::string& name : list)
          {
            std::unique_ptr< content_group_record<Payload> > group;
            this->query_content(name, group);
            db.insert( std::make_pair(group->get_name(), std::move(group)) );
          }
      }


    template <typename number>
    void repository_sqlite3<number>::query_content(const std::string& name, std::unique_ptr< content_group_record<integration_payload> >& group)
      {
        group = this->query_integration_content(name);
      }


    template <typename number>
    void repository_sqlite3<number>::query_content(const std::string& name, std::unique_ptr< content_group_record<postintegration_payload> >& group)
      {
        group = this->query_postintegration_content(name);
      }


    template <typename number>
    void repository_sqlite3<number>::query_content(const std::string& name, std::unique_ptr< content_group_record<output_payload> >& group)
      {
        group = this->query_output_content(name);
      }


    // REMOVE CONTENT GROUPS


    template <typename number>
    template <typename Payload>
    void repository_sqlite3<number>::delete_content(task_record<number>& rec, content_group_record<Payload>& group_rec, transaction_manager& mgr)
      {
        // verify that content group is unlocked
        if(group_rec.get_lock_status())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_GROUP_IS_LOCKED << " '" << group_rec.get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        // delete this content group from the task record
        rec.delete_content_group(group_rec.get_name());
        rec.commit();

        // delete this content group from content table in the SQLite backend
        sqlite3_operations::delete_group<Payload>(mgr, this->db, group_rec.get_name(), rec.get_name());

        // move the content group to the trash
        boost::filesystem::path trash_path = this->get_root_path() / CPPTRANSPORT_REPO_TRASH_LEAF / rec.get_name();

        if(!boost::filesystem::exists(trash_path)) boost::filesystem::create_directories(trash_path);
        if(boost::filesystem::is_directory(trash_path))
          {
            boost::filesystem::path abs_dest = trash_path / group_rec.get_name();
            mgr.journal_move(group_rec.get_abs_output_path(), abs_dest);
          }
      }


    template <typename number>
    void repository_sqlite3<number>::delete_integration_content(const std::string& name, const std::string& task_name, transaction_manager& mgr)
      {
        // query for integration task record in read/write mode
        std::unique_ptr< task_record<number> > raw_rec = this->query_task(task_name, mgr);

        if(!raw_rec || raw_rec->get_type() != task_type::integration)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        // query for content group record in readonly mode
        std::unique_ptr< content_group_record<integration_payload> > group_rec = this->query_integration_content(name);

        if(group_rec) this->delete_content(*raw_rec, *group_rec, mgr);
      }


    template <typename number>
    void repository_sqlite3<number>::delete_postintegration_content(const std::string& name, const std::string& task_name, transaction_manager& mgr)
      {
        // query for integration task record in read/write mode
        std::unique_ptr< task_record<number> > raw_rec = this->query_task(task_name, mgr);

        if(!raw_rec || raw_rec->get_type() != task_type::postintegration)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_POSTINT << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        // query for content group record in readonly mode
        std::unique_ptr< content_group_record<postintegration_payload> > group_rec = this->query_postintegration_content(name);

        if(group_rec) this->delete_content(*raw_rec, *group_rec, mgr);
      }


    template <typename number>
    void repository_sqlite3<number>::delete_output_content(const std::string& name, const std::string& task_name, transaction_manager& mgr)
      {
        // query for integration task record in read/write mode
        std::unique_ptr< task_record<number> > raw_rec = this->query_task(task_name, mgr);

        if(!raw_rec || raw_rec->get_type() != task_type::output)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_POSTINT << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        // query for content group record in readonly mode
        std::unique_ptr< content_group_record<output_payload> > group_rec = this->query_output_content(name);

        if(group_rec) this->delete_content(*raw_rec, *group_rec, mgr);
      }


    // CONTENT GROUP MANAGEMENT


    template <typename number>
    std::string repository_sqlite3<number>::reserve_content_name(const std::string& tk, boost::filesystem::path& parent_path, boost::posix_time::ptime& now,
                                                                 const std::string& suffix, unsigned int num_cores)
      {
        std::string posix_time_string = boost::posix_time::to_iso_string(now);

        transaction_manager transaction = this->transaction_factory();
        std::string         name        = sqlite3_operations::reserve_content_name(transaction, this->db, tk, parent_path, posix_time_string, suffix, num_cores);
        transaction.commit();

        return(name);
      }


    template <typename number>
    inflight_db repository_sqlite3<number>::enumerate_inflight()
      {
        inflight_db db;
        sqlite3_operations::enumerate_inflight_groups(this->db, db);
        return(std::move(db));
      }


    template <typename number>
    inflight_integration_db repository_sqlite3<number>::enumerate_inflight_integrations()
      {
        inflight_integration_db db;
        sqlite3_operations::enumerate_inflight_integrations(this->db, db);
        return(std::move(db));
      }


    template <typename number>
    inflight_postintegration_db repository_sqlite3<number>::enumerate_inflight_postintegrations()
      {
        inflight_postintegration_db db;
        sqlite3_operations::enumerate_inflight_postintegrations(this->db, db);
        return(std::move(db));
      }


    template <typename number>
    inflight_derived_content_db repository_sqlite3<number>::enumerate_inflight_derived_content()
      {
        inflight_derived_content_db db;
        sqlite3_operations::enumerate_inflight_derived_content(this->db, db);
        return(std::move(db));
      }


    template <typename number>
    void repository_sqlite3<number>::perform_recovery(data_manager<number>& data_mgr, unsigned int worker)
      {
        // ensure no transactions are in progress
        if(this->transactions != 0) throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_RECOVER_WHILE_TRANSACTIONS);

        // detect if lockfile is still present in the repository; if so, forcibly remove it.
        // this will cause any concurrent process which has locked the repository to fail,
        // so recovery should be used with caution
        boost::filesystem::path lockfile = this->root_path / CPPTRANSPORT_REPO_LOCKFILE_LEAF;
        if(boost::filesystem::exists(lockfile)) boost::filesystem::remove(lockfile);

        // TODO: would be nice to lock the whole database at this point by setting up a transaction manager
        // and using it to control the entire recovery process.
        // At the moment this cannot be done, because the writers which are constructed to perform recovery
        // expect themselves to generate internal transactions
        // To get round this, the writers would have to be refactored to accept a transaction_manager
        // and use it to handle all their internal affairs.

        // get SQLite layer to enumerate hot writers
        inflight_integration_db     hot_integrations;
        inflight_postintegration_db hot_postintegrations;
        inflight_derived_content_db hot_derived_content;

        sqlite3_operations::enumerate_inflight_integrations(this->db, hot_integrations);
        sqlite3_operations::enumerate_inflight_postintegrations(this->db, hot_postintegrations);
        sqlite3_operations::enumerate_inflight_derived_content(this->db, hot_derived_content);

        // recover these writers
        // do postintegrations first, to pick up any paired integrations
        this->recover_postintegrations(data_mgr, hot_postintegrations, hot_integrations, worker);

        // recover any integrations left over after handling paired integrations
        this->recover_integrations(data_mgr, hot_integrations, worker);

        // recover any output tasks
        this->recover_derived_content(data_mgr, hot_derived_content, worker);

        // break any surviving locks on data containers
        this->remove_container_locks();
      }


    template <typename number>
    void repository_sqlite3<number>::remove_container_locks()
      {
        integration_content_db integration_content = this->enumerate_integration_task_content();
        postintegration_content_db postintegration_content = this->enumerate_postintegration_task_content();

        this->remove_container_locks(integration_content);
        this->remove_container_locks(postintegration_content);
      }


    template <typename number>
    template <typename DatabaseType>
    void repository_sqlite3<number>::remove_container_locks(const DatabaseType& db)
      {
        for(const typename DatabaseType::value_type& item : db)
          {
            if(item.second)
              {
                const typename DatabaseType::value_type::second_type::element_type& record = *item.second;

                boost::filesystem::path container_path = record.get_abs_repo_path() / record.get_payload().get_container_path();
                boost::filesystem::path lockfile_path = container_path.parent_path() / CPPTRANSPORT_DATAMGR_LOCKFILE_LEAF;

                if(boost::filesystem::exists(lockfile_path)) boost::filesystem::remove(lockfile_path);
              }
          }
      }


    template <typename number>
    void repository_sqlite3<number>::recover_integrations(data_manager<number>& data_mgr, inflight_integration_db& list, unsigned int worker)
      {
        // loop through hot integrations; for each one, set up a new integration_writer which is populated
        // with the configuration of the hot writer

        // then, carry out an integrity check and commit the writer

        for(const inflight_integration_db_value_type& inflight : list)
          {
            // get task record
            std::unique_ptr< task_record<number> > pre_rec = this->query_task(inflight.second->task_name);
            integration_task_record<number>* rec = dynamic_cast< integration_task_record<number>* >(pre_rec.get());

            assert(rec != nullptr);
            if(rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

            std::unique_ptr< integration_writer<number> > writer = this->get_integration_recovery_writer(*inflight.second, data_mgr, *rec, worker);

            // metadata for the writer are likely to be inconsistent
            // try to recover correct metadata directly from the container
            this->recover_integration_metadata(data_mgr, *writer);

            // close writer, performing integrity check to update all missing serial numbers
            // if any are missing, the writer will be marked as failed
            // also updates writer's metadata with correct number of configurations stored in the container
            // and performs finalization step
            // (closing the writer will remove it from the list of active integrations)
            data_mgr.close_writer(*writer);

            // commit output
            writer->commit();
          }
      }


    template <typename number>
    std::unique_ptr< integration_writer<number> >
    repository_sqlite3<number>::get_integration_recovery_writer(const inflight_integration& data, data_manager<number>& data_mgr,
                                                                integration_task_record<number>& rec, unsigned int worker)
      {
        // set up a new writer instance for this content group
        std::unique_ptr< integration_writer<number> > writer = this->recover_integration_task_content(data.content_group, rec, data.output, data.container, data.logdir, data.tempdir, worker, data.workgroup_number);

        // initialize writer in recovery mode
        data_mgr.initialize_writer(*writer, true);

        // was this container seeded?
        if(data.is_seeded) writer->set_seed(data.seed_group);
        writer->set_collecting_statistics(data.is_collecting_stats);
        writer->set_collecting_initial_conditions(data.is_collecting_ics);

        return(writer);
      }


    template <typename number>
    void repository_sqlite3<number>::recover_integration_metadata(data_manager<number>& data_mgr, integration_writer<number>& writer)
      {
        integration_metadata metadata = writer.get_metadata();

        timing_db timing_data = data_mgr.read_timing_information(writer.get_abs_container_path());

        boost::timer::nanosecond_type max_integration = 0;
        boost::timer::nanosecond_type min_integration = 0;
        boost::timer::nanosecond_type max_batching = 0;
        boost::timer::nanosecond_type min_batching = 0;

        boost::timer::nanosecond_type total_integration = 0;
        boost::timer::nanosecond_type total_batching = 0;

        for(const timing_db::value_type& item : timing_data)
          {
            if(item.second)
              {
                const timing_record& record = *item.second;

                if(max_integration == 0 || record.get_integration_time() > max_integration) max_integration = record.get_integration_time();
                if(min_integration == 0 || record.get_integration_time() < min_integration) min_integration = record.get_integration_time();
                if(max_batching == 0    || record.get_batch_time() > max_batching)          max_batching = record.get_batch_time();
                if(min_batching == 0    || record.get_batch_time() < min_batching)          min_batching = record.get_batch_time();

                total_integration += record.get_integration_time();
                total_batching += record.get_batch_time();
              }
          }

        metadata.total_integration_time = total_integration;
        metadata.global_min_integration_time = min_integration;
        metadata.global_max_integration_time = max_integration;

        metadata.total_batching_time = total_batching;
        metadata.global_min_batching_time = min_batching;
        metadata.global_max_batching_time = max_batching;

        // leave total_wallclock_time, total_aggregation_time and total_refinements unchanged
        writer.set_metadata(metadata);
      }


    template <typename number>
    void repository_sqlite3<number>::recover_postintegrations(data_manager<number>& data_mgr, inflight_postintegration_db& p_list,
                                                              inflight_integration_db& i_list, unsigned int worker)
      {
        for(const inflight_postintegration_db_value_type& inflight : p_list)
          {
            // get task record
            std::unique_ptr< task_record<number> > pre_rec = this->query_task(inflight.second->task_name);
            postintegration_task_record<number>* rec = dynamic_cast< postintegration_task_record<number>* >(pre_rec.get());

            assert(rec != nullptr);
            if(rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

            if(inflight.second->is_paired) this->recover_paired_postintegration(*inflight.second, data_mgr, *rec, i_list, worker);
            else                           this->recover_unpaired_postintegration(*inflight.second, data_mgr, *rec, worker);
          }
      }


    template <typename number>
    void repository_sqlite3<number>::recover_unpaired_postintegration(const inflight_postintegration& data, data_manager<number>& data_mgr,
                                                                      postintegration_task_record<number>& rec, unsigned int worker)
      {
        std::unique_ptr< postintegration_writer<number> > writer = this->get_postintegration_recovery_writer(data, data_mgr, rec, worker);

        // close writer, performing integrity check to update all missing serial numbers
        // if any are missing, the writer will be marked as failed
        // also updates writer's metadata with correct number of configurations stored in the container
        // and performs finalization step
        // (closing the writer will remove it from the list of active integrations)
        data_mgr.close_writer(*writer);

        // commit output
        // (closing the writer will remove it from the list of active postintegrations)
        writer->commit();
      }


    namespace repository_sqlite3_impl
      {

        template <typename InFlightDatabaseObject>
        class FindInFlight
          {
          public:
            FindInFlight(const std::string& n)
              : name(n)
              {
              }

            bool operator()(const InFlightDatabaseObject& obj)
              {
                return(obj.first == name);
              }

          protected:
            const std::string name;
          };

      }   // namespace repository_sqlite3_impl


    template <typename number>
    void repository_sqlite3<number>::recover_paired_postintegration(const inflight_postintegration& data, data_manager<number>& data_mgr,
                                                                    postintegration_task_record<number>& p_rec,
                                                                    inflight_integration_db& i_list, unsigned int worker)
      {
        // try to find paired integration in i_list
        inflight_integration_db::iterator t = std::find_if(i_list.begin(), i_list.end(),
                                                           repository_sqlite3_impl::FindInFlight<inflight_integration_db_value_type>(data.parent_group));

        if(t == i_list.end()) this->recover_unpaired_postintegration(data, data_mgr, p_rec, worker);    // no match for paired integration, treat as unpaired
        else
          {
            // get task record
            std::unique_ptr< task_record<number> > pre_rec = this->query_task(t->second->task_name);
            integration_task_record<number>& i_rec = dynamic_cast< integration_task_record<number>& >(*pre_rec);

            std::unique_ptr< integration_writer<number> >     i_writer = this->get_integration_recovery_writer(*t->second, data_mgr, i_rec, worker);
            std::unique_ptr< postintegration_writer<number> > p_writer = this->get_postintegration_recovery_writer(data, data_mgr, p_rec, worker);

            // metadata for the writer are likely to be inconsistent
            // try to recover correct metadata directly from the container
            this->recover_integration_metadata(data_mgr, *i_writer);

            // close writers, performing integrity check to update all missing serial numbers
            // if any are missing, the writer will be marked as failed
            // any missing serials are synchornized between the containers
            // also updates writer's metadata with correct number of configurations stored in the container
            // and performs finalization step
            // (closing the writer will remove it from the list of active integrations)
            data_mgr.close_writer(*i_writer, *p_writer);

            // commit output
            // (closing the writers will remove them from the list of active postintegrations)
            i_writer->commit();
            p_writer->commit();

            // remove integration from database of remaining hot integrations
            // (we shouldn't try to recover it twice)
            i_list.erase(t);
          }
      }


    template <typename number>
    std::unique_ptr< postintegration_writer<number> >
    repository_sqlite3<number>::get_postintegration_recovery_writer(const inflight_postintegration& data, data_manager<number>& data_mgr,
                                                                    postintegration_task_record<number>& rec, unsigned int worker)
      {
        // set up a new writer instance for this content group
        std::unique_ptr< postintegration_writer<number> > writer = this->recover_postintegration_task_content(data.content_group, rec, data.output, data.container, data.logdir, data.tempdir, worker);

        // initialize writer in recovery mode
        data_mgr.initialize_writer(*writer, true);

        writer->set_pair(data.is_paired);
        writer->set_parent_group(data.parent_group);

        // was this container seeded?
        if(data.is_seeded) writer->set_seed(data.seed_group);

        return(writer);
      }


    template <typename number>
    void repository_sqlite3<number>::recover_derived_content(data_manager<number>& data_mgr, inflight_derived_content_db& list, unsigned int worker)
      {
        // loop through hot derived content
        // for each item, set up a new derived_content_writer which is populated with the configuration of the hot writer.
        // However, in recovery mode, the content group is always treated as if it has failed.

        // When we commit the writer, it will automatically move to the fail cache

        for(const inflight_derived_content_db_value_type& inflight : list)
          {
            // get task record
            std::unique_ptr< task_record<number> > pre_rec = this->query_task(inflight.second->task_name);
            output_task_record<number>* rec = dynamic_cast< output_task_record<number>* >(pre_rec.get());

            assert(rec != nullptr);
            if(rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

            std::unique_ptr< derived_content_writer<number> > writer = this->get_derived_content_recovery_writer(*inflight.second, data_mgr, *rec, worker);

            // commit writer (automatically removes this content group from database of active tasks))
            data_mgr.close_writer(*writer);
          }
      }


    template <typename number>
    std::unique_ptr< derived_content_writer<number> >
    repository_sqlite3<number>::get_derived_content_recovery_writer(const inflight_derived_content& data, data_manager<number>& data_mgr,
                                                                    output_task_record<number>& rec, unsigned int worker)
      {
        std::unique_ptr< derived_content_writer<number> > writer = this->recover_output_task_content(data.content_group, rec, data.output, data.logdir, data.tempdir, worker);

        // initialize writer in recover mode
        data_mgr.initialize_writer(*writer, true);

        // force writer to fail
        writer->set_fail(true);

        return(writer);
      }


    // separate in-flight tables are needed for the writers because when the global
    // in-flight entries are written (in reserve_content_name() above) not all information is
    // available -- eg. seeding or pairing of the writers.
    // In order to recover a content group if a crash occurs we need to be able to
    // reconstuct these details, so we need the separate tables


    template <typename number>
    void repository_sqlite3<number>::register_writer(integration_writer<number>& writer)
      {
        transaction_manager transaction = this->transaction_factory();

        sqlite3_operations::register_integration_writer(transaction, this->db,
                                                        writer.get_name(),
                                                        writer.get_task_name(),
                                                        writer.get_relative_output_path(),
                                                        writer.get_relative_container_path(),
                                                        writer.get_relative_logdir_path(),
                                                        writer.get_relative_tempdir_path(),
                                                        writer.get_workgroup_number(),
                                                        writer.is_seeded(),
                                                        writer.get_seed_group(),
                                                        writer.is_collecting_statistics(),
                                                        writer.is_collecting_initial_conditions());

        transaction.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::register_writer(postintegration_writer<number>& writer)
      {
        transaction_manager transaction = this->transaction_factory();

        sqlite3_operations::register_postintegration_writer(transaction, this->db,
                                                            writer.get_name(),
                                                            writer.get_task_name(),
                                                            writer.get_relative_output_path(),
                                                            writer.get_relative_container_path(),
                                                            writer.get_relative_logdir_path(),
                                                            writer.get_relative_tempdir_path(),
                                                            writer.is_paired(),
                                                            writer.get_parent_group(),
                                                            writer.is_seeded(),
                                                            writer.get_seed_group());

        transaction.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::register_writer(derived_content_writer<number>& writer)
      {
        transaction_manager transaction = this->transaction_factory();

        sqlite3_operations::register_derived_content_writer(transaction, this->db,
                                                            writer.get_name(),
                                                            writer.get_task_name(),
                                                            writer.get_relative_output_path(),
                                                            writer.get_relative_logdir_path(),
                                                            writer.get_relative_tempdir_path());

        transaction.commit();
      }


    template <typename number>
    void repository_sqlite3<number>::deregister_writer(integration_writer<number>& writer, transaction_manager& mgr)
      {
        sqlite3_operations::deregister_content_group(mgr, this->db, writer.get_name(), sqlite3_operations::CPPTRANSPORT_SQLITE_INTEGRATION_WRITERS_TABLE);
      }


    template <typename number>
    void repository_sqlite3<number>::deregister_writer(postintegration_writer<number>& writer, transaction_manager& mgr)
      {
        sqlite3_operations::deregister_content_group(mgr, this->db, writer.get_name(), sqlite3_operations::CPPTRANSPORT_SQLITE_POSTINTEGRATION_WRITERS_TABLE);
      }


    template <typename number>
    void repository_sqlite3<number>::deregister_writer(derived_content_writer<number>& writer, transaction_manager& mgr)
      {
        sqlite3_operations::deregister_content_group(mgr, this->db, writer.get_name(), sqlite3_operations::CPPTRANSPORT_SQLITE_DERIVED_WRITERS_TABLE);
      }


    template <typename number>
    void repository_sqlite3<number>::advise_completion_time(const std::string& name, const boost::posix_time::ptime& time)
      {
        transaction_manager transaction = this->transaction_factory();

        std::string time_string = boost::posix_time::to_iso_string(time);
        sqlite3_operations::advise_completion_time(transaction, this->db, name, time_string);

        transaction.commit();
      }

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_SQLITE3_IMPL_H
