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

#include "transport-runtime-api/manager/json_repository_interface.h"

#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "transport-runtime-api/derived-products/data_products.h"

#include "transport-runtime-api/unqlite/unqlite_data.h"
#include "transport-runtime-api/unqlite/unqlite_serializable.h"
#include "transport-runtime-api/unqlite/unqlite_operations.h"

#include "boost/filesystem/operations.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/scoped_ptr.hpp"


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


    //! Class 'repository_unqlite' implements the 'repository' interface using
    //! UnQLite as the database backend.
    //! It also implements the 'json_interface_repository' interface.
    //! It replaces an earlier DbXml-based repository implementation
    template <typename number>
    class repository_unqlite : public json_interface_repository<number>
      {

      public:

        //! Error-reporting callback object
        typedef std::function<void(const std::string&)> error_callback;

        //! Warning callback object
        typedef std::function<void(const std::string&)> warning_callback;

        //! Message callback object
        typedef std::function<void(const std::string&)> message_callback;


      public:

        class array_extraction_data
          {
          public:
            // input data
            unsigned int cmp;
            std::string str_cmp;

            // output data
            int id;
            unqlite_serialization_reader* reader;
          };

        typedef std::function<int(unqlite_value*, unqlite_value*)> array_extraction_functor;


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


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Open a repository with a specific pathname
        //! and specified warning and error handlers
        repository_unqlite(const std::string& path,
                           typename repository<number>::access_type mode = repository<number>::access_type::readwrite,
                           error_callback e = default_error_handler(),
                           warning_callback w = default_warning_handler(),
                           message_callback m = default_message_handler());

        //! Create a repository with a specific pathname
        repository_unqlite(const std::string& path, const repository_creation_key& key);

        //! Close a repository, including any corresponding containers
        ~repository_unqlite();


        // TRANSACTION MANAGEMENT

      protected:

        //! Notionally begin a transaction on the database.
        //! Currently this is implemented by opening the database at the beginning of an 'atomic' transaction,
        //! and then closing it at the end.
        //! As UnQLite develops in sophistication it may be possible to replace this by an internal transaction manager.
        void begin_transaction();

        //! Notionally commit a transaction to the database.
        //! Currently this is implemented by closing the database at the end of an 'atomic' transaction.
        //! As UnQLite develops in sophistication it may be possible to replace this by an internal transaction manager.
        void end_transaction();


        // ADMINISTRATION

      public:

        virtual void set_model_finder(typename instance_manager<number>::model_finder& f) override;


        // CREATE RECORDS -- implements a 'repository' interface

      public:

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        virtual void commit_package(const initial_conditions<number>& ics) override;

        //! Write an integration task to the database.
        virtual void commit_task(const integration_task<number>& tk) override;

        //! Write an output task to the database
        virtual void commit_task(const output_task<number>& tk) override;

        //! Write a derived product specification
        virtual void commit_derived_product(const derived_data::derived_product<number>& d) override;


        // READ RECORDS FROM THE DATABASE -- implements a 'repository' interface

      public:

        //! Read a package record from the database
        virtual package_record* query_package(const std::string& name) override;

        //! Read a task record from the database
        virtual task_record* query_task(const std::string& name) override;

        //! Read a derived product specification from the database
        virtual derived_product_record* query_derived_product(const std::string& name) override;

        //! Enumerate the output groups available from a named integration task
        virtual std::list<typename repository<number>::template output_group_record<typename repository<number>::integration_payload>* >
          enumerate_integration_task_content(const std::string& name) override;

        //! Enumerate the output groups available from a named output task
        virtual std::list<typename repository<number>::template output_group_record<typename repository<number>::output_payload>* >
          enumerate_output_task_content(const std::string& name) override;


        // ADD CONTENT ASSOCIATED WITH A TASK -- implements a 'repository' interface

      public:

        //! Generate a writer object for new integration output
        virtual typename repository<number>::integration_writer new_integration_task_content(integration_task_record& rec, const std::list<std::string>& tags, unsigned int worker) override;

        //! Generate a writer object for new derived-content output
        virtual typename repository<number>::derived_content_writer new_output_task_content(output_task_record& rec, const std::list<std::string>& tags, unsigned int worker) override;


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


        // DATABASE UTILITY FUNCTIONS

      protected:

        //! Apply a functor to a collection of records, applying a validator object
        template <typename ValidatorObject>
        void array_apply_functor(const std::string& name, const std::string& collection,
                                 typename repository_unqlite<number>::array_extraction_functor& f, ValidatorObject& v,
                                 const std::string search_field = __CPP_TRANSPORT_NODE_RECORD_NAME);

        //! Apply a functor to a collection of records, with no validator object
        void array_apply_functor(const std::string& name, const std::string& collection,
                                 typename repository_unqlite<number>::array_extraction_functor& f,
                                 const std::string search_field = __CPP_TRANSPORT_NODE_RECORD_NAME);

        //! Extract a repository record and its UnQLite id
        int array_extract_id(unqlite_value* key, unqlite_value* value, array_extraction_data* data);

        //! Extract output groups with integration payload from UnQLite array
        int array_extract_content_groups(unqlite_value* key, unqlite_value* value,
                                         std::list<typename repository<number>::template output_group_record<typename repository<number>::integration_payload>* >* list);

        //! Extract output groups with output payload from UnQLite array
        int array_extract_content_groups(unqlite_value* key, unqlite_value* value,
                                         std::list<typename repository<number>::template output_group_record<typename repository<number>::output_payload> >*);

        //! Get a serialization reader from the database. The internal UnQLite record id is stored in 'id'
        unqlite_serialization_reader* query_serialization_reader(const std::string& name, const std::string& collection,
                                                                 const std::string& record_type, unsigned int& id);

        //! Get a serialization reader from the database.
        unqlite_serialization_reader* query_serialization_reader(const std::string& name, const std::string& collection, const std::string& record_type);



        // CREATE RECORD - INTERNAL FUNCTIONS

      protected:

        //! Check whether a task already exists in the database. Throws an exception if so.
        void check_task_duplicate(const std::string& name);

        // INTEGRATION WRITER CALLBACKS

      protected:

        //! Commit the output of an integration writer to the database. Called as a callback
        void close_integration_writer(typename repository<number>::integration_writer& writer, model<number>* m);

        //! Advise that an output group has been committed
        template <typename Payload>
        void advise_commit(typename repository<number>::template output_group_record<Payload>& group);

        //! Move a failed output group to a safe location
        virtual void abort_integration_writer(typename repository<number>::integration_writer& writer) override;


        // DERIVED CONTENT WRITER CALLBACKS

      protected:

        //! Commit the output of a derived content writer to the database. Called as a callback
        void close_derived_content_writer(typename repository<number>::derived_content_writer& writer);

        //! Move a failed output group to a safe location
        virtual void abort_derived_content_writer(typename repository<number>::derived_content_writer& writer) override;


        // COMMIT CALLBACK METHODS FOR REPOSITORY RECORDS

      protected:

        //! Commit callback for a first-time write to the database
        void first_time_commit(const typename repository<number>::repository_record& record,
                               const std::string& collection, const std::string& record_type);

        //! Commit callback for updates to the database
        void replace_commit(const typename repository<number>::repository_record& record,
                            const std::string& collection, const std::string& record_type);


        // REPOSITORY RECORD FACTORIES

      protected:

        //! Create a new package record from an explicit object
        typename repository<number>::package_record* package_record_factory(const initial_conditions<number>& ics);

        //! Create a package record from a serialization reader
        typename repository<number>::package_record* package_record_factory(serialization_reader* reader);

        //! Create a new integration task record from an explicit object
        typename repository<number>::integration_task_record* integration_task_record_factory(const integration_task<number>& tk);

        //! Create an integration task record from a serialization reader
        typename repository<number>::integration_task_record* integration_task_record_factory(serialization_reader* reader);

        //! Create a new output task record from an explicit object
        typename repository<number>::output_task_record* output_task_record_factory(const output_task<number>& tk);

        //! Creat an output task record from a serialization reader
        typename repository<number>::output_task_record* output_task_record_factory(serialization_reader* reader);

        //! Create a new derived product record from explicit object
        typename repository<number>::derived_product_record* derived_product_record_factory(const derived_data::derived_product<number>& prod);

        //! Create a new content record from an explicit object
        template <typename Payload>
        typename repository<number>::output_group_record<Payload>* output_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                               bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg);

        //! Create a new content from from a serialization reader
        template <typename Payload>
        typename repository<number>::output_group_record<Payload>* output_group_record_factory(serialization_reader* reader);


        // ******* OLD STUFF


        //! Extract an integration task from a serialization reader
        task<number>* lookup_integration_task(const std::string& name, task_type type,
                                              model<number>*& m, typename instance_manager<number>::model_finder finder,
                                              unqlite_serialization_reader* reader);

        //! Extract an output task from a serialization reader
        task<number>* lookup_output_task(const std::string& name, typename instance_manager<number>::model_finder finder,
                                         unqlite_serialization_reader* reader);

        //! Convert a named database task record into a serialization reader, returned as a pointer.
        //! It's up to the calling function to destroy the pointer which is returned.
        unqlite_serialization_reader* get_task_serialization_reader(const std::string& name, int& unqlite_id, task_type& type);

        //! Convert a named database package record into a serialization reader, returned as a pointer.
        //! It's up to the calling function to destroy the pointer which is returned.
        unqlite_serialization_reader* get_package_serialization_reader(const std::string& name, int& unqlite_id);

        //! Convert a named database derived product record into a serialization reader, returned as a pointer.
        //! It's up to the calling function to destroy the pointer which is returned.
        unqlite_serialization_reader* get_derived_product_serialization_reader(const std::string& name, int& unqlite_id);

      protected:

        //! Create an integration writer and its associated environment.
        typename repository<number>::integration_writer create_integration_writer(model<number>* m,
                                                                                  unsigned int worker, integration_task<number>* tk,
                                                                                  const std::list<std::string>& tags);


        // ADD DERIVED CONTENT TO AN OUTPUT TASK -- implements a 'repository' interface

      public:

        //! Lookup an output group for a task, given a set of tags
        virtual typename repository<number>::template output_group_record<typename repository<number>::integration_payload>
          find_integration_task_output_group(const integration_task<number>* tk, const std::list<std::string>& tags) override;

      protected:

        //! Create a derived content writer and its associated environment.
        typename repository<number>::derived_content_writer
          create_derived_content_writer(unsigned int worker, output_task<number>* tk, const std::list<std::string>& tags);


        // ********** END OLD STUFF


        // INTERNAL DATA

      private:

        //! Error handler
        error_callback error;

        //! Warning handler
        warning_callback warning;

        //! Message handler
        message_callback message;

        //! BOOST path to database
        boost::filesystem::path db_path;

        //! Cached model-finder supplied by instance manager
        typename instance_manager<number>::model_finder model_finder;

        //! Cached task-finder instance
        typename repository<number>::task_finder task_finder;


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
                                                   typename repository_unqlite<number>::error_callback e,
                                                   typename repository_unqlite<number>::warning_callback w,
                                                   typename repository_unqlite<number>::message_callback m)
      : json_interface_repository<number>(path, mode),
        db(nullptr), open_clients(0),
        error(e), warning(w), message(m)
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
        unsigned int unqlite_mode = (mode == repository<number>::access_type::readonly ? UNQLITE_OPEN_READONLY : UNQLITE_OPEN_READWRITE);

        if(unqlite_open(&db, db_path.c_str(), unqlite_mode) != UNQLITE_OK)
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
      : json_interface_repository<number>(path, repository<number>::access_type::readwrite),
        db(nullptr), open_clients(0),
        error(typename repository_unqlite<number>::default_error_handler()),
        warning(typename repository_unqlite<number>::default_warning_handler()),
        message(typename repository_unqlite<number>::default_message_handler())
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


    // ADMINISTRATION

    template <typename number>
    void repository_unqlite<number>::set_model_finder(const typename instance_manager<number>::model_finder f)
      {
        this->model_finder = f;
        this->task_finder = std::bind(&repository_unqlite<number>::query_task, this, std::placeholders::_1);
      }


    // TRANSACTION MANAGEMENT


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
    void repository_unqlite<number>::end_transaction()
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


    // DATABASE UTILITY FUNCTIONS


    namespace
      {

        template <typename number>
        int array_walker(unqlite_value* key, unqlite_value* value, void* handle)
          {
            assert(key != nullptr);
            assert(value != nullptr);
            assert(handle != nullptr);

            typename repository_unqlite<number>::array_extraction_functor* f = static_cast<typename repository_unqlite<number>::array_extraction_functor*>(f);

            return (*f)(key,value);
          }


        // check number of records
        void check_number_records(unsigned int count, const std::string& name, const std::string& record_type)
          {
            if(count == 0)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_RECORD_MISSING_A << " " << record_type
                  << " " << __CPP_TRANSPORT_REPO_RECORD_MISSING_B << " '" << name << "' "
                  << __CPP_TRANSPORT_REPO_RECORD_MISSING_C;
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
              }
            else if(count > 1)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_RECORD_DUPLICATE_A << " " << record_type
                  << " " << __CPP_TRANSPORT_REPO_RECORD_DUPLICATE_B << " '" << name << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
              }
          }

      }   // unnamed namespace


    template <typename number>
    int repository_unqlite<number>::array_extract_id(unqlite_value* key, unqlite_value* value, typename repository_unqlite<number>::array_extraction_data* data)
      {
        assert(key != nullptr);
        assert(value != nullptr);
        assert(data != nullptr);

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


    template <typename number, typename Payload>
    int repository_unqlite<number>::array_extract_content_groups(unqlite_value* key, unqlite_value* value,
                                                                 std::list<typename repository<number>::template output_group_record<typename repository<number>::integration_payload>* >* list)
      {
        assert(key != nullptr);
        assert(value != nullptr);
        assert(list != nullptr);

        unqlite_serialization_reader reader(value);
        list->push_back(this->output_group_record_factory<typename repository<number>::integration_payload>(&reader));

        return(UNQLITE_OK);
      }


    template <typename number, typename Payload>
    int repository_unqlite<number>::array_extract_content_groups(unqlite_value* key, unqlite_value* value,
                                                                 std::list<typename repository<number>::template output_group_record<typename repository<number>::output_payload>* >* list)
      {
        assert(key != nullptr);
        assert(value != nullptr);
        assert(list != nullptr);

        unqlite_serialization_reader reader(value);
        list->push_back(this->output_group_record_factory<typename repository<number>::output_payload>(&reader));

        return(UNQLITE_OK);
      }


    template <typename number, typename ValidatorObject>
    void repository_unqlite<number>::array_apply_functor(const std::string& name, const std::string& collection,
                                                         typename repository_unqlite<number>::array_extraction_functor& f, ValidatorObject& v,
                                                         const std::string search_field)
      {
        this->begin_transaction();

        unqlite_vm   * vm   = nullptr;
        unqlite_value* recs = unqlite_operations::query(this->db, vm, collection, name, search_field);

        // set up scoped pointer to ensure deallocation of virtual machine if an exception occurs
        boost::scoped_ptr<unqlite_vm> scoped_vm(vm);

        if(recs == nullptr || !unqlite_value_is_json_array(recs)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

        unsigned int count = static_cast<unsigned int>(unqlite_array_count(recs));
        v(count);

        unqlite_array_walk(recs, array_walker, &f);

        this->end_transaction();
      }


    template <typename number, typename ValidatorObject>
    void repository_unqlite<number>::array_apply_functor(const std::string& name, const std::string& collection,
                                                         typename repository_unqlite<number>::array_extraction_functor& f,
                                                         const std::string search_field)
      {
        this->begin_transaction();

        unqlite_vm   * vm   = nullptr;
        unqlite_value* recs = unqlite_operations::query(this->db, vm, collection, name, search_field);

        // set up scoped pointer to ensure deallocation of virtual machine if an exception occurs
        boost::scoped_ptr<unqlite_vm> scoped_vm(vm);

        if(recs == nullptr || !unqlite_value_is_json_array(recs)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

        unqlite_array_walk(recs, array_walker, &f);

        this->end_transaction();
      }


    template <typename number>
    unqlite_serialization_reader* repository_unqlite<number>::query_serialization_reader(const std::string& name, const std::string& collection,
                                                                                         const std::string& record_type, unsigned int& id)
      {
        array_extraction_data data;
        data.cmp     = 0;
        data.str_cmp = boost::lexical_cast<std::string>(data.cmp);
        data.id      = -1;
        data.reader  = nullptr;

        array_extraction_functor          f = std::bind(&repository_unqlite<number>::array_extract_id, this, std::placeholders::_1, std::placeholders::_2, &data);
        std::function<void(unsigned int)> v = std::bind(&check_number_records, std::placeholders::_1, name, record_type);

        this->array_apply_functor(name, collection, f, v);

        assert(data.reader != nullptr);
        if(data.reader == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

        id = static_cast<unsigned int>(data.id);
        return(data.reader);
      }


    template <typename number>
    unqlite_serialization_reader* repository_unqlite<number>::query_serialization_reader(const std::string& name, const std::string& collection,
                                                                                         const std::string& record_type)
      {
        unsigned int task_id;
        return this->query_serialization_reader(name, collection, record_type, task_id);
      }


    // COMMIT CALLBACK METHODS FOR REPOSITORY RECORDS


    template <typename number>
    void repository_unqlite<number>::first_time_commit(const typename repository<number>::repository_record& record,
                                                       const std::string& collection, const std::string& record_type)
      {
        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check that no record with this name already exists
        unsigned int count = unqlite_operations::query_count(this->db, collection, record.get_name(), __CPP_TRANSPORT_NODE_RECORD_NAME);
        if(count > 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_RECORD_EXISTS_A << " " << record_type
              << " " << __CPP_TRANSPORT_REPO_RECORD_EXISTS_B << " '" << record.get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        unqlite_serialization_writer writer;
        record.serialize(writer);

        unqlite_operations::store(this->db, collection, writer.get_contents());

        // commit transaction
        this->end_transaction();
      }


    template <typename number>
    void repository_unqlite<number>::replace_commit(const typename repository<number>::repository_record& record,
                                                    const std::string& collection, const std::string& record_type)
      {
        // find existing record in the database
        unsigned int task_id;
        boost::scoped_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(record.get_name(), collection, record_type, task_id));

        unqlite_serialization_writer writer;
        record.serialize(writer);

        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        unqlite_operations::drop(this->db, collection, task_id);
        unqlite_operations::store(this->db, collection, writer.get_contents());

        // commit transaction
        this->end_transaction();
      }


    // REPOSITORY RECORD FACTORIES


    template <typename number>
    typename repository<number>::package_record* repository_unqlite<number>::package_record_factory(const initial_conditions<number>& ics)
      {
        typename repository<number>::repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, __CPP_TRANSPORT_REPO_PACKAGE_RECORD);

        return new typename repository<number>::package_record(ics, pkg);
      }


    template <typename number>
    typename repository<number>::package_record* repository_unqlite<number>::package_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        typename repository<number>::repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, __CPP_TRANSPORT_REPO_PACKAGE_RECORD);

        return new typename repository<number>::package_record(reader, pkg);
      }


    template <typename number>
    typename repository<number>::integration_task_record* repository_unqlite<number>::integration_task_record_factory(const integration_task<number>& tk)
      {
        typename repository<number>::repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, __CPP_TRANSPORT_REPO_INTEGRATION_TASK_RECORD);

        return new typename repository<number>::integration_task_record(tk, pkg);
      }


    template <typename number>
    typename repository<number>::integration_task_record* repository_unqlite<number>::integration_task_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        typename repository<number>::repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, __CPP_TRANSPORT_REPO_INTEGRATION_TASK_RECORD);

        return new typename repository<number>::integration_task_record(reader, this->model_finder, pkg);
      }


    template <typename number>
    typename repository<number>::output_task_record* repository_unqlite<number>::output_task_record_factory(const output_task<number>& tk)
      {
        typename repository<number>::repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, __CPP_TRANSPORT_REPO_OUTPUT_TASK_RECORD);

        return new typename repository<number>::output_task_record(tk, pkg);
      }


    template <typename number>
    typename repository<number>::output_task_record* repository_unqlite<number>::output_task_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        typename repository<number>::repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, __CPP_TRANSPORT_REPO_OUTPUT_TASK_RECORD);

        return new typename repository<number>::output_task_record(reader, this->model_finder, pkg);
      }


    template <typename number>
    typename repository<number>::derived_product_record* repository_unqlite<number>::derived_product_record_factory(const derived_data::derived_product<number>& prod)
      {
        typename repository<number>::repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_RECORD);

        return new typename repository<number>::derived_product_record(prod, pkg);
      }


    template <typename number>
    typename repository<number>::derived_product_record* repository_unqlite<number>::derived_product_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        typename repository<number>::repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_RECORD);

        return new typename repository<number>::derived_product_record(reader, this->task_finder, pkg);
      }


    template <typename number, typename Payload>
    typename repository<number>::output_group_record<Payload>* repository_unqlite<number>::output_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                                                       bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg)
      {
        typename repository<number>::repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, __CPP_TRANSPORT_REPO_CONTENT_RECORD);

        return new typename repository<number>::output_group_record<Payload>(tn, this->root_path, path, lock, nt, tg, pkg);
      }


    template <typename number, typename Payload>
    typename repository<number>::output_group_record<Payload>* repository_unqlite<number>::output_group_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        typename repository<number>::repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, __CPP_TRANSPORT_REPO_CONTENT_RECORD);

        return new typename repository<number>::output_group_record<Payload>(reader, this->root_path, pkg);
      }


    // CREATE RECORDS


    // Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
    template <typename number>
    void repository_unqlite<number>::commit_package(const initial_conditions<number>& ics)
	    {
        boost::scoped_ptr<typename repository<number>::package_record> record(package_record_factory(ics));
        record.get()->commit();
	    }


    // Check for a duplicate task name
    template <typename number>
    void repository_unqlite<number>::check_task_duplicate(const std::string& name)
      {
        this->begin_transaction();

        unsigned int A = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);
        unsigned int B = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);

        this->end_transaction();

        if(A+B > 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_TASK << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
      }


    // Write an integration task to the database.
    template <typename number>
    void repository_unqlite<number>::commit_task(const integration_task<number>& tk)
	    {
        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        boost::scoped_ptr<typename repository<number>::integration_task_record> record(integration_task_record_factory(tk));
        record.get()->commit();

        // check whether the initial conditions package for this task is already present; if not, insert it
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, tk.get_ics().get_name(), __CPP_TRANSPORT_NODE_RECORD_NAME);
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
		void repository_unqlite<number>::commit_task(const output_task<number>& tk)
	    {
        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        boost::scoped_ptr<typename repository<number>::output_task_record> record(output_task_record_factory(tk));
        record.get()->commit();

        // check whether derived products on which this task depends have already been committed to the database
        const typename std::vector< output_task_element<number> > elements = tk.get_elements();
        for(typename std::vector< output_task_element<number> >::const_iterator t = elements.begin(); t != elements.end(); t++)
          {
            derived_data::derived_product<number>* product = (*t).get_product();
            unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, product->get_name(), __CPP_TRANSPORT_NODE_RECORD_NAME);
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


    // Write a derived product specification
    template <typename number>
    void repository_unqlite<number>::commit_derived_product(const derived_data::derived_product<number>& d)
      {
        boost::scoped_ptr<typename repository<number>::derived_product_record> record(derived_product_record_factory(d));
        record.get()->commit();

        // check whether all tasks on which this derived product depends are already in the database
        typename std::vector< integration_task<number>* > task_list;
        d.get_task_list(task_list);

        for(typename std::vector< typename integration_task<number>* >::iterator t = task_list.begin(); t != task_list.end(); t++)
          {
            unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, (*t)->get_name(), __CPP_TRANSPORT_NODE_RECORD_NAME);
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


    // Read a package record from the database
    template <typename number>
    typename repository<number>::package_record* repository_unqlite<number>::query_package(const std::string& name)
      {
        boost::scoped_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(name, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, __CPP_TRANSPORT_REPO_PACKAGE_RECORD));

        return this->package_record_factory(reader.get());
      }


    // Read a task record from the database
    template <typename number>
    typename repository<number>::task_record* repository_unqlite<number>::query_task(const std::string& name)
      {
        this->begin_transaction();
        unsigned int A = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);
        unsigned int B = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);
        this->end_transaction();

        check_number_records(A+B, name, __CPP_TRANSPORT_REPO_TASK_RECORD);

        if(A > 0) // can assume A=1, B=0
          {
            boost::scoped_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(name, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, __CPP_TRANSPORT_REPO_INTEGRATION_TASK_RECORD));
            return this->integration_task_record_factory(reader.get());
          }
        else      // can assume A=0, B=1
          {
            boost::scoped_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(name, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, __CPP_TRANSPORT_REPO_OUTPUT_TASK_RECORD));
            return this->output_task_record_factory(reader.get());
          }
      }


    // Read a derived product record from the database
    template <typename number>
    typename repository<number>::derived_product_record* repository_unqlite<number>::query_derived_product(const std::string& name)
      {
        boost::scoped_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(name, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_RECORD));

        return this->derived_product_record_factory(reader.get());
      }


    // Enumerate the output groups available from a named integration task
    template <typename number>
    std::list< typename repository<number>::template output_group_record< typename repository<number>::integration_payload >* >
    repository_unqlite<number>::enumerate_integration_task_content(const std::string& name)
      {
        boost::scoped_ptr<typename repository<number>::task_record> record(query_task(name));

        if(record.get()->get_type() != typename repository<number>::task_record::integration)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        std::list< typename repository<number>::template output_group_record< typename repository<number>::integration_payload >* > list;

        array_extraction_functor f = std::bind(&repository_unqlite<number>::array_extract_content_groups, this,
                                               std::placeholders::_1, std::placeholders::_2, &list);

        this->array_apply_functor(name, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, f, __CPP_TRANSPORT_NODE_OUTPUTGROUP_TASK_NAME);

        // sort the output groups into descending order of creation date, so the first element in the
        // list is the most recent data group.
        // This is usually what would be required.
        list->sort(&output_group_helper::comparator<number, typename repository<number>::integration_payload>);

        return(list);
      }


    // Enumerate the output groups available from a named output task
    template <typename number>
    std::list< typename repository<number>::template output_group_record< typename repository<number>::output_payload >* >
    repository_unqlite<number>::enumerate_output_task_content(const std::string& name)
      {
        boost::scoped_ptr<typename repository<number>::task_record> record(query_task(name));

        if(record.get()->get_type() != typename repository<number>::task_record::output)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_OUTPUT << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        std::list< typename repository<number>::template output_group_record< typename repository<number>::output_payload >* > list;

        array_extraction_functor f = std::bind(&repository_unqlite<number>::array_extract_content_groups, this,
                                               std::placeholders::_1, std::placeholders::_2, &list);

        this->array_apply_functor(name, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, f, __CPP_TRANSPORT_NODE_OUTPUTGROUP_TASK_NAME);

        // sort the output groups into descending order of creation date, so the first element in the
        // list is the most recent data group.
        // This is usually what would be required.
        list->sort(&output_group_helper::comparator<number, typename repository<number>::integration_payload>);

        return(list);
      }


    // Add output for a twopf task
		// DATABASE ENTRY POINT
    template <typename number>
    typename repository<number>::integration_writer
    repository_unqlite<number>::new_integration_task_content(twopf_task<number>* tk, const std::list<std::string>& tags,
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

		    // close database handles
        this->end_transaction();

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
		    typename repository<number>::integration_writer::callback_group callbacks;
		    callbacks.commit = std::bind(&repository_unqlite<number>::close_integration_writer, this, std::placeholders::_1, m);
		    callbacks.abort  = std::bind(&repository_unqlite<number>::abort_integration_writer, this, std::placeholders::_1);

		    return typename repository<number>::integration_writer(tk, tags, now, callbacks,
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
		    typename repository<number>::template output_group_record<typename repository<number>::integration_payload> group(tk->get_name(), this->get_root_path(),
		                                                                                                               writer.get_relative_output_path(),
		                                                                                                               writer.get_creation_time(),
                                                                                                                   false, std::list<std::string>(), tags);
		    group.get_payload().set_backend(m->get_backend());
		    group.get_payload().set_container_path(writer.get_relative_container_path());
		    group.get_payload().set_metadata(writer.get_metadata());
		    unqlite_serialization_writer swriter;
		    group.serialize(swriter);

		    // write new output group to the database
		    unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, swriter.get_contents());

		    // refresh edit time
		    task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_METADATA);
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_TASK_METADATA_EDITED, boost::posix_time::to_iso_string(now));   // insert overwrites previous value
		    task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_METADATA);

        // insert cross-reference to new output group
        std::string output_group_name = boost::posix_time::to_iso_string(writer.get_creation_time());
        task_reader->start_array(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS);
        task_reader->insert_node("arrayelt", false);    // node names are ignored in arrays
        task_reader->insert_value(__CPP_TRANSPORT_NODE_TASK_OUTPUT_XREF, output_group_name);
        task_reader->end_element("arrayelt");
        task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS);

        // update the task entry for this database
        // that means: first, drop this existing record; then, store a copy of the updated one
		    unqlite_operations::drop(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, task_id);
		    unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, task_reader->get_contents());

		    delete task_reader;

		    // commit transaction
        this->end_transaction();
        this->advise_commit(group);
			}


    template <typename number>
    template <typename Payload>
    void repository_unqlite<number>::advise_commit(typename repository<number>::template output_group_record<Payload>& group)
      {
        std::ostringstream msg;

        std::string group_name = boost::posix_time::to_iso_string(group.get_creation_time());

        msg << __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_A << " '" << group_name << "' "
            << __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_B << " '" << group.get_task_name() << "'";

        this->message(msg.str());
      }


    template <typename number>
    void repository_unqlite<number>::abort_integration_writer(typename repository<number>::integration_writer& writer)
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

            msg << __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_A << " '" << writer.get_task()->get_name() << "': "
                << __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_B << " '" << group_name << "' "
                << __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_C;

            this->message(msg.str());
          }
        else throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
      }


		//! Query a derived product specification
		template <typename number>
		derived_data::derived_product<number>* repository_unqlite<number>::query_derived_product(const std::string& product, typename instance_manager<number>::model_finder finder)
			{
				// open a new transaction, if necessary. After this we can assume the database handles are live
				this->begin_transaction();

				// get serialization_reader for the named product
				int product_id = 0;
				unqlite_serialization_reader* product_reader = this->get_derived_product_serialization_reader(product, product_id);

				typename repository<number>::task_finder tfinder = std::bind(&repository_unqlite<number>::query_task, this, std::placeholders::_1, std::placeholders::_2, finder);

        // move reader to details block
        product_reader->start_node(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_DETAILS);
		    derived_data::derived_product<number>* rval = derived_data::derived_product_helper::deserialize<number>(product, product_reader, tfinder);
        product_reader->end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_DETAILS);

				// commit transaction
        this->end_transaction();

		    delete product_reader;

				if(rval == nullptr)
					{
				    std::ostringstream msg;
						msg << __CPP_TRANSPORT_REPO_MISSING_DERIVED_PRODUCT << " '" << product << "'";
						throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
					}

				return(rval);
			}


		template <typename number>
		typename repository<number>::derived_content_writer
		repository_unqlite<number>::new_output_task_content(output_task<number>* tk, const std::list<std::string>& tags, unsigned int worker)
			{
		    assert(tk != nullptr);

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
		    typename repository<number>::derived_content_writer writer = this->create_derived_content_writer(worker, tk, tags);

		    // close database handles
        this->end_transaction();

		    return(writer);
			}


		template <typename number>
		typename repository<number>::derived_content_writer
		repository_unqlite<number>::create_derived_content_writer(unsigned int worker, output_task<number>* tk, const std::list<std::string>& tags)
			{
		    // FIXME: tags not currently enforced

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

				typename repository<number>::derived_content_writer::callback_group callbacks;
				callbacks.commit = std::bind(&repository_unqlite<number>::close_derived_content_writer, this, std::placeholders::_1);
				callbacks.abort  = std::bind(&repository_unqlite<number>::abort_derived_content_writer, this, std::placeholders::_1);

		    return typename repository<number>::derived_content_writer(tk, tags, now, callbacks,
		                                                               this->get_root_path(),
		                                                               output_path, log_path, task_path, temp_path, worker);
			}


		template <typename number>
		void repository_unqlite<number>::close_derived_content_writer(typename repository<number>::derived_content_writer& writer)
			{
				output_task<number>* tk = writer.get_task();
				const std::list<std::string>& tags = writer.get_tags();
				assert(tk != nullptr);

		    // open a new transaction, if necessary. After this we can assume the database handles are live
		    this->begin_transaction();

		    // get serialization_reader for the named output task record
		    int task_id = 0;
		    task_type type;
		    unqlite_serialization_reader* task_reader = this->get_task_serialization_reader(tk->get_name(), task_id, type);
		    assert(type == output_record);

		    // get current time
		    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

		    // create and serialize an empty output group
		    typename repository<number>::template output_group_record<typename repository<number>::output_payload> group(tk->get_name(), this->get_root_path(),
		                                                                                                          writer.get_relative_output_path(),
		                                                                                                          writer.get_creation_time(),
		                                                                                                          false, std::list<std::string>(), tags);

        const std::list< typename repository<number>::derived_content >& content = writer.get_content();
        for(typename std::list< typename repository<number>::derived_content >::const_iterator t = content.begin(); t != content.end(); t++)
          {
            group.get_payload().add_derived_content(*t);
          }
        group.get_payload().set_metadata(writer.get_metadata());
		    unqlite_serialization_writer swriter;
		    group.serialize(swriter);

		    // write new output group to the database
		    unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, swriter.get_contents());

		    // refresh edit time
		    task_reader->start_node(__CPP_TRANSPORT_NODE_TASK_METADATA);
		    task_reader->insert_value(__CPP_TRANSPORT_NODE_TASK_METADATA_EDITED, boost::posix_time::to_iso_string(now));   // insert overwrites previous value
		    task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_METADATA);

        // insert cross-reference to new output group
        std::string output_group_name = boost::posix_time::to_iso_string(writer.get_creation_time());
        task_reader->start_array(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS);
        task_reader->insert_node("arrayelt", false);    // node names are ignored in arrays
        task_reader->insert_value(__CPP_TRANSPORT_NODE_TASK_OUTPUT_XREF, output_group_name);
        task_reader->end_element("arrayelt");
        task_reader->end_element(__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS);

        // update the task entry for this database
        // that means: first, drop this existing record; then, store a copy of the updated one
		    unqlite_operations::drop(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, task_id);
		    unqlite_operations::store(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, task_reader->get_contents());

		    delete task_reader;

				// commit transaction
        this->end_transaction();
				this->advise_commit(group);
			}


		template <typename number>
		void repository_unqlite<number>::abort_derived_content_writer(typename repository<number>::derived_content_writer& writer)
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

		        msg << __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_A << " '" << writer.get_task()->get_name() << "': "
			          << __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_B << " '" << group_name << "' "
			          << __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_C;

		        this->message(msg.str());
			    }
		    else throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
			}


    template<typename number>
    typename repository<number>::template output_group_record<typename repository<number>::integration_payload>
    repository_unqlite<number>::find_integration_task_output_group(const integration_task<number> *tk, const std::list<std::string> &tags)
	    {
		    assert(tk != nullptr);

		    if(tk == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_TASK);

		    // search for output groups associated with this task
        std::list< typename repository<number>::template output_group_record< typename repository<number>::integration_payload > >
          output = this->enumerate_integration_task_content(tk->get_name());

		    // remove items from the list which have mismatching tags
        output.remove_if( [&] (const typename repository<number>::template output_group_record< typename repository<number>::integration_payload >& group) { return(group.check_tags(tags)); } );

		    if(output.empty())
			    {
		        std::ostringstream msg;
				    msg << __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS   << " '" << tk->get_name() << "'";
				    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

		    return(output.front());
	    }


    // JSON INTERFACE


    template <typename number>
    std::string repository_unqlite<number>::export_JSON_package_record(const std::string& name)
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
        this->end_transaction();

        return(this->format_JSON(document));
      }


    template <typename number>
    std::string repository_unqlite<number>::export_JSON_task_record(const std::string& name)
      {
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
        this->end_transaction();

        return(this->format_JSON(rval));
      }


    template <typename number>
    std::string repository_unqlite<number>::export_JSON_product_record(const std::string& name)
      {
        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check a suitable record exists
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, name, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME);

        if(count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_PRODUCT << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
        else if(count > 1)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_PRODUCT << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // extract this record in JSON format
        std::string document = unqlite_operations::extract_json(this->db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, name, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_NAME);

        // commit transaction
        this->end_transaction();

        return(this->format_JSON(document));
      }


    template <typename number>
    std::string repository_unqlite<number>::export_JSON_content_record(const std::string& name)
      {
        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check a suitable record exists
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, name, __CPP_TRANSPORT_NODE_OUTPUTGROUP_CREATED);

        if(count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_CONTENT << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
        else if(count > 1)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_CONTENT << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // extract this record in JSON format
        std::string document = unqlite_operations::extract_json(this->db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, name, __CPP_TRANSPORT_NODE_OUTPUTGROUP_CREATED);

        // commit transaction
        this->end_transaction();

        return(this->format_JSON(document));
      }



    // FACTORY FUNCTIONS TO BUILD A REPOSITORY


    template <typename number>
    json_interface_repository<number>* repository_factory(const std::string& path,
                                                          typename repository<number>::access_type mode = repository<number>::access_type::readwrite)
      {
        return new repository_unqlite<number>(path, mode);
      }


    template <typename number>
    json_interface_repository<number>* repository_factory(const std::string& path,
                                                          typename repository<number>::access_type mode,
                                                          typename repository_unqlite<number>::error_callback e,
                                                          typename repository_unqlite<number>::warning_callback w,
                                                          typename repository_unqlite<number>::message_callback m)
      {
        return new repository_unqlite<number>(path, mode, e, w, m);
      }


    template <typename number>
    json_interface_repository<number>* repository_factory(const std::string& path, const repository_creation_key& key)
      {
        return new repository_unqlite<number>(path, key);
      }

	}   // namespace transport


#endif //__repository_unqlite_H_
