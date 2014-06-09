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

    //! Error-reporting callback object
    typedef std::function<void(const std::string&)> repository_unqlite_error_callback;

    //! Warning callback object
    typedef std::function<void(const std::string&)> repository_unqlite_warning_callback;

    // class 'repository_unqlite' implements the 'repository' interface using
    // UnQLite as the database backend.
    // It replaces the earlier DbXml-based repository implementation
    template <typename number>
    class repository_unqlite: public repository<number>
      {

      protected:

		    //! Internal record-type flag
        typedef enum { integration_record, output_record } task_type;

      private:

        class default_error_handler
          {
          public:
            void operator() (const std::string& msg) { std::cerr << msg << std::endl; }
          };

        class default_warning_handler
          {
          public:
            void operator() (const std::string& msg) {std::cout << msg << std::endl; }
          };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Open a repository with a specific pathname
        //! and specified warning and error handlers
        repository_unqlite(const std::string& path,
                           typename repository<number>::access_type mode = repository<number>::access_type::readwrite,
                           repository_unqlite_error_callback e=default_error_handler(),
                           repository_unqlite_warning_callback w=default_warning_handler());

        //! Create a repository with a specific pathname
        repository_unqlite(const std::string& path, const repository_creation_key& key);

        //! Close a repository, including any corresponding containers
        ~repository_unqlite();


        // PUSH TASKS TO THE REPOSITORY DATABASE -- implements a 'repository' interface

      public:

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        virtual void write_package(const initial_conditions<number>& ics, const model<number>* m) override;

        //! Write an integration task to the database.
        //! Delegates write_integration_task() to do the work.
        virtual void write_task(const integration_task<number>& tk, const model<number>* m) override;

        //! Write an output task to the database
        virtual void write_task(const output_task<number>& tk) override;


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
        //! Delegates create_integration_writer to do the work.
        virtual typename repository<number>::integration_writer new_integration_task_output(twopf_task<number>* tk, const std::list<std::string>& tags,
                                                                                            model<number>* m, unsigned int worker) override;

        //! Insert a record for a new threepf output in the task database, and set up paths to a suitable data container.
        //! Delegates create_integration_writer to do the work.
        virtual typename repository<number>::integration_writer new_integration_task_output(threepf_task<number>* tk, const std::list<std::string>& tags,
                                                                                            model<number>* m, unsigned int worker) override;

      protected:

        //! Create an integration writer and its associated environment.
        typename repository<number>::integration_writer create_integration_writer(model<number>* m,
                                                                                  unsigned int worker, integration_task<number>* tk,
                                                                                  const std::list<std::string>& tags);

		    //! Commit the output of an integration writer to the database. Called as a callback
		    void close_integration_writer(typename repository<number>::integration_writer& writer, model<number>* m);


        // PULL OUTPUT-GROUPS FROM A TASK -- implements a 'repository' interface

      public:

		    //! Enumerate the output available from a named task
        virtual std::list<typename repository<number>::template output_group< typename repository<number>::integration_payload > >
          enumerate_integration_task_output(const std::string& name) override;


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
        virtual typename repository<number>::derived_content_writer
          new_output_task_output(output_task<number> *tk, const std::list<std::string> &tags,
                                 unsigned int worker) override;

        //! Lookup an output group for a task, given a set of tags
        virtual typename repository<number>::template output_group<typename repository<number>::integration_payload>
          find_integration_task_output_group(const integration_task<number> *tk,
                                             const std::list<std::string> &tags) override;

      protected:

		    //! Insert an output record for a named task
        typename repository<number>::derived_content_writer
          insert_derived_content_output_group(unsigned int worker, output_task<number> *tk, const std::list<std::string> &tags);


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

        //! Error handler
        repository_unqlite_error_callback error;

        //! Warning handler
        repository_unqlite_warning_callback warning;

        //! BOOST path to database
        boost::filesystem::path db_path;


        // UNQLITE DATABASE HANDLES

      private:

        //! UnQLite database connexion
        unqlite* db;

        //! Number of open clients on the database, used for keep track of when the
        //! database connexion can be closed
		    unsigned int open_clients;

	    };


    // IMPLEMENTATION


    // Create a repository object associated with a pathname
    template <typename number>
    repository_unqlite<number>::repository_unqlite(const std::string& path, typename repository<number>::access_type mode,
                                                   repository_unqlite_error_callback e, repository_unqlite_warning_callback w)
      : db(nullptr), open_clients(0), repository<number>(path, mode), error(e), warning(w)
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

        // open database connexions for reading (slave processes) or reading/writing (master process)
		    unsigned int m = (mode == repository<number>::access_type::readonly ? UNQLITE_OPEN_READONLY : UNQLITE_OPEN_READWRITE);

        if(unqlite_open(&db, db_path.c_str(), m) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_DATABASE_OPEN << " " << db_path;
            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
          }

        // ensure default collections are present
        unqlite_operations::ensure_collection(db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);

        unqlite_operations::ensure_collection(db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION);
        unqlite_operations::ensure_collection(db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION);

        unqlite_operations::ensure_collection(db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION);

        unqlite_operations::ensure_collection(db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION);

		    // close database connexions until they are needed later
		    unqlite_close(db);
        db = nullptr;
      }


    // Create a named repository
    template <typename number>
    repository_unqlite<number>::repository_unqlite(const std::string& path, const repository_creation_key& key)
      : db(nullptr), open_clients(0), repository<number>(path, repository<number>::access_type::readwrite),
        error(typename repository_unqlite<number>::default_error_handler()),
        warning(typename repository_unqlite<number>::default_warning_handler())
      {
        // check whether root directory for the repository already exists -- it shouldn't
        if(boost::filesystem::exists(path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_ROOT_EXISTS << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        db_path = this->get_root_path() / __CPP_TRANSPORT_REPO_REPOSITORY_LEAF;

        // create directory structure
        boost::filesystem::create_directories(this->get_root_path());

        // create container
        if(unqlite_open(&db, db_path.c_str(), UNQLITE_OPEN_CREATE) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_DATABASE_OPEN << " " << db_path;
            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
          }

        // create default collection within each containers
        unqlite_operations::create_collection(db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);

        unqlite_operations::create_collection(db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION);
        unqlite_operations::create_collection(db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION);

        unqlite_operations::create_collection(db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION);

        unqlite_operations::create_collection(db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION);

        // close repository until it is needed later
		    unqlite_close(db);
        db = nullptr;
      }


    // Destroy a repository_unqlite object, closing the associated repository
    template <typename number>
    repository_unqlite<number>::~repository_unqlite()
      {
		    // clean up open handles if they exist
        if(this->db != nullptr) unqlite_close(this->db);
      }


		// Open database handles.
		template <typename number>
		void repository_unqlite<number>::begin_transaction()
			{
				// If open_clients > 0, then the handles should be open; otherwise, they should be closed
				assert((this->open_clients > 0 && this->db != nullptr) || (this->open_clients == 0 && this->db == nullptr));

				if(this->open_clients == 0 && this->db != nullptr)
					throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_OPEN);

				if(this->open_clients > 0 && this->db == nullptr)
					throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_CLOSED);

				if(this->open_clients == 0)
					{
				    unsigned int m = (this->get_access_mode() == repository<number>::access_type::readonly ? UNQLITE_OPEN_READONLY : UNQLITE_OPEN_READWRITE);

				    if(unqlite_open(&(this->db), this->db_path.c_str(), m) != UNQLITE_OK)
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_REPO_FAIL_DATABASE_OPEN << " " << this->db_path;
				        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
					    }

				    // ensure default collections are present within each containers
//				    unqlite_operations::ensure_collection(this->db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);
//
//				    unqlite_operations::ensure_collection(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION);
//				    unqlite_operations::ensure_collection(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION);
//
//				    unqlite_operations::ensure_collection(this->db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION);
//
//            unqlite_operations::ensure_collection(this->db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION);
					}

				this->open_clients++;
			}


    // Close database handles
    template <typename number>
    void repository_unqlite<number>::commit_transaction()
	    {
		    // open_clients should be > 0 and the handles should be open
		    assert(this->open_clients > 0);
		    assert(this->db != nullptr);

		    if(this->open_clients <= 0) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_NOT_OPEN);
        if(this->db == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_DATABASES_CLOSED);

		    this->open_clients--;

		    if(this->open_clients == 0)
			    {
		        unqlite_close(this->db), this->db = nullptr;
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
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, ics.get_name(), __CPP_TRANSPORT_NODE_PACKAGE_NAME);
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
        writer.write_value(__CPP_TRANSPORT_NODE_PKG_METADATA_CREATED, boost::posix_time::to_iso_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_PKG_METADATA_EDITED, boost::posix_time::to_iso_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_PKG_METADATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

        writer.end_element(__CPP_TRANSPORT_NODE_PACKAGE_METADATA);

        // commit initial conditions
        writer.start_node(__CPP_TRANSPORT_NODE_PACKAGE_ICS);
        ics.serialize(writer);
        writer.end_element(__CPP_TRANSPORT_NODE_PACKAGE_ICS);

        // insert this record in the package database
        unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, writer.get_contents());

        // commit transaction
        this->commit_transaction();
	    }


    // Write a task to the repository
		// DATABASE ENTRY POINT
    template <typename number>
    void repository_unqlite<number>::write_task(const integration_task<number>& tk, const model<number>* m)
	    {
        assert(m != nullptr);

        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        // check if a task with this name already exists
        unsigned int integration_count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, tk.get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
		    unsigned int output_count      = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, tk.get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);

		    unsigned int total = integration_count + output_count;

        if(total > 0)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_TASK_EXISTS << " '" << tk.get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

        // check whether the specified model+initial conditions package is already in the database; if not, insert it
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, tk.get_ics().get_name(), __CPP_TRANSPORT_NODE_PACKAGE_NAME);
        if(count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_AUTOCOMMIT_INTEGRATION_A << " '" << tk.get_name() << "' "
                << __CPP_TRANSPORT_REPO_AUTOCOMMIT_INTEGRATION_B << " '" << tk.get_ics().get_name() << "'";
            this->warning(msg.str());
            this->write_package(tk.get_ics(), m);
          }

        // create an unqlite_serialization_writer, used to emit the serialized record to the database
        unqlite_serialization_writer writer;

        // commit task name
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_NAME, tk.get_name());

        // commit metadata block
        writer.start_node(__CPP_TRANSPORT_NODE_TASK_METADATA);

        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_PACKAGE, tk.get_ics().get_name());

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_CREATED, boost::posix_time::to_iso_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_EDITED, boost::posix_time::to_iso_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

        writer.end_element(__CPP_TRANSPORT_NODE_TASK_METADATA);

        // commit task block
        writer.start_node(__CPP_TRANSPORT_NODE_TASK_INTEGRATION_DETAILS);
        tk.serialize(writer);
        writer.end_element(__CPP_TRANSPORT_NODE_TASK_INTEGRATION_DETAILS);

        // write empty array of output blocks: will be populated when integrations are run
        writer.start_array(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS, true);
        writer.end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS);

        // insert this record in the task database
        unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, writer.get_contents());

        // commit transaction
        this->commit_transaction();
	    }


    // Write an output task to the database
		// DATABASE ENTRY POINT
		template <typename number>
		void repository_unqlite<number>::write_task(const output_task<number>& tk)
	    {
		    // open a new transaction, if necessary. After this we can assume the database handles are live
		    this->begin_transaction();

        // check if a task with this name already exists
        unsigned int integration_count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, tk.get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
        unsigned int output_count      = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, tk.get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);

        unsigned int total = integration_count + output_count;

        if(total > 0)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_TASK_EXISTS << " '" << tk.get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }

		    // An output task consists of a number of derived product specifications.
		    // Each derived product may utilize a number of content providers, each of which
		    // may be associated with a different task.
		    // At the top of the hierarchy, each derived product name should be unique.

        // check whether derived products on which this task depends have already been committed to the database
        const typename std::vector< output_task_element<number> > elements = tk.get_elements();
        for(typename std::vector< output_task_element<number> >::const_iterator t = elements.begin(); t != elements.end(); t++)
          {
            derived_data::derived_product<number>* product = (*t).get_product();
            unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, product->get_name(), __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME);
            if(count == 0)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_AUTOCOMMIT_OUTPUT_A << " '" << tk.get_name() << "' "
                    << __CPP_TRANSPORT_REPO_AUTOCOMMIT_OUTPUT_B << " '" << product->get_name() << "'";
                this->warning(msg.str());
                this->write_derived_product(*product);
              }
          }

		    // create an unqlite_serialization_writer, used to emit the serialized record to the database
		    unqlite_serialization_writer writer;

		    // commit task name
		    writer.write_value(__CPP_TRANSPORT_NODE_TASK_NAME, tk.get_name());

		    // commit data block
		    writer.start_node(__CPP_TRANSPORT_NODE_TASK_METADATA);

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_CREATED, boost::posix_time::to_iso_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_EDITED, boost::posix_time::to_iso_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_TASK_METADATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

        writer.end_element(__CPP_TRANSPORT_NODE_TASK_METADATA);

		    // commit task block
		    writer.start_node(__CPP_TRANSPORT_NODE_TASK_OUTPUT_DETAILS);
		    tk.serialize(writer);
		    writer.end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT_DETAILS);

        // insert this record in the task database
        unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, writer.get_contents());

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
        unqlite_vm* vm_int     = nullptr;
		    unqlite_vm* vm_output  = nullptr;

        unqlite_value* int_recs   = unqlite_operations::query(this->db, vm_int, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, name, __CPP_TRANSPORT_NODE_TASK_NAME);
		    unqlite_value* output_recs  = unqlite_operations::query(this->db, vm_output, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, name, __CPP_TRANSPORT_NODE_TASK_NAME);

        if(int_recs == nullptr || output_recs == nullptr ||
	         !unqlite_value_is_json_array(int_recs) || !unqlite_value_is_json_array(output_recs))
	        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

        unsigned int int_count     = static_cast<unsigned int>(unqlite_array_count(int_recs));
		    unsigned int output_count  = static_cast<unsigned int>(unqlite_array_count(output_recs));

		    unsigned int total = int_count + output_count;

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

        if(int_count == 1) { recs = int_recs;     vm = vm_int;     type = integration_record; }
		    else               { recs = output_recs;  vm = vm_output;  type = output_record;      }

        if(unqlite_array_count(recs) != 1)   // shouldn't happen because checked for above
	        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_TASK_EXTRACT_FAIL);

        array_extraction_data data;
        data.cmp     = 0;
        data.str_cmp = boost::lexical_cast<std::string>(data.cmp);
        data.id      = -1;
        data.reader  = nullptr;

        unqlite_array_walk(recs, &array_extract, &data);

        unqlite_vm_release(vm_int);
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

        unqlite_value* recs = unqlite_operations::query(this->db, vm, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, name, __CPP_TRANSPORT_NODE_PACKAGE_NAME);

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

		    unqlite_value* recs = unqlite_operations::query(this->db, vm, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION,
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
            case integration_record:
	            rval = this->lookup_integration_task(name, type, m, finder, task_reader);
              break;

            case output_record:
	            rval = this->lookup_output_task(name, finder, task_reader);
		          m = nullptr;  // output tasks aren't associated with a unique model
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
        initial_conditions<number> ics = initial_conditions<number>(package_name, package_reader,
                                                                    m->get_param_names(), m->get_state_names(),
                                                                    p_validator, ics_validator);
        package_reader->pop_bookmark();
        package_reader->end_element(__CPP_TRANSPORT_NODE_PACKAGE_ICS);

        // reset read data from the task record, and use it to reconstruct a task<> object
        task_reader->reset();

        // move the task reader to the task description block, and use it to reconstruct a task<>
        task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_INTEGRATION_DETAILS);
        task_reader->push_bookmark();
				task<number>* rval = integration_task_helper::deserialize<number>(name, task_reader, ics);
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

		    typename output_task<number>::derived_product_finder pfinder = std::bind(&repository_unqlite<number>::lookup_derived_product, this, std::placeholders::_1, finder);

		    task<number>* rval = output_task_helper::deserialize<number>(name, reader, pfinder);

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
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, name, __CPP_TRANSPORT_NODE_PACKAGE_NAME);

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
        std::string document = unqlite_operations::extract_json(this->db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, name, __CPP_TRANSPORT_NODE_PACKAGE_NAME);

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
        unsigned int integration_count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, name, __CPP_TRANSPORT_NODE_TASK_NAME);
		    unsigned int output_count      = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, name, __CPP_TRANSPORT_NODE_TASK_NAME);

		    unsigned int total = integration_count + output_count;

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
            if(integration_count == 1) collection = __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION;
		        else                       collection = __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION;

            rval = unqlite_operations::extract_json(this->db, collection, name, __CPP_TRANSPORT_NODE_TASK_NAME);
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
                                                            model<number>* m, unsigned int worker)
      {
        assert(tk != nullptr);
        assert(m != nullptr);

        if(tk == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_TASK);
        if(tk == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check this task name exists in the database
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, tk->get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
        if(count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // insert a new output record, and return the corresponding integration_writer handle
        typename repository<number>::integration_writer writer = this->create_integration_writer(m, worker, tk, tags);

		    // close database handles
        this->commit_transaction();

        return(writer);
      }


    // Add output for a threepf task
		// DATABASE ENTRY POINT
    template <typename number>
    typename repository<number>::integration_writer
    repository_unqlite<number>::new_integration_task_output(threepf_task<number>* tk, const std::list<std::string>& tags,
                                                            model<number>* m, unsigned int worker)
      {
        assert(tk != nullptr);
        assert(m != nullptr);

        if(tk == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_TASK);
        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check this task name exists in the database
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, tk->get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
        if(count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // insert a new output record, and return the corresponding integration_writer handle
        typename repository<number>::integration_writer writer = this->create_integration_writer(m, worker, tk, tags);

		    // commit transaction
        this->commit_transaction();

        return(writer);
      }


    // Add output for a specified task.
		// We are allowed to assume that the database handles are already open, and that the task exists
		// in an appropriate collection
    template <typename number>
    typename repository<number>::integration_writer
    repository_unqlite<number>::create_integration_writer(model<number>* m,
                                                          unsigned int worker, integration_task<number>* tk, const std::list<std::string>& tags)
      {
		    // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

		    // construct paths for the various output files and directories.
		    // We use the ISO form of the current time to label the output group directory.
		    // This means the repository directory structure will be human-readable if necessary.
        std::string output_leaf = boost::posix_time::to_iso_string(now);

        boost::filesystem::path output_path  = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF) / tk->get_name() / output_leaf;
        boost::filesystem::path sql_path     = output_path / __CPP_TRANSPORT_REPO_DATABASE_LEAF;

        // temporary stuff, location not recorded in the database
        boost::filesystem::path log_path     = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path task_path    = output_path / __CPP_TRANSPORT_REPO_TASKFILE_LEAF;
        boost::filesystem::path temp_path    = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

		    // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

		    // integration_writer constructor takes copies of tk and tags because we can't be sure that they're long-lived objects;
		    // model is ok because mode instances are handled by the instance_manager
		    return typename repository<number>::integration_writer(tk, tags,
		                                                           std::bind(&repository_unqlite<number>::close_integration_writer, this, std::placeholders::_1, m),
		                                                           this->get_root_path(),
		                                                           output_path, sql_path, log_path, task_path, temp_path,
		                                                           worker, m->supports_per_configuration_statistics());
      }


		template <typename number>
		void repository_unqlite<number>::close_integration_writer(typename repository<number>::integration_writer& writer, model<number>* m)
			{
				assert(m != nullptr);

				integration_task<number>* tk = writer.get_task();
				const std::list<std::string>& tags = writer.get_tags();
		    assert(tk != nullptr);

		    // open a new transaction, if necessary. After this we can assume the database handles are live
		    this->begin_transaction();

				// get serialization_reader for the named task record
		    int task_id = 0;
		    task_type type;
		    unqlite_serialization_reader* task_reader = this->get_task_serialization_reader(tk->get_name(), task_id, type);
		    assert(type == integration_record);

		    // get current time
		    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

		    // create and serialize an empty output group
		    std::list<std::string> notes;
				if(!m->supports_per_configuration_statistics())
					{
				    std::ostringstream msg;
						msg << __CPP_TRANSPORT_REPO_NOTE_NO_STATISTICS << " '" << m->get_backend() << "'";
						notes.push_back(msg.str());
					}
				// FIXME: insert note about integrability for 3pf tasks, __CPP_TRANSPORT_REPO_NOTE_NO_INTEGRATION
		    typename repository<number>::template output_group<typename repository<number>::integration_payload> group(tk->get_name(), this->get_root_path(),
		                                                                                                               writer.get_relative_output_path(),
		                                                                                                               now, false, std::list<std::string>(), tags);
		    group.get_payload().set_backend(m->get_backend());
		    group.get_payload().set_container_path(writer.get_relative_container_path());
				group.get_payload().set_timing_metadata(writer.get_timing_metadata());
		    unqlite_serialization_writer swriter;
		    group.serialize(swriter);

		    // write new output group to the database
		    unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, swriter.get_contents());

		    // refresh edit time and update the task entry for this database
		    // that means: first, drop this existing record; then, store a copy of the updated one
		    task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_METADATA);
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_TASK_METADATA_EDITED, boost::posix_time::to_iso_string(now));   // insert overwrites previous value
		    task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_METADATA);
		    unqlite_operations::drop(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, task_id);
		    unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, task_reader->get_contents());

		    delete task_reader;

		    // commit transaction
		    this->commit_transaction();
			}


    namespace
      {

        template <typename number>
        int array_extract_output_groups(unqlite_value* key, unqlite_value* value, void* handle)
          {
            assert(key != nullptr);
            assert(value != nullptr);
            assert(handle != nullptr);

            std::list< typename repository<number>::template output_group< typename repository<number>::integration_payload > >* list =
              static_cast< std::list< typename repository<number>::template output_group< typename repository<number>::integration_payload > >* >(handle);

            unqlite_serialization_reader reader(value);

            list->push_back( typename repository<number>::template output_group< typename repository<number>::integration_payload >(&reader) );

            return(UNQLITE_OK);
          }

      }   // unnamed namespace


		// Enumerate the output groups associated with some named task.
		// The output group descriptors returned by this function can be used to set up
		// derived_content_writer objects, which allow the corresponding integration output
		// to be extracted from the database
		template <typename number>
		std::list< typename repository<number>::template output_group< typename repository<number>::integration_payload > >
    repository_unqlite<number>::enumerate_integration_task_output(const std::string& name)
			{
		    std::list<typename repository<number>::template output_group< typename repository<number>::integration_payload > > group_list;

				// open a new transaction, if necessary. After this we can assume the database handles are live
				this->begin_transaction();

        // get a serialization_reader for the named task record, and ensure it is of integration type
        int task_id = 0;
        task_type type;
        unqlite_serialization_reader* task_reader = this->get_task_serialization_reader(name, task_id, type);

        if(type != integration_record)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // enumerate all output groups associated with this task
        unqlite_vm* vm = nullptr;
        unqlite_value* groups = unqlite_operations::query(this->db, vm, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, name, __CPP_TRANSPORT_NODE_OUTPUTGROUP_TASK_NAME);

        if(groups == nullptr || !unqlite_value_is_json_array(groups))
          throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

        std::list< typename repository<number>::template output_group< typename repository<number>::integration_payload > > list;
        unqlite_array_walk(groups, &array_extract_output_groups<number>, &list);

        unqlite_vm_release(vm);

				// commit transaction
				this->commit_transaction();

				delete task_reader;

				// sort the output groups into descending order of creation date, so the first element in the
				// list is the most recent data group.
				// This is usually what would be required.
				list.sort(&output_group_helper::comparator<number, typename repository<number>::integration_payload>);

				return(list);
			}


    // Write a derived product specification
		template <typename number>
    void repository_unqlite<number>::write_derived_product(const derived_data::derived_product<number>& d)
	    {
		    // open a new transaction, if necessary. After this we can assume the database handles are live
		    this->begin_transaction();

		    // check whether a derived product with this name already exists
		    unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, d.get_name(), __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME);

		    if(count > 0)
			    {
		        std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_EXISTS << " '" << d.get_name() << "'";
				    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

        // check whether all tasks on which this derived product depends are already in the database
        typename std::vector< typename std::pair< integration_task<number>*, model<number>* > > task_list;
        d.get_task_list(task_list);
        for(typename std::vector< typename std::pair< integration_task<number>*, model<number>* > >::iterator t = task_list.begin(); t != task_list.end(); t++)
          {
            unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, (*t).first->get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
            if(count == 0)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_AUTOCOMMIT_PRODUCT_A << " '" << d.get_name() << "' "
                    << __CPP_TRANSPORT_REPO_AUTOCOMMIT_PRODUCT_B << " '" << (*t).first->get_name() << "'";
                this->warning(msg.str());
                this->write_task(*((*t).first), (*t).second);
              }
          }

		    // create a serialization writer, used to emit the serialized record to the database
		    unqlite_serialization_writer writer;

		    // commit derived product name
		    writer.write_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME, d.get_name());

		    // commit metadata block
		    writer.start_node(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_METADATA);

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

		    writer.write_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_METADATA_CREATED, boost::posix_time::to_iso_string(now));
		    writer.write_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_METADATA_EDITED, boost::posix_time::to_iso_string(now));

		    writer.write_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_METADATA_RUNTIMEAPI, static_cast<unsigned int>(__CPP_TRANSPORT_RUNTIME_API_VERSION));

		    writer.end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_METADATA);

		    // commit derived-product block
		    writer.start_node(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_DETAILS);
		    d.serialize(writer);
		    writer.end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_DETAILS);

		    // insert this record in the database
        unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, writer.get_contents());

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

				typename repository<number>::task_finder tfinder = std::bind(&repository_unqlite<number>::lookup_task, this, std::placeholders::_1, std::placeholders::_2, finder);

        // move reader to details block
        product_reader->start_node(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_DETAILS);
		    derived_data::derived_product<number>* rval = derived_data::derived_product_helper::deserialize<number>(product, product_reader, tfinder);
        product_reader->end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_DETAILS);

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
		    unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, tk->get_name(), __CPP_TRANSPORT_NODE_TASK_NAME);
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

        // temporary stuff, location not recorded in the database
		    boost::filesystem::path log_path    = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
		    boost::filesystem::path task_path   = output_path / __CPP_TRANSPORT_REPO_TASKFILE_LEAF;
		    boost::filesystem::path temp_path   = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

				// create directories
		    boost::filesystem::create_directories(this->get_root_path() / output_path);
		    boost::filesystem::create_directories(this->get_root_path() / log_path);
		    boost::filesystem::create_directories(this->get_root_path() / temp_path);

        // create and serialize an empty output group
        typename repository<number>::template output_group<typename repository<number>::output_payload> group(tk->get_name(), this->get_root_path(), output_path,
                                                                                                              now, false, std::list<std::string>(), tags);

        unqlite_serialization_writer writer;
        group.serialize(writer);

        // write new output group to the database
        unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, writer.get_contents());

        // refresh edit time and update the task entry for this database
        // that means: first, drop this existing record; then, store a copy of the updated one
        task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_METADATA);
        task_reader->insert_value(__CPP_TRANSPORT_NODE_TASK_METADATA_EDITED, boost::posix_time::to_iso_string(now));   // insert overwrites previous value
        task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_METADATA);
        unqlite_operations::drop(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, task_id);
        unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, task_reader->get_contents());

        delete task_reader;

				return typename repository<number>::derived_content_writer(this->get_root_path()/output_path, this->get_root_path()/log_path,
				                                                           this->get_root_path()/task_path, this->get_root_path()/temp_path, worker);
			}


    template<typename number>
    typename repository<number>::template output_group<typename repository<number>::integration_payload>
    repository_unqlite<number>::find_integration_task_output_group(const integration_task<number> *tk, const std::list<std::string> &tags)
	    {
		    assert(tk != nullptr);

		    if(tk == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_TASK);

		    // search for output groups associated with this task
        std::list< typename repository<number>::template output_group< typename repository<number>::integration_payload > >
          output = this->enumerate_integration_task_output(tk->get_name());

		    // remove items from the list which have mismatching tags
        output.remove_if( [&] (const typename repository<number>::template output_group< typename repository<number>::integration_payload >& group) { return(group.check_tags(tags)); } );

		    if(output.empty())
			    {
		        std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS   << " '" << tk->get_name() << "'";
				    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

		    return(output.front());
	    }


    // FACTORY FUNCTIONS TO BUILD A REPOSITORY


    template <typename number>
    repository <number>* repository_factory(const std::string& path,
                                            typename repository<number>::access_type mode=repository<number>::access_type::readwrite)
      {
        return new repository_unqlite<number>(path, mode);
      }


    template <typename number>
    repository<number>* repository_factory(const std::string& path,
                                           typename repository<number>::access_type mode,
                                           repository_unqlite_error_callback e,
                                           repository_unqlite_warning_callback w)
      {
        return new repository_unqlite<number>(path, mode, e, w);
      }


    template <typename number>
    repository<number>* repository_factory(const std::string& path, const repository_creation_key& key)
      {
        return new repository_unqlite<number>(path, key);
      }

	}   // namespace transport


#endif //__repository_unqlite_H_
