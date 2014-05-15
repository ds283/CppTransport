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
        virtual task<number>* query_task(const std::string& name, model_list<number>& mlist,
                                         typename instance_manager<number>::model_finder finder) override;

      protected:

        //! Extract an integration task from a serialization reader
        task<number>* query_integration_task(const std::string& name, task_type type,
                                             model_list<number>& mlist, typename instance_manager<number>::model_finder finder,
                                             unqlite_serialization_reader* reader);

        //! Extract an output task from a serialization reader
        task<number>* query_output_task(const std::string& name,
                                        model_list<number>& mlist, typename instance_manager<number>::model_finder finder,
                                        unqlite_serialization_reader* reader);


        // ADD AN OUTPUT-GROUP TO A TASK -- implements a 'repository' interface

      public:

        //! Insert a record for new twopf output in the task database, and set up paths to a suitable data container.
        //! Delegates insert_output to do the work.
        virtual typename repository<number>::integration_writer new_output_group(twopf_task<number>* tk, const std::list<std::string>& tags,
                                                                                 const std::string& backend, unsigned int worker) override;

        //! Insert a record for a new threepf output in the task database, and set up paths to a suitable data container.
        //! Delegates insert_output to do the work.
        virtual typename repository<number>::integration_writer new_output_group(threepf_task<number>* tk, const std::list<std::string>& tags,
                                                                                 const std::string& backend, unsigned int worker) override;

      protected:

        //! Insert an output record for a specified task, in a specified collection.
        typename repository<number>::integration_writer insert_output(const std::string& collection, const std::string& backend, unsigned int worker, task <number>* tk, const std::list<std::string>& tags);

        //! Allocate a new output-group serial number based on the list of used serial numbers from an output record
        unsigned int allocate_new_serial_number(unqlite_serialization_reader* reader);


        // PULL OUTPUT-GROUPS FROM A TASK -- implements a 'repository' interface

      public:

		    //! Enumerate the output available from a named task
        virtual std::list<typename repository<number>::output_group> enumerate_task_output(const std::string& name) override;

      protected:

        //! Extract an output-group data block.
        //! The supplied unqlite_serialization_reader should point to an unread output-group JSON object.
        typename repository<number>::output_group extract_output_group_data(const std::string& task, unqlite_serialization_reader* reader);


        // PUSH DERIVED-PRODUCT SPECIFICATIONS TO THE DATABASE -- implements a 'repository' interface

      public:

        //! Write a derived product specification
        virtual void write_derived_data(const derived_data::derived_product<number>& d) override;


		    // PULL DERIVED-PRODUCT SPECIFICATIONS FROM THE DATABASE -- implements a 'repository' interface

      public:

		    //! Enumerate the derived products associated with a named task
		    virtual std::list<typename repository<number>::derived_product> enumerate_task_derived_products(const std::string& name) override;

        //! Query a derived product specification
		    virtual derived_data::derived_product<number>* query_derived_data(integration_task<number>* tk, const std::string& product, model<number>* m) override;


      protected:

		    //! Extract a derived-product data block.
		    //! The supplied unqlite_serialization_reader should point to an unread derived-product JSON object.
		    typename repository<number>::derived_product extract_derived_product_data(const std::string& task, unqlite_serialization_reader* reader);

        //! Version of query_derived_data for rebinding and use when deserializing an output_task.
        //! Essentially the same as query_derived_data, but accepts a model_list<> as the last parameter and picks off the back component.
				derived_data::derived_product<number>* rebind_query_derived_data(integration_task<number>* tk, const std::string& product, model_list<number>& mlist)
	        {
		        if(mlist.size() == 0)
			        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_MODEL_LIST_REBIND_MISMATCH);

		        return(this->query_derived_data(tk, product, mlist.back()));
	        }


        // ADD DERIVED CONTENT FROM AN OUTPUT TASK -- implements a 'repository' interface

      public:

        //! Add derived content
        virtual typename repository<number>::derived_content_writer new_derived_content(output_task<number>* tk, const std::list<std::string>& tags,
                                                                                        unsigned int worker) override;


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

		    //! Convert a named database task into a serialization reader, returned as a pointer.
		    //! It's up to the calling function to destroy the pointer which is returned.
        unqlite_serialization_reader* deserialize_task(const std::string& name, int& unqlite_id, task_type& type);

		    //! Convert a named database package into a serialization reader, returned as a pointer.
		    //! It's up to the calling function to destroy the pointer which is returned.
		    unqlite_serialization_reader* deserialize_package(const std::string& name, int& unqlite_id);


        // INTERNAL DATA

      private:

        //! BOOST path to data containers (parent directory for the following containers)
        boost::filesystem::path containers_path;
        //! BOOST path to container for packages
        boost::filesystem::path packages_path;
        //! BOOST path to container for integrations
        boost::filesystem::path tasks_path;


        // UNQLITE DATABASE HANDLES

      private:

        unqlite* package_db;
        unqlite* task_db;

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
            msg << __CPP_TRANSPORT_REPO_MISSING_PACKAGES << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        // integrations database should be present inside the container directory
        tasks_path = containers_path / __CPP_TRANSPORT_CNTR_TASKS_LEAF;
        if(!boost::filesystem::is_regular_file(tasks_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_INTGRTNS << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        // open containers for reading (slave processes) or reading/writing (master process)
		    unsigned int m = (mode == repository<number>::access_type::readonly ? UNQLITE_OPEN_READONLY : UNQLITE_OPEN_READWRITE);

		    unqlite* pkg_db;
		    unqlite* tk_db;

        if(unqlite_open(&pkg_db, packages_path.c_str(), m) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_PKG << " '" << packages_path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        if(unqlite_open(&tk_db, tasks_path.c_str(), m) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_INTGN << " '" << tasks_path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // ensure default collections are present within each containers
        unqlite_operations::ensure_collection(pkg_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);
        unqlite_operations::ensure_collection(tk_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION);
        unqlite_operations::ensure_collection(tk_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION);
        unqlite_operations::ensure_collection(tk_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION);

		    // close repository until it is needed later
		    unqlite_close(pkg_db);
		    unqlite_close(tk_db);
      }


    // Create a named repository
    template <typename number>
    repository_unqlite<number>::repository_unqlite(const std::string& path, const repository_creation_key& key)
      : package_db(nullptr), task_db(nullptr), open_clients(0), repository<number>(path, repository<number>::access_type::readwrite)
      {
        // check whether root directory for the repository already exists -- it shouldn't
        if(boost::filesystem::exists(path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_ROOT_EXISTS << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        containers_path = this->get_root_path() / __CPP_TRANSPORT_REPO_REPOSITORY_LEAF;
        packages_path = containers_path / __CPP_TRANSPORT_CNTR_PACKAGES_LEAF;
        tasks_path = containers_path / __CPP_TRANSPORT_CNTR_TASKS_LEAF;

        // create directory structure
        boost::filesystem::create_directories(this->get_root_path());
        boost::filesystem::create_directories(containers_path);

        // create containers
        unqlite* pkg_db;
        unqlite* tk_db;

        if(unqlite_open(&pkg_db, packages_path.c_str(), UNQLITE_OPEN_CREATE) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_PKG << " '" << packages_path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        if(unqlite_open(&tk_db, tasks_path.c_str(), UNQLITE_OPEN_CREATE) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_INTGN << " '" << tasks_path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // create default collection within each containers
        unqlite_operations::create_collection(pkg_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);
        unqlite_operations::create_collection(tk_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION);
        unqlite_operations::create_collection(tk_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION);
        unqlite_operations::create_collection(tk_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION);

        // close repository until it is needed later
        unqlite_close(pkg_db);
        unqlite_close(tk_db);
      }


    // Destroy a repository_unqlite object, closing the associated repository
    template <typename number>
    repository_unqlite<number>::~repository_unqlite()
      {
		    // clean up open handles if they exist
        if(this->package_db != nullptr) unqlite_close(this->package_db);
        if(this->task_db != nullptr) unqlite_close(this->task_db);
      }


		// Open database handles.
		template <typename number>
		void repository_unqlite<number>::begin_transaction()
			{
				// If open_clients > 0, then the handles should be open; otherwise, they should be closed
				assert((this->open_clients > 0 && this->package_db != nullptr) || (this->open_clients == 0 && this->package_db == nullptr));
				assert((this->open_clients > 0 && this->task_db != nullptr)    || (this->open_clients == 0 && this->task_db == nullptr));

				if((this->open_clients == 0 && this->package_db != nullptr) ||
					 (this->open_clients == 0 && this->task_db != nullptr))
					throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_OPEN);

				if((this->open_clients > 0 && this->package_db == nullptr) ||
					 (this->open_clients > 0 && this->task_db == nullptr))
					throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_CLOSED);

				if(this->open_clients == 0)
					{
				    unsigned int m = (this->get_access_mode() == repository<number>::access_type::readonly ? UNQLITE_OPEN_READONLY : UNQLITE_OPEN_READWRITE);

				    if(unqlite_open(&(this->package_db), packages_path.c_str(), m) != UNQLITE_OK)
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_REPO_FAIL_PKG << " '" << this->packages_path << "'";
				        throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					    }

				    if(unqlite_open(&(this->task_db), tasks_path.c_str(), m) != UNQLITE_OK)
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_REPO_FAIL_INTGN << " '" << this->tasks_path << "'";
				        throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					    }

				    // ensure default collections are present within each containers
				    unqlite_operations::ensure_collection(this->package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);
				    unqlite_operations::ensure_collection(this->task_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION);
				    unqlite_operations::ensure_collection(this->task_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION);
				    unqlite_operations::ensure_collection(this->task_db, __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION);
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

		    if(this->open_clients <= 0) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_NOT_OPEN);

        if(this->package_db == nullptr || this->task_db == nullptr)
	        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_CLOSED);

		    this->open_clients--;

		    if(this->open_clients == 0)
			    {
		        unqlite_close(this->package_db), this->package_db = nullptr;
		        unqlite_close(this->task_db), this->task_db = nullptr;
			    }
	    }


    // Write a model/initial-conditions/parameters combination to the repository
    template <typename number>
    void repository_unqlite<number>::write_package(const initial_conditions<number>& ics, const model<number>* m)
	    {
        assert(m != nullptr);

        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

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
        writer.start_node(__CPP_TRANSPORT_NODE_PACKAGE_DATA);

        writer.write_value(__CPP_TRANSPORT_NODE_PKG_DATA_NAME, m->get_name());
        writer.write_value(__CPP_TRANSPORT_NODE_PKG_DATA_AUTHOR, m->get_author());
        writer.write_value(__CPP_TRANSPORT_NODE_PKG_DATA_TAG, m->get_tag());

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        writer.write_value(__CPP_TRANSPORT_NODE_PKG_DATA_CREATED, boost::posix_time::to_simple_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_PKG_DATA_EDITED, boost::posix_time::to_simple_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_PKG_DATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

        writer.end_element(__CPP_TRANSPORT_NODE_PACKAGE_DATA);

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
        if(count == 0)
	        {
            this->write_package(t.get_ics(), m);
	        }

        // create an unqlite_serialization_writer, used to emit the serialized record to the database
        unqlite_serialization_writer writer;

        // commit task name
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_NAME, t.get_name());

        // commit data block
        writer.start_node(__CPP_TRANSPORT_NODE_TASK_DATA);

        writer.write_value(__CPP_TRANSPORT_NODE_TASK_DATA_PACKAGE, t.get_ics().get_name());

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_DATA_CREATED, boost::posix_time::to_simple_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_DATA_EDITED, boost::posix_time::to_simple_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_TASK_DATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

        writer.end_element(__CPP_TRANSPORT_NODE_TASK_DATA);

        // commit task block
        writer.start_node(__CPP_TRANSPORT_NODE_TASK_INTGRTN);
        t.serialize(writer);
        writer.end_element(__CPP_TRANSPORT_NODE_TASK_INTGRTN);

        // write empty array of output blocks: will be populated when integrations are run
        writer.start_array(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT, true);
        writer.end_element(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);

		    // write empty array of derived data product specifications
		    writer.start_array(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_SPEC, true);
		    writer.end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_SPEC);

        // insert this record in the task database
        unqlite_operations::store(this->task_db, collection, writer.get_contents());

        // commit transaction
        this->commit_transaction();
	    }


    // Write an output task to the database
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

		    // create an unqlite_serialization_writer, used to emit the serialized record to the database
		    unqlite_serialization_writer writer;

		    // commit task name
		    writer.write_value(__CPP_TRANSPORT_NODE_TASK_NAME, t.get_name());

		    // commit data block
		    writer.start_node(__CPP_TRANSPORT_NODE_TASK_DATA);

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_DATA_CREATED, boost::posix_time::to_simple_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_DATA_EDITED, boost::posix_time::to_simple_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_TASK_DATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

        writer.end_element(__CPP_TRANSPORT_NODE_TASK_DATA);

		    // commit task block
		    writer.start_node(__CPP_TRANSPORT_NODE_TASK_OUTPUT);
		    t.serialize(writer);
		    writer.end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT);

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
    unqlite_serialization_reader* repository_unqlite<number>::deserialize_task(const std::string& name, int& unqlite_id, task_type& type)
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
    unqlite_serialization_reader* repository_unqlite<number>::deserialize_package(const std::string& name, int& unqlite_id)
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


    // Query the database for a named task, which is reconstructed and returned as a task<> object
    template <typename number>
    task<number>* repository_unqlite<number>::query_task(const std::string& name, model_list<number>& mlist,
                                                         typename instance_manager<number>::model_finder finder)
	    {
        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // get serialization_reader for the named task record
        int task_id = 0;
        task_type type;
        unqlite_serialization_reader* task_reader = this->deserialize_task(name, task_id, type);

        task<number>* rval = nullptr;

		    // work out how to build an appropriate task based on the record type
        switch(type)
	        {
            case twopf_record:
	            rval = this->query_integration_task(name, type, mlist, finder, task_reader);
            break;

            case threepf_record:
	            rval = this->query_integration_task(name, type, mlist, finder, task_reader);
            break;

            case output_record:
	            rval = this->query_output_task(name, mlist, finder, task_reader);
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
		task<number>* repository_unqlite<number>::query_integration_task(const std::string& name, task_type type,
		                                                                 model_list<number>& mlist, typename instance_manager<number>::model_finder finder,
		                                                                 unqlite_serialization_reader* task_reader)
			{
				assert(mlist.size() == 0);
				assert(task_reader != nullptr);

        // extract data:
        task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_DATA);

        // extract the model/initial-conditions/parameters package corresponding to this task
        std::string package_name;
        task_reader->read_value(__CPP_TRANSPORT_NODE_TASK_DATA_PACKAGE, package_name);

        task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_DATA);

        // get serialization_reader for the named package
        int package_id = 0;
        unqlite_serialization_reader* package_reader = this->deserialize_package(package_name, package_id);

        // extract UID for model
        std::string uid;
        package_reader->read_value(__CPP_TRANSPORT_NODE_PACKAGE_MODELUID, uid);

        // use the supplied finder to recover a model object for this UID
        model<number>* m = finder(uid);
				mlist.push_back(m);

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
        task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_INTGRTN);
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
		    task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_INTGRTN);

		    delete package_reader;

        return(rval);
      }


		// Extract output task
		template <typename number>
		task<number>* repository_unqlite<number>::query_output_task(const std::string& name,
		                                                            model_list<number>& mlist, typename instance_manager<number>::model_finder finder,
		                                                            unqlite_serialization_reader* reader)
			{
				assert(reader != nullptr);

		    // move the task reader to the task description block, and use it to reconstruct a task<>
				reader->start_node(__CPP_TRANSPORT_NODE_TASK_OUTPUT);
				reader->push_bookmark();

				// generate lookup functions for tasks and derived products

				//! query_task will automatically push the required models to mlist, so there is nothing to do
				typename std::reference_wrapper< model_list<number> > mlist_wrapper = mlist;

		    typename output_task_helper::task_finder<number>::type tfinder = std::bind(&repository_unqlite<number>::query_task, this, std::placeholders::_1, mlist_wrapper, finder);

		    typename output_task_helper::derived_product_finder<number>::type pfinder = std::bind(&repository_unqlite<number>::rebind_query_derived_data, this, std::placeholders::_1, std::placeholders::_2, mlist_wrapper);

				output_task<number> tk = output_task_helper::deserialize<number>(reader, name, tfinder, pfinder);
		    task<number>* rval = new output_task<number>(tk);

		    reader->pop_bookmark();
		    reader->end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT);

		    return(rval);
			}


    // Extract package database record as a JSON document
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
    template <typename number>
    std::string repository_unqlite<number>::json_task_document(const std::string& name)
      {
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
    template <typename number>
    typename repository<number>::integration_writer
    repository_unqlite<number>::new_output_group(twopf_task<number>* tk, const std::list<std::string>& tags,
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
        typename repository<number>::integration_writer ctr = this->insert_output(__CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, backend, worker, tk, tags);

		    // close database handles
        this->commit_transaction();

        return(ctr);
      }


    // Add output for a threepf task
    template <typename number>
    typename repository<number>::integration_writer
    repository_unqlite<number>::new_output_group(threepf_task<number>* tk, const std::list<std::string>& tags,
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
        typename repository<number>::integration_writer ctr = this->insert_output(__CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, backend, worker, tk, tags);

		    // commit transaction
        this->commit_transaction();

        return(ctr);
      }


    // Add output for a specified task.
		// We are allowed to assume that the database handles are already open, and that the task exists
		// in an appropriate collection
    template <typename number>
    typename repository<number>::integration_writer
    repository_unqlite<number>::insert_output(const std::string& collection, const std::string& backend,
                                              unsigned int worker, task <number>* tk, const std::list<std::string>& tags)
      {
        // get serialization_reader for the named task record
        int task_id = 0;
		    task_type type;
        unqlite_serialization_reader* task_reader = this->deserialize_task(tk->get_name(), task_id, type);

        // allocate a new serial number
		    // note that this will mark the __CPP_TRANSPORT_NODE_INTGRTN_OUTPUT node as read
		    task_reader->push_bookmark();
		    unsigned int serial_number = this->allocate_new_serial_number(task_reader);
		    task_reader->pop_bookmark();

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
        boost::filesystem::path derived_path = output_path / __CPP_TRANSPORT_REPO_DERIVED_DATA_LEAF;

		    // reset reader to that __CPP_TRANSPORT_NODE_TASK_DATA becomes available for reading again
		    task_reader->reset();

        // update task_reader with information about the new output group
		    task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_DATA);
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_TASK_DATA_EDITED, boost::posix_time::to_simple_string(now));   // insert overwrites previous value
		    task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_DATA);

		    task_reader->start_array(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);

        std::string array_serial = boost::lexical_cast<std::string>(serial_number);
		    task_reader->insert_node(array_serial);  // name of node doesn't matter; it is ignored in arrays, but should be unique
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_ID, serial_number);
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_BACKEND, backend);
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_PATH, output_path.string());
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DATABASE, sql_path.string());
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DERIVED, derived_path.string());
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

		    task_reader->insert_array(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DERIVED_PRODUCTS, true);
		    task_reader->insert_end_element(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DERIVED_PRODUCTS);

		    task_reader->insert_end_element(array_serial);

		    task_reader->end_element(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);

		    // update the task entry for this database
		    // that means: first, drop this existing record; then, store a copy of the updated one
        unqlite_operations::drop(this->task_db, collection, task_id);

        unqlite_operations::store(this->task_db, collection, task_reader->get_contents());

		    // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);
        boost::filesystem::create_directories(this->get_root_path() / derived_path);

		    delete task_reader;

		    return typename repository<number>::integration_writer(this->get_root_path()/output_path, this->get_root_path()/sql_path,
		                                                           this->get_root_path()/log_path, this->get_root_path()/task_path,
		                                                           this->get_root_path()/temp_path, serial_number, worker);
      }


		template <typename number>
    unsigned int repository_unqlite<number>::allocate_new_serial_number(unqlite_serialization_reader* reader)
	    {
				// populate list of serial numbers
        std::list<unsigned int> serial_numbers;

		    // start reading from integration output block
		    unsigned int entries = reader->start_array(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);
		    for(unsigned int i = 0; i < entries; i++)
			    {
				    reader->start_array_element();
				    unsigned int sn;
				    reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_ID, sn);
				    serial_numbers.push_back(sn);
				    reader->end_array_element();
			    }
				reader->end_element(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);

        unsigned int serial_number;

        for(serial_number = 0;
            serial_number < serial_numbers.size() && std::find(serial_numbers.begin(), serial_numbers.end(), serial_number) != serial_numbers.end();
            serial_number++)
	        ;

        if(serial_number > serial_numbers.size()) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NO_SERIALNO);

        return(serial_number);
	    }


		// Enumerate the output groups associated with some named task.
		// The output group descriptors returned by this function can be used to set up
		// derived_content_writer objects, which allow the corresponding integration output
		// to be extracted from the database
		template <typename number>
		std::list<typename repository<number>::output_group> repository_unqlite<number>::enumerate_task_output(const std::string& name)
			{
		    std::list<typename repository<number>::output_group> group_list;

				// open a new transaction, if necessary. After this we can assume the database handles are live
				this->begin_transaction();

				// get a serialization_reader for the named task record
				int task_id = 0;
				task_type type;
				unqlite_serialization_reader* task_reader = this->deserialize_task(name, task_id, type);

				// move the serialization reader to the output-group block
				unsigned int num_groups = task_reader->start_array(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);

				for(unsigned int i = 0; i < num_groups; i++)
					{
						task_reader->start_array_element();
						typename repository<number>::output_group group_data = this->extract_output_group_data(name, task_reader);
						group_list.push_back(group_data);
						task_reader->end_array_element();
					}

				task_reader->end_element(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);

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
				unsigned int serial_number;
				reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_ID, serial_number);

		    std::string backend;
				reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_BACKEND, backend);

		    std::string data_root;
				reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_PATH, data_root);

		    std::string data_container;
				reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DATABASE, data_container);

		    std::string derived_root;
				reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_DERIVED, derived_root);

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

						typename repository<number>::data_product pd = typename repository<number>::data_product(product, path, creation_time);
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

				return typename repository<number>::output_group(task, serial_number, backend, data_root,
				                                                 data_container, derived_root, creation_time,
				                                                 locked, products, notes, tags);
			}


    // Write a derived product specification
		template <typename number>
    void repository_unqlite<number>::write_derived_data(const derived_data::derived_product<number>& d)
	    {
		    // get name of task associated with this derived product
        const std::string& task_name = d.get_parent_task()->get_name();

				// enumerate derived products associated with this task
        std::list<typename repository<number>::derived_product> product_list = this->enumerate_task_derived_products(task_name);

		    // check whether a derived product with this name already exists
		    bool exists = false;
		    for(typename std::list<typename repository<number>::derived_product>::const_iterator t = product_list.begin(); !exists && t != product_list.end(); t++)
			    {
				    if((*t).get_name() == d.get_name()) exists = true;
			    }

		    if(exists)
			    {
		        std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_EXISTS << " '" << d.get_name() << "'";
				    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

		    // open a new transaction, if necessary. After this we can assume the database handles are live.
		    this->begin_transaction();

		    // get a serialization_reader for the named task record
		    int task_id = 0;
		    task_type type;
		    unqlite_serialization_reader* task_reader = this->deserialize_task(task_name, task_id, type);

				// navigate to derived-product block
		    task_reader->start_array(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_SPEC);

        // insert new derived product specification
        task_reader->insert_node("arrayelt");  // node name doesn't matter for an array
        task_reader->insert_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME, d.get_name());

        unqlite_serialization_writer writer;
        d.serialize(writer);
        task_reader->insert_writer_contents(writer);

        task_reader->insert_end_element("arrayelt");

		    task_reader->end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_SPEC);

		    // update the task entry for this database
		    // that means, first: drop the existing record; second: store a new copy of the updated one
        std::string collection;
		    if(type == twopf_record)        collection = __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION;
		    else if(type == threepf_record) collection = __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION;
		    else
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN << " '" << task_name << "'";
		        throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

        unqlite_operations::drop(this->task_db, collection, task_id);
        unqlite_operations::store(this->task_db, collection, task_reader->get_contents());

		    // commit transaction
		    this->commit_transaction();

		    delete task_reader;
	    }


		// Enumerate derived products associated with a task
		template <typename number>
		std::list<typename repository<number>::derived_product> repository_unqlite<number>::enumerate_task_derived_products(const std::string& name)
			{
		    std::list<typename repository<number>::derived_product> product_list;

				// open a new transaction, if necessary. After this we can assume the database handles are live
				this->begin_transaction();

				// get a serialization_reader for the named task record
				int task_id = 0;
				task_type type;
				unqlite_serialization_reader* task_reader = this->deserialize_task(name, task_id, type);

				if(type != twopf_record && type != threepf_record)
					{
				    std::ostringstream msg;
						msg << __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN << " '" << name << "'";
						throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					}

				// move the serialization reader to the derived-product block
				unsigned int num_products = task_reader->start_array(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_SPEC);

				for(unsigned int i = 0; i < num_products; i++)
					{
						task_reader->start_array_element();
						typename repository<number>::derived_product product_data = this->extract_derived_product_data(name, task_reader);
						task_reader->end_array_element();
					}

				task_reader->end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_SPEC);

				// commit transaction
				this->commit_transaction();

				delete task_reader;

				return(product_list);
			}


		// Extract a derived-product data block from a task record.
		template <typename number>
		typename repository<number>::derived_product repository_unqlite<number>::extract_derived_product_data(const std::string& task, unqlite_serialization_reader* reader)
			{
				assert(reader != nullptr);

		    std::string name;
				reader->read_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME, name);

				return typename repository<number>::derived_product(task, name);
			}


		//! Query a derived product specification
		template <typename number>
		derived_data::derived_product<number>* repository_unqlite<number>::query_derived_data(integration_task<number>* tk, const std::string& product, model<number>* m)
			{
				// open a new transaction, if necessary. After this we can assume the dataabse handles are live
				this->begin_transaction();

				// get a serialization_reader for the task record
				int task_id = 0;
				task_type type;
				unqlite_serialization_reader* task_reader = this->deserialize_task(tk->get_name(), task_id, type);

				// move the serialization reader to the derived-product specification block
				unsigned int num_products = task_reader->start_array(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_SPEC);

		    derived_data::derived_product<number>* rval = nullptr;

				for(unsigned int i = 0; rval == nullptr && i < num_products; i++)
					{
						task_reader->start_array_element();

				    std::string pname;
						task_reader->read_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME, pname);

						if(pname == product) rval = derived_data::derived_product_helper::deserialize<number>(pname, task_reader, tk, m);

						task_reader->end_array_element();
					}

				task_reader->end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_SPEC);

				// commit transaction
				this->commit_transaction();

		    delete task_reader;

				if(rval == nullptr)
					{
				    std::ostringstream msg;
						msg << __CPP_TRANSPORT_REPO_MISSING_DERIVED_PRODUCT   << " '" << product << "', "
								<< __CPP_TRANSPORT_REPO_MISSING_DERIVED_PRODUCT_A << " '" << tk->get_name() << "'";
						throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					}

				return(rval);
			}


		template <typename number>
		typename repository<number>::derived_content_writer
		repository_unqlite<number>::new_derived_content(output_task<number>* tk, const std::list<std::string>& tags, unsigned int worker)
			{
				assert(tk != nullptr);

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

				// close database handles
				this->commit_transaction();

				// get current time
		    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

				// construct paths for output
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
