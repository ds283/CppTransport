//
// Created by David Seery on 30/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __repository_sqlite3_H_
#define __repository_sqlite3_H_

#include <assert.h>
#include <string>
#include <sstream>
#include <list>
#include <functional>
#include <utility>

#include "transport-runtime-api/repository/json_repository_interface.h"

#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "transport-runtime-api/derived-products/data_products.h"

#include "boost/filesystem/operations.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "sqlite3.h"
#include "transport-runtime-api/sqlite3/operations/repository_admin.h"


#define __CPP_TRANSPORT_REPO_REPOSITORY_LEAF "database.sqlite"
#define __CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF "output"
#define __CPP_TRANSPORT_REPO_STORE_LEAF      "repository"
#define __CPP_TRANSPORT_REPO_PACKAGES_LEAF   "packages"
#define __CPP_TRANSPORT_REPO_TASKS_LEAF      "tasks"
#define __CPP_TRANSPORT_REPO_PRODUCTS_LEAF   "products"
#define __CPP_TRANSPORT_REPO_OUTPUT_LEAF     "output"
#define __CPP_TRANSPORT_REPO_LOGDIR_LEAF     "logs"
#define __CPP_TRANSPORT_REPO_TEMPDIR_LEAF    "tempfiles"
#define __CPP_TRANSPORT_REPO_DATABASE_LEAF   "integration.sqlite"
#define __CPP_TRANSPORT_REPO_FAILURE_LEAF    "failed"


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
		class repository_sqlite3: public json_interface_repository<number>
			{

		  private:

		    class default_error_handler
			    {
		      public:
		        void operator()(const std::string& msg)
			        {
		            std::cerr << msg << std::endl;
			        }
			    };

		    class default_warning_handler
			    {
		      public:
		        void operator()(const std::string& msg)
			        {
		            std::cout << msg << std::endl;
			        }
			    };

		    class default_message_handler
			    {
		      public:
		        void operator()(const std::string& msg)
			        {
		            std::cout << msg << std::endl;
			        }
			    };


		    // RAII transaction manager
		    class scoped_transaction
			    {

		      public:

		        typedef std::function<void()> open_handler;
		        typedef std::function<void()> close_handler;

		        scoped_transaction(open_handler& o, close_handler& c)
			        : opener(o), closer(c)
			        {
		            opener();
			        }

		        ~scoped_transaction()
			        {
		            this->closer();
			        }

		      private:

		        open_handler opener;
		        close_handler closer;

			    };


				//! record count callback, used when committing to the database
				typedef std::function<unsigned int(sqlite3*,std::string)> count_function;

				//! record store callback, used when committing to the database
				typedef std::function<void(sqlite3*,std::string,std::string)> store_function;

				//! record find function, used when replacing records in the database
				typedef std::function<std::string(sqlite3*,std::string)> find_function;


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



		    // TRANSACTION MANAGEMENT

		  protected:

		    //! Begin a transaction on the database.
		    void begin_transaction();

		    //! Commit a transaction to the database.
		    void end_transaction();

		    //! Generate an RAII transaction management object
		    scoped_transaction scoped_transaction_factory();


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
		    virtual std::list< std::shared_ptr< output_group_record<integration_payload> > > enumerate_integration_task_content(const std::string& name) override;

		    //! Enumerate the output groups available from a named output task
		    virtual std::list< std::shared_ptr< output_group_record<output_payload> > > enumerate_output_task_content(const std::string& name) override;


		    // ADD CONTENT ASSOCIATED WITH A TASK -- implements a 'repository' interface

		  public:

		    //! Generate a writer object for new integration output
		    virtual std::shared_ptr< integration_writer<number> > new_integration_task_content(integration_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker) override;

		    //! Generate a writer object for new derived-content output
		    virtual std::shared_ptr< derived_content_writer <number> > new_output_task_content(output_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker) override;

		    //! Generate a writer object for new postintegration output
		    virtual std::shared_ptr< postintegration_writer<number> > new_postintegration_task_content(postintegration_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker) override;

		    // FIND AN OUTPUT GROUP MATCHING DEFINED TAGS

		  public:

		    //! Find an output group for an integration task
		    virtual std::shared_ptr< output_group_record<integration_payload> > find_integration_task_output(const std::string& name, const std::list<std::string>& tags) override;


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


		    // WRITER CALLBACKS

		  protected:

		    //! Advise that an output group has been committed
		    template <typename Payload>
		    void advise_commit(output_group_record<Payload>* group);

		    //! Advise that postintegration products have been committed to an output group
		    template <typename Payload>
		    void advise_commit(postintegration_task_record<number>* rec, output_group_record<Payload>* group);

		    //! Commit the products from an integration to the database
		    void close_integration_writer(integration_writer<number>& writer);

		    //! Rollback a failed integration
		    void abort_integration_writer(integration_writer<number>& writer);

		    //! Commit hte products from a postintegration to the database
		    void close_postintegration_writer(postintegration_writer<number>& writer);

		    //! Rollback a failed postintegration
		    void abort_postintegration_writer(postintegration_writer<number>& writer);

		    //! Commit the products from an output task to the database
		    void close_derived_content_writer(derived_content_writer<number>& writer);

		    //! Rollback a failed integration
		    void abort_derived_content_writer(derived_content_writer<number>& writer);


		    // COMMIT CALLBACK METHODS FOR REPOSITORY RECORDS

		  protected:

				//! Commit callback: commit to database for the first time
				void commit_first(repository_record& record, count_function counter, store_function storer, std::string store_root, std::string exists_err);

				//! Commit callback: replacement commit to database
				void commit_replace(repository_record& record, find_function finder);


		    // REPOSITORY RECORD FACTORIES

		  protected:

		    //! Create a new package record from an explicit object
		    package_record<number>* package_record_factory(const initial_conditions<number>& ics);

		    //! Create a package record from a JSON value
		    package_record<number>* package_record_factory(Json::Value& reader);

		    //! Create a new integration task record from an explicit object
		    integration_task_record<number>* integration_task_record_factory(const integration_task<number>& tk);

		    //! Create an integration task record from a JSON value
		    integration_task_record<number>* integration_task_record_factory(Json::Value& reader);

		    //! Create a new output task record from an explicit object
		    output_task_record<number>* output_task_record_factory(const output_task<number>& tk);

		    //! Create an output task record from a JSON value
		    output_task_record<number>* output_task_record_factory(Json::Value& reader);

		    //! Create a postintegration task record from an explicit object
		    postintegration_task_record<number>* postintegration_task_record_factory(const postintegration_task<number>& tk);

		    //! Create a postintegration task record from a JSON value
		    postintegration_task_record<number>* postintegration_task_record_factory(Json::Value& reader);

		    //! Create a new derived product record from explicit object
		    derived_product_record<number>* derived_product_record_factory(const derived_data::derived_product<number>& prod);

		    //! create a new derived product record from a JSON value
		    derived_product_record<number>* derived_product_record_factory(Json::Value& reader);

		    //! Create a new content record from an explicit object
				template <typename Payload>
		    output_group_record<Payload>* output_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
								                                                  bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg);

		    //! Create a new content from from a serialization reader
		    template <typename Payload>
		    output_group_record<Payload>* output_group_record_factory(Json::Value& reader);


				// UTILITY FUNCTIONS

		    //! Check whether a task already exists in the database. Throws an exception if so.
		    void check_task_duplicate(const std::string& name);

		    //! Write a serialized JSON record to a file
		    void commit_document(const boost::filesystem::path& path, const repository_record& record);

				//! Read a serialized JSON record from a file
				Json::Value deserialize_document(boost::filesystem::path& path);

				//! Enumerate content groups
				template <typename Payload>
				void enumerate_content_groups(const std::string& name, std::list< std::shared_ptr< output_group_record<Payload> > >& list, find_function finder);


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

		    //! Number of open clients on the database, used for keep track of when the
		    //! database connexion can be closed
		    unsigned int open_clients;

			};


		// IMPLEMENTATION


		// Create a repository object associated with a pathname
		template <typename number>
		repository_sqlite3<number>::repository_sqlite3(const std::string& path, typename repository<number>::access_type mode,
		                                               typename repository<number>::error_callback e,
		                                               typename repository<number>::warning_callback w,
		                                               typename repository<number>::message_callback m)
			: json_interface_repository<number>(path, mode, e, w, m,
			                                    std::bind(&repository_sqlite3<number>::query_package, this, std::placeholders::_1),
			                                    std::bind(&repository_sqlite3<number>::query_task, this, std::placeholders::_1),
			                                    std::bind(&repository_sqlite3<number>::query_derived_product, this, std::placeholders::_1)),
			  db(nullptr), open_clients(0)
			{
		    // supplied path should be a directory which exists
		    if(!boost::filesystem::is_directory(path))
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_MISSING_ROOT << " '" << path << "'";
		        throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
			    }

		    // database should be present in an existing directory
		    db_path = this->get_root_path() / __CPP_TRANSPORT_REPO_REPOSITORY_LEAF;
		    if(!boost::filesystem::is_regular_file(db_path))
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_MISSING_DATABASE << " '" << path << "'";
		        throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
			    }

				// package store should be present
				package_store = boost::filesystem::path(__CPP_TRANSPORT_REPO_STORE_LEAF) / __CPP_TRANSPORT_REPO_PACKAGES_LEAF;
		    if(!boost::filesystem::is_directory(this->db_path / package_store))
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_MISSING_PACKAGE_STORE << " '" << path << "'";
		        throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
			    }

				// task store should be present
				task_store = boost::filesystem::path(__CPP_TRANSPORT_REPO_STORE_LEAF) / __CPP_TRANSPORT_REPO_TASKS_LEAF;
		    if(!boost::filesystem::is_directory(this->db_path / task_store))
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_MISSING_TASK_STORE << " '" << path << "'";
		        throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
			    }

		    // product store should be present
		    product_store = boost::filesystem::path(__CPP_TRANSPORT_REPO_STORE_LEAF) / __CPP_TRANSPORT_REPO_PRODUCTS_LEAF;
		    if(!boost::filesystem::is_directory(this->db_path / product_store))
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_MISSING_PRODUCT_STORE << " '" << path << "'";
		        throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
			    }

		    // output store should be present
		    output_store = boost::filesystem::path(__CPP_TRANSPORT_REPO_STORE_LEAF) / __CPP_TRANSPORT_REPO_OUTPUT_LEAF;
		    if(!boost::filesystem::is_directory(this->db_path / output_store))
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_MISSING_OUTPUT_STORE << " '" << path << "'";
		        throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
			    }

				unsigned int sqlite_mode = (mode == repository<number>::access_type::readonly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE);
				if(sqlite3_open_v2(db_path.string().c_str(), &db, sqlite_mode, nullptr) != SQLITE_OK)
					{
				    std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_FAIL_DATABASE_OPEN << " " << db_path;
				    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
					}

				// TODO: consider checking whether required tables are present
			}


		// Create a named repository
		template <typename number>
		repository_sqlite3<number>::repository_sqlite3(const std::string& path, const repository_creation_key& key)
			: json_interface_repository<number>(path, repository<number>::access_type::readwrite,
			                                    typename repository<number>::error_callback(repository_sqlite3<number>::default_error_handler()),
			                                    typename repository<number>::warning_callback(repository_sqlite3<number>::default_warning_handler()),
			                                    typename repository<number>::message_callback(repository_sqlite3<number>::default_message_handler()),
			                                    std::bind(&repository_sqlite3<number>::query_package, this, std::placeholders::_1),
			                                    std::bind(&repository_sqlite3<number>::query_task, this, std::placeholders::_1),
			                                    std::bind(&repository_sqlite3<number>::query_derived_product, this, std::placeholders::_1)),
			  db(nullptr),
			  open_clients(0)
			{
		    // check whether root directory for the repository already exists -- it shouldn't
		    if(boost::filesystem::exists(path))
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_ROOT_EXISTS << " '" << path << "'";
		        throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

		    db_path       = this->get_root_path() / __CPP_TRANSPORT_REPO_REPOSITORY_LEAF;
		    package_store = boost::filesystem::path(__CPP_TRANSPORT_REPO_STORE_LEAF) / __CPP_TRANSPORT_REPO_PACKAGES_LEAF;
		    task_store    = boost::filesystem::path(__CPP_TRANSPORT_REPO_STORE_LEAF) / __CPP_TRANSPORT_REPO_TASKS_LEAF;
		    product_store = boost::filesystem::path(__CPP_TRANSPORT_REPO_STORE_LEAF) / __CPP_TRANSPORT_REPO_PRODUCTS_LEAF;
		    output_store  = boost::filesystem::path(__CPP_TRANSPORT_REPO_STORE_LEAF) / __CPP_TRANSPORT_REPO_OUTPUT_LEAF;

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
		        msg << __CPP_TRANSPORT_REPO_FAIL_DATABASE_OPEN << " " << db_path;
		        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
			    }

		    sqlite3_operations::create_repository_tables(db);
			}


		// Destroy a repository object, closing the associated container
		template <typename number>
		repository_sqlite3<number>::~repository_sqlite3()
			{
				// close open handles if it exists
				if(this->db != nullptr) sqlite3_close(this->db);
			}


		// TRANSACTION MANAGEMENT


		// Begin transaction
		template <typename number>
		void repository_sqlite3<number>::begin_transaction()
			{
				assert(this->db != nullptr);

				if(this->open_clients == 0)
					{
				    sqlite3_operations::exec(this->db, "BEGIN TRANSACTION");
					}

				this->open_clients++;
			}


		// End transaction
		template <typename number>
		void repository_sqlite3<number>::end_transaction()
			{
				assert(this->db != nullptr);

		    if(this->open_clients <= 0) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_NOT_OPEN);

		    this->open_clients--;

				if(this->open_clients == 0)
					{
				    sqlite3_operations::exec(this->db, "END TRANSACTION");
					}
			}


    template <typename number>
    typename repository_sqlite3<number>::scoped_transaction repository_sqlite3<number>::scoped_transaction_factory()
	    {
        typename repository_sqlite3<number>::scoped_transaction::open_handler opener = std::bind(&repository_sqlite3<number>::begin_transaction, this);
        typename repository_sqlite3<number>::scoped_transaction::close_handler closer = std::bind(&repository_sqlite3<number>::end_transaction, this);

        return typename repository_sqlite3<number>::scoped_transaction(opener, closer);
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
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

        // commit entry to the database
        boost::filesystem::path document_path = boost::filesystem::path(store_root) / record.get_name();

        {
	        scoped_transaction scoped_xn = this->scoped_transaction_factory();
	        storer(this->db, record.get_name(), document_path.string());
        }

        // store package on disk
        this->commit_document(document_path, record);
	    }


    template <typename number>
    void repository_sqlite3<number>::commit_replace(repository_record& record, typename repository_sqlite3<number>::find_function finder)
	    {
		    // find existing record in the
        boost::filesystem::path document_path = finder(this->db, record.get_name());

		    // replace package on disk
		    this->commit_document(document_path, record);
	    }


    // REPOSITORY RECORD FACTORIES


    template <typename number>
    package_record<number>* repository_sqlite3<number>::package_record_factory(const initial_conditions<number>& ics)
	    {
        repository_record::handler_package pkg;
		    count_function counter = std::bind(&sqlite3_operations::count_packages, std::placeholders::_1, std::placeholders::_2);
		    store_function storer  = std::bind(&sqlite3_operations::store_package, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->package_store.string(), __CPP_TRANSPORT_REPO_PACKAGE_EXISTS);

        return new package_record<number>(ics, pkg);
	    }


    template <typename number>
    package_record<number>* repository_sqlite3<number>::package_record_factory(Json::Value& reader)
	    {
        repository_record::handler_package pkg;
        find_function finder = std::bind(&sqlite3_operations::find_package, std::placeholders::_1, std::placeholders::_2, __CPP_TRANSPORT_REPO_PACKAGE_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new package_record<number>(reader, this->model_finder, pkg);
	    }


    template <typename number>
    integration_task_record<number>* repository_sqlite3<number>::integration_task_record_factory(const integration_task<number>& tk)
	    {
        repository_record::handler_package pkg;
        count_function counter = std::bind(&sqlite3_operations::count_tasks, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_integration_task, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, tk.get_ics().get_name());
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->task_store.string(), __CPP_TRANSPORT_REPO_TASK_EXISTS);

        return new integration_task_record<number>(tk, pkg);
	    }


    template <typename number>
    integration_task_record<number>* repository_sqlite3<number>::integration_task_record_factory(Json::Value& reader)
	    {
        repository_record::handler_package pkg;
        find_function finder = std::bind(&sqlite3_operations::find_integration_task, std::placeholders::_1, std::placeholders::_2, __CPP_TRANSPORT_REPO_TASK_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new integration_task_record<number>(reader, this->pkg_finder, pkg);
	    }


    template <typename number>
    output_task_record<number>* repository_sqlite3<number>::output_task_record_factory(const output_task<number>& tk)
	    {
        repository_record::handler_package pkg;
        count_function counter = std::bind(&sqlite3_operations::count_tasks, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_output_task, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->task_store.string(), __CPP_TRANSPORT_REPO_TASK_EXISTS);

        return new output_task_record<number>(tk, pkg);
	    }


    template <typename number>
    output_task_record<number>* repository_sqlite3<number>::output_task_record_factory(Json::Value& reader)
	    {
        repository_record::handler_package pkg;
        find_function finder = std::bind(&sqlite3_operations::find_output_task, std::placeholders::_1, std::placeholders::_2, __CPP_TRANSPORT_REPO_TASK_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new output_task_record<number>(reader, this->dprod_finder, pkg);
	    }


    template <typename number>
    postintegration_task_record<number>* repository_sqlite3<number>::postintegration_task_record_factory(const postintegration_task<number>& tk)
	    {
        repository_record::handler_package pkg;
        count_function counter = std::bind(&sqlite3_operations::count_tasks, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_postintegration_task, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, tk.get_parent_task()->get_name());
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->task_store.string(), __CPP_TRANSPORT_REPO_TASK_EXISTS);

        return new postintegration_task_record<number>(tk, pkg);
	    }


    template <typename number>
    postintegration_task_record<number>* repository_sqlite3<number>::postintegration_task_record_factory(Json::Value& reader)
	    {
        repository_record::handler_package pkg;
        find_function finder = std::bind(&sqlite3_operations::find_postintegration_task, std::placeholders::_1, std::placeholders::_2, __CPP_TRANSPORT_REPO_TASK_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new postintegration_task_record<number>(reader, this->tk_finder, pkg);
	    }


    template <typename number>
    derived_product_record<number>* repository_sqlite3<number>::derived_product_record_factory(const derived_data::derived_product<number>& prod)
	    {
        repository_record::handler_package pkg;
        count_function counter = std::bind(&sqlite3_operations::count_products, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_product, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->product_store.string(), __CPP_TRANSPORT_REPO_PRODUCT_EXISTS);

        return new derived_product_record<number>(prod, pkg);
	    }


    template <typename number>
    derived_product_record<number>* repository_sqlite3<number>::derived_product_record_factory(Json::Value& reader)
	    {
        repository_record::handler_package pkg;
        find_function finder = std::bind(&sqlite3_operations::find_product, std::placeholders::_1, std::placeholders::_2, __CPP_TRANSPORT_REPO_PRODUCT_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new derived_product_record<number>(reader, this->tk_finder, pkg);
	    }


    template <typename number>
    template <typename Payload>
    output_group_record<Payload>* repository_sqlite3<number>::output_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                          bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg)
	    {
        repository_record::handler_package pkg;
        count_function counter = std::bind(&sqlite3_operations::count_groups, std::placeholders::_1, std::placeholders::_2);
        store_function storer  = std::bind(&sqlite3_operations::store_group<Payload>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, tn);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_first, this, std::placeholders::_1, counter, storer, this->product_store.string(), __CPP_TRANSPORT_REPO_OUTPUT_EXISTS);

        typename output_group_record<Payload>::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = path;

        return new output_group_record<Payload>(tn, paths, lock, nt, tg, pkg);
	    }


    template <typename number>
    template <typename Payload>
    output_group_record<Payload>* repository_sqlite3<number>::output_group_record_factory(Json::Value& reader)
	    {
        repository_record::handler_package pkg;
        find_function finder = std::bind(&sqlite3_operations::find_group<Payload>, std::placeholders::_1, std::placeholders::_2, __CPP_TRANSPORT_REPO_OUTPUT_MISSING);
        pkg.commit = std::bind(&repository_sqlite3<number>::commit_replace, this, std::placeholders::_1, finder);

        return new output_group_record<Payload>(reader, this->root_path, pkg);
	    }


    // CREATE RECORDS


    // Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
    template <typename number>
    void repository_sqlite3<number>::commit_package(const initial_conditions<number>& ics)
	    {
        std::unique_ptr< package_record<number> > record(package_record_factory(ics));
        record->commit();
	    }


    // Write an integration task to the database.
    template <typename number>
    void repository_sqlite3<number>::commit_task(const integration_task<number>& tk)
	    {
        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        std::unique_ptr< integration_task_record<number> > record(integration_task_record_factory(tk));
        record->commit();

        // check whether the initial conditions package for this task is already present; if not, insert it
        unsigned int count = sqlite3_operations::count_packages(this->db, tk.get_ics().get_name());
        if(count == 0)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_AUTOCOMMIT_INTEGRATION_A << " '" << tk.get_name() << "' "
	            << __CPP_TRANSPORT_REPO_AUTOCOMMIT_INTEGRATION_B << " '" << tk.get_ics().get_name() << "'";
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

        std::unique_ptr< output_task_record<number> > record(output_task_record_factory(tk));
        record->commit();

        // check whether derived products on which this task depends have already been committed to the database
        const typename std::vector< output_task_element<number> > elements = tk.get_elements();
        for(typename std::vector< output_task_element<number> >::const_iterator t = elements.begin(); t != elements.end(); t++)
	        {
            derived_data::derived_product<number>* product = (*t).get_product();
            unsigned int count = sqlite3_operations::count_products(this->db, product->get_name());
            if(count == 0)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_AUTOCOMMIT_OUTPUT_A << " '" << tk.get_name() << "' "
	                << __CPP_TRANSPORT_REPO_AUTOCOMMIT_OUTPUT_B << " '" << product->get_name() << "'";
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

        std::unique_ptr< postintegration_task_record<number> > record(postintegration_task_record_factory(tk));
        record->commit();

        // check whether parent task is already committed to the database
        unsigned int count = sqlite3_operations::count_tasks(this->db, tk.get_parent_task()->get_name());
        if(count == 0)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_A << " '" << tk.get_name() << "' "
	            << __CPP_TRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_B << " '" << tk.get_parent_task()->get_name() << "'";
            this->warning(msg.str());
            this->commit_task(*(tk.get_parent_task()));
	        }
	    }


    // Write a derived product specification
    template <typename number>
    void repository_sqlite3<number>::commit_derived_product(const derived_data::derived_product<number>& d)
	    {
        std::unique_ptr< derived_product_record<number> > record(derived_product_record_factory(d));
        record->commit();

        // check whether all tasks on which this derived product depends are already in the database
        typename std::vector< integration_task<number>* > task_list;
        d.get_task_list(task_list);

        for(typename std::vector< integration_task<number>* >::iterator t = task_list.begin(); t != task_list.end(); t++)
	        {
            unsigned int count = sqlite3_operations::count_tasks(this->db, (*t)->get_name());
            if(count == 0)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_AUTOCOMMIT_PRODUCT_A << " '" << d.get_name() << "' "
	                << __CPP_TRANSPORT_REPO_AUTOCOMMIT_PRODUCT_B << " '" << (*t)->get_name() << "'";
                this->warning(msg.str());
                this->commit_task(*(*t));
	            }
	        }
	    }


		// READ RECORDS FROM THE DATABASE


		//! Read a package record from the database
		template <typename number>
		package_record<number>* repository_sqlite3<number>::query_package(const std::string& name)
			{
		    boost::filesystem::path filename = sqlite3_operations::find_package(this->db, name, __CPP_TRANSPORT_REPO_PACKAGE_MISSING);
		    Json::Value root = this->deserialize_document(filename);
				return this->package_record_factory(root);
			}


		//! Read a task record from the database
		template <typename number>
		task_record<number>* repository_sqlite3<number>::query_task(const std::string& name)
			{
				if(sqlite3_operations::count_integration_tasks(this->db, name) > 0)
					{
				    boost::filesystem::path filename = sqlite3_operations::find_integration_task(this->db, name, __CPP_TRANSPORT_REPO_TASK_MISSING);
				    Json::Value root = this->deserialize_document(filename);
						return this->integration_task_record_factory(root);
					}
				else if(sqlite3_operations::count_postintegration_tasks(this->db, name) > 0)
					{
				    boost::filesystem::path filename = sqlite3_operations::find_postintegration_task(this->db, name, __CPP_TRANSPORT_REPO_TASK_MISSING);
				    Json::Value root = this->deserialize_document(filename);
						return this->postintegration_task_record_factory(root);
					}
				else if(sqlite3_operations::count_output_tasks(this->db, name) > 0)
					{
				    boost::filesystem::path filename = sqlite3_operations::find_output_task(this->db, name, __CPP_TRANSPORT_REPO_TASK_MISSING);
				    Json::Value root = this->deserialize_document(filename);
						return this->output_task_record_factory(root);
					}

		    std::stringstream msg;
				msg << __CPP_TRANSPORT_REPO_TASK_MISSING << " '" << name << "'";
				throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			}


		// Read a derived product from the database
		template <typename number>
		derived_product_record<number>* repository_sqlite3<number>::query_derived_product(const std::string& name)
			{
		    boost::filesystem::path filename = sqlite3_operations::find_product(this->db, name, __CPP_TRANSPORT_REPO_PRODUCT_MISSING);
		    Json::Value root = this->deserialize_document(filename);
				return this->derived_product_record_factory(root);
			}


		// Enumerate the output groups available from a named integration task
		template <typename number>
		std::list< std::shared_ptr< output_group_record<integration_payload> > >
		repository_sqlite3<number>::enumerate_integration_task_content(const std::string& name)
			{
		    std::unique_ptr< task_record<number> > record(this->query_task(name));

				if(record->get_type() != task_record<number>::integration)
					{
				    std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN << " '" << name << "'";
				    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					}

		    std::list< std::shared_ptr< output_group_record<integration_payload> > > list;
				find_function finder = std::bind(sqlite3_operations::find_integration_task, std::placeholders::_1, std::placeholders::_2, __CPP_TRANSPORT_REPO_TASK_MISSING);
				this->enumerate_content_groups<integration_payload>(name, list, finder);

				list.sort(&output_group_helper::comparator<integration_payload>);
				return(list);
			}


		// Enumerate the output groups available from a named output task
		template <typename number>
		std::list< std::shared_ptr< output_group_record<output_payload> > >
		repository_sqlite3<number>::enumerate_output_task_content(const std::string& name)
			{
		    std::unique_ptr< task_record<number> > record(this->query_task(name));

				if(record->get_type() != task_record<number>::output)
					{
				    std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_OUTPUT << " '" << name << "'";
				    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					}

		    std::list< std::shared_ptr< output_group_record<output_payload> > > list;
		    find_function finder = std::bind(sqlite3_operations::find_output_task, std::placeholders::_1, std::placeholders::_2, __CPP_TRANSPORT_REPO_OUTPUT_MISSING);
		    this->enumerate_content_groups<output_payload>(name, list, finder);

		    list.sort(&output_group_helper::comparator<output_payload>);
		    return(list);
			}


    // ADD CONTENT ASSOCIATED WITH A TASK


    template <typename number>
    std::shared_ptr< integration_writer<number> >
    repository_sqlite3<number>::new_integration_task_content(integration_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker)
	    {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        // construct paths for the various output files and directories.
        // We use the ISO form of the current time to label the output group directory.
        // This means the repository directory structure will be human-readable if necessary.
        std::string output_leaf = boost::posix_time::to_iso_string(now);

        boost::filesystem::path output_path = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF) / rec->get_name() / output_leaf;
        boost::filesystem::path sql_path    = output_path / __CPP_TRANSPORT_REPO_DATABASE_LEAF;

        // temporary stuff, location not recorded in the database
        boost::filesystem::path log_path  = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path temp_path = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        typename integration_writer<number>::callback_group callbacks;
        callbacks.commit = std::bind(&repository_sqlite3<number>::close_integration_writer, this, std::placeholders::_1);
        callbacks.abort  = std::bind(&repository_sqlite3<number>::abort_integration_writer, this, std::placeholders::_1);

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
        return std::shared_ptr< integration_writer<number> >(new integration_writer<number>(rec, callbacks, metadata, paths, worker));
	    }


    template <typename number>
    std::shared_ptr< derived_content_writer<number> >
    repository_sqlite3<number>::new_output_task_content(output_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker)
	    {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        // construct paths for output files and directories
        // We use the ISO form of the current time to label the output group directory.
        // This means the repository directory structure is human-readable if necessary.
        std::string output_leaf = boost::posix_time::to_iso_string(now);

        boost::filesystem::path output_path = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF) / rec->get_name() / output_leaf;

        // temporary stuff, location not recorded in the database
        boost::filesystem::path log_path  = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path temp_path = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        typename derived_content_writer<number>::callback_group callbacks;
        callbacks.commit = std::bind(&repository_sqlite3<number>::close_derived_content_writer, this, std::placeholders::_1);
        callbacks.abort  = std::bind(&repository_sqlite3<number>::abort_derived_content_writer, this, std::placeholders::_1);

        generic_writer::metadata_group metadata;
        metadata.tags          = tags;
        metadata.creation_time = now;

        generic_writer::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.log    = log_path;
        paths.temp   = temp_path;

        return std::shared_ptr< derived_content_writer<number> >(new derived_content_writer<number>(rec, callbacks, metadata, paths, worker));
	    }


    template <typename number>
    std::shared_ptr< postintegration_writer<number> >
    repository_sqlite3<number>::new_postintegration_task_content(postintegration_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker)
	    {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        // construct paths for output files and directories
        std::string output_leaf = boost::posix_time::to_iso_string(now);

        boost::filesystem::path output_path = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF) / rec->get_name() / output_leaf;
        boost::filesystem::path sql_path     = output_path / __CPP_TRANSPORT_REPO_DATABASE_LEAF;

        // temporary stuff, location not recorded in the database
        boost::filesystem::path log_path    = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path temp_path   = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        typename postintegration_writer<number>::callback_group callbacks;
        callbacks.commit        = std::bind(&repository_sqlite3<number>::close_postintegration_writer, this, std::placeholders::_1);
        callbacks.abort         = std::bind(&repository_sqlite3<number>::abort_postintegration_writer, this, std::placeholders::_1);

        generic_writer::metadata_group metadata;
        metadata.tags          = tags;
        metadata.creation_time = now;

        generic_writer::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.data   = sql_path;
        paths.log    = log_path;
        paths.temp   = temp_path;

        return std::shared_ptr< postintegration_writer<number> >(new postintegration_writer<number>(rec, callbacks, metadata, paths, worker));
	    }


    // WRITER CALLBACKS


    template <typename number>
    template <typename Payload>
    void repository_sqlite3<number>::advise_commit(output_group_record<Payload>* group)
	    {
        std::ostringstream msg;

        msg << __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_A << " '" << group->get_name() << "' "
	        << __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_B << " '" << group->get_task_name() << "'";

        this->message(msg.str());
	    }


    template <typename number>
    template <typename Payload>
    void repository_sqlite3<number>::advise_commit(postintegration_task_record<number>* rec, output_group_record<Payload>* group)
	    {
        std::ostringstream msg;

        msg << __CPP_TRANSPORT_REPO_WRITEBACK_POSTINT_GROUP_A << " '" << group->get_name() << "' "
	        << __CPP_TRANSPORT_REPO_WRITEBACK_POSTINT_GROUP_B << " '" << rec->get_name() << "' ("
	        << __CPP_TRANSPORT_REPO_WRITEBACK_POSTINT_GROUP_C << " '" << group->get_task_name() << "')";

        this->message(msg.str());
	    }


    template <typename number>
    void repository_sqlite3<number>::close_integration_writer(integration_writer<number>& writer)
	    {
        integration_task_record<number>* rec = writer.get_record();
        const std::list<std::string>& tags = writer.get_tags();
        assert(rec != nullptr);

        // set up notes for the new output record, if it exists
        std::list<std::string> notes;
        if(!writer.collect_statistics())
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_NOTE_NO_STATISTICS << " '" << rec->get_task()->get_model()->get_backend() << "'";
            notes.push_back(msg.str());
	        }

        // create a new, empty output group record
        std::unique_ptr< output_group_record<integration_payload> >
	        output_record(this->output_group_record_factory<integration_payload>(rec->get_task()->get_name(), writer.get_relative_output_path(), false, notes, tags));

        // stamp output group with the correct 'created' time stamp
        output_record->set_creation_time(writer.get_creation_time());
        output_record->set_name_from_creation_time();

        // populate output group with content from the writer
        output_record->get_payload().set_backend(rec->get_task()->get_model()->get_backend());
        output_record->get_payload().set_container_path(writer.get_relative_container_path());
        output_record->get_payload().set_metadata(writer.get_metadata());

        // commit new output record
        output_record->commit();

        // add this output group to the integration task record
        rec->add_new_output_group(output_record->get_name());
        rec->update_last_edit_time();
        rec->commit();

        this->advise_commit(output_record.get());
	    }


    template <typename number>
    void repository_sqlite3<number>::abort_integration_writer(integration_writer<number>& writer)
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

            std::string group_name = boost::posix_time::to_iso_string(writer.get_creation_time());

            msg << __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_A << " '" << writer.get_record()->get_task()->get_name() << "': "
	            << __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_B << " '" << group_name << "' "
	            << __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_C;

            this->message(msg.str());
	        }
        else throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
	    }


    template <typename number>
    void repository_sqlite3<number>::close_postintegration_writer(postintegration_writer<number>& writer)
	    {
        // get repository record for postintegration task
        postintegration_task_record<number>* rec = writer.get_record();
        const std::list<std::string>& tags = writer.get_tags();

        // get repository record for parent integration task
        postintegration_task<number>* tk = rec->get_task();
        assert(tk != nullptr);

        integration_task<number>* ptk = tk->get_parent_task();
        assert(ptk != nullptr);

        // get repository record for content group we have used to computation
        std::shared_ptr< output_group_record<integration_payload> > parent_content_record = find_integration_task_output(ptk->get_name(), tags);

        // get source and destination data containers
        boost::filesystem::path source_container = writer.get_abs_container_path();
        boost::filesystem::path dest_container   = parent_content_record->get_abs_repo_path() / parent_content_record->get_payload().get_container_path();

        // create a new, empty output group record
        std::list<std::string> notes;
        std::unique_ptr< output_group_record<postintegration_payload> >
                               output_record(this->output_group_record_factory<postintegration_payload>(rec->get_task()->get_name(), writer.get_relative_output_path(), false, notes, tags));

        // stamp output group with the correct 'created' time stamp
        output_record->set_creation_time(writer.get_creation_time());
        output_record->set_name_from_creation_time();

        // populate output group with content from the writer
        output_record->get_payload().set_container_path(writer.get_relative_container_path());
        output_record->get_payload().set_metadata(writer.get_metadata());

        // commit new output record
        output_record->commit();

        // add this output group to the integration task record
        rec->add_new_output_group(output_record->get_name());
        rec->update_last_edit_time();
        rec->commit();

        this->advise_commit(output_record.get());

        zeta_twopf_task<number>* z2pf = nullptr;
        zeta_threepf_task<number>* z3pf = nullptr;
        fNL_task<number>* zfNL = nullptr;

        // write back postintegration task output into the parent container if the task stipulates that
        if(tk->get_write_back())
	        {
            if((z2pf = dynamic_cast<zeta_twopf_task<number>*>(tk)) != nullptr)
	            {
                writer.merge_zeta_twopf(source_container, dest_container);
                parent_content_record->get_payload().get_precomputed_products().add_zeta_twopf();
	            }
            else if((z3pf = dynamic_cast<zeta_threepf_task<number>*>(tk)) != nullptr)
	            {
                writer.merge_zeta_twopf(source_container, dest_container);
                writer.merge_zeta_threepf(source_container, dest_container);
                writer.merge_zeta_redbsp(source_container, dest_container);
                parent_content_record->get_payload().get_precomputed_products().add_zeta_twopf();
                parent_content_record->get_payload().get_precomputed_products().add_zeta_threepf();
                parent_content_record->get_payload().get_precomputed_products().add_zeta_redbsp();
	            }
            else if((zfNL = dynamic_cast<fNL_task<number>*>(tk)) != nullptr)
	            {
                writer.merge_fNL(source_container, dest_container, zfNL->get_template());

                switch(zfNL->get_template())
	                {
                    case derived_data::fNLlocal:
	                    parent_content_record->get_payload().get_precomputed_products().add_fNL_local();
                      break;

                    case derived_data::fNLequi:
	                    parent_content_record->get_payload().get_precomputed_products().add_fNL_equi();
                      break;

                    case derived_data::fNLortho:
	                    parent_content_record->get_payload().get_precomputed_products().add_fNL_ortho();
                      break;

                    case derived_data::fNLDBI:
	                    parent_content_record->get_payload().get_precomputed_products().add_fNL_DBI();
                      break;

                    default:
	                    assert(false);
                      throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
	                }
	            }

            // update and commit parent group record
            parent_content_record->update_last_edit_time();
            parent_content_record->commit();

            this->advise_commit(rec, parent_content_record.get());
	        }
	    }


    template <typename number>
    void repository_sqlite3<number>::abort_postintegration_writer(postintegration_writer<number>& writer)
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

            std::string group_name = boost::posix_time::to_iso_string(writer.get_creation_time());

            msg << __CPP_TRANSPORT_REPO_FAILED_POSTINT_GROUP_A << " '" << writer.get_record()->get_task()->get_name() << "': "
	            << __CPP_TRANSPORT_REPO_FAILED_POSTINT_GROUP_B << " '" << group_name << "' "
	            << __CPP_TRANSPORT_REPO_FAILED_POSTINT_GROUP_C;

            this->message(msg.str());
	        }
        else throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
	    }


    template <typename number>
    void repository_sqlite3<number>::close_derived_content_writer(derived_content_writer<number>& writer)
	    {
        output_task_record<number>* rec = writer.get_record();
        const std::list<std::string>& tags = writer.get_tags();
        assert(rec != nullptr);

        // create a new, empty output group record
        std::unique_ptr< output_group_record<output_payload> >
	        output_record(this->output_group_record_factory<output_payload>(rec->get_task()->get_name(), writer.get_relative_output_path(), false, std::list<std::string>(), tags));

        // stamp output group with the correct 'created' time stamp
        output_record->set_creation_time(writer.get_creation_time());
        output_record->set_name_from_creation_time();

        // populate output group with content from the writer
        const std::list<derived_content>& content = writer.get_content();
        for(std::list<derived_content>::const_iterator t = content.begin(); t != content.end(); t++)
	        {
            output_record->get_payload().add_derived_content(*t);
	        }
        output_record->get_payload().set_metadata(writer.get_metadata());

        // commit new output record
        output_record->commit();

        // add this output group to the integration task record
        rec->add_new_output_group(output_record->get_name());
        rec->update_last_edit_time();
        rec->commit();

        this->advise_commit(output_record.get());
	    }


    template <typename number>
    void repository_sqlite3<number>::abort_derived_content_writer(derived_content_writer<number>& writer)
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

            std::string group_name = boost::posix_time::to_iso_string(writer.get_creation_time());

            msg << __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_A << " '" << writer.get_record()->get_task()->get_name() << "': "
	            << __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_B << " '" << group_name << "' "
	            << __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_C;

            this->message(msg.str());
	        }
        else throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
	    }


    template<typename number>
    std::shared_ptr< output_group_record<integration_payload> >
    repository_sqlite3<number>::find_integration_task_output(const std::string& name, const std::list<std::string>& tags)
	    {
        // search for output groups associated with this task
        std::list< std::shared_ptr< output_group_record<integration_payload> > > output = this->enumerate_integration_task_content(name);

        // remove items from the list which have mismatching tags
        output.remove_if( [&] (const std::shared_ptr< output_group_record<integration_payload> > group) { return(group.get()->check_tags(tags)); } );

        if(output.empty())
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

        return(output.front());
	    }


    // JSON INTERFACE


		template <typename number>
		std::string repository_sqlite3<number>::export_JSON_package_record(const std::string& name)
			{
		    boost::filesystem::path filename = sqlite3_operations::find_package(this->db, name, __CPP_TRANSPORT_REPO_PACKAGE_MISSING);
		    Json::Value root = this->deserialize_document(filename);

		    Json::StreamWriterBuilder builder;
				return Json::writeString(builder, root);
			}


    template <typename number>
    std::string repository_sqlite3<number>::export_JSON_task_record(const std::string& name)
	    {
        if(sqlite3_operations::count_integration_tasks(this->db, name) > 0)
	        {
            boost::filesystem::path filename = sqlite3_operations::find_integration_task(this->db, name, __CPP_TRANSPORT_REPO_TASK_MISSING);
            Json::Value root = this->deserialize_document(filename);

            Json::StreamWriterBuilder builder;
            return Json::writeString(builder, root);
	        }
        else if(sqlite3_operations::count_postintegration_tasks(this->db, name) > 0)
	        {
            boost::filesystem::path filename = sqlite3_operations::find_postintegration_task(this->db, name, __CPP_TRANSPORT_REPO_TASK_MISSING);
            Json::Value root = this->deserialize_document(filename);

            Json::StreamWriterBuilder builder;
            return Json::writeString(builder, root);
	        }
        else if(sqlite3_operations::count_output_tasks(this->db, name) > 0)
	        {
            boost::filesystem::path filename = sqlite3_operations::find_output_task(this->db, name, __CPP_TRANSPORT_REPO_TASK_MISSING);
            Json::Value root = this->deserialize_document(filename);

            Json::StreamWriterBuilder builder;
            return Json::writeString(builder, root);
	        }

        std::stringstream msg;
        msg << __CPP_TRANSPORT_REPO_TASK_MISSING << " '" << name << "'";
        throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	    }


    template <typename number>
    std::string repository_sqlite3<number>::export_JSON_product_record(const std::string& name)
	    {
        boost::filesystem::path filename = sqlite3_operations::find_product(this->db, name, __CPP_TRANSPORT_REPO_PRODUCT_MISSING);
        Json::Value root = this->deserialize_document(filename);

        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, root);
	    }


		template <typename number>
		std::string repository_sqlite3<number>::export_JSON_content_record(const std::string& name)
			{
		    if(sqlite3_operations::count_integration_groups(this->db, name) > 0)
			    {
		        boost::filesystem::path filename = sqlite3_operations::find_group<integration_payload>(this->db, name, __CPP_TRANSPORT_REPO_OUTPUT_MISSING);
		        Json::Value root = this->deserialize_document(filename);

		        Json::StreamWriterBuilder builder;
		        return Json::writeString(builder, root);
			    }
		    else if(sqlite3_operations::count_postintegration_groups(this->db, name) > 0)
			    {
		        boost::filesystem::path filename = sqlite3_operations::find_group<postintegration_payload>(this->db, name, __CPP_TRANSPORT_REPO_OUTPUT_MISSING);
		        Json::Value root = this->deserialize_document(filename);

		        Json::StreamWriterBuilder builder;
		        return Json::writeString(builder, root);
			    }
		    else if(sqlite3_operations::count_output_groups(this->db, name) > 0)
			    {
		        boost::filesystem::path filename = sqlite3_operations::find_group<output_payload>(this->db, name, __CPP_TRANSPORT_REPO_OUTPUT_MISSING);
		        Json::Value root = this->deserialize_document(filename);

		        Json::StreamWriterBuilder builder;
		        return Json::writeString(builder, root);
			    }

		    std::stringstream msg;
		    msg << __CPP_TRANSPORT_REPO_OUTPUT_MISSING << " '" << name << "'";
		    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			}


    // DATABASE UTILITY FUNCTIONS


    // Check for a duplicate task name
    template <typename number>
    void repository_sqlite3<number>::check_task_duplicate(const std::string& name)
	    {
        if(sqlite3_operations::count_tasks(this->db, name) > 0) // should always =1, because primary key constraints in the database prevent duplicates
	        {
            std::ostringstream msg;
            msg <<  __CPP_TRANSPORT_REPO_TASK_DUPLICATE << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
	    }


    template <typename number>
    void repository_sqlite3<number>::commit_document(const boost::filesystem::path& path, const repository_record& record)
	    {
        // serialize the record
        Json::Value root(Json::objectValue);
        record.serialize(root);

        // write out contents to a temporary file
        boost::filesystem::path filename = path.filename();
        boost::filesystem::path parent   = path;
		    parent.remove_filename();
        boost::filesystem::path temp_filename = boost::filesystem::path(filename.string() + "-temp");

        boost::filesystem::path abs_record    = this->get_root_path() / path;
        boost::filesystem::path abs_temporary = this->get_root_path() / (parent / temp_filename);

        std::ofstream out;
        out.open(abs_temporary.string().c_str(), std::ios_base::out | std::ios_base::trunc);
		    if(out.is_open() && !out.fail())
			    {
		        out << root;
		        out.close();
			    }
		    else
			    {
		        std::stringstream msg;
				    msg << __CPP_TRANSPORT_REPO_COMMIT_FAILURE << " '" << record.get_name() << "'";
				    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

        // if this succeeded, remove any existing file and replace it with the new one
        boost::filesystem::rename(abs_temporary, abs_record);
	    }


		template <typename number>
		Json::Value repository_sqlite3<number>::deserialize_document(boost::filesystem::path& path)
			{
		    boost::filesystem::path abs_record = this->get_root_path() / path;

		    Json::Value root;

		    std::ifstream in;
				in.open(abs_record.string().c_str(), std::ios_base::in);
				if(in.is_open() && !in.fail())
					{
						in >> root;
					}
				else
					{
				    std::stringstream msg;
						msg << __CPP_TRANSPORT_REPO_DESERIALIZE_FAILURE << " '" << path << "'";
						throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					}

				return(root);
			}


		template <typename number>
		template <typename Payload>
		void repository_sqlite3<number>::enumerate_content_groups(const std::string& name, std::list< std::shared_ptr< output_group_record<Payload> > >& list, find_function finder)
			{
				std::list<std::string> group_names;

				// get list of group names associated with the task 'name'
				sqlite3_operations::enumerate_content_groups<Payload>(this->db, name, group_names);

				for(std::list<std::string>::iterator t = group_names.begin(); t != group_names.end(); t++)
					{
						boost::filesystem::path filename = sqlite3_operations::find_group<Payload>(this->db, *t, __CPP_TRANSPORT_REPO_OUTPUT_MISSING);
						Json::Value root = this->deserialize_document(filename);
						list.push_back(std::shared_ptr< output_group_record<Payload> >(this->template output_group_record_factory<Payload>(root)));
					}
			}


    // FACTORY FUNCTIONS TO BUILD A REPOSITORY


    template <typename number>
    json_interface_repository<number>* repository_factory(const std::string& path,
                                                          typename repository<number>::access_type mode = repository<number>::access_type::readwrite)
	    {
        return new repository_sqlite3<number>(path, mode);
	    }


    template <typename number>
    json_interface_repository<number>* repository_factory(const std::string& path,
                                                          typename repository<number>::access_type mode,
                                                          typename repository<number>::error_callback e,
                                                          typename repository<number>::warning_callback w,
                                                          typename repository<number>::message_callback m)
	    {
        return new repository_sqlite3<number>(path, mode, e, w, m);
	    }


    template <typename number>
    json_interface_repository<number>* repository_factory(const std::string& path, const repository_creation_key& key)
	    {
        return new repository_sqlite3<number>(path, key);
	    }


	}   // namespace transport


#endif //__repository_sqlite3_H_
