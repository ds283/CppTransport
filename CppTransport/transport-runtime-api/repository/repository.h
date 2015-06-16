//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __repository_H_
#define __repository_H_


#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/manager/instance_manager.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare initial conditions class if needed
#include "transport-runtime-api/concepts/initial_conditions_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare derived_product if needed
#include "transport-runtime-api/derived-products/derived_product_forward_declare.h"

#include "transport-runtime-api/repository/records/repository_records.h"
#include "transport-runtime-api/repository/writers/writers.h"
#include "transport-runtime-api/repository/transaction_manager.h"

#include "boost/filesystem/operations.hpp"


#define __CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF "output"
#define __CPP_TRANSPORT_REPO_FAILURE_LEAF    "failed"
#define __CPP_TRANSPORT_REPO_LOGDIR_LEAF     "logs"
#define __CPP_TRANSPORT_REPO_TEMPDIR_LEAF    "tempfiles"
#define __CPP_TRANSPORT_REPO_DATABASE_LEAF   "data.sqlite"


namespace transport
	{

    // forward-declare 'key' class used to create repositories
    // the complete declaration is in a separate file,
    // which must be included to allow creation of repositories
    class repository_creation_key;


    template <typename number>
    class repository
	    {

      public:

        //! Error-reporting callback object
        typedef std::function<void(const std::string&)> error_callback;

        //! Warning callback object
        typedef std::function<void(const std::string&)> warning_callback;

        //! Message callback object
        typedef std::function<void(const std::string&)> message_callback;


        // LOGGING SERVICES

      public:

        //! Read-only/Read-write access to the repository
        typedef enum { readonly, readwrite } access_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a repository object
        repository(const std::string& path, access_type mode,
                   typename repository<number>::error_callback e,
                   typename repository<number>::warning_callback w,
                   typename repository<number>::message_callback m,
                   typename repository_finder<number>::package_finder pf,
                   typename repository_finder<number>::task_finder tf,
                   typename repository_finder<number>::derived_product_finder dpf);


        //! Close a repository, including the corresponding containers and environment. In practice this would always be delegated to the implementation class
        virtual ~repository() = default;


        // ADMINISTRATION

      public:

		    //! Set model_finder object
		    void set_model_finder(const typename instance_manager<number>::model_finder& f);

        //! Get path to root of repository
        const boost::filesystem::path& get_root_path() const { return (this->root_path); };

        //! Get access mode
        const access_type& get_access_mode() const { return (this->access_mode); }


		    // TRANSACTIONS

      protected:

		    //! Generate a transaction management object
		    transaction_manager transaction_factory(transaction_manager::open_handler o, transaction_manager::commit_handler c, transaction_manager::rollback_handler r);

		    //! Release resources after end of transaction
		    void release_transaction();


		    // CREATE RECORDS

      public:

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        virtual void commit_package(const initial_conditions<number>& ics) = 0;

        //! Write an integration task to the database.
        virtual void commit_task(const integration_task<number>& tk) = 0;

        //! Write an output task to the database
        virtual void commit_task(const output_task<number>& tk) = 0;

        //! Write a postintegration task to the database
        virtual void commit_task(const postintegration_task<number>& tk) = 0;

        //! Write a derived product specification
        virtual void commit_derived_product(const derived_data::derived_product<number>& d) = 0;


		    // READ RECORDS FROM THE DATABASE

      public:

		    //! Read a package record from the database
		    virtual package_record<number>* query_package(const std::string& name) = 0;

		    //! Read a task record from the database
		    virtual task_record<number>* query_task(const std::string& name) = 0;

		    //! Read a derived product specification from the database
		    virtual derived_product_record<number>* query_derived_product(const std::string& name) = 0;

        //! Enumerate the output groups available from a named integration task
        virtual std::list< std::shared_ptr< output_group_record<integration_payload> > > enumerate_integration_task_content(const std::string& name) = 0;

		    //! Enumerate the output groups available for a named postintegration task
		    virtual std::list< std::shared_ptr< output_group_record<postintegration_payload> > > enumerate_postintegration_task_content(const std::string& name) = 0;

		    //! Enumerate the output groups available from a named output task
		    virtual std::list< std::shared_ptr< output_group_record<output_payload> > > enumerate_output_task_content(const std::string& name) = 0;
		    

        // GENERATE WRITERS FOR TASKS
		    // The 'base' forms perform common tasks, using a suite of callbacks provided.
		    // The repository<> class provides a default implementation, but it can be overridden
		    // by an implementation if desired.

      public:

		    //! Generate a writer object for new integration output
		    virtual std::shared_ptr< integration_writer<number> > new_integration_task_content(integration_task_record<number>* rec,
                                                                                           const std::list<std::string>& tags,
                                                                                           unsigned int worker, unsigned int workgroup, std::string suffix="");

		    //! Generate a writer object for new derived-content output
		    virtual std::shared_ptr< derived_content_writer<number> > new_output_task_content(output_task_record<number>* rec,
                                                                                          const std::list<std::string>& tags,
                                                                                          unsigned int worker, std::string suffix="");

        //! Generate a writer object for new postintegration output
        virtual std::shared_ptr< postintegration_writer<number> > new_postintegration_task_content(postintegration_task_record<number>* rec,
                                                                                                   const std::list<std::string>& tags,
                                                                                                   unsigned int worker, std::string suffix="");

      protected:

        virtual std::string reserve_content_name(boost::posix_time::ptime& now, const std::string& suffix) = 0;

        std::shared_ptr <integration_writer<number>> base_new_integration_task_content(integration_task_record<number>* rec,
                                                                                       const std::list<std::string>& tags,
                                                                                       unsigned int worker, unsigned int workgroup,
                                                                                       typename integration_writer<number>::callback_group& callbacks,
                                                                                       std::string suffix="");

        std::shared_ptr <derived_content_writer<number>> base_new_output_task_content(output_task_record<number>* rec,
                                                                                      const std::list<std::string>& tags,
                                                                                      unsigned int worker,
                                                                                      typename derived_content_writer<number>::callback_group& callbacks,
                                                                                      std::string suffix="");

        std::shared_ptr <postintegration_writer<number>> base_new_postintegration_task_content(postintegration_task_record<number>* rec,
                                                                                               const std::list<std::string>& tags,
                                                                                               unsigned int worker,
                                                                                               typename postintegration_writer<number>::callback_group& callbacks,
                                                                                               std::string suffix="");


        // FIND AN OUTPUT GROUP MATCHING DEFINED TAGS

      public:

        //! Find an output group for an integration task
        std::shared_ptr< output_group_record<integration_payload> > find_integration_task_output(const std::string& name, const std::list<std::string>& tags);

		    //! Find an output group for a postintegration task
		    std::shared_ptr< output_group_record<postintegration_payload> > find_postintegration_task_output(const std::string& name, const std::list<std::string>& tags);


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


		    // REPOSITORY RECORD FACTORIES -- USED TO OBTAIN REPOSITORY RECORD CLASSES FROM OTHER REPRESENTATIONS
		    // A basic repository need only provide factories based on the real class hierarchy

      protected:

        //! Create a new package record from an explicit object
        virtual package_record<number>* package_record_factory(const initial_conditions<number>& ics) = 0;

        //! Create a new integration task record from an explicit object
        virtual integration_task_record<number>* integration_task_record_factory(const integration_task<number>& tk) = 0;

        //! Create a new output task record from an explicit object
        virtual output_task_record<number>* output_task_record_factory(const output_task<number>& tk) = 0;

        //! Create a postintegration task record from an explicit object
        virtual postintegration_task_record<number>* postintegration_task_record_factory(const postintegration_task<number>& tk) = 0;

        //! Create a new derived product record from explicit object
        virtual derived_product_record<number>* derived_product_record_factory(const derived_data::derived_product<number>& prod) = 0;

		    //! Create a new content group for an integration task
		    virtual output_group_record<integration_payload>* integration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                   bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) = 0;

		    //! Create a new content group for a postintegration task
		    virtual output_group_record<postintegration_payload>* postintegration_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                           bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) = 0;

		    //! Create a new content group for an output task
		    virtual output_group_record<output_payload>* output_content_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
		                                                                                     bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg) = 0;


        // PRIVATE DATA

      protected:

        //! Access mode
        const access_type access_mode;

        //! BOOST path to the repository root directory
        const boost::filesystem::path root_path;


		    // ERROR, WARNING, MESSAGE HANDLERS

		    //! error handler
		    error_callback error;

		    //! warning handler
		    warning_callback warning;

		    //! message handler
		    message_callback message;


		    // FINDER SERVICES

        //! Cached model-finder supplied by instance manager
        typename instance_manager<number>::model_finder model_finder;

        //! Cached package-finder instance
        typename repository_finder<number>::package_finder pkg_finder;

        //! Cached task-finder instance
        typename repository_finder<number>::task_finder tk_finder;

        //! Cached derived-product-finder instance
        typename repository_finder<number>::derived_product_finder dprod_finder;


		    // TRANSACTIONS

        //! Number of open transactions on the database
        unsigned int transactions;

	    };


    template <typename Payload>
    class OutputGroupFinder
      {
      public:
        OutputGroupFinder(const std::string& n)
          : name(n)
          {
          }

        bool operator()(const std::shared_ptr< output_group_record<Payload> >& a) { return(a->get_name() == name); }

      private:
        std::string name;
      };


    // ADMINISTRATION


    template <typename number>
    repository<number>::repository(const std::string& path, typename repository<number>::access_type mode,
                                   typename repository<number>::error_callback e,
                                   typename repository<number>::warning_callback w,
                                   typename repository<number>::message_callback m,
                                   typename repository_finder<number>::package_finder pf,
                                   typename repository_finder<number>::task_finder tf,
                                   typename repository_finder<number>::derived_product_finder dpf)
	    : root_path(path),
	      access_mode(mode),
	      error(e),
	      warning(w),
	      message(m),
	      pkg_finder(pf),
	      tk_finder(tf),
	      dprod_finder(dpf),
        transactions(0)
	    {
	    }


    template <typename number>
    void repository<number>::set_model_finder(const typename instance_manager<number>::model_finder& f)
	    {
        this->model_finder = f;
	    }


		// TRANSACTIONS


		template <typename number>
		transaction_manager repository<number>::transaction_factory(transaction_manager::open_handler o, transaction_manager::commit_handler c, transaction_manager::rollback_handler r)
			{
				if(this->transactions > 0) throw runtime_exception(runtime_exception::REPOSITORY_TRANSACTION_ERROR, __CPP_TRANSPORT_REPO_TRANSACTION_UNDERWAY);
				this->transactions++;

				typename transaction_manager::release_handler releaser = std::bind(&repository<number>::release_transaction, this);

				return transaction_manager(o, c, r, releaser);
			}


		template <typename number>
		void repository<number>::release_transaction()
			{
				if(this->transactions == 0) throw runtime_exception(runtime_exception::REPOSITORY_TRANSACTION_ERROR, __CPP_TRANSPORT_REPO_TRANSACTION_OVER_RELEASE);
				this->transactions--;
			}


    // GENERATE WRITERS FOR TASKS


    template <typename number>
    std::shared_ptr< integration_writer<number> >
    repository<number>::new_integration_task_content(integration_task_record<number>* rec, const std::list<std::string>& tags,
                                                     unsigned int worker, unsigned int workgroup, std::string suffix)
	    {
        typename integration_writer<number>::callback_group callbacks;
        callbacks.commit = std::bind(&repository<number>::close_integration_writer, this, std::placeholders::_1);
        callbacks.abort  = std::bind(&repository<number>::abort_integration_writer, this, std::placeholders::_1);

        return this->base_new_integration_task_content(rec, tags, worker, workgroup, callbacks, suffix);
	    }


    template <typename number>
    std::shared_ptr< derived_content_writer<number> >
    repository<number>::new_output_task_content(output_task_record<number>* rec, const std::list<std::string>& tags,
                                                unsigned int worker, std::string suffix)
	    {
        typename derived_content_writer<number>::callback_group callbacks;
        callbacks.commit = std::bind(&repository<number>::close_derived_content_writer, this, std::placeholders::_1);
        callbacks.abort  = std::bind(&repository<number>::abort_derived_content_writer, this, std::placeholders::_1);

        return this->base_new_output_task_content(rec, tags, worker, callbacks, suffix);
	    }


    template <typename number>
    std::shared_ptr< postintegration_writer<number> >
    repository<number>::new_postintegration_task_content(postintegration_task_record<number>* rec, const std::list<std::string>& tags,
                                                         unsigned int worker, std::string suffix)
	    {
        typename postintegration_writer<number>::callback_group callbacks;
        callbacks.commit        = std::bind(&repository<number>::close_postintegration_writer, this, std::placeholders::_1);
        callbacks.abort         = std::bind(&repository<number>::abort_postintegration_writer, this, std::placeholders::_1);

        return this->base_new_postintegration_task_content(rec, tags, worker, callbacks, suffix);
	    }


		template <typename number>
		std::shared_ptr< integration_writer<number> >
		repository<number>::base_new_integration_task_content(integration_task_record<number>* rec, const std::list<std::string>& tags,
                                                          unsigned int worker, unsigned int workgroup,
		                                                      typename integration_writer<number>::callback_group& callbacks, std::string suffix)
			{
		    // get current time
		    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        // request a name for this content group
        std::string output_leaf = this->reserve_content_name(now, suffix);

		    boost::filesystem::path output_path = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF) / rec->get_name() / output_leaf;
		    boost::filesystem::path sql_path    = output_path / __CPP_TRANSPORT_REPO_DATABASE_LEAF;

		    // temporary stuff, location not recorded in the database
		    boost::filesystem::path log_path  = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
		    boost::filesystem::path temp_path = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

		    // create directories
		    boost::filesystem::create_directories(this->get_root_path() / output_path);
		    boost::filesystem::create_directories(this->get_root_path() / log_path);
		    boost::filesystem::create_directories(this->get_root_path() / temp_path);

		    generic_writer::metadata_group metadata;
		    metadata.tags          = tags;
		    metadata.creation_time = now;

		    generic_writer::paths_group paths;
		    paths.root   = this->get_root_path();
		    paths.output = output_path;
		    paths.data   = sql_path;
		    paths.log    = log_path;
		    paths.temp   = temp_path;

		    // integration_writer constructor takes a copy of the integration_task_record
		    return std::shared_ptr< integration_writer<number> >(new integration_writer<number>(output_leaf, rec, callbacks, metadata, paths, worker, workgroup));
			}


    template <typename number>
    std::shared_ptr< derived_content_writer<number> >
    repository<number>::base_new_output_task_content(output_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker,
                                                     typename derived_content_writer<number>::callback_group& callbacks, std::string suffix)
	    {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        // request a name for this content group
        std::string output_leaf = this->reserve_content_name(now, suffix);

        boost::filesystem::path output_path = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF) / rec->get_name() / output_leaf;

        // temporary stuff, location not recorded in the database
        boost::filesystem::path log_path  = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path temp_path = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        generic_writer::metadata_group metadata;
        metadata.tags          = tags;
        metadata.creation_time = now;

        generic_writer::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.log    = log_path;
        paths.temp   = temp_path;

        return std::shared_ptr< derived_content_writer<number> >(new derived_content_writer<number>(output_leaf, rec, callbacks, metadata, paths, worker));
	    }


    template <typename number>
    std::shared_ptr< postintegration_writer<number> >
    repository<number>::base_new_postintegration_task_content(postintegration_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker,
                                                              typename postintegration_writer<number>::callback_group& callbacks, std::string suffix)
	    {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        // request a name for this content group
        std::string output_leaf = this->reserve_content_name(now, suffix);

        boost::filesystem::path output_path = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF) / rec->get_name() / output_leaf;
        boost::filesystem::path sql_path     = output_path / __CPP_TRANSPORT_REPO_DATABASE_LEAF;

        // temporary stuff, location not recorded in the database
        boost::filesystem::path log_path    = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path temp_path   = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        generic_writer::metadata_group metadata;
        metadata.tags          = tags;
        metadata.creation_time = now;

        generic_writer::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.data   = sql_path;
        paths.log    = log_path;
        paths.temp   = temp_path;

        return std::shared_ptr< postintegration_writer<number> >(new postintegration_writer<number>(output_leaf, rec, callbacks, metadata, paths, worker));
	    }


    // STANDARD WRITER CALLBACKS


    template <typename number>
    template <typename Payload>
    void repository<number>::advise_commit(output_group_record<Payload>* group)
	    {
        std::ostringstream msg;

        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

        msg << __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_A << " '" << group->get_name() << "' "
	        << __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_B << " '" << group->get_task_name() << "' "
	        << __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_D << " " << boost::posix_time::to_simple_string(now);

        this->message(msg.str());

        if(group->get_payload().is_failed())
          {
            std::ostringstream warn;
            warn << __CPP_TRANSPORT_REPO_WARN_OUTPUT_GROUP_A << " '" << group->get_name() << "' "
              << __CPP_TRANSPORT_REPO_WARN_OUTPUT_GROUP_B;
            this->warning(warn.str());
          }
	    }


    template <typename number>
    void repository<number>::close_integration_writer(integration_writer<number>& writer)
	    {
        integration_task_record<number>* rec = writer.get_record();
        const std::list<std::string>& tags = writer.get_tags();
        assert(rec != nullptr);

        // set up notes for the new output record, if it exists
        std::list<std::string> notes;
        if(!writer.is_collecting_statistics())
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_NOTE_NO_STATISTICS << " '" << rec->get_task()->get_model()->get_backend() << "'";
            notes.push_back(msg.str());
	        }

        // create a new, empty output group record
        std::unique_ptr< output_group_record<integration_payload> >
	        output_record(this->integration_content_group_record_factory(rec->get_task()->get_name(), writer.get_relative_output_path(), false, notes, tags));

        // stamp output group with the correct 'created' time stamp
        output_record->set_creation_time(writer.get_creation_time());
        output_record->set_name(writer.get_name());

        // populate output group with content from the writer
        output_record->get_payload().set_container_path(writer.get_relative_container_path());
        output_record->get_payload().set_metadata(writer.get_metadata());
        output_record->get_payload().set_workgroup_number(writer.get_workgroup_number());

        if(writer.is_seeded()) output_record->get_payload().set_seed(writer.get_seed_group());

        output_record->get_payload().set_fail(writer.is_failed());
        output_record->get_payload().set_failed_serials(writer.get_missing_serials());

        output_record->get_payload().set_statistics(writer.is_collecting_statistics());
		    output_record->get_payload().set_initial_conditions(writer.is_collecting_initial_conditions());

        // commit new output record
        output_record->commit();

        // add this output group to the integration task record
        rec->add_new_output_group(output_record->get_name());
        rec->update_last_edit_time();
        rec->commit();

        this->advise_commit(output_record.get());
	    }


    template <typename number>
    void repository<number>::abort_integration_writer(integration_writer<number>& writer)
	    {
        boost::filesystem::path fail_path = this->get_root_path() / __CPP_TRANSPORT_REPO_FAILURE_LEAF;

        if(!boost::filesystem::exists(fail_path)) boost::filesystem::create_directories(fail_path);
        if(boost::filesystem::is_directory(fail_path))
	        {
            boost::filesystem::path abs_dest = fail_path / writer.get_relative_output_path().leaf();

            try
	            {
                boost::filesystem::rename(writer.get_abs_output_path(), abs_dest);
	            }
            catch(boost::filesystem::filesystem_error& xe)
	            {
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
	            }

            std::ostringstream msg;

            msg << __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_A << " '" << writer.get_record()->get_task()->get_name() << "': "
	            << __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_B << " '" << writer.get_name() << "' "
	            << __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_C;

            this->message(msg.str());
	        }
        else throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
	    }


    template <typename number>
    void repository<number>::close_postintegration_writer(postintegration_writer<number>& writer)
	    {
        // get repository record for this task
        postintegration_task_record<number>* rec = writer.get_record();
        const std::list<std::string>& tags = writer.get_tags();

        // convert to task class
        postintegration_task<number>* tk = rec->get_task();
        assert(tk != nullptr);

		    // get task class for parent
        derivable_task<number>* ptk = tk->get_parent_task();
        assert(ptk != nullptr);

        // create a new, empty output group record
        std::list<std::string> notes;
        std::unique_ptr<output_group_record<postintegration_payload>>
                               output_record(this->postintegration_content_group_record_factory(rec->get_task()->get_name(), writer.get_relative_output_path(), false, notes, tags));

        // stamp output group with the correct 'created' time stamp
        output_record->set_creation_time(writer.get_creation_time());
        output_record->set_name(writer.get_name());

        // populate output group with content from the writer
        output_record->get_payload().set_container_path(writer.get_relative_container_path());
        output_record->get_payload().set_metadata(writer.get_metadata());

        if(writer.is_seeded()) output_record->get_payload().set_seed(writer.get_seed_group());

        output_record->get_payload().set_pair(writer.is_paired());
        output_record->get_payload().set_parent_group(writer.get_parent_group());

        output_record->get_payload().set_fail(writer.is_failed());
        output_record->get_payload().set_failed_serials(writer.get_missing_serials());

        // tag this output group with its contents
        if(writer.get_products().get_zeta_twopf())   output_record->get_payload().get_precomputed_products().add_zeta_twopf();
		    if(writer.get_products().get_zeta_threepf()) output_record->get_payload().get_precomputed_products().add_zeta_threepf();
		    if(writer.get_products().get_zeta_redbsp())  output_record->get_payload().get_precomputed_products().add_zeta_redbsp();
		    if(writer.get_products().get_fNL_local())    output_record->get_payload().get_precomputed_products().add_fNL_local();
		    if(writer.get_products().get_fNL_equi())     output_record->get_payload().get_precomputed_products().add_fNL_equi();
		    if(writer.get_products().get_fNL_ortho())    output_record->get_payload().get_precomputed_products().add_fNL_ortho();
		    if(writer.get_products().get_fNL_DBI())      output_record->get_payload().get_precomputed_products().add_fNL_DBI();

        // commit new output record
        output_record->commit();

        // add this output group to the integration task record
        rec->add_new_output_group(output_record->get_name());
        rec->update_last_edit_time();
        rec->commit();

        this->advise_commit(output_record.get());
	    }


    template <typename number>
    void repository<number>::abort_postintegration_writer(postintegration_writer<number>& writer)
	    {
        boost::filesystem::path fail_path = this->get_root_path() / __CPP_TRANSPORT_REPO_FAILURE_LEAF;

        if(!boost::filesystem::exists(fail_path)) boost::filesystem::create_directories(fail_path);
        if(boost::filesystem::is_directory(fail_path))
	        {
            boost::filesystem::path abs_dest = fail_path / writer.get_relative_output_path().leaf();

            try
	            {
                boost::filesystem::rename(writer.get_abs_output_path(), abs_dest);
	            }
            catch(boost::filesystem::filesystem_error& xe)
	            {
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
	            }

            std::ostringstream msg;

            msg << __CPP_TRANSPORT_REPO_FAILED_POSTINT_GROUP_A << " '" << writer.get_record()->get_task()->get_name() << "': "
	            << __CPP_TRANSPORT_REPO_FAILED_POSTINT_GROUP_B << " '" << writer.get_name() << "' "
	            << __CPP_TRANSPORT_REPO_FAILED_POSTINT_GROUP_C;

            this->message(msg.str());
	        }
        else throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
	    }


    template <typename number>
    void repository<number>::close_derived_content_writer(derived_content_writer<number>& writer)
	    {
        output_task_record<number>* rec = writer.get_record();
        const std::list<std::string>& tags = writer.get_tags();
        assert(rec != nullptr);

        // create a new, empty output group record
        std::unique_ptr< output_group_record<output_payload> >
	        output_record(this->output_content_group_record_factory(rec->get_task()->get_name(), writer.get_relative_output_path(), false, std::list<std::string>(), tags));

        // stamp output group with the correct 'created' time stamp
        output_record->set_creation_time(writer.get_creation_time());
        output_record->set_name(writer.get_name());

        // populate output group with content from the writer
        const std::list<derived_content>& content = writer.get_content();
        for(std::list<derived_content>::const_iterator t = content.begin(); t != content.end(); ++t)
	        {
            output_record->get_payload().add_derived_content(*t);
	        }
        output_record->get_payload().set_metadata(writer.get_metadata());
        output_record->get_payload().set_fail(false);

        // commit new output record
        output_record->commit();

        // add this output group to the integration task record
        rec->add_new_output_group(output_record->get_name());
        rec->update_last_edit_time();
        rec->commit();

        this->advise_commit(output_record.get());
	    }


    template <typename number>
    void repository<number>::abort_derived_content_writer(derived_content_writer<number>& writer)
	    {
        boost::filesystem::path fail_path = this->get_root_path() / __CPP_TRANSPORT_REPO_FAILURE_LEAF;

        if(!boost::filesystem::exists(fail_path)) boost::filesystem::create_directories(fail_path);
        if(boost::filesystem::is_directory(fail_path))
	        {
            boost::filesystem::path abs_dest = fail_path / writer.get_relative_output_path().leaf();

            try
	            {
                boost::filesystem::rename(writer.get_abs_output_path(), abs_dest);
	            }
            catch(boost::filesystem::filesystem_error& xe)
	            {
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
	            }

            std::ostringstream msg;

            msg << __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_A << " '" << writer.get_record()->get_task()->get_name() << "': "
	            << __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_B << " '" << writer.get_name() << "' "
	            << __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_C;

            this->message(msg.str());
	        }
        else throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
	    }


    // FIND AN OUTPUT GROUP MATCHING DEFINED TAGS


    template<typename number>
    std::shared_ptr< output_group_record<integration_payload> >
    repository<number>::find_integration_task_output(const std::string& name, const std::list<std::string>& tags)
	    {
        // search for output groups associated with this task
        std::list< std::shared_ptr< output_group_record<integration_payload> > > output = this->enumerate_integration_task_content(name);

        // remove items which are marked as failed
        output.remove_if( [&] (const std::shared_ptr< output_group_record<integration_payload> >& group) { return(group->get_payload().is_failed()); } );

        // remove items from the list which have mismatching tags
        output.remove_if( [&] (const std::shared_ptr< output_group_record<integration_payload> >& group) { return(group->check_tags(tags)); } );

        if(output.empty())
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

        return(output.front());
	    }


    template<typename number>
    std::shared_ptr< output_group_record<postintegration_payload> >
    repository<number>::find_postintegration_task_output(const std::string& name, const std::list<std::string>& tags)
	    {
        // search for output groups associated with this task
        std::list< std::shared_ptr< output_group_record<postintegration_payload> > > output = this->enumerate_postintegration_task_content(name);

        // remove items which are marked as failed
        output.remove_if( [&] (const std::shared_ptr< output_group_record<postintegration_payload> >& group) { return(group->get_payload().is_failed()); } );

        // remove items from the list which have mismatching tags
        output.remove_if( [&] (const std::shared_ptr< output_group_record<postintegration_payload> > group) { return(group.get()->check_tags(tags)); } );

        if(output.empty())
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

        return(output.front());
	    }


	}   // namespace transport



#endif //__repository_H_
