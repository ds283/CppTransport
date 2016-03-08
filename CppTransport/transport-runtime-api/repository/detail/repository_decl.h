//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_DECL_H
#define CPPTRANSPORT_REPOSITORY_DECL_H


namespace transport
  {

    enum class repository_mode
      {
        readonly,
        readwrite
      };


    template <typename number>
    class repository
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a repository object
        repository(const std::string& path, model_manager <number>& f, repository_mode mode,
                   error_handler e, warning_handler w, message_handler m,
                   package_finder<number> pf, task_finder<number> tf, derived_product_finder<number> dpf);

        //! Explicitly delete copy constructor, preventing duplication (repository instances should be unique)
        repository(const repository<number>& obj) = delete;

        //! Close a repository, including the corresponding containers and environment. In practice this would always be delegated to the implementation class
        virtual ~repository() = default;


        // ADMINISTRATION

      public:

        //! Get path to root of repository
        const boost::filesystem::path& get_root_path() const { return (this->root_path); };

        //! Get access mode
        const repository_mode& get_access_mode() const { return (this->access_mode); }


        // TRANSACTIONS

      protected:

        //! Generate a transaction management object
        transaction_manager transaction_factory(std::unique_ptr<transaction_handler> handle);

        //! Release resources after end of transaction
        void release_transaction();


        // CREATE RECORDS

      public:

        // For all commit() methods, no record with the supplied name should
        // already exist; if it does, this is an error.
        // This prevents records being changed later, which could make already-generated output
        // inconsistent with the task specifications stored in the repository

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        virtual void commit(const initial_conditions<number>& ics) = 0;

        //! Write an integration task to the database.
        virtual void commit(const integration_task<number>& tk) = 0;

        //! Write an output task to the database
        virtual void commit(const output_task<number>& tk) = 0;

        //! Write a postintegration task to the database
        virtual void commit(const postintegration_task<number>& tk) = 0;

        //! Write a derived product specification
        virtual void commit(const derived_data::derived_product<number>& d) = 0;


        // READ RECORDS FROM THE DATABASE

      public:

        //! Read a package record from the database
        virtual std::unique_ptr< package_record<number> > query_package(const std::string& name) = 0;

        //! Read a task record from the database
        virtual std::unique_ptr< task_record<number> > query_task(const std::string& name) = 0;

        //! Read a derived product specification from the database
        virtual std::unique_ptr< derived_product_record<number> > query_derived_product(const std::string& name) = 0;

        //! Enumerate the output groups available from a named integration task
        virtual std::list< std::unique_ptr< output_group_record<integration_payload> > > enumerate_integration_task_content(const std::string& name) = 0;

        //! Enumerate the output groups available for a named postintegration task
        virtual std::list< std::unique_ptr< output_group_record<postintegration_payload> > > enumerate_postintegration_task_content(const std::string& name) = 0;

        //! Enumerate the output groups available from a named output task
        virtual std::list< std::unique_ptr< output_group_record<output_payload> > > enumerate_output_task_content(const std::string& name) = 0;


        // GENERATE WRITERS FOR TASKS
        // The 'base' forms perform common tasks, using a suite of callbacks provided.
        // The repository<> class provides a default implementation, but it can be overridden
        // by an implementation if desired.

      public:

        //! Generate a writer object for new integration output
        virtual std::unique_ptr< integration_writer<number> > new_integration_task_content(integration_task_record<number>* rec,
                                                                                           const std::list<std::string>& tags,
                                                                                           unsigned int worker, unsigned int workgroup, std::string suffix="");

        //! Generate a writer object for new derived-content output
        virtual std::unique_ptr< derived_content_writer<number> > new_output_task_content(output_task_record<number>* rec,
                                                                                          const std::list<std::string>& tags,
                                                                                          unsigned int worker, std::string suffix="");

        //! Generate a writer object for new postintegration output
        virtual std::unique_ptr< postintegration_writer<number> > new_postintegration_task_content(postintegration_task_record<number>* rec,
                                                                                                   const std::list<std::string>& tags,
                                                                                                   unsigned int worker, std::string suffix="");

      protected:

        //! Generate a writer object for integration recovery
        virtual std::unique_ptr< integration_writer<number> > recover_integration_task_content(const std::string& name, integration_task_record<number>* rec,
                                                                                               const boost::filesystem::path& output, const boost::filesystem::path& sql_path,
                                                                                               const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                                               unsigned int worker, unsigned int workgroup);

        //! Generate a writer object for postintegration recovery
        virtual std::unique_ptr< postintegration_writer<number> > recover_postintegration_task_content(const std::string& name, postintegration_task_record<number>* rec,
                                                                                                       const boost::filesystem::path& output, const boost::filesystem::path& sql_path,
                                                                                                       const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                                                       unsigned int worker);

        //! Generate a writer object for output-task recovery
        virtual std::unique_ptr< derived_content_writer<number> > recover_output_task_content(const std::string& name, output_task_record<number>* rec,
                                                                                              const boost::filesystem::path& output,
                                                                                              const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                                              unsigned int worker);


      protected:

        // register an in-flight content group
        virtual std::string reserve_content_name(const std::string& tk, boost::filesystem::path& parent_path, boost::posix_time::ptime& now, const std::string& suffix) = 0;

        //! generate an integration writer
        std::unique_ptr< integration_writer<number> > base_new_integration_task_content(integration_task_record<number>* rec,
                                                                                        const std::list<std::string>& tags,
                                                                                        unsigned int worker, unsigned int workgroup,
                                                                                        std::unique_ptr< repository_integration_writer_commit<number> > commit,
                                                                                        std::unique_ptr< repository_integration_writer_abort<number> > abort,
                                                                                        std::string suffix="");

        //! generate an integration writer for recovery
        std::unique_ptr< integration_writer<number> > base_recover_integration_task_content(const std::string& name, integration_task_record<number>* rec,
                                                                                            const boost::filesystem::path& output_path, const boost::filesystem::path& sql_path,
                                                                                            const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                                            unsigned int worker, unsigned int workgroup,
                                                                                            std::unique_ptr< repository_integration_writer_commit<number> > commit,
                                                                                            std::unique_ptr< repository_integration_writer_abort<number> > abort);

        //! generate a derived content writer
        std::unique_ptr< derived_content_writer<number> > base_new_output_task_content(output_task_record<number>* rec,
                                                                                       const std::list<std::string>& tags, unsigned int worker,
                                                                                       std::unique_ptr< repository_derived_content_writer_commit<number> > commit,
                                                                                       std::unique_ptr< repository_derived_content_writer_abort<number> > abort,
                                                                                       std::string suffix="");

        //! generate a postintegration writer for recovery
        std::unique_ptr < derived_content_writer<number> > base_recover_output_task_content(const std::string& name, output_task_record<number>* rec,
                                                                                            const boost::filesystem::path& output_path, const boost::filesystem::path& logdir_path,
                                                                                            const boost::filesystem::path& tempdir_path, unsigned int worker,
                                                                                            std::unique_ptr< repository_derived_content_writer_commit<number> > commit,
                                                                                            std::unique_ptr< repository_derived_content_writer_abort<number> > abort);

        std::unique_ptr< postintegration_writer<number> > base_new_postintegration_task_content(postintegration_task_record<number>* rec,
                                                                                                const std::list<std::string>& tags, unsigned int worker,
                                                                                                std::unique_ptr< repository_postintegration_writer_commit<number> > commit,
                                                                                                std::unique_ptr< repository_postintegration_writer_abort<number> > abort,
                                                                                                std::string suffix="");

        //! generate a postintegration writer for recovery
        std::unique_ptr < postintegration_writer<number> > base_recover_postintegration_task_content(const std::string& name, postintegration_task_record<number>* rec,
                                                                                                     const boost::filesystem::path& output_path, const boost::filesystem::path& sql_path,
                                                                                                     const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                                                     unsigned int worker,
                                                                                                     std::unique_ptr< repository_postintegration_writer_commit<number> > commit,
                                                                                                     std::unique_ptr< repository_postintegration_writer_abort<number> > abort);


        // PERFORM RECOVERY ON CRASHED WRITERS

      public:

        //! recover crashed writers
        virtual void perform_recovery(data_manager<number>& data_mgr, unsigned int worker) = 0;

        //! register an integration writer
        virtual void register_writer(integration_writer<number>& writer) = 0;

        //! register a postintegration writer
        virtual void register_writer(postintegration_writer<number>& writer) = 0;

        //! register a derived-content writer
        virtual void register_writer(derived_content_writer<number>& writer) = 0;


        // FIND AN OUTPUT GROUP MATCHING DEFINED TAGS

      public:

        //! Find an output group for an integration task
        std::unique_ptr< output_group_record<integration_payload> > find_integration_task_output(const std::string& name, const std::list<std::string>& tags);

        //! Find an output group for a postintegration task
        std::unique_ptr< output_group_record<postintegration_payload> > find_postintegration_task_output(const std::string& name, const std::list<std::string>& tags);


        // STANDARD WRITER CALLBACKS

      protected:

        //! Advise that an output group has been committed
        template <typename Payload>
        void advise_commit(output_group_record<Payload>* group);

        //! Commit the products from an integration to the database
        void close_integration_writer(integration_writer<number>& writer);

        //! Rollback a failed integration
        void abort_integration_writer(integration_writer<number>& writer);

        //! Commit the products from a postintegration to the database
        void close_postintegration_writer(postintegration_writer<number>& writer);

        //! Rollback a failed postintegration
        void abort_postintegration_writer(postintegration_writer<number>& writer);

        //! Commit the products from an output task to the database
        void close_derived_content_writer(derived_content_writer<number>& writer);

        //! Rollback a failed integration
        void abort_derived_content_writer(derived_content_writer<number>& writer);


        friend repository_integration_writer_commit<number>;
        friend repository_postintegration_writer_commit<number>;
        friend repository_derived_content_writer_commit<number>;

        friend repository_integration_writer_abort<number>;
        friend repository_postintegration_writer_abort<number>;
        friend repository_derived_content_writer_abort<number>;


        // REPOSITORY RECORD FACTORIES -- USED TO OBTAIN REPOSITORY RECORD CLASSES FROM OTHER REPRESENTATIONS
        // A basic repository need only provide factories based on the real class hierarchy

      protected:

        //! Create a new package record from an explicit object
        virtual std::unique_ptr< package_record<number> > package_record_factory(const initial_conditions<number>& ics) = 0;

        //! Create a new integration task record from an explicit object
        virtual std::unique_ptr< integration_task_record<number> > integration_task_record_factory(const integration_task<number>& tk) = 0;

        //! Create a new output task record from an explicit object
        virtual std::unique_ptr< output_task_record<number> > output_task_record_factory(const output_task<number>& tk) = 0;

        //! Create a postintegration task record from an explicit object
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(const postintegration_task<number>& tk) = 0;

        //! Create a new derived product record from explicit object
        virtual std::unique_ptr< derived_product_record<number> > derived_product_record_factory(const derived_data::derived_product<number>& prod) = 0;

        //! Create a new content group for an integration task
        virtual std::unique_ptr< output_group_record<integration_payload> > integration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                                     bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) = 0;

        //! Create a new content group for a postintegration task
        virtual std::unique_ptr< output_group_record<postintegration_payload> > postintegration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                                             bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) = 0;

        //! Create a new content group for an output task
        virtual std::unique_ptr< output_group_record<output_payload> > output_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                           bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) = 0;


        // PRIVATE DATA

      protected:

        //! Access mode
        const repository_mode access_mode;

        //! BOOST path to the repository root directory
        const boost::filesystem::path root_path;


        // ERROR, WARNING, MESSAGE HANDLERS

        //! error handler
        error_handler error;

        //! warning handler
        warning_handler warning;

        //! message handler
        message_handler message;


        // FINDER SERVICES

        //! Cached model-finder supplied by instance manager
        model_manager<number>& m_finder;

        //! Cached package-finder instance
        package_finder<number> pkg_finder;

        //! Cached task-finder instance
        task_finder<number> tk_finder;

        //! Cached derived-product-finder instance
        derived_product_finder<number> dprod_finder;


        // TRANSACTIONS

        //! Number of open transactions on the database
        unsigned int transactions;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_DECL_H
