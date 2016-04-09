//
// Created by David Seery on 27/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_SQLITE3_DECL_H
#define CPPTRANSPORT_REPOSITORY_SQLITE3_DECL_H


#include "transport-runtime-api/sqlite3/detail/repository_sqlite3_decl.h"


namespace transport
  {

    constexpr auto CPPTRANSPORT_REPO_REPOSITORY_LEAF = "database.sqlite";
    constexpr auto CPPTRANSPORT_REPO_STORE_LEAF      = "repository";
    constexpr auto CPPTRANSPORT_REPO_PACKAGES_LEAF   = "packages";
    constexpr auto CPPTRANSPORT_REPO_TASKS_LEAF      = "tasks";
    constexpr auto CPPTRANSPORT_REPO_PRODUCTS_LEAF   = "products";
    constexpr auto CPPTRANSPORT_REPO_OUTPUT_LEAF     = "output";


    // forward-declare transaction handler
    template <typename number> class sqlite3_transaction_handler;


    //! repository_sqlite3<> implements the repository interface using
    //! libjsoncpp and sqlite3 as the database backend.
    //! This implementation replaces two previous ones, the first
    //! using Oracle DBXML and the second using UnQLite.
    template <typename number>
    class repository_sqlite3: public json_repository<number>
      {

      private:

        //! record count callback, used when committing to the database
        typedef std::function <unsigned int(sqlite3*, const std::string&)>                                   count_function;

        //! record store callback, used when committing to the database
        typedef std::function <void(transaction_manager&, sqlite3*, const std::string&, const std::string&)> store_function;

        //! record find function, used when replacing records in the database
        typedef std::function<std::string(sqlite3*, const std::string&)>                                     find_function;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Open a repository with a specified pathname, and specified warning and error handlers.
        //! Creates the repository directory structure if it does not already exist
        repository_sqlite3(const boost::filesystem::path path, model_manager<number>& f, repository_mode mode,
                           local_environment& ev, error_handler e, warning_handler w, message_handler m);

        //! Close a repository, including any corresponding containers
        virtual ~repository_sqlite3();


        // CREATE REPOSITORY

      protected:

        //! Create a repository at the root specified during construction
        void create_repository();

        //! Validate an existing repository at the root specified during construction
        void validate_repository();


        // TRANSACTIONS

      public:

        //! Generate a transaction management object
        transaction_manager transaction_factory() override final;

      protected:

        //! Begin a transaction on the database.
        void begin_transaction();

        //! Commit a transaction to the database.
        void commit_transaction();

        //! Rollback database to beginning of last transaction
        void abort_transaction();

        //! Release a transaction on the database
        void release_transaction();


        friend class sqlite3_transaction_handler<number>;


        // CREATE RECORDS -- implements a 'repository' interface

      public:

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        virtual void commit(const initial_conditions<number>& ics) override;
        virtual void commit(transaction_manager& mgr, const initial_conditions<number>& ics) override;

        //! Write an integration task to the database.
        virtual void commit(const twopf_task<number>& tk) override;
        virtual void commit(const threepf_task<number>& tk) override;
        virtual void commit(transaction_manager& mgr, const twopf_task<number>& tk) override;
        virtual void commit(transaction_manager& mgr, const threepf_task<number>& tk) override;

        //! Write an output task to the database
        virtual void commit(const output_task<number>& tk) override;
        virtual void commit(transaction_manager& mgr, const output_task<number>& tk) override;

        //! Write a postintegration task to the database
        virtual void commit(const zeta_twopf_task<number>& tk) override;
        virtual void commit(const zeta_threepf_task<number>& tk) override;
        virtual void commit(const fNL_task<number>& tk) override;
        virtual void commit(transaction_manager& mgr, const zeta_twopf_task<number>& tk) override;
        virtual void commit(transaction_manager& mgr, const zeta_threepf_task<number>& tk) override;
        virtual void commit(transaction_manager& mgr, const fNL_task<number>& tk) override;

        //! Write a derived product specification
        virtual void commit(const derived_data::derived_product<number>& d) override;
        virtual void commit(transaction_manager& mgr, const derived_data::derived_product<number>& d) override;

      protected:

        //! Generic commit method for integration task; public commit methods delegate to this
        template <typename TaskType>
        void commit_integration_task(transaction_manager& mgr, const TaskType& tk);

        //! Generic commit method for postintegration task; public commit methods delegate to this
        template <typename TaskType>
        void commit_postintegration_task(transaction_manager& mgr, const TaskType& tk);

        //! Autocommit a derivable task encountered as part of another task specification
        void autocommit(transaction_manager& mgr, derivable_task <number>& tk, std::string parent,
                        std::string commit_int_A, std::string commit_int_B,
                        std::string commit_pint_A, std::string commit_pint_B);


        // SEARCH FOR DATABASE RECORDS -- implements a 'repository' interface

      public:

        // READ ONLY

        //! Read a package record from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< package_record<number> > query_package(const std::string& name) override;

        //! Read a task record from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< task_record<number> > query_task(const std::string& name) override;

        //! Read a derived product specification from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< derived_product_record<number> > query_derived_product(const std::string& name) override;

        //! Read an integration content group specification from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< content_group_record<integration_payload> > query_integration_content(const std::string& name) override;

        //! Read a postintegration content group specification from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< content_group_record<postintegration_payload> > query_postintegration_content(const std::string& name) override;

        //! Read an output content group specification from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< content_group_record<output_payload> > query_output_content(const std::string& name) override;

        // READ/WRITE

        //! Read a package record from the database
        virtual std::unique_ptr< package_record<number> > query_package(const std::string& name, transaction_manager& mgr) override;

        //! Read a task record from the database
        virtual std::unique_ptr< task_record<number> > query_task(const std::string& name, transaction_manager& mgr) override;

        //! Read a derived product specification from the database
        virtual std::unique_ptr< derived_product_record<number> > query_derived_product(const std::string& name, transaction_manager& mgr) override;

        //! Read an integration content group specification from the database
        virtual std::unique_ptr< content_group_record<integration_payload> > query_integration_content(const std::string& name, transaction_manager& mgr) override;

        //! Read a postintegration content group specification from the database
        virtual std::unique_ptr< content_group_record<postintegration_payload> > query_postintegration_content(const std::string& name, transaction_manager& mgr) override;

        //! Read an output content group specification from the database
        virtual std::unique_ptr< content_group_record<output_payload> > query_output_content(const std::string& name, transaction_manager& mgr) override;

      protected:

        //! Generic method to read a package record from the database
        std::unique_ptr< package_record<number> > query_package(const std::string& name, boost::optional<transaction_manager&> mgr);

        //! Generic method to read a task from the database
        std::unique_ptr< task_record<number> > query_task(const std::string& name, boost::optional<transaction_manager&> mgr);

        //! Generic method to read a derived product specification from the database
        std::unique_ptr< derived_product_record<number> > query_derived_product(const std::string& name, boost::optional<transaction_manager&> mgr);

        //! Generic method to read a content group from the database
        template <typename Payload>
        std::unique_ptr< content_group_record<Payload> > query_content_group(const std::string& name, boost::optional<transaction_manager&> mgr);


        // ENUMERATE DATABASE RECORDS

      public:

        //! Enumerate package records; all records are returned in a readonly state, so if updates are required
        //! they must be re-queried using the query_*() methods
        virtual typename package_db<number>::type enumerate_packages() override;

        //! Enumerate task records; all records are returned in a readonly state, so if updates are required
        //! they must be re-queried using the query_*() methods
        virtual typename task_db<number>::type enumerate_tasks() override;

        //! Enumerate derived product records; all records are returned in a readonly state, so if updates are required
        //! they must be re-queried using the query_*() methods
        virtual typename derived_product_db<number>::type enumerate_derived_products() override;


        // ENUMERATE OUTPUT GROUPS

        //! Enumerate the content groups available from a named integration task; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual integration_content_db enumerate_integration_task_content(const std::string& name) override;

        //! Enumerate all integration content groups; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual integration_content_db enumerate_integration_task_content() override;

        //! Enumerate the content groups available for a named postintegration task; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual postintegration_content_db enumerate_postintegration_task_content(const std::string& name) override;

        //! Enumerate all postintegration content groups; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual postintegration_content_db enumerate_postintegration_task_content() override;

        //! Enumerate the content groups available from a named output task; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual output_content_db enumerate_output_task_content(const std::string& name) override;

        //! Enumerate all content groups; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual output_content_db enumerate_output_task_content() override;


        // REMOVE OUTPUT GROUPS

        // TO BE USED WITH CARE, in case the repository gets into an inconsistent state

      public:

        //! Remove an integration group
        virtual void delete_integration_content(const std::string& name, const std::string& task_name, transaction_manager& mgr) override;

        //! Remove a postintegration group
        virtual void delete_postintegration_content(const std::string& name, const std::string& task_name, transaction_manager& mgr) override;

        //! Remove an outpout group
        virtual void delete_output_content(const std::string& name, const std::string& task_name, transaction_manager& mgr) override;

      protected:

        //! generic utility function to remove a content group from a task record
        template <typename Payload>
        void delete_content(task_record<number>& rec, content_group_record<Payload>& group_rec, transaction_manager& mgr);


        // ENUMERATE INFLIGHT TASKS

      public:

        //! Enumerate all active jobs
        virtual inflight_db enumerate_inflight() override;

        //! Enumerate all active integrations
        virtual inflight_integration_db enumerate_inflight_integrations() override;

        //! Enumerate all active postintegrations
        virtual inflight_postintegration_db enumerate_inflight_postintegrations() override;

        //! Enumerate all active derived content tasks
        virtual inflight_derived_content_db enumerate_inflight_derived_content() override;


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
        virtual std::unique_ptr< package_record<number> > package_record_factory(const initial_conditions<number>& ics, transaction_manager& mgr) override;

        //! Create a new integration task record from an explicit object
        virtual std::unique_ptr< integration_task_record<number> > integration_task_record_factory(const twopf_task<number>& tk, transaction_manager& mgr) override;
        virtual std::unique_ptr< integration_task_record<number> > integration_task_record_factory(const threepf_task<number>& tk, transaction_manager& mgr) override;

        // generic integration task factory
        template <typename TaskType>
        std::unique_ptr< integration_task_record<number> > integration_task_record_factory_generic(const TaskType& tk, transaction_manager& mgr);

        //! Create a new output task record from an explicit object
        virtual std::unique_ptr< output_task_record<number> > output_task_record_factory(const output_task<number>& tk, transaction_manager& mgr) override;

        //! Create a postintegration task record from an explicit object
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(const zeta_twopf_task<number>& tk, transaction_manager& mgr) override;
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(const zeta_threepf_task<number>& tk, transaction_manager& mgr) override;
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(const fNL_task<number>& tk, transaction_manager& mgr) override;

        //! generic postintegration task record factory
        template <typename TaskType>
        std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory_generic(const TaskType& tk, transaction_manager& mgr);

        //! Create a new derived product record from explicit object
        virtual std::unique_ptr< derived_product_record<number> > derived_product_record_factory(const derived_data::derived_product<number>& prod, transaction_manager& mgr) override;

        //! Create a new content group for an integration task
        virtual std::unique_ptr< content_group_record<integration_payload> > integration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                                     bool lock, const std::list<note>& nt, const std::list<std::string>& tg,
                                                                                                                     transaction_manager& mgr) override;

        //! Create a new content group for a postintegration task
        virtual std::unique_ptr< content_group_record<postintegration_payload> > postintegration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                                             bool lock, const std::list<note>& nt, const std::list<std::string>& tg,
                                                                                                                             transaction_manager& mgr) override;

        //! Create a new content group for an output task
        virtual std::unique_ptr< content_group_record<output_payload> > output_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                           bool lock, const std::list<note>& nt, const std::list<std::string>& tg,
                                                                                                           transaction_manager& mgr) override;

        //! Implementation -- Create a new content group record
        template <typename Payload>
        std::unique_ptr< content_group_record<Payload> > content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                     bool lock, const std::list<note>& nt, const std::list<std::string>& tg,
                                                                                     transaction_manager& mgr);

        // 2. Factories from JSON representations: these are needed as part of the 'json_repository<>' interface

      protected:

        //! Create a package record from a JSON value
        virtual std::unique_ptr< package_record<number> > package_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) override;

        //! Create an integration task record from a JSON value
        virtual std::unique_ptr< integration_task_record<number> > integration_task_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) override;

        //! Create an output task record from a JSON value
        virtual std::unique_ptr< output_task_record<number> > output_task_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) override;

        //! Create a postintegration task record from a JSON value
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) override;

        //! create a new derived product record from a JSON value
        virtual std::unique_ptr< derived_product_record<number> > derived_product_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) override;

        //! Create a new content group for an integration task from a JSON value
        virtual std::unique_ptr< content_group_record<integration_payload> > integration_content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) override;

        //! Create a new content group for a postintegration task from a JSON value
        virtual std::unique_ptr< content_group_record<postintegration_payload> > postintegration_content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) override;

        //! Create a new content group for an output task from a JSON value
        virtual std::unique_ptr< content_group_record<output_payload> > output_content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) override;

        //! Implementation -- Create a new content group record
        template <typename Payload>
        std::unique_ptr< content_group_record<Payload> > content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr);


        // UTILITY FUNCTIONS

      protected:

        //! Check whether a package already exists in the database. Throws an exception if so.
        void check_package_duplicate(const std::string& name);

        //! Check whether a task already exists in the database. Throws an exception if so.
        void check_task_duplicate(const std::string& name);

        //! Check whether a derived product already exists in the database. Throws an exception if so.
        void check_product_duplicate(const std::string& name);

        //! Enumerate content groups for a named task, or all tasks of a specified payload if no name is provided
        template <typename Payload>
        void enumerate_content_groups(std::map< std::string, std::unique_ptr < content_group_record<Payload> > >& db, const std::string name="");

        //! Build a database of content groups from a list of names
        template <typename Payload>
        void content_groups_from_list(const std::list<std::string>& list, std::map< std::string, std::unique_ptr < content_group_record<Payload> > >& db);


        // COMMIT CALLBACK METHODS FOR REPOSITORY RECORDS

      protected:

        //! Commit callback: commit to database for the first time
        void commit_first(repository_record& record, transaction_manager& mgr, count_function counter, store_function storer, std::string store_root, std::string exists_err);

        //! Commit callback: replacement commit to database
        void commit_replace(repository_record& record, transaction_manager& mgr, find_function finder);

        //! Commit callback: commit integration task to database for the first time (need special treatment)
        void commit_integration_first(repository_record& record, transaction_manager& mgr, count_function counter, store_function storer, std::string store_root, std::string exists_err);

        //! Commit callback: replacement commit to database for integration tasks (need special treatment)
        void commit_integration_replace(repository_record& record, transaction_manager& mgr, find_function finder);

        //! Commit k-configuration databases
        void commit_kconfig_database(transaction_manager& mgr, integration_task_record<number>& record);


        // CONTENT GROUP MANAGEMENT

      protected:

        virtual std::string reserve_content_name(const std::string& tk, boost::filesystem::path& parent_path, boost::posix_time::ptime& now,
                                                 const std::string& suffix, unsigned int num_cores) override;


        // WRITER MANAGEMENT

      public:

        //! perform recovery on the repository (this is a global operation -- it affects all tasks)
        virtual void perform_recovery(data_manager<number>& data_mgr, unsigned int worker) override;

        //! register an integration writer
        virtual void register_writer(integration_writer<number>& writer) override;

        //! register a postintegration writer
        virtual void register_writer(postintegration_writer<number>& writer) override;

        //! register a derived-content writer
        virtual void register_writer(derived_content_writer<number>& writer) override;

        //! deregister an integration writer
        virtual void deregister_writer(integration_writer<number>& writer, transaction_manager& mgr) override;

        //! deregister a postintegration writer
        virtual void deregister_writer(postintegration_writer<number>& writer, transaction_manager& mgr) override;

        //! deregister a derived-content writer
        virtual void deregister_writer(derived_content_writer<number>& writer, transaction_manager& mgr) override;

        //! Advise completion time for a writer
        virtual void advise_completion_time(const std::string& name, const boost::posix_time::ptime& time) override;

      protected:

        //! recover a list of hot integrations
        void recover_integrations(data_manager<number>& data_mgr, inflight_integration_db& list, unsigned int worker);

        //! recover a list of hot postintegrations
        void recover_postintegrations(data_manager<number>& data_mgr, inflight_postintegration_db& p_list,
                                      inflight_integration_db& i_list, unsigned int worker);

        //! recover a list of hot derived-content writers
        void recover_derived_content(data_manager<number>& data_mgr, inflight_derived_content_db& list, unsigned int worker);

      protected:

        //! construct an integration_writer in recovery mode
        std::unique_ptr< integration_writer<number> > get_integration_recovery_writer(const inflight_integration& data, data_manager<number>& data_mgr,
                                                                                      integration_task_record<number>& rec, unsigned int worker);

        //! construct a postintegration_writer in recovery mode
        std::unique_ptr< postintegration_writer<number> > get_postintegration_recovery_writer(const inflight_postintegration& data, data_manager<number>& data_mgr,
                                                                                              postintegration_task_record<number>& rec, unsigned int worker);

        void recover_unpaired_postintegration(const inflight_postintegration& data, data_manager<number>& data_mgr,
                                              postintegration_task_record<number>& rec, unsigned int worker);

        void recover_paired_postintegration(const inflight_postintegration& data, data_manager<number>& data_mgr,
                                            postintegration_task_record<number>& p_rec,
                                            inflight_integration_db& i_list, unsigned int worker);

        //! construct a derived_content_writer in recovery mode
        std::unique_ptr< derived_content_writer<number> > get_derived_content_recovery_writer(const inflight_derived_content& data, data_manager<number>& data_mgr,
                                                                                              output_task_record<number>& rec, unsigned int worker);

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

        //! relative BOOST path to content group store
        boost::filesystem::path output_store;


        // SQLITE3 DATABASE HANDLES

      private:

        //! SQLite database connexion
        sqlite3* db;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_SQLITE3_DECL_H
