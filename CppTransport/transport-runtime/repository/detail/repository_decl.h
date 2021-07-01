//
// Created by David Seery on 25/01/2016.
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

#ifndef CPPTRANSPORT_REPOSITORY_DECL_H
#define CPPTRANSPORT_REPOSITORY_DECL_H



namespace transport
  {

    // hint for query_task() method
    enum query_task_hint
      {
        integration, postintegration, output, no_hint
      };


    namespace repository_impl
      {

        class TrivialContentGroupFilterPredicate
          {
          public:

            //! constructor is default
            TrivialContentGroupFilterPredicate() = default;

            //! destructor is default
            ~TrivialContentGroupFilterPredicate() = default;


            // INTERFACE

          public:

            //! return true for every content group
            bool operator()(const std::string& group) const { return true; }

          };


        //! MostRecentContentPolicy is the default content group policy.
        //! It returns the most recent matching content group.
        template <typename ContentDatabaseType>
        class MostRecentContentPolicy
          {

          public:

            //! constructor is default
            MostRecentContentPolicy() = default;

            //! destructor is default
            ~MostRecentContentPolicy() = default;


            // INTERFACE

          public:

            //! Select a content group from a database of candidate groups according to our policy.
            //! Note that the database of candidates may be empty.
            //! If we decline to select a group, we can return an empty boost::optional<>
            boost::optional< typename ContentDatabaseType::value_type& > operator()(ContentDatabaseType& matches) const
              {
                // test whether database is empty
                if(matches.empty())
                  return boost::none;

                // assume ContentDatabaseType is stored in lexical order, in which case the most recent
                // content group will be at the back.
                // here, it is safe to assume that the database is not empty.
                return *(matches.rbegin());
              }

          };


      }   // namespace repository_impl


    template <typename number>
    class repository
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a repository object
        repository(const boost::filesystem::path& path, model_manager <number>& f, repository_mode mode,
                   local_environment& ev, argument_cache& ar,
                   package_finder<number> pf, task_finder<number> tf, derived_product_finder<number> dpf);

        //! Explicitly delete copy constructor, preventing duplication (repository instances should be unique)
        repository(const repository<number>& obj) = delete;

        //! Close a repository, including the corresponding containers and environment. In practice this would always be delegated to the implementation class
        virtual ~repository() = default;


        // ADMINISTRATION

      public:

        //! Get path to root of repository
        const boost::filesystem::path& get_root_path() const { return(this->root_path); };

        //! Get access mode
        const repository_mode& get_access_mode() const { return (this->access_mode); }

        //! Get repository name, defined to be the directory leafname
        std::string get_name() const;


        // TRANSACTIONS

      public:

        //! Generate a transaction management object -- supplied by implementation class
        virtual transaction_manager transaction_factory() = 0;

      protected:

        //! Generate a transaction management object
        transaction_manager generate_transaction_manager(std::unique_ptr<transaction_handler> handle);

        //! Begin transaction
        void begin_transaction();

        //! Commit transaction
        void commit_transaction();

        //! Abort transaction
        void abort_transaction();

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
        virtual void commit(transaction_manager& mgr, const initial_conditions<number>& ics) = 0;

        //! Write an integration task to the database.
        virtual void commit(const twopf_task<number>& tk) = 0;
        virtual void commit(const threepf_task<number>& tk) = 0;
        virtual void commit(transaction_manager& mgr, const twopf_task<number>& tk) = 0;
        virtual void commit(transaction_manager& mgr, const threepf_task<number>& tk) = 0;

        //! Write an output task to the database
        virtual void commit(const output_task<number>& tk) = 0;
        virtual void commit(transaction_manager& mgr, const output_task<number>& tk) = 0;

        //! Write a postintegration task to the database
        virtual void commit(const zeta_twopf_task<number>& tk) = 0;
        virtual void commit(const zeta_threepf_task<number>& tk) = 0;
        virtual void commit(const fNL_task<number>& tk) = 0;
        virtual void commit(transaction_manager& mgr, const zeta_twopf_task<number>& tk) = 0;
        virtual void commit(transaction_manager& mgr, const zeta_threepf_task<number>& tk) = 0;
        virtual void commit(transaction_manager& mgr, const fNL_task<number>& tk) = 0;

        //! Write a derived product specification
        virtual void commit(const derived_data::derived_product<number>& d) = 0;
        virtual void commit(transaction_manager& mgr, const derived_data::derived_product<number>& d) = 0;


        // SEARCH FOR DATABASE RECORDS

      public:

        // READ ONLY

        // Read-only records are first checked against a cache. If a match is found, the record is returned from
        // the cache rather than deserializing it from the disk and constructing a new record object.
        // This avoids expensive multiple deserializations, either where the physical medium is slow (eg. network drive)
        // or there is a lot of data to ingest (eg. large integration task which requires deserializing a big
        // SQLite database)

        // The cache is invalidated when a transaction begins, ends or aborts. After any of these events,
        // the repository will go out to the physical database in order to serve an up-to-date copy

        // Read-write records do not use the cache, but are always reconstructed by deserializing data in the
        // physical database. This is to ensure that we always have an up-to-date version of the repository record
        // before a new write takes place, so that data can't get lost.

        // For read-only records, this procedure does give a risk that the repository will serve a stale copy
        // of a record where two processes are independently writing to the database. Suppose Process A reads a copy
        // of record X and stores it in its cache. Then Process B opens a transaction on the database and modified X.
        // A knows nothing of this, since it can only track its own transactions. Then if a client of the repository in
        // A asks for a read-only copy of X it will be served from the cache, meaning that it is stale.
        // Of course, if A subsequently converts this to a read-write copy then it will pick up the changes made by B.

        // This problem could be avoided with a significant overhead on constructing all read-only records, by checking
        // a datestamp in the database. But the problem isn't any different to the first part of the scenario
        // above, where A reads and caches a record which B subsequently alters. Then the original read-only record
        // served by A is already out-of-date. Because there is no realistic way to eliminate this in any multi-process
        // distributed system (and because all read-write records are always safe) we choose to live with it.

        //! Read a package record from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< package_record<number> > query_package(const std::string& name) = 0;

        //! Read a task record from the database
        //! Without a transaction_manager object, the returned record is readonly
        //! If supplied, the hint is used to search more efficiently.
        //! Note the use of a default argument in a virtual function: to avoid problems with static scoping rules, all
        //! implementations should use the same default argument
        //! (the choice of which default to supply is determined by the static scope)
        virtual std::unique_ptr< task_record<number> > query_task(const std::string& name, query_task_hint hint=query_task_hint::no_hint) = 0;

        //! Read a derived product specification from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< derived_product_record<number> > query_derived_product(const std::string& name) = 0;

        //! Read an integration content group specification from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< content_group_record<integration_payload> > query_integration_content(const std::string& name) = 0;

        //! Read a postintegration content group specification from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< content_group_record<postintegration_payload> > query_postintegration_content(const std::string& name) = 0;

        //! Read an output content group specification from the database
        //! Without a transaction_manager object, the returned record is readonly
        virtual std::unique_ptr< content_group_record<output_payload> > query_output_content(const std::string& name) = 0;

        // READ/WRITE

        //! Read a package record from the database
        virtual std::unique_ptr< package_record<number> > query_package(const std::string& name, transaction_manager& mgr) = 0;

        //! Read a task record from the database
        //! If supplied, the hint is used to search more efficiently.
        //! Note the use of a default argument in a virtual function: to avoid problems with static scoping rules, all
        //! implementations should use the same default argument
        //! (the choice of which default to supply is determined by the static scope)
        virtual std::unique_ptr< task_record<number> > query_task(const std::string& name, transaction_manager& mgr, query_task_hint hint=query_task_hint::no_hint) = 0;

        //! Read a derived product specification from the database
        virtual std::unique_ptr< derived_product_record<number> > query_derived_product(const std::string& name, transaction_manager& mgr) = 0;

        //! Read an integration content group specification from the database
        virtual std::unique_ptr< content_group_record<integration_payload> > query_integration_content(const std::string& name, transaction_manager& mgr) = 0;

        //! Read a postintegration content group specification from the database
        virtual std::unique_ptr< content_group_record<postintegration_payload> > query_postintegration_content(const std::string& name, transaction_manager& mgr) = 0;

        //! Read an output content group specification from the database
        virtual std::unique_ptr< content_group_record<output_payload> > query_output_content(const std::string& name, transaction_manager& mgr) = 0;


        // ENUMERATE DATABASE RECORDS

      public:

        //! Enumerate package records; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual typename package_db<number>::type enumerate_packages() = 0;

        //! Enumerate task records; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual typename task_db<number>::type enumerate_tasks() = 0;

        //! Enumerate derived product records; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual typename derived_product_db<number>::type enumerate_derived_products() = 0;


        // ENUMERATE OUTPUT GROUPS

      public:

        //! Enumerate the content groups available from a named integration task; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual integration_content_db enumerate_integration_task_content(const std::string& name) = 0;

        //! Enumerate all integration content groups; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual integration_content_db enumerate_integration_task_content() = 0;

        //! Enumerate the content groups available for a named postintegration task; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual postintegration_content_db enumerate_postintegration_task_content(const std::string& name) = 0;

        //! Enumerate all postintegration content groups; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual postintegration_content_db enumerate_postintegration_task_content() = 0;

        //! Enumerate the content groups available from a named output task; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual output_content_db enumerate_output_task_content(const std::string& name) = 0;

        //! Enumerate all content groups; all records are returned in a readonly state, so if updates
        //! are required each individual record must be re-queried using the query_*() methods
        virtual output_content_db enumerate_output_task_content() = 0;


        // RECORD CACHE, CACHE MANAGEMENT

      protected:

        //! flush record cache
        void flush_caches();


        // REMOVE OUTPUT GROUPS

        // TO BE USED WITH CARE, in case the repository gets into an inconsistent state
        // Only content groups without dependents should be removed.
        // The repository itself will not necessarily check for dependents (this is implementation dependent)

        // repository_toolkit provides a front end for this API that builds a proper dependency graph
        // and checks for safety

      public:

        //! Remove an integration group
        virtual void delete_integration_content(const std::string& name, const std::string& task_name, transaction_manager& mgr) = 0;

        //! Remove a postintegration group
        virtual void delete_postintegration_content(const std::string& name, const std::string& task_name, transaction_manager& mgr) = 0;

        //! Remove an output group
        virtual void delete_output_content(const std::string& name, const std::string& task_name, transaction_manager& mgr) = 0;


        // ENUMERATE INFLIGHT TASKS

      public:

        //! Enumerate all active jobs
        virtual inflight_db enumerate_inflight() = 0;

        //! Enumerate all active integrations
        virtual inflight_integration_db enumerate_inflight_integrations() = 0;

        //! Enumerate all active postintegrations
        virtual inflight_postintegration_db enumerate_inflight_postintegrations() = 0;

        //! Enumerate all active derived content tasks
        virtual inflight_derived_content_db enumerate_inflight_derived_content() = 0;


        // GENERATE WRITERS FOR TASKS
        // The 'base' forms perform common tasks, using a suite of callbacks provided.
        // The repository<> class provides a default implementation, but it can be overridden
        // by an implementation if desired.

      public:

        //! Generate a writer object for new integration output
        virtual std::unique_ptr< integration_writer<number> > new_integration_task_content(integration_task_record<number>& rec, const tag_list& tags,
                                                                                           unsigned int worker, unsigned int workgroup,
                                                                                           unsigned int num_cores, std::string suffix="");

        //! Generate a writer object for new derived-content output
        virtual std::unique_ptr< derived_content_writer<number> > new_output_task_content(output_task_record<number>& rec, const tag_list& tags,
                                                                                          unsigned int worker, unsigned int num_cores, std::string suffix="");

        //! Generate a writer object for new postintegration output
        virtual std::unique_ptr< postintegration_writer<number> > new_postintegration_task_content(postintegration_task_record<number>& rec, const tag_list& tags,
                                                                                                   unsigned int worker, unsigned int num_cores, std::string suffix="");

      protected:

        //! Generate a writer object for integration recovery
        virtual std::unique_ptr< integration_writer<number> > recover_integration_task_content(const std::string& name, integration_task_record<number>& rec,
                                                                                               const boost::filesystem::path& output, const boost::filesystem::path& sql_path,
                                                                                               const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                                               unsigned int worker, unsigned int workgroup);

        //! Generate a writer object for postintegration recovery
        virtual std::unique_ptr< postintegration_writer<number> > recover_postintegration_task_content(const std::string& name, postintegration_task_record<number>& rec,
                                                                                                       const boost::filesystem::path& output, const boost::filesystem::path& sql_path,
                                                                                                       const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                                                       unsigned int worker);

        //! Generate a writer object for output-task recovery
        virtual std::unique_ptr< derived_content_writer<number> > recover_output_task_content(const std::string& name, output_task_record<number>& rec,
                                                                                              const boost::filesystem::path& output,
                                                                                              const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                                              unsigned int worker);


      protected:

        // register an in-flight content group
        virtual std::string reserve_content_name(const std::string& tk, boost::filesystem::path& parent_path, boost::posix_time::ptime& now,
                                                 const std::string& suffix, unsigned int num_cores) = 0;

        //! generate an integration writer
        std::unique_ptr< integration_writer<number> > base_new_integration_task_content(integration_task_record<number>& rec,
                                                                                        const tag_list& tags,
                                                                                        unsigned int worker, unsigned int workgroup, unsigned int num_cores,
                                                                                        std::unique_ptr< repository_integration_writer_commit<number> > commit,
                                                                                        std::unique_ptr< repository_integration_writer_abort<number> > abort,
                                                                                        std::string suffix="");

        //! generate an integration writer for recovery
        std::unique_ptr< integration_writer<number> > base_recover_integration_task_content(const std::string& name, integration_task_record<number>& rec,
                                                                                            const boost::filesystem::path& output_path, const boost::filesystem::path& sql_path,
                                                                                            const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                                            unsigned int worker, unsigned int workgroup,
                                                                                            std::unique_ptr< repository_integration_writer_commit<number> > commit,
                                                                                            std::unique_ptr< repository_integration_writer_abort<number> > abort);

        //! generate a derived content writer
        std::unique_ptr< derived_content_writer<number> > base_new_output_task_content(output_task_record<number>& rec,
                                                                                       const tag_list& tags,
                                                                                       unsigned int worker, unsigned int num_cores,
                                                                                       std::unique_ptr< repository_derived_content_writer_commit<number> > commit,
                                                                                       std::unique_ptr< repository_derived_content_writer_abort<number> > abort,
                                                                                       std::string suffix="");

        //! generate a postintegration writer for recovery
        std::unique_ptr < derived_content_writer<number> > base_recover_output_task_content(const std::string& name, output_task_record<number>& rec,
                                                                                            const boost::filesystem::path& output_path, const boost::filesystem::path& logdir_path,
                                                                                            const boost::filesystem::path& tempdir_path, unsigned int worker,
                                                                                            std::unique_ptr< repository_derived_content_writer_commit<number> > commit,
                                                                                            std::unique_ptr< repository_derived_content_writer_abort<number> > abort);

        std::unique_ptr< postintegration_writer<number> > base_new_postintegration_task_content(postintegration_task_record<number>& rec,
                                                                                                const tag_list& tags,
                                                                                                unsigned int worker, unsigned int num_cores,
                                                                                                std::unique_ptr< repository_postintegration_writer_commit<number> > commit,
                                                                                                std::unique_ptr< repository_postintegration_writer_abort<number> > abort,
                                                                                                std::string suffix="");

        //! generate a postintegration writer for recovery
        std::unique_ptr < postintegration_writer<number> > base_recover_postintegration_task_content(const std::string& name, postintegration_task_record<number>& rec,
                                                                                                     const boost::filesystem::path& output_path, const boost::filesystem::path& sql_path,
                                                                                                     const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                                                     unsigned int worker,
                                                                                                     std::unique_ptr< repository_postintegration_writer_commit<number> > commit,
                                                                                                     std::unique_ptr< repository_postintegration_writer_abort<number> > abort);


        // WRITER MANAGEMENT

      public:

        //! Advise completion time for a writer
        virtual void advise_completion_time(const std::string& name, const boost::posix_time::ptime& time) = 0;

        //! register an integration writer
        virtual void register_writer(integration_writer<number>& writer) = 0;

        //! register a postintegration writer
        virtual void register_writer(postintegration_writer<number>& writer) = 0;

        //! register a derived-content writer
        virtual void register_writer(derived_content_writer<number>& writer) = 0;

        //! deregister an integration writer
        virtual void deregister_writer(integration_writer<number>& writer, transaction_manager& mgr) = 0;

        //! deregister a postintegration writer
        virtual void deregister_writer(postintegration_writer<number>& writer, transaction_manager& mgr) = 0;

        //! deregister a derived-content writer
        virtual void deregister_writer(derived_content_writer<number>& writer, transaction_manager& mgr) = 0;

        //! recover crashed writers
        virtual void perform_recovery(data_manager<number>& data_mgr, unsigned int worker) = 0;


        // FIND AN OUTPUT GROUP MATCHING DEFINED TAGS

      public:

        //! Find a content group for an integration task
        //! If raise is true, an exception is raised if no suitable content group can be found.
        //! This is the default behaviour. If raise is false, no exception is raised and
        //! an empty std::unique_ptr<> is returned if a group cannot be matched.
        template <typename FilterPredicate = repository_impl::TrivialContentGroupFilterPredicate,
                  typename ContentSelectorPolicy = repository_impl::MostRecentContentPolicy<integration_content_db> >
        std::unique_ptr< content_group_record<integration_payload> >
        find_integration_task_output(const std::string& name, FilterPredicate filter=FilterPredicate(),
                                     bool raise=true, ContentSelectorPolicy policy=ContentSelectorPolicy());

        //! Find a content group for a postintegration task
        //! If raise is true, an exception is raised if no suitable content group can be found.
        //! This is the default behaviour. If raise is false, no exception is raised and
        //! an empty std::unique_ptr<> is returned if a group cannot be matched.
        template <typename FilterPredicate = repository_impl::TrivialContentGroupFilterPredicate,
                  typename ContentSelectorPolicy = repository_impl::MostRecentContentPolicy<postintegration_content_db> >
        std::unique_ptr< content_group_record<postintegration_payload> >
        find_postintegration_task_output(const std::string& name, FilterPredicate filter=FilterPredicate(),
                                         bool raise=true, ContentSelectorPolicy policy=ContentSelectorPolicy());


        // STANDARD WRITER CALLBACKS

      protected:

        //! Advise that a content group has been committed
        template <typename Payload>
        void advise_commit(content_group_record<Payload>* group);

        //! Commit the products from an integration to the database
        void close_integration_writer(integration_writer<number>& writer, transaction_manager& mgr);

        //! Rollback a failed integration
        void abort_integration_writer(integration_writer<number>& writer, transaction_manager& mgr);

        //! Commit the products from a postintegration to the database
        void close_postintegration_writer(postintegration_writer<number>& writer, transaction_manager& mgr);

        //! Rollback a failed postintegration
        void abort_postintegration_writer(postintegration_writer<number>& writer, transaction_manager& mgr);

        //! Commit the products from an output task to the database
        void close_derived_content_writer(derived_content_writer<number>& writer, transaction_manager& mgr);

        //! Rollback a failed integration
        void abort_derived_content_writer(derived_content_writer<number>& writer, transaction_manager& mgr);


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
        virtual std::unique_ptr< package_record<number> > package_record_factory(const initial_conditions<number>& ics, transaction_manager& mgr) = 0;

        //! Create a new integration task record from an explicit object
        virtual std::unique_ptr< integration_task_record<number> > integration_task_record_factory(const twopf_task<number>& tk, transaction_manager& mgr) = 0;
        virtual std::unique_ptr< integration_task_record<number> > integration_task_record_factory(const threepf_task<number>& tk, transaction_manager& mgr) = 0;

        //! Create a new output task record from an explicit object
        virtual std::unique_ptr< output_task_record<number> > output_task_record_factory(const output_task<number>& tk, transaction_manager& mgr) = 0;

        //! Create a postintegration task record from an explicit object
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(const zeta_twopf_task<number>& tk, transaction_manager& mgr) = 0;
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(const zeta_threepf_task<number>& tk, transaction_manager& mgr) = 0;
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(const fNL_task<number>& tk, transaction_manager& mgr) = 0;

        //! Create a new derived product record from explicit object
        virtual std::unique_ptr< derived_product_record<number> > derived_product_record_factory(const derived_data::derived_product<number>& prod, transaction_manager& mgr) = 0;

        //! Create a new content group for an integration task
        virtual std::unique_ptr< content_group_record<integration_payload> > integration_content_group_record_factory(integration_writer<number>& writer,
                                                                                                                      bool lock, const std::list<note>& nt,
                                                                                                                      transaction_manager& mgr) = 0;

        //! Create a new content group for a postintegration task
        virtual std::unique_ptr< content_group_record<postintegration_payload> > postintegration_content_group_record_factory(postintegration_writer<number>& writer,
                                                                                                                              bool lock, const std::list<note>& nt,
                                                                                                                              transaction_manager& mgr) = 0;

        //! Create a new content group for an output task
        virtual std::unique_ptr< content_group_record<output_payload> > output_content_group_record_factory(derived_content_writer<number>& writer,
                                                                                                            bool lock, const std::list<note>& nt,
                                                                                                            transaction_manager& mgr) = 0;


        // PRIVATE DATA

      protected:

        //! Access mode
        const repository_mode access_mode;

        //! BOOST path to the repository root directory
        const boost::filesystem::path root_path;


        // POLICY CLASSES

      protected:

        //! local environment
        local_environment& env;

        //! argument cache
        argument_cache& args;


        // ERROR, WARNING, MESSAGE HANDLERS

      protected:

        //! error handler
        error_handler error;

        //! warning handler
        warning_handler warning;

        //! message handler
        message_handler message;


        // FINDER SERVICES

      protected:

        //! Cached model-finder supplied by instance manager
        model_manager<number>& m_finder;

        //! Cached package-finder instance
        package_finder<number> pkg_finder;

        //! Cached task-finder instance
        task_finder<number> tk_finder;

        //! Cached derived-product-finder instance
        derived_product_finder<number> dprod_finder;


        // TRANSACTIONS

      protected:

        //! Number of open transactions on the database
        unsigned int transactions;


        // CACHES

      protected:

        //! package record cache type
        typedef std::map< std::string, std::unique_ptr< package_record<number> > > package_record_cache;

        //! package record cache
        package_record_cache pkg_cache;

        //! task record cache type
        typedef std::map< std::string, std::unique_ptr< task_record<number> > > task_record_cache;

        //! task record cache
        task_record_cache task_cache;

        //! derived product record cache type
        typedef std::map< std::string, std::unique_ptr< derived_product_record<number> > > derived_record_cache;

        //! derived record cache
        derived_record_cache derived_cache;

        //! integration content group cache type
        typedef std::map< std::string, std::unique_ptr< content_group_record<integration_payload> > > integration_content_cache;

        //! integration content group cache
        integration_content_cache int_cache;

        //! postintegration content group cache type
        typedef std::map< std::string, std::unique_ptr< content_group_record<postintegration_payload> > > postintegration_content_cache;

        //! postintegration content group cache
        postintegration_content_cache pint_cache;

        //! output content group cache type
        typedef std::map< std::string, std::unique_ptr< content_group_record<output_payload> > > output_content_cache;

        //! output content group cache
        output_content_cache out_cache;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_DECL_H
