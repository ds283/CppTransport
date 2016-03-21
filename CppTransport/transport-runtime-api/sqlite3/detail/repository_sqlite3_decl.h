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
        repository_sqlite3(const std::string& path, model_manager<number>& f, repository_mode mode,
                           error_handler e, warning_handler w, message_handler m);

        //! Close a repository, including any corresponding containers
        virtual ~repository_sqlite3();


        // CREATE REPOSITORY

      protected:

        //! Create a repository at the root specified during construction
        void create_repository();

        //! Validate an existing repository at the root specified during construction
        void validate_repository();


        // TRANSACTIONS

      protected:

        //! Generate a transaction management object
        transaction_manager transaction_factory() override final;

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


        // READ RECORDS FROM THE DATABASE -- implements a 'repository' interface

      public:

        //! Read a package record from the database
        virtual std::unique_ptr< package_record<number> > query_package(const std::string& name) override;

        //! Read a task record from the database
        virtual std::unique_ptr< task_record<number> > query_task(const std::string& name) override;

        //! Read a derived product specification from the database
        virtual std::unique_ptr< derived_product_record<number> > query_derived_product(const std::string& name) override;


        // ENUMERATE DATABASE RECORDS

      public:

        //! Enumerate package records
        virtual typename package_db<number>::type enumerate_packages() override;

        //! Enumerate task records
        virtual typename task_db<number>::type enumerate_tasks() override;

        //! Enumerate derived product records
        virtual typename derived_product_db<number>::type enumerate_derived_products() override;


        // ENUMERATE OUTPUT GROUPS

        //! Enumerate the output groups available from a named integration task
        virtual integration_content_db enumerate_integration_task_content(const std::string& name) override;

        //! Enumerate the output groups available for a named postintegration task
        virtual postintegration_content_db enumerate_postintegration_task_content(const std::string& name) override;

        //! Enumerate the output groups available from a named output task
        virtual output_content_db enumerate_output_task_content(const std::string& name) override;


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
        virtual std::unique_ptr< package_record<number> > package_record_factory(const initial_conditions<number>& ics) override;

        //! Create a new integration task record from an explicit object
        virtual std::unique_ptr< integration_task_record<number> > integration_task_record_factory(const twopf_task<number>& tk) override;
        virtual std::unique_ptr< integration_task_record<number> > integration_task_record_factory(const threepf_task<number>& tk) override;

        // generic integration task factory
        template <typename TaskType>
        std::unique_ptr< integration_task_record<number> > integration_task_record_factory_generic(const TaskType& tk);

        //! Create a new output task record from an explicit object
        virtual std::unique_ptr< output_task_record<number> > output_task_record_factory(const output_task<number>& tk) override;

        //! Create a postintegration task record from an explicit object
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(const zeta_twopf_task<number>& tk) override;
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(const zeta_threepf_task<number>& tk) override;
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(const fNL_task<number>& tk) override;

        //! generic postintegration task record factory
        template <typename TaskType>
        std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory_generic(const TaskType& tk);

        //! Create a new derived product record from explicit object
        virtual std::unique_ptr< derived_product_record<number> > derived_product_record_factory(const derived_data::derived_product<number>& prod) override;

        //! Create a new content group for an integration task
        virtual std::unique_ptr< output_group_record<integration_payload> > integration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                                     bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) override;

        //! Create a new content group for a postintegration task
        virtual std::unique_ptr< output_group_record<postintegration_payload> > postintegration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                                             bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) override;

        //! Create a new content group for an output task
        virtual std::unique_ptr< output_group_record<output_payload> > output_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                           bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) override;

        //! Implementation -- Create a new content group record
        template <typename Payload>
        std::unique_ptr< output_group_record<Payload> > content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                     bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg);

        // 2. Factories from JSON representations: these are needed as part of the 'json_repository<>' interface

      protected:

        //! Create a package record from a JSON value
        virtual std::unique_ptr< package_record<number> > package_record_factory(Json::Value& reader) override;

        //! Create an integration task record from a JSON value
        virtual std::unique_ptr< integration_task_record<number> > integration_task_record_factory(Json::Value& reader) override;

        //! Create an output task record from a JSON value
        virtual std::unique_ptr< output_task_record<number> > output_task_record_factory(Json::Value& reader) override;

        //! Create a postintegration task record from a JSON value
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(Json::Value& reader) override;

        //! create a new derived product record from a JSON value
        virtual std::unique_ptr< derived_product_record<number> > derived_product_record_factory(Json::Value& reader) override;

        //! Create a new content group for an integration task from a JSON value
        virtual std::unique_ptr< output_group_record<integration_payload> > integration_content_group_record_factory(Json::Value& reader) override;

        //! Create a new content group for a postintegration task from a JSON value
        virtual std::unique_ptr< output_group_record<postintegration_payload> > postintegration_content_group_record_factory(Json::Value& reader) override;

        //! Create a new content group for an output task from a JSON value
        virtual std::unique_ptr< output_group_record<output_payload> > output_content_group_record_factory(Json::Value& reader) override;

        //! Implementation -- Create a new content group record
        template <typename Payload>
        std::unique_ptr< output_group_record<Payload> > content_group_record_factory(Json::Value& reader);


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
        void enumerate_content_groups(const std::string& name, std::map< boost::posix_time::ptime, std::unique_ptr < output_group_record<Payload> > >& db);


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

        virtual std::string reserve_content_name(const std::string& tk, boost::filesystem::path& parent_path, boost::posix_time::ptime& now, const std::string& suffix) override;


        // REGISTER AND RECOVER WRITERS

      public:

        //! perform recovery on the repository (this is a global operation -- it affects all tasks)
        virtual void perform_recovery(data_manager<number>& data_mgr, unsigned int worker) override;

        //! register an integration writer
        virtual void register_writer(integration_writer<number>& writer) override;

        //! register a postintegration writer
        virtual void register_writer(postintegration_writer<number>& writer) override;

        //! register a derived-content writer
        virtual void register_writer(derived_content_writer<number>& writer) override;

      protected:

        //! recover a list of hot integrations
        void recover_integrations(data_manager<number>& data_mgr, std::list<inflight_integration>& list, unsigned int worker);

        //! recover a list of hot postintegrations
        void recover_postintegrations(data_manager<number>& data_mgr, std::list<inflight_postintegration>& p_list,
                                      std::list<inflight_integration>& i_list, unsigned int worker);

        //! recover a list of hot derived-content writers
        void recover_derived_content(data_manager<number>& data_mgr, std::list<inflight_derived_content>& list, unsigned int worker);

      protected:

        //! construct an integration_writer in recovery mode
        std::unique_ptr< integration_writer<number> > get_integration_recovery_writer(const inflight_integration& data, data_manager<number>& data_mgr,
                                                                                      integration_task_record<number>& rec, unsigned int worker);

        //! construct a postintegration_writer in recovery mode
        std::unique_ptr< postintegration_writer<number> > get_postintegration_recovery_writer(const inflight_postintegration& data,
                                                                                              data_manager<number>& data_mgr,
                                                                                              postintegration_task_record<number>& rec, unsigned int worker);

        void recover_unpaired_postintegration(const inflight_postintegration& data, data_manager<number>& data_mgr,
                                              postintegration_task_record<number>& rec, unsigned int worker);

        void recover_paired_postintegration(const inflight_postintegration& data, data_manager<number>& data_mgr,
                                            postintegration_task_record<number>& p_rec,
                                            std::list<inflight_integration>& i_list, unsigned int worker);


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

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_SQLITE3_DECL_H
