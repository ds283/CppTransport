//
// Created by David Seery on 04/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __repository_unqlite_H_
#define __repository_unqlite_H_

#include <assert.h>
#include <string>
#include <sstream>
#include <list>
#include <functional>
#include <utility>

#include "transport-runtime-api/manager/repository.h"

#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "transport-runtime-api/derived-products/data_products.h"

#include "transport-runtime-api/unqlite/unqlite_data.h"
#include "transport-runtime-api/unqlite/unqlite_serializable.h"
#include "transport-runtime-api/unqlite/unqlite_operations.h"

#include "boost/filesystem/operations.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


extern "C"
{
#include "unqlite/unqlite.h"
}


namespace transport
  {

    // forward-declare 'key' class used to create a repository.
    // the complete declaration is in a separate file,
    // which must be included to allow creation of repositories
    class repository_creation_key;

    // class 'repository_unqlite' implements the 'repository' interface using
    // UnQLite as the database backend.
    // It replaces the earlier DbXml-based repository implementation
    template <typename number>
    class repository_unqlite: public repository<number>
      {

      protected:

		    //! Internal record-type flag
        typedef enum { twopf_record, threepf_record, output_record } task_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Open a repository with a specific pathname
        repository_unqlite(const std::string& path, typename repository<number>::access_type mode=repository<number>::access_type::readwrite);

        //! Create a repository with a specific pathname
        repository_unqlite(const std::string& path, const repository_creation_key& key);

        //! Close a repository, including any corresponding containers
        ~repository_unqlite();


        // PUSH TASKS TO THE REPOSITORY DATABASE -- implements a 'repository' interface

      public:

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        virtual void write_package(const initial_conditions<number>& ics, const model<number>* m) override;

        //! Write a threepf integration task to the database.
        //! Delegates write_integration_task() to do the work.
        virtual void write_task(const threepf_task<number>& t, const model<number>* m) override
          { this->write_integration_task(t, m, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION); }

        //! Write a twopf integration task to the database.
        //! Delegates write_integration_task() to do the work.
        virtual void write_task(const twopf_task<number>& t, const model<number>* m) override
          { this->write_integration_task(t, m, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION); }

        //! Write an output task to the database
        virtual void write_task(const output_task<number>& t) override;

      protected:

        //! Write a generic integration task to the database, using a supplied node tag
        void write_integration_task(const integration_task<number>& t, const model<number>* m, const std::string& collection);


        // PULL TASKS FROM THE REPOSITORY DATABASE -- implements a 'repository' interface

      public:

        //! Query the database for a named task, and reconstruct it if present.
		    //! Supports both integration_task<> and output_task<> items.
		    //! Output tasks write nullptr to the model* handle.
        virtual task<number>* lookup_task(const std::string& name, model<number>*& m,
                                          typename instance_manager<number>::model_finder finder) override;

      protected:

        //! Extract an integration task from a serialization reader
        task<number>* lookup_integration_task(const std::string& name, task_type type,
                                              model<number>*& m, typename instance_manager<number>::model_finder finder,
                                              unqlite_serialization_reader* reader);

        //! Extract an output task from a serialization reader
        task<number>* lookup_output_task(const std::string& name, typename instance_manager<number>::model_finder finder,
                                         unqlite_serialization_reader* reader);


        // ADD AN OUTPUT-GROUP TO A TASK -- implements a 'repository' interface

      public:

        //! Insert a record for new twopf output in the task database, and set up paths to a suitable data container.
        //! Delegates insert_integration_output_group to do the work.
        virtual typename repository<number>::integration_writer new_integration_task_output(twopf_task<number>* tk, const std::list<std::string>& tags,
                                                                                            const std::string& backend, unsigned int worker) override;

        //! Insert a record for a new threepf output in the task database, and set up paths to a suitable data container.
        //! Delegates insert_integration_output_group to do the work.
        virtual typename repository<number>::integration_writer new_integration_task_output(threepf_task<number>* tk, const std::list<std::string>& tags,
                                                                                            const std::string& backend, unsigned int worker) override;

      protected:

        //! Insert an output record for a specified task, in a specified collection.
        typename repository<number>::integration_writer insert_integration_output_group(const std::string& collection, const std::string& backend,
                                                                                        unsigned int worker, task<number>* tk, const std::list<std::string>& tags);

        //! Allocate a new output-group serial number based on the list of used serial numbers from an output record
        unsigned int allocate_new_serial_number(unqlite_serialization_reader* reader);


        // PULL OUTPUT-GROUPS FROM A TASK -- implements a 'repository' interface

      public:

		    //! Enumerate the output available from a named task
        virtual std::list<typename repository<number>::output_group> enumerate_integration_task_output(const std::string& name) override;

      protected:

        //! Extract an output-group data block.
        //! The supplied unqlite_serialization_reader should point to an unread output-group JSON object.
        typename repository<number>::output_group extract_output_group_data(const std::string& task, unqlite_serialization_reader* reader);


        // PUSH DERIVED-PRODUCT SPECIFICATIONS TO THE DATABASE -- implements a 'repository' interface

      public:

        //! Write a derived product specification
        virtual void write_derived_product(const derived_data::derived_product<number>& d) override;


		    // PULL DERIVED-PRODUCT SPECIFICATIONS FROM THE DATABASE -- implements a 'repository' interface

      public:

        //! Query a derived product specification
		    virtual derived_data::derived_product<number>* lookup_derived_product(const std::string& product, typename instance_manager<number>::model_finder finder) override;


      protected:


        // ADD DERIVED CONTENT TO AN OUTPUT TASK -- implements a 'repository' interface

      public:

        //! Add derived content
        virtual typename repository<number>::derived_content_writer new_output_task_output(output_task<number>* tk, const std::list<std::string>& tags,
                                                                                           unsigned int worker) override;

        //! Lookup the output group for a task+derived-product, given a set of tags
        virtual typename repository<number>::output_group find_derived_product_output_group(const derived_data::derived_product<number>* product,
                                                                                            const std::list<std::string>& tags) override;

      protected:

		    //! Insert an output record for a named task
		    typename repository<number>::derived_content_writer insert_derived_content_output_group(unsigned int worker, output_task<number>* tk, const std::list<std::string>& tags);


        // PULL RECORDS FROM THE REPOSITORY DATABASE IN JSON FORMAT -- implements a 'json_extractible_repository' interface
        // FIXME: This interface isn't codified yet; need to add a 'json_extractible_repository' concept

      public:

        //! Extract the JSON document for a named package
        std::string json_package_document(const std::string& name);

        //! Extract the JSON document for a named integration
        std::string json_task_document(const std::string& name);


        // INTERNAL UTILITY FUNCTIONS

      protected:

		    //! Notionally begin a transaction on the database.
		    //! Currently this is implemented by opening the database at the beginning of an 'atomic' transaction,
		    //! and then closing it at the end.
		    //! As UnQLite develops in sophistication it may be possible to replace this by an internal transaction manager.
		    void begin_transaction();

		    //! Notionally commit a transaction to the database.
		    //! Currently this is implemented by closing the database at the end of an 'atomic' transaction.
		    //! As UnQLite develops in sophistication it may be possible to replace this by an internal transaction manager.
		    void commit_transaction();

		    //! Convert a named database task record into a serialization reader, returned as a pointer.
		    //! It's up to the calling function to destroy the pointer which is returned.
        unqlite_serialization_reader* get_task_serialization_reader(const std::string& name, int& unqlite_id, task_type& type);

		    //! Convert a named database package record into a serialization reader, returned as a pointer.
		    //! It's up to the calling function to destroy the pointer which is returned.
		    unqlite_serialization_reader* get_package_serialization_reader(const std::string& name, int& unqlite_id);

		    //! Convert a named database derived product record into a serialization reader, returned as a pointer.
		    //! It's up to the calling function to destroy the pointer which is returned.
		    unqlite_serialization_reader* get_derived_product_serialization_reader(const std::string& name, int& unqlite_id);


        // INTERNAL DATA

      private:

        //! BOOST path to data containers (parent directory for the following containers)
        boost::filesystem::path containers_path;
        //! BOOST path to container for packages
        boost::filesystem::path packages_path;
        //! BOOST path to container for integrations
        boost::filesystem::path tasks_path;
		    //! BOOST path to container for derived products
		    boost::filesystem::path derived_products_path;


        // UNQLITE DATABASE HANDLES

      private:

        unqlite* package_db;
        unqlite* task_db;
		    unqlite* derived_product_db;

		    unsigned int open_clients;
	    };


    // IMPLEMENTATION


    // Create a repository object associated with a pathname
    template <typename number>
    repository_unqlite<number>::repository_unqlite(const std::string& path, typename repository<number>::access_type mode)
      : package_db(nullptr), task_db(nullptr), open_clients(0), repository<number>(path, mode)
      {
        // supplied path should be a directory which exists
        if(!boost::filesystem::is_directory(path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_ROOT << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        // database containers should be present in an existing directory
        containers_path = this->get_root_path() / __CPP_TRANSPORT_REPO_REPOSITORY_LEAF;
        if(!boost::filesystem::is_directory(containers_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_CNTR << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        // package database should be present inside the container directory
        packages_path = containers_path / __CPP_TRANSPORT_CNTR_PACKAGES_LEAF;
        if(!boost::filesystem::is_regular_file(packages_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_PACKAGES << " " << packages_path;
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        // tasks database should be present inside the container directory
        tasks_path = containers_path / __CPP_TRANSPORT_CNTR_TASKS_LEAF;
        if(!boost::filesystem::is_regular_file(tasks_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASKS << " " << tasks_path;
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

		    // derived products database should be present inside the container directory
		    derived_products_path = containers_path / __CPP_TRANSPORT_CNTR_DERIVED_PRODUCTS_LEAF;
		    if(!boost::filesystem::is_regular_file(derived_products_path))
			    {
		        std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_MISSING_DERIVED_PRODUCTS << " " << derived_products_path;
				    throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
			    }

        // open database connexions for reading (slave processes) or reading/writing (master process)
		    unsigned int m = (mode == repository<number>::access_type::readonly ? UNQLITE_OPEN_READONLY : UNQLITE_OPEN_READWRITE);

		    unqlite* pkg_db;
		    unqlite* tk_db;
		    unqlite* dp_db;

        if(unqlite_open(&pkg_db, packages_path.c_str(), m) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_PKG_OPEN << " " << packages_path;
            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
          }

        if(unqlite_open(&tk_db, tasks_path.c_str(), m) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_TASKS_OPEN << " '" << tasks_path;
            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
          }

		    if(unqlite_open(&dp_db, derived_products_path.c_str(), m) != UNQLITE_OK)
			    {
		        std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_FAIL_DERIVED_PRODUCTS_OPEN << " " << derived_products_path;
				    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
			    }

        // ensure default collections are present within each containers
        unqlite_operations::ensure_collection(pkg_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);

        unqlite_operations::ensure_collection(tk_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION);
        unqlite_operations::ensure_collection(tk_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION);
        unqlite_operations::ensure_collection(tk_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION);

        unqlite_operations::ensure_collection(dp_db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION);

		    // close database connexions until they are needed later
		    unqlite_close(pkg_db);
		    unqlite_close(tk_db);
		    unqlite_close(dp_db);
      }


    // Create a named repository
    template <typename number>
    repository_unqlite<number>::repository_unqlite(const std::string& path, const repository_creation_key& key)
      : package_db(nullptr), task_db(nullptr), derived_product_db(nullptr),
        open_clients(0), repository<number>(path, repository<number>::access_type::readwrite)
      {
        // check whether root directory for the repository already exists -- it shouldn't
        if(boost::filesystem::exists(path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_ROOT_EXISTS << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        containers_path       = this->get_root_path() / __CPP_TRANSPORT_REPO_REPOSITORY_LEAF;
        packages_path         = containers_path / __CPP_TRANSPORT_CNTR_PACKAGES_LEAF;
        tasks_path            = containers_path / __CPP_TRANSPORT_CNTR_TASKS_LEAF;
        derived_products_path = containers_path / __CPP_TRANSPORT_CNTR_DERIVED_PRODUCTS_LEAF;

        // create directory structure
        boost::filesystem::create_directories(this->get_root_path());
        boost::filesystem::create_directories(containers_path);

        // create containers
        unqlite* pkg_db;
        unqlite* tk_db;
		    unqlite* dp_db;

        if(unqlite_open(&pkg_db, packages_path.c_str(), UNQLITE_OPEN_CREATE) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_PKG_OPEN << " " << packages_path;
            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
          }

        if(unqlite_open(&tk_db, tasks_path.c_str(), UNQLITE_OPEN_CREATE) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_TASKS_OPEN << " " << tasks_path;
            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
          }

		    if(unqlite_open(&dp_db, derived_products_path.c_str(), UNQLITE_OPEN_CREATE) != UNQLITE_OK)
			    {
		        std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_FAIL_DERIVED_PRODUCTS_OPEN << " " << derived_products_path;
				    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
			    }

        // create default collection within each containers
        unqlite_operations::create_collection(pkg_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);

        unqlite_operations::create_collection(tk_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION);
        unqlite_operations::create_collection(tk_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION);
        unqlite_operations::create_collection(tk_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION);

        unqlite_operations::create_collection(dp_db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION);

        // close repository until it is needed later
        unqlite_close(pkg_db);
        unqlite_close(tk_db);
		    unqlite_close(dp_db);
      }


    // Destroy a repository_unqlite object, closing the associated repository
    template <typename number>
    repository_unqlite<number>::~repository_unqlite()
      {
		    // clean up open handles if they exist
        if(this->package_db != nullptr) unqlite_close(this->package_db);
        if(this->task_db != nullptr) unqlite_close(this->task_db);
        if(this->derived_product_db != nullptr) unqlite_close(this->derived_product_db);
      }


		// Open database handles.
		template <typename number>
		void repository_unqlite<number>::begin_transaction()
			{
				// If open_clients > 0, then the handles should be open; otherwise, they should be closed
				assert((this->open_clients > 0 && this->package_db != nullptr)         || (this->open_clients == 0 && this->package_db == nullptr));
				assert((this->open_clients > 0 && this->task_db != nullptr)            || (this->open_clients == 0 && this->task_db == nullptr));
				assert((this->open_clients > 0 && this->derived_product_db != nullptr) || (this->open_clients == 0 && this->derived_product_db == nullptr));

				if((this->open_clients == 0 && this->package_db != nullptr) ||
					 (this->open_clients == 0 && this->task_db != nullptr)    ||
					 (this->open_clients == 0 && this->derived_product_db != nullptr))
					throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_OPEN);

				if((this->open_clients > 0 && this->package_db == nullptr) ||
					 (this->open_clients > 0 && this->task_db == nullptr)    ||
					 (this->open_clients > 0 && this->derived_product_db == nullptr))
					throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_CLOSED);

				if(this->open_clients == 0)
					{
				    unsigned int m = (this->get_access_mode() == repository<number>::access_type::readonly ? UNQLITE_OPEN_READONLY : UNQLITE_OPEN_READWRITE);

				    if(unqlite_open(&(this->package_db), this->packages_path.c_str(), m) != UNQLITE_OK)
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_REPO_FAIL_PKG_OPEN << " " << this->packages_path;
				        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
					    }

				    if(unqlite_open(&(this->task_db), this->tasks_path.c_str(), m) != UNQLITE_OK)
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_REPO_FAIL_TASKS_OPEN << " " << this->tasks_path;
				        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
					    }

						if(unqlite_open(&(this->derived_product_db), this->derived_products_path.c_str(), m) != UNQLITE_OK)
							{
						    std::ostringstream msg;
								msg << __CPP_TRANSPORT_REPO_FAIL_DERIVED_PRODUCTS_OPEN << " " << this->derived_products_path;
								throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
							}

				    // ensure default collections are present within each containers
				    unqlite_operations::ensure_collection(this->package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);

				    unqlite_operations::ensure_collection(this->task_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION);
				    unqlite_operations::ensure_collection(this->task_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION);
				    unqlite_operations::ensure_collection(this->task_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION);

				    unqlite_operations::ensure_collection(this->derived_product_db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION);
					}

				this->open_clients++;
			}


    // Close database handles
    template <typename number>
    void repository_unqlite<number>::commit_transaction()
	    {
		    // open_clients should be > 0 and the handles should be open
		    assert(this->open_clients > 0);
        assert(this->package_db != nullptr);
        assert(this->task_db != nullptr);
		    assert(this->derived_product_db != nullptr);

		    if(this->open_clients <= 0) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_NOT_OPEN);

        if(this->package_db == nullptr || this->task_db == nullptr || this->derived_product_db == nullptr)
	        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_CLOSED);

		    this->open_clients--;

		    if(this->open_clients == 0)
			    {
		        unqlite_close(this->package_db), this->package_db = nullptr;
		        unqlite_close(this->task_db), this->task_db = nullptr;
				    unqlite_close(this->derived_product_db), this->derived_product_db = nullptr;
			    }
	    }


    // Write a model/initial-conditions/parameters combination to the repository
		// DATABASE ENTRY POINT
    template <typename number>
    void repository_unqlite<number>::write_package(const initial_conditions<number>& ics, const model<number>* m)
	    {
        assert(m != nullptr);

        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check if a package with this name already exists
        unsigned int count = unqlite_operations::query_count(this->package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, ics.get_name(), __CPP_TRANSPORT_NODE_PACKAGE_NAME);
        if(count > 0)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_PACKAGE_EXISTS << " '" << ics.get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

        // create an unqlite_serialization_writer, used to emit the serialized record to the database
        unqlite_serialization_writer writer;

        // commit initial-conditions name
        writer.write_value(__CPP_TRANSPORT_NODE_PACKAGE_NAME, ics.get_name());

        // commit model uid
        std::string uid = m->get_identity_string();
        writer.write_value(__CPP_TRANSPORT_NODE_PACKAGE_MODELUID, uid);

        // commit data block
        writer.start_node(__CPP_TRANSPORT_NODE_PACKAGE_METADATA);

        writer.write_value(__CPP_TRANSPORT_NODE_PKG_METADATA_NAME, m->get_name());
        writer.write_value(__CPP_TRANSPORT_NODE_PKG_METADATA_AUTHOR, m->get_author());
        writer.write_value(__CPP_TRANSPORT_NODE_PKG_METADATA_TAG, m->get_tag());

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        writer.write_value(__CPP_TRANSPORT_NODE_PKG_METADATA_CREATED, boost::posix_time::to_simple_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_PKG_METADATA_EDITED, boost::posix_time::to_simple_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_PKG_METADATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

        writer.end_element(__CPP_TRANSPORT_NODE_PACKAGE_METADATA);

        // commit initial conditions
        writer.start_node(__CPP_TRANSPORT_NODE_PACKAGE_ICS);
        ics.serialize(writer);
        writer.end_element(__CPP_TRANSPORT_NODE_PACKAGE_ICS);

        // insert this record in the package database
        unqlite_operations::store(this->package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, writer.get_contents());

        // commit transaction
        this->commit_transaction();
	    }


    // Write a task to the repository
		// DATABASE ENTRY POINT
    template <typename number>
    void repository_unqlite<number>::write_integration_task(const integration_task<number>& t, const model<number>* m, const std::string& collection)
	    {
        assert(m != nullptr);

        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        // check if a task with this name already exists
        unsigned int twopf_count   = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, t.get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
        unsigned int threepf_count = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, t.get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
		    unsigned int output_count  = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION, t.get_name(), __CPP_TRANSPORT_NODE_OUTPUT_NAME);

		    unsigned int total = twopf_count + threepf_count + output_count;

        if(total > 0)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_TASK_EXISTS << " '" << t.get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

        // check whether the specified initial_conditions object is already in the database; if not, insert it
        unsigned int count = unqlite_operations::query_count(this->package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, t.get_ics().get_name(), __CPP_TRANSPORT_NODE_PACKAGE_NAME);
        if(count == 0) this->write_package(t.get_ics(), m);

        // create an unqlite_serialization_writer, used to emit the serialized record to the database
        unqlite_serialization_writer writer;

        // commit task name
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_NAME, t.get_name());

        // commit metadata block
        writer.start_node(__CPP_TRANSPORT_NODE_TASK_METADATA);

        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_PACKAGE, t.get_ics().get_name());

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_CREATED, boost::posix_time::to_simple_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_EDITED, boost::posix_time::to_simple_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

        writer.end_element(__CPP_TRANSPORT_NODE_TASK_METADATA);

        // commit task block
        writer.start_node(__CPP_TRANSPORT_NODE_TASK_INTEGRATION_DETAILS);
        t.serialize(writer);
        writer.end_element(__CPP_TRANSPORT_NODE_TASK_INTEGRATION_DETAILS);

        // write empty array of output blocks: will be populated when integrations are run
        writer.start_array(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS, true);
        writer.end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS);

        // insert this record in the task database
        unqlite_operations::store(this->task_db, collection, writer.get_contents());

        // commit transaction
        this->commit_transaction();
	    }


    // Write an output task to the database
		// DATABASE ENTRY POINT
		template <typename number>
		void repository_unqlite<number>::write_task(const output_task<number>& t)
	    {
		    // open a new transaction, if necessary. After this we can assume the database handles are live
		    this->begin_transaction();

        // check if a task with this name already exists
        unsigned int twopf_count   = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, t.get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
        unsigned int threepf_count = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, t.get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
        unsigned int output_count  = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION, t.get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);

        unsigned int total = twopf_count + threepf_count + output_count;

        if(total > 0)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_TASK_EXISTS << " '" << t.get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

		    // An output task consists of a number of derived product specifications.
		    // Each derived product may utilize a number of content providers, each of which
		    // may be associated with a different task.
		    // At the top of the hierarchy, each derived product name should be unique.

		    // create an unqlite_serialization_writer, used to emit the serialized record to the database
		    unqlite_serialization_writer writer;

		    // commit task name
		    writer.write_value(__CPP_TRANSPORT_NODE_TASK_NAME, t.get_name());

		    // commit data block
		    writer.start_node(__CPP_TRANSPORT_NODE_TASK_METADATA);

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_CREATED, boost::posix_time::to_simple_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_EDITED, boost::posix_time::to_simple_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

        writer.end_element(__CPP_TRANSPORT_NODE_TASK_METADATA);

		    // commit task block
		    writer.start_node(__CPP_TRANSPORT_NODE_TASK_OUTPUT_DETAILS);
		    t.serialize(writer);
		    writer.end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT_DETAILS);

        // insert this record in the task database
        unqlite_operations::store(this->task_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION, writer.get_contents());

        // commit transaction
        this->commit_transaction();
	    }


    namespace
	    {

        typedef struct
	        {
            // input data
            unsigned int cmp;
            std::string str_cmp;

            // output data
            int id;
            unqlite_serialization_reader* reader;
	        } array_extraction_data;


        // array_walk callback to extract n'th component of an array
        int array_extract(unqlite_value* key, unqlite_value* value, void* handle)
	        {
            assert(key != nullptr);
            assert(value != nullptr);
            assert(handle != nullptr);

            array_extraction_data* data = static_cast<array_extraction_data*>(handle);

            bool match = false;

            if(unqlite_value_is_int(key) && data->cmp == unqlite_value_to_int(key)) match = true;
            if(unqlite_value_is_string(key) && data->str_cmp == std::string(unqlite_value_to_string(key, nullptr))) match = true;

            if(match)
	            {
                assert(data->reader == nullptr);

                // convert this JSON object to a serialization reader
                data->reader = new unqlite_serialization_reader(value);

                // extract internal UnQLite ID for this record
                unqlite_value* id = unqlite_array_fetch(value, __CPP_TRANSPORT_UNQLITE_RECORD_ID, -1);

                if(id == nullptr || !unqlite_value_is_int(id))
	                throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_NO_ID);

                data->id = unqlite_value_to_int(id);
	            }

            return(UNQLITE_OK);
	        }

	    }   // unnamed namespace


    // Query the database for a named task, returned as a serialization_reader
    template <typename number>
    unqlite_serialization_reader* repository_unqlite<number>::get_task_serialization_reader(const std::string& name, int& unqlite_id, task_type& type)
	    {
        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check if a suitable record exists
        unqlite_vm* vm_twopf   = nullptr;
        unqlite_vm* vm_threepf = nullptr;
		    unqlite_vm* vm_output  = nullptr;

        unqlite_value* twopf_recs   = unqlite_operations::query(this->task_db, vm_twopf, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, name, __CPP_TRANSPORT_NODE_TASK_NAME);
        unqlite_value* threepf_recs = unqlite_operations::query(this->task_db, vm_threepf, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, name, __CPP_TRANSPORT_NODE_TASK_NAME);
		    unqlite_value* output_recs  = unqlite_operations::query(this->task_db, vm_output, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION, name, __CPP_TRANSPORT_NODE_TASK_NAME);

        if(twopf_recs == nullptr || threepf_recs == nullptr || output_recs == nullptr ||
	         !unqlite_value_is_json_array(twopf_recs) || !unqlite_value_is_json_array(threepf_recs) || !unqlite_value_is_json_array(output_recs))
	        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

        unsigned int twopf_count   = static_cast<unsigned int>(unqlite_array_count(twopf_recs));
        unsigned int threepf_count = static_cast<unsigned int>(unqlite_array_count(threepf_recs));
		    unsigned int output_count  = static_cast<unsigned int>(unqlite_array_count(output_recs));

		    unsigned int total = twopf_count + threepf_count + output_count;

        if(total == 0)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
        else if(total > 1)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_TASK << " '" << name << "'" __CPP_TRANSPORT_RUN_REPAIR;
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

        unqlite_value* recs = nullptr;
        unqlite_vm*    vm   = nullptr;

        if(twopf_count == 1)        { recs = twopf_recs;   vm = vm_twopf;   type = twopf_record;   }
        else if(threepf_count == 1) { recs = threepf_recs; vm = vm_threepf; type = threepf_record; }
		    else                        { recs = output_recs;  vm = vm_output;  type = output_record;  }

        if(unqlite_array_count(recs) != 1)   // shouldn't happen because checked for above
	        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_TASK_EXTRACT_FAIL);

        array_extraction_data data;
        data.cmp     = 0;
        data.str_cmp = boost::lexical_cast<std::string>(data.cmp);
        data.id      = -1;
        data.reader  = nullptr;

        unqlite_array_walk(recs, &array_extract, &data);

        unqlite_vm_release(vm_twopf);
        unqlite_vm_release(vm_threepf);
		    unqlite_vm_release(vm_output);

        // commit transaction
        this->commit_transaction();

        unqlite_id = data.id;
        return(data.reader);
	    }


    // Query the database for a named package, returned as a serialization_reader
    template <typename number>
    unqlite_serialization_reader* repository_unqlite<number>::get_package_serialization_reader(const std::string& name, int& unqlite_id)
	    {
        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check if a suitable record exists
        unqlite_vm* vm = nullptr;

        unqlite_value* recs = unqlite_operations::query(this->package_db, vm, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, name, __CPP_TRANSPORT_NODE_PACKAGE_NAME);

        if(recs == nullptr || !unqlite_value_is_json_array(recs))
	        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

        unsigned int count = static_cast<unsigned int>(unqlite_array_count(recs));

        if(count == 0)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_PACKAGE << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
        else if(count > 1)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_PACKAGE << " '" << name << "'" __CPP_TRANSPORT_RUN_REPAIR;
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

        if(unqlite_array_count(recs) != 1)   // shouldn't happen because checked for above
	        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_PKG_EXTRACT_FAIL);

        array_extraction_data data;
        data.cmp     = 0;
        data.str_cmp = boost::lexical_cast<std::string>(data.cmp);
        data.id      = -1;
        data.reader  = nullptr;

        unqlite_array_walk(recs, &array_extract, &data);

        unqlite_vm_release(vm);

        // commit transaction
        this->commit_transaction();

        unqlite_id = data.id;
        return(data.reader);
	    }


		// Query the database for a named derived product, returned as a serialization_reader
		template <typename number>
		unqlite_serialization_reader* repository_unqlite<number>::get_derived_product_serialization_reader(const std::string& name, int& unqlite_id)
			{
				// open a new transaction if necessary. After this we can cassume the database handles are live
				this->begin_transaction();

				// check if a suitable record exists
				unqlite_vm* vm = nullptr;

		    unqlite_value* recs = unqlite_operations::query(this->derived_product_db, vm, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION,
		                                                    name, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME);

				if(recs == nullptr || !unqlite_value_is_json_array(recs))
					throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

				unsigned int count = static_cast<unsigned int>(unqlite_array_count(recs));

				if(count == 0)
					{
				    std::ostringstream msg;
						msg << __CPP_TRANSPORT_REPO_MISSING_DERIVED_PRODUCT << " '" << name << "'";
						throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					}
				else if(count > 1)
					{
				    std::ostringstream msg;
						msg << __CPP_TRANSPORT_REPO_DUPLICATE_DERIVED_PRODUCT << " '" << name << "'" __CPP_TRANSPORT_RUN_REPAIR;
						throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					}

				if(unqlite_array_count(recs) != 1)    // shouldn't happen
					throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_EXTRACT_FAIL);

				array_extraction_data data;
		    data.cmp     = 0;
		    data.str_cmp = boost::lexical_cast<std::string>(data.cmp);
		    data.id      = -1;
		    data.reader  = nullptr;

				unqlite_array_walk(recs, &array_extract, &data);

				unqlite_vm_release(vm);

				// commit transaction
				this->commit_transaction();

				unqlite_id = data.id;
				return(data.reader);
			}


    // Query the database for a named task, which is reconstructed and returned as a task<> object
		// DATABASE ENTRY POINT
    template <typename number>
    task<number>* repository_unqlite<number>::lookup_task(const std::string& name, model<number>*& m,
                                                          typename instance_manager<number>::model_finder finder)
	    {
        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // get serialization_reader for the named task record
        int task_id = 0;
        task_type type;
        unqlite_serialization_reader* task_reader = this->get_task_serialization_reader(name, task_id, type);

        task<number>* rval = nullptr;

		    // work out how to build an appropriate task based on the record type
        switch(type)
	        {
            case twopf_record:
	            rval = this->lookup_integration_task(name, type, m, finder, task_reader);
            break;

            case threepf_record:
	            rval = this->lookup_integration_task(name, type, m, finder, task_reader);
            break;

            case output_record:
	            rval = this->lookup_output_task(name, finder, task_reader);
		          m = nullptr;  // outout tasks aren't associated with a unique model
            break;

            default:
	            assert(false);
	        }

        // commit transaction
        this->commit_transaction();

        delete task_reader;

        return (rval);
	    }


		// Extract integration task
		template <typename number>
		task<number>* repository_unqlite<number>::lookup_integration_task(const std::string& name, task_type type,
		                                                                  model<number>*& m, typename instance_manager<number>::model_finder finder,
		                                                                  unqlite_serialization_reader* task_reader)
			{
				assert(task_reader != nullptr);

        // extract data:
        task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_METADATA);

        // extract the model/initial-conditions/parameters package corresponding to this task
        std::string package_name;
        task_reader->read_value(__CPP_TRANSPORT_NODE_TASK_METADATA_PACKAGE, package_name);

        task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_METADATA);

        // get serialization_reader for the named package
        int package_id = 0;
        unqlite_serialization_reader* package_reader = this->get_package_serialization_reader(package_name, package_id);

        // extract UID for model
        std::string uid;
        package_reader->read_value(__CPP_TRANSPORT_NODE_PACKAGE_MODELUID, uid);

        // use the supplied finder to recover a model object for this UID
        m = finder(uid);

        // obtain parameter and initial-conditions validators from this model
        typename parameters<number>::params_validator p_validator = m->params_validator_factory();
        typename initial_conditions<number>::ics_validator ics_validator = m->ics_validator_factory();

        // move the package serialization reader to the initial conditions block, and use it to
        // reconstruct a initial_conditions<> object
        package_reader->start_node(__CPP_TRANSPORT_NODE_PACKAGE_ICS);
        package_reader->push_bookmark();
        initial_conditions<number> ics = initial_conditions_helper::deserialize<number>(package_reader, package_name,
                                                                                        m->get_param_names(), m->get_state_names(),
                                                                                        p_validator, ics_validator);
        package_reader->pop_bookmark();
        package_reader->end_element(__CPP_TRANSPORT_NODE_PACKAGE_ICS);

        // reset read data from the task record, and use it to reconstruct a task<> object
        task_reader->reset();

        // move the task reader to the task description block, and use it to reconstruct a task<>
        task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_INTEGRATION_DETAILS);
        task_reader->push_bookmark();

				task<number>* rval = nullptr;

        switch(type)
	        {
            case twopf_record:
	            {
                twopf_task<number> tk = twopf_task_helper::deserialize(task_reader, name, ics, m->kconfig_kstar_factory());
                rval = new twopf_task<number>(tk);
		            break;
			        }

		        case threepf_record:
			        {
		            threepf_task<number> tk = threepf_task_helper::deserialize(task_reader, name, ics, m->kconfig_kstar_factory());
				        rval = new threepf_task<number>(tk);
		            break;
			        }

		        default:
			        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_TASK);
			    }
		    task_reader->pop_bookmark();
		    task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_INTEGRATION_DETAILS);

		    delete package_reader;

        return(rval);
      }


		// Extract output task
		template <typename number>
		task<number>* repository_unqlite<number>::lookup_output_task(const std::string& name, typename instance_manager<number>::model_finder finder,
		                                                             unqlite_serialization_reader* reader)
			{
				assert(reader != nullptr);

		    // move the task reader to the task description block, and use it to reconstruct a task<>
				reader->start_node(__CPP_TRANSPORT_NODE_TASK_OUTPUT_DETAILS);
				reader->push_bookmark();

				// generate lookup functions for tasks and derived products

		    typename output_task_helper::derived_product_finder<number>::type pfinder =
			                                                                      std::bind(&repository_unqlite<number>::lookup_derived_product, this, std::placeholders::_1, finder);

				output_task<number> tk = output_task_helper::deserialize<number>(reader, name, pfinder);
		    task<number>* rval = new output_task<number>(tk);

		    reader->pop_bookmark();
		    reader->end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT_DETAILS);

		    return(rval);
			}


    // Extract package database record as a JSON document
    // DATABASE ENTRY POINT
    template <typename number>
    std::string repository_unqlite<number>::json_package_document(const std::string& name)
      {
        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check a suitable record exists
        unsigned int count = unqlite_operations::query_count(this->package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, name, __CPP_TRANSPORT_NODE_PACKAGE_NAME);

        if(count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_PACKAGE << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
        else if(count > 1)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_PACKAGE << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // extract this record in JSON format
        std::string document = unqlite_operations::extract_json(this->package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, name, __CPP_TRANSPORT_NODE_PACKAGE_NAME);

		    // commit transaction
        this->commit_transaction();

        return(document);
      }


    // Extract integration task database record as a JSON document
		// DATABASE ENTRY POINT
    template <typename number>
    std::string repository_unqlite<number>::json_task_document(const std::string& name)
      {
        std::cerr << "Getting JSON task document '" << name << "'" << std::endl;
        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check a suitable record exists
        unsigned int twopf_count   = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, name, __CPP_TRANSPORT_NODE_TASK_NAME);
        unsigned int threepf_count = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, name, __CPP_TRANSPORT_NODE_TASK_NAME);
		    unsigned int output_count  = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION, name, __CPP_TRANSPORT_NODE_TASK_NAME);

		    unsigned int total = twopf_count + threepf_count + output_count;

        std::string rval;

        if(total == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
        else if(total > 1)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_TASK << " '" << name << "'" __CPP_TRANSPORT_RUN_REPAIR;
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
        else
          {
            std::string collection;
            if(twopf_count == 1)        collection = __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION;
            else if(threepf_count == 1) collection = __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION;
		        else                        collection = __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION;

            rval = unqlite_operations::extract_json(this->task_db, collection, name, __CPP_TRANSPORT_NODE_TASK_NAME);
          }

		    // commit transaction
        this->commit_transaction();

        return(rval);
      }


    // Add output for a twopf task
		// DATABASE ENTRY POINT
    template <typename number>
    typename repository<number>::integration_writer
    repository_unqlite<number>::new_integration_task_output(twopf_task<number>* tk, const std::list<std::string>& tags,
                                                            const std::string& backend, unsigned int worker)
      {
        assert(tk != nullptr);

        if(tk == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_TASK);

        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check this task name exists in the database
        unsigned int count = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, tk->get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
        if(count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // insert a new output record, and return the corresponding integration_writer handle
        typename repository<number>::integration_writer writer = this->insert_integration_output_group(__CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, backend, worker, tk, tags);

		    // close database handles
        this->commit_transaction();

        return(writer);
      }


    // Add output for a threepf task
		// DATABASE ENTRY POINT
    template <typename number>
    typename repository<number>::integration_writer
    repository_unqlite<number>::new_integration_task_output(threepf_task<number>* tk, const std::list<std::string>& tags,
                                                            const std::string& backend, unsigned int worker)
      {
        assert(tk != nullptr);

        if(tk == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_TASK);

        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check this task name exists in the database
        unsigned int count = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, tk->get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
        if(count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // insert a new output record, and return the corresponding integration_writer handle
        typename repository<number>::integration_writer writer = this->insert_integration_output_group(__CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, backend, worker, tk, tags);

		    // commit transaction
        this->commit_transaction();

        return(writer);
      }


    // Add output for a specified task.
		// We are allowed to assume that the database handles are already open, and that the task exists
		// in an appropriate collection
    template <typename number>
    typename repository<number>::integration_writer
    repository_unqlite<number>::insert_integration_output_group(const std::string& collection, const std::string& backend,
                                                                unsigned int worker, task<number>* tk, const std::list<std::string>& tags)
      {
        // get serialization_reader for the named task record
        int task_id = 0;
		    task_type type;
        unqlite_serialization_reader* task_reader = this->get_task_serialization_reader(tk->get_name(), task_id, type);
		    assert(type == twopf_record || type == threepf_record);

		    // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

		    // construct paths for the various output files and directories.
		    // We use the ISO form of the current time to label the output group directory.
		    // This means the repository directory structure will be human-readable if necessary.
        std::string output_leaf = boost::posix_time::to_iso_string(now);

        boost::filesystem::path output_path  = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF) / tk->get_name() / output_leaf;
        boost::filesystem::path sql_path     = output_path / __CPP_TRANSPORT_REPO_DATABASE_LEAF;
        boost::filesystem::path log_path     = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path task_path    = output_path / __CPP_TRANSPORT_REPO_TASKFILE_LEAF;
        boost::filesystem::path temp_path    = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

        // update task_reader with information about the new output group
		    task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_METADATA);
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_TASK_METADATA_EDITED, boost::posix_time::to_simple_string(now));   // insert overwrites previous value
		    task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_METADATA);

		    task_reader->start_array(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS);

		    task_reader->insert_node("arrayelt");  // name of node doesn't matter; it is ignored in arrays
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_BACKEND, backend);
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_PATH, output_path.string());
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DATABASE, sql_path.string());
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_CREATED, boost::posix_time::to_simple_string(now));
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_LOCKED, false);

		    task_reader->insert_array(__CPP_TRANSPORT_NODE_OUTPUTGROUP_NOTES, true);
		    task_reader->insert_end_element(__CPP_TRANSPORT_NODE_OUTPUTGROUP_NOTES);

		    task_reader->insert_array(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS, tags.size() == 0);
		    for(std::list<std::string>::const_iterator t = tags.begin(); t != tags.end(); t++)
			    {
				    task_reader->insert_node("arrayelt");   // node name doesn't matter for an array
				    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAG, *t);
				    task_reader->insert_end_element("arrayelt");
			    }
		    task_reader->insert_end_element(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS);

		    task_reader->insert_end_element("arrayelt");

		    task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS);

		    // update the task entry for this database
		    // that means: first, drop this existing record; then, store a copy of the updated one
        unqlite_operations::drop(this->task_db, collection, task_id);

        unqlite_operations::store(this->task_db, collection, task_reader->get_contents());

		    // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

		    delete task_reader;

		    return typename repository<number>::integration_writer(this->get_root_path()/output_path, this->get_root_path()/sql_path,
		                                                           this->get_root_path()/log_path, this->get_root_path()/task_path,
		                                                           this->get_root_path()/temp_path, serial_number, worker);
      }


		// Enumerate the output groups associated with some named task.
		// The output group descriptors returned by this function can be used to set up
		// derived_content_writer objects, which allow the corresponding integration output
		// to be extracted from the database
		template <typename number>
		std::list<typename repository<number>::output_group> repository_unqlite<number>::enumerate_integration_task_output(const std::string& name)
			{
		    std::list<typename repository<number>::output_group> group_list;

				// open a new transaction, if necessary. After this we can assume the database handles are live
				this->begin_transaction();

				// get a serialization_reader for the named task record
				int task_id = 0;
				task_type type;
				unqlite_serialization_reader* task_reader = this->get_task_serialization_reader(name, task_id, type);

				// move the serialization reader to the output-group block
				unsigned int num_groups = task_reader->start_array(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS);

				for(unsigned int i = 0; i < num_groups; i++)
					{
						task_reader->start_array_element();
						typename repository<number>::output_group group_data = this->extract_output_group_data(name, task_reader);
						group_list.push_back(group_data);
						task_reader->end_array_element();
					}

				task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS);

				// commit transaction
				this->commit_transaction();

				delete task_reader;

				// sort the output groups into descending order of creation date, so the first element in the
				// list is the most recent data group.
				// This is usually what would be required.
				group_list.sort(&output_group_helper::comparator<number>);

				return(group_list);
			}


		// Extract an output-group data block from a task record.
		template <typename number>
		typename repository<number>::output_group repository_unqlite<number>::extract_output_group_data(const std::string& task, unqlite_serialization_reader* reader)
			{
				assert(reader != nullptr);

				// extract data about the group

		    std::string backend;
				reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_BACKEND, backend);

		    std::string data_root;
				reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_PATH, data_root);

		    std::string data_container;
				reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DATABASE, data_container);

		    std::string creation_time;
				reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_CREATED, creation_time);

				bool locked;
			  reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_LOCKED, locked);

		    std::list<typename repository<number>::data_product> products;
				unsigned int num_products = reader->start_array(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DERIVED_PRODUCTS);
				for(unsigned int i = 0; i < num_products; i++)
					{
						reader->start_array_element();

				    std::string product;
						reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DERIVED_PRODUCT, product);
						// FIXME: Do some validation to check that this data product is really available for this output group?

				    std::string path;
						reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DERIVED_PATH, path);

				    std::string creation_time;
						reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DERIVED_CREATED, creation_time);

						typename repository<number>::data_product pd = typename repository<number>::data_product(product, creation_time, path);
						products.push_back(pd);

						reader->end_array_element();
					}
				reader->end_element(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DERIVED_PRODUCTS);

		    std::list<std::string> notes;
				unsigned int num_notes = reader->start_array(__CPP_TRANSPORT_NODE_OUTPUTGROUP_NOTES);
				for(unsigned int i = 0; i < num_notes; i++)
					{
						reader->start_array_element();

				    std::string note;
						reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_NOTE, note);
						notes.push_back(note);

						reader->end_array_element();
					}
				reader->end_element(__CPP_TRANSPORT_NODE_OUTPUTGROUP_NOTES);

		    std::list<std::string> tags;
				unsigned int num_tags = reader->start_array(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS);
				for(unsigned int i = 0; i < num_tags; i++)
					{
						reader->start_array_element();

				    std::string tag;
						reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAG, tag);
						tags.push_back(tag);

						reader->end_array_element();
					}
				reader->end_element(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS);

				return typename repository<number>::output_group(task, backend, this->root_path, data_root,
				                                                 data_container, creation_time, locked, notes, tags);
			}


    // Write a derived product specification
		template <typename number>
    void repository_unqlite<number>::write_derived_product(const derived_data::derived_product<number>& d)
	    {
		    // open a new transaction, if necessary. After this we can assume the database handles are live
		    this->begin_transaction();

		    // check whether a derived product with this name already exists
		    unsigned int count = unqlite_operations::query_count(this->derived_product_db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, d.get_name(), __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME);

		    if(count > 0)
			    {
		        std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_EXISTS << " '" << d.get_name() << "'";
				    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

		    // create a serialization writer, used to emit the serialized record to the database
		    unqlite_serialization_writer writer;

		    // commit derived product name
		    writer.write_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME, d.get_name());

		    // commit metadata block
		    writer.start_node(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_METADATA);

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

		    writer.write_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_METADATA_CREATED, boost::posix_time::to_simple_string(now));
		    writer.write_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_METADATA_EDITED, boost::posix_time::to_simple_string(now));

		    writer.write_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_METADATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

		    writer.end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_METADATA);

		    // commit derived-product block
		    writer.start_node(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_DETAILS);
		    d.serialize(writer);
		    writer.end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_DETAILS);

		    // insert this record in the database
        unqlite_operations::store(this->derived_product_db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, writer.get_contents());

		    // commit transaction
		    this->commit_transaction();
	    }


		//! Query a derived product specification
		template <typename number>
		derived_data::derived_product<number>* repository_unqlite<number>::lookup_derived_product(const std::string& product, typename instance_manager<number>::model_finder finder)
			{
				// open a new transaction, if necessary. After this we can assume the database handles are live
				this->begin_transaction();

				// get serialization_reader for the named product
				int product_id = 0;
				unqlite_serialization_reader* product_reader = this->get_derived_product_serialization_reader(product, product_id);

				task_finder tfinder = std::bind(&repository_unqlite<number>::lookup_task, this, std::placeholders::_1, std::placeholders::_2, finder);

		    derived_data::derived_product<number>* rval = derived_data::derived_product_helper::deserialize<number>(product, product_reader, tfinder);

				// commit transaction
				this->commit_transaction();

		    delete product_reader;

				if(rval == nullptr)
					{
				    std::ostringstream msg;
						msg << __CPP_TRANSPORT_REPO_MISSING_DERIVED_PRODUCT   << " '" << product << "'";
						throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					}

				return(rval);
			}


		template <typename number>
		typename repository<number>::derived_content_writer
		repository_unqlite<number>::new_output_task_output(output_task<number>* tk, const std::list<std::string>& tags, unsigned int worker)
			{
		    assert(tk != nullptr);
		    // FIXME: tags not currently enforced

		    // open a new transaction if necessary. After this we can assume the database handles are live
		    this->begin_transaction();

		    // check this task name exists in the database
		    unsigned int count = unqlite_operations::query_count(this->task_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION, tk->get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
		    if(count == 0)
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << tk->get_name() << "'";
		        throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

		    // insert a new output record, and return the corresponding derived_content_writer handle
		    typename repository<number>::derived_content_writer writer = this->insert_derived_content_output_group(worker, tk, tags);

		    // close database handles
		    this->commit_transaction();

		    return(writer);
			}


		template <typename number>
		typename repository<number>::derived_content_writer
		repository_unqlite<number>::insert_derived_content_output_group(unsigned int worker, output_task<number>* tk, const std::list<std::string>& tags)
			{
				// get serialization_reader for the named output task record
				int task_id = 0;
				task_type type;
				unqlite_serialization_reader* task_reader = this->get_task_serialization_reader(tk->get_name(), task_id, type);
				assert(type == output_record);

				// get current time
		    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

				// construct paths for output files and directories
				// We use the ISO form of the current time to label the output group directory.
				// This means the repository directory structure is human-readable if necessary.
		    std::string output_leaf = boost::posix_time::to_iso_string(now);

		    boost::filesystem::path output_path = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF) / tk->get_name() / output_leaf;
		    boost::filesystem::path log_path    = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
		    boost::filesystem::path task_path   = output_path / __CPP_TRANSPORT_REPO_TASKFILE_LEAF;
		    boost::filesystem::path temp_path   = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

				// create directories
		    boost::filesystem::create_directories(this->get_root_path() / output_path);
		    boost::filesystem::create_directories(this->get_root_path() / log_path);
		    boost::filesystem::create_directories(this->get_root_path() / temp_path);

				return typename repository<number>::derived_content_writer(this->get_root_path()/output_path, this->get_root_path()/log_path,
				                                                           this->get_root_path()/task_path, this->get_root_path()/temp_path, worker);
			}


    template <typename number>
    typename repository<number>::output_group repository_unqlite<number>::find_derived_product_output_group(const derived_data::derived_product<number>* product,
                                                                                                            const std::list<std::string>& tags)
	    {
		    assert(product != nullptr);

		    if(product == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_DERIVED_PRODUCT);

		    const task<number>* parent = product->get_parent_task();

		    // search for output groups associated with this task
        std::list< typename repository<number>::output_group> output = this->enumerate_integration_task_output(parent->get_name());

		    // remove items from the list which have mismatching tags
        output.remove_if( [&] (const typename repository<number>::output_group group) { return(group.check_tags(tags)); } );

		    if(output.empty())
			    {
		        std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS   << " '" << product->get_name() << "', "
					      << __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS_A << "task '" << parent->get_name() << "'";
				    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

		    return(output.front());
	    }


    // FACTORY FUNCTIONS TO BUILD A REPOSITORY


    template <typename number>
    repository <number>* repository_factory(const std::string& path, typename repository<number>::access_type mode=repository<number>::access_type::readwrite)
      {
        return new repository_unqlite<number>(path, mode);
      }


    template <typename number>
    repository<number>* repository_factory(const std::string& path, const repository_creation_key& key)
      {
        return new repository_unqlite<number>(path, key);
      }

	}   // namespace transport


#endif //__repository_unqlite_H_
