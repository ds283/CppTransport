//
// Created by David Seery on 04/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __repository_unqlite_H_
#define __repository_unqlite_H_

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


        // RAII scoped wrapper for unqlite_vm*
        class unqlite_scoped_vm
          {
          public:
            unqlite_scoped_vm(unqlite_vm* p)
              : vm_ptr(p)
              {
              }

            unqlite_scoped_vm(const unqlite_scoped_vm& obj) = delete;

            ~unqlite_scoped_vm() { unqlite_vm_release(vm_ptr); }

            unqlite_vm* get() const { return(this->vm_ptr); }

          private:

            unqlite_vm* vm_ptr;
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

            ~scoped_transaction() { this->closer(); }

          private:

            open_handler opener;
            close_handler closer;
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

        //! Generate an RAII transaction management object
        scoped_transaction scoped_transaction_factory();

        // ADMINISTRATION

      public:

        virtual void set_model_finder(const typename instance_manager<number>::model_finder& f) override;


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


        // DATABASE UTILITY FUNCTIONS

      protected:

        //! Apply a functor to a collection of records, applying a validator object.
        //! If omitted, the validator is trivial.
        template <typename ValidatorObject>
        void array_apply_functor(const std::string& name, const std::string& collection,
                                 typename repository_unqlite<number>::array_extraction_functor& f,
                                 const std::string search_field, ValidatorObject);

        void array_apply_functor(const std::string& name, const std::string& collection,
                                 typename repository_unqlite<number>::array_extraction_functor& f,
                                 const std::string search_field = __CPP_TRANSPORT_NODE_RECORD_NAME);

        //! Extract a repository record and its UnQLite id
        int array_extract_id(unqlite_value* key, unqlite_value* value, array_extraction_data* data);

        //! Extract output groups  from UnQLite array
        template <typename Payload>
        int array_extract_content_groups(unqlite_value* key, unqlite_value* value, std::list< std::shared_ptr< output_group_record<Payload> > >* list);

        //! Get a serialization reader from the database. The internal UnQLite record id is stored in 'id'
        unqlite_serialization_reader* query_serialization_reader(const std::string& name, const std::string& collection,
                                                                 const std::string& record_type, unsigned int& id);

        //! Get a serialization reader from the database.
        unqlite_serialization_reader* query_serialization_reader(const std::string& name, const std::string& collection, const std::string& record_type);

        //! Check whether a task already exists in the database. Throws an exception if so.
        void check_task_duplicate(const std::string& name);


        // WRITER CALLBACKS

      protected:

        // The interface here isn't very satisfactory, because we'd prefer that these callbacks take a reference of
        // the correct type (integration_writer, postintegration_writer, derived_content_writer).
        // The problem is incomplete types at instantiation of repository<number>; see comments in
        // repository.h near declaration of generic_writer for details

        //! Advise that an output group has been committed
        template <typename Payload>
        void advise_commit(output_group_record<Payload>* group);

        //! Advise that postintegration products have been committed to an output group
        template <typename Payload>
        void advise_commit(postintegration_task_record<number>* rec, output_group_record<Payload>* group);

        //! Commit the products from an integration to the database
        void close_integration_writer(base_writer& gwriter);

        //! Rollback a failed integration
        void abort_integration_writer(base_writer& gwriter);

        //! Commit hte products from a postintegration to the database
        void close_postintegration_writer(base_writer& gwriter);

        //! Rollback a failed postintegration
        void abort_postintegration_writer(base_writer& gwriter);

        //! Commit the products from an output task to the database
        void close_derived_content_writer(base_writer& gwriter);

        //! Rollback a failed integration
        void abort_derived_content_writer(base_writer& gwriter);


        // COMMIT CALLBACK METHODS FOR REPOSITORY RECORDS

      protected:

        //! Commit callback for a first-time write to the database
        void first_time_commit(const repository_record& record, const std::string& collection, const std::string& record_type);

        //! Commit callback for updates to the database
        void replace_commit(const repository_record& record, const std::string& collection, const std::string& record_type);


        // REPOSITORY RECORD FACTORIES

      protected:

        //! Create a new package record from an explicit object
        package_record<number>* package_record_factory(const initial_conditions<number>& ics);

        //! Create a package record from a serialization reader
        package_record<number>* package_record_factory(serialization_reader* reader);

        //! Create a new integration task record from an explicit object
        integration_task_record<number>* integration_task_record_factory(const integration_task<number>& tk);

        //! Create an integration task record from a serialization reader
        integration_task_record<number>* integration_task_record_factory(serialization_reader* reader);

        //! Create a new output task record from an explicit object
        output_task_record<number>* output_task_record_factory(const output_task<number>& tk);

        //! Create an output task record from a serialization reader
        output_task_record<number>* output_task_record_factory(serialization_reader* reader);

        //! Create a postintegration task record from an explicit object
        postintegration_task_record<number>* postintegration_task_record_factory(const postintegration_task<number>& tk);

        //! Create a postintegration task record from a serialization reader
        postintegration_task_record<number>* postintegration_task_record_factory(serialization_reader* reader);

        //! Create a new derived product record from explicit object
        derived_product_record<number>* derived_product_record_factory(const derived_data::derived_product<number>& prod);

        //! create a new derived product record from a serialization reader
        derived_product_record<number>* derived_product_record_factory(serialization_reader* reader);

        //! Create a new content record from an explicit object
        template <typename Payload>
        output_group_record<Payload>* output_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                  bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg);

        //! Create a new content from from a serialization reader
        template <typename Payload>
        output_group_record<Payload>* output_group_record_factory(serialization_reader* reader);


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

        //! Cached package-finder instance
        typename repository_finder<number>::package_finder pkg_finder;

        //! Cached task-finder instance
        typename repository_finder<number>::task_finder tk_finder;

        //! Cached derived-product-finder instance
        typename repository_finder<number>::derived_product_finder dprod_finder;


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
        unqlite_operations::ensure_collection(db, __CPP_TRANSPORT_UNQLITE_TASKS_POSTINTEGRATION_COLLECTION);

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
        unqlite_operations::create_collection(db, __CPP_TRANSPORT_UNQLITE_TASKS_POSTINTEGRATION_COLLECTION);

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
    void repository_unqlite<number>::set_model_finder(const typename instance_manager<number>::model_finder& f)
      {
        this->model_finder           = f;
        this->pkg_finder = std::bind(&repository_unqlite<number>::query_package, this, std::placeholders::_1);
        this->tk_finder = std::bind(&repository_unqlite<number>::query_task, this, std::placeholders::_1);
        this->dprod_finder = std::bind(&repository_unqlite<number>::query_derived_product, this, std::placeholders::_1);
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


    template <typename number>
    typename repository_unqlite<number>::scoped_transaction repository_unqlite<number>::scoped_transaction_factory()
      {
        typename repository_unqlite<number>::scoped_transaction::open_handler opener = std::bind(&repository_unqlite<number>::begin_transaction, this);
        typename repository_unqlite<number>::scoped_transaction::close_handler closer = std::bind(&repository_unqlite<number>::end_transaction, this);

        return typename repository_unqlite<number>::scoped_transaction(opener, closer);
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

            typename repository_unqlite<number>::array_extraction_functor* f = static_cast<typename repository_unqlite<number>::array_extraction_functor*>(handle);

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
                throw runtime_exception(runtime_exception::RECORD_NOT_FOUND, msg.str());
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


    template <typename number>
    template <typename Payload>
    int repository_unqlite<number>::array_extract_content_groups(unqlite_value* key, unqlite_value* value,
                                                                 std::list< std::shared_ptr< output_group_record<Payload> > >* list)
      {
        assert(key != nullptr);
        assert(value != nullptr);
        assert(list != nullptr);

        unqlite_serialization_reader reader(value);

        std::shared_ptr< output_group_record<Payload> > record(this->output_group_record_factory<Payload>(&reader));
        list->push_back(record);

        return(UNQLITE_OK);
      }


    template <typename number>
    template <typename ValidatorObject>
    void repository_unqlite<number>::array_apply_functor(const std::string& name, const std::string& collection,
                                                         typename repository_unqlite<number>::array_extraction_functor& f,
                                                         const std::string search_field, ValidatorObject v)
      {
        assert(this->db != nullptr);

        unqlite_vm*    vm   = nullptr;
        unqlite_value* recs = unqlite_operations::query(this->db, vm, collection, name, search_field);

        // set up scoped pointer to ensure deallocation of virtual machine if an exception occurs
        assert(vm != nullptr);
        unqlite_scoped_vm scoped_vm(vm);

        assert(recs != nullptr);
        assert(unqlite_value_is_json_array(recs));
        if(recs == nullptr || !unqlite_value_is_json_array(recs)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

        unsigned int count = static_cast<unsigned int>(unqlite_array_count(recs));
        v(count);

        unqlite_array_walk(recs, &array_walker<number>, &f);
      }


    template <typename number>
    void repository_unqlite<number>::array_apply_functor(const std::string& name, const std::string& collection,
                                                         typename repository_unqlite<number>::array_extraction_functor& f,
                                                         const std::string search_field)
      {
        this->array_apply_functor(name, collection, f, search_field, [](unsigned int){});
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

        // have to specify the JSON field we're searching for if we specify a validator
        this->begin_transaction();
        this->array_apply_functor(name, collection, f, __CPP_TRANSPORT_NODE_RECORD_NAME, v);
        this->end_transaction();

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
    void repository_unqlite<number>::first_time_commit(const repository_record& record, const std::string& collection, const std::string& record_type)
      {
        scoped_transaction scoped_xn = this->scoped_transaction_factory();

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
      }


    template <typename number>
    void repository_unqlite<number>::replace_commit(const repository_record& record, const std::string& collection, const std::string& record_type)
      {
        scoped_transaction scoped_xn = this->scoped_transaction_factory();

        // find existing record in the database
        unsigned int task_id;
        std::unique_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(record.get_name(), collection, record_type, task_id));

        unqlite_serialization_writer writer;
        record.serialize(writer);

        unqlite_operations::drop(this->db, collection, task_id);
        unqlite_operations::store(this->db, collection, writer.get_contents());
      }


    // REPOSITORY RECORD FACTORIES


    template <typename number>
    package_record<number>* repository_unqlite<number>::package_record_factory(const initial_conditions<number>& ics)
      {
        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, __CPP_TRANSPORT_REPO_PACKAGE_RECORD);

        return new package_record<number>(ics, pkg);
      }


    template <typename number>
    package_record<number>* repository_unqlite<number>::package_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, __CPP_TRANSPORT_REPO_PACKAGE_RECORD);

        return new package_record<number>(reader, this->model_finder, pkg);
      }


    template <typename number>
    integration_task_record<number>* repository_unqlite<number>::integration_task_record_factory(const integration_task<number>& tk)
      {
        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, __CPP_TRANSPORT_REPO_INTEGRATION_TASK_RECORD);

        return new integration_task_record<number>(tk, pkg);
      }


    template <typename number>
    integration_task_record<number>* repository_unqlite<number>::integration_task_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, __CPP_TRANSPORT_REPO_INTEGRATION_TASK_RECORD);

        return new integration_task_record<number>(reader, this->pkg_finder, pkg);
      }


    template <typename number>
    output_task_record<number>* repository_unqlite<number>::output_task_record_factory(const output_task<number>& tk)
      {
        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, __CPP_TRANSPORT_REPO_OUTPUT_TASK_RECORD);

        return new output_task_record<number>(tk, pkg);
      }


    template <typename number>
    output_task_record<number>* repository_unqlite<number>::output_task_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, __CPP_TRANSPORT_REPO_OUTPUT_TASK_RECORD);

        return new output_task_record<number>(reader, this->dprod_finder, pkg);
      }


    template <typename number>
    postintegration_task_record<number>* repository_unqlite<number>::postintegration_task_record_factory(const postintegration_task<number>& tk)
      {
        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_TASKS_POSTINTEGRATION_COLLECTION, __CPP_TRANSPORT_REPO_POSTINTEGRATION_TASK_RECORD);

        return new postintegration_task_record<number>(tk, pkg);
      }


    template <typename number>
    postintegration_task_record<number>* repository_unqlite<number>::postintegration_task_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_TASKS_POSTINTEGRATION_COLLECTION, __CPP_TRANSPORT_REPO_POSTINTEGRATION_TASK_RECORD);

        return new postintegration_task_record<number>(reader, this->tk_finder, pkg);
      }


    template <typename number>
    derived_product_record<number>* repository_unqlite<number>::derived_product_record_factory(const derived_data::derived_product<number>& prod)
      {
        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_RECORD);

        return new derived_product_record<number>(prod, pkg);
      }


    template <typename number>
    derived_product_record<number>* repository_unqlite<number>::derived_product_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_RECORD);

        return new derived_product_record<number>(reader, this->tk_finder, pkg);
      }


    template <typename number>
    template <typename Payload>
    output_group_record<Payload>* repository_unqlite<number>::output_group_record_factory(const std::string& tn, const boost::filesystem::path& path,
                                                                                          bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg)
      {
        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::first_time_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, __CPP_TRANSPORT_REPO_CONTENT_RECORD);

        typename output_group_record<Payload>::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = path;

        return new output_group_record<Payload>(tn, paths, lock, nt, tg, pkg);
      }


    template <typename number>
    template <typename Payload>
    output_group_record<Payload>* repository_unqlite<number>::output_group_record_factory(serialization_reader* reader)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER);

        repository_record::handler_package pkg;
        pkg.commit = std::bind(&repository_unqlite::replace_commit, this, std::placeholders::_1, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, __CPP_TRANSPORT_REPO_CONTENT_RECORD);

        return new output_group_record<Payload>(reader, this->root_path, pkg);
      }


    // CREATE RECORDS


    // Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
    template <typename number>
    void repository_unqlite<number>::commit_package(const initial_conditions<number>& ics)
	    {
        scoped_transaction scoped_xn = this->scoped_transaction_factory();

        std::unique_ptr< package_record<number> > record(package_record_factory(ics));
        record->commit();
	    }


    // Check for a duplicate task name
    template <typename number>
    void repository_unqlite<number>::check_task_duplicate(const std::string& name)
      {
        scoped_transaction scoped_xn = this->scoped_transaction_factory();

        unsigned int A = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);
        unsigned int B = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);
        unsigned int C = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_POSTINTEGRATION_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);

        if(A+B+C > 0)
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
        scoped_transaction scoped_xn = this->scoped_transaction_factory();

        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        std::unique_ptr< integration_task_record<number> > record(integration_task_record_factory(tk));
        record->commit();

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
        scoped_transaction scoped_xn = this->scoped_transaction_factory();

        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        std::unique_ptr< output_task_record<number> > record(output_task_record_factory(tk));
        record->commit();

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



    // Write a postintegration task to the database
    template <typename number>
    void repository_unqlite<number>::commit_task(const postintegration_task<number>& tk)
      {
        scoped_transaction scoped_xn = this->scoped_transaction_factory();

        // check for a task with a duplicate name
        this->check_task_duplicate(tk.get_name());

        std::unique_ptr< postintegration_task_record<number> > record(postintegration_task_record_factory(tk));
        record->commit();

        // check whether parent task is already committed to the database
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, tk.get_parent_task()->get_name(), __CPP_TRANSPORT_NODE_RECORD_NAME);
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
    void repository_unqlite<number>::commit_derived_product(const derived_data::derived_product<number>& d)
      {
        scoped_transaction scoped_xn = this->scoped_transaction_factory();

        std::unique_ptr< derived_product_record<number> > record(derived_product_record_factory(d));
        record->commit();

        // check whether all tasks on which this derived product depends are already in the database
        typename std::vector< integration_task<number>* > task_list;
        d.get_task_list(task_list);

        for(typename std::vector< integration_task<number>* >::iterator t = task_list.begin(); t != task_list.end(); t++)
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

    // Calls to the record factories fall under the scope of a scoped_transaction, and are therefore all managed
    // by a *single* database session.
    // This is intended to prevent too many open/close events on the database.


    // Read a package record from the database
    template <typename number>
    package_record<number>* repository_unqlite<number>::query_package(const std::string& name)
      {
        scoped_transaction scoped_xn = this->scoped_transaction_factory();
        std::unique_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(name, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, __CPP_TRANSPORT_REPO_PACKAGE_RECORD));

        return this->package_record_factory(reader.get());
      }


    // Read a task record from the database
    template <typename number>
    task_record<number>* repository_unqlite<number>::query_task(const std::string& name)
      {
        scoped_transaction scoped_xn = this->scoped_transaction_factory();
        unsigned int A = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);
        unsigned int B = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);
        unsigned int C = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_POSTINTEGRATION_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);

        check_number_records(A+B+C, name, __CPP_TRANSPORT_REPO_TASK_RECORD);

        if(A==1)      // can assume A=1, B=0, C=0
          {
            std::unique_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(name, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, __CPP_TRANSPORT_REPO_INTEGRATION_TASK_RECORD));
            return this->integration_task_record_factory(reader.get());
          }
        else if(B==1) // can assume A=0, B=1, C=0
          {
            std::unique_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(name, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, __CPP_TRANSPORT_REPO_OUTPUT_TASK_RECORD));
            return this->output_task_record_factory(reader.get());
          }
        else          // can assume A=0, B=0, C=1
          {
            std::unique_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(name, __CPP_TRANSPORT_UNQLITE_TASKS_POSTINTEGRATION_COLLECTION, __CPP_TRANSPORT_REPO_POSTINTEGRATION_TASK_RECORD));
            return this->postintegration_task_record_factory(reader.get());
          }
      }


    // Read a derived product record from the database
    template <typename number>
    derived_product_record<number>* repository_unqlite<number>::query_derived_product(const std::string& name)
      {
        scoped_transaction scoped_xn = this->scoped_transaction_factory();
        std::unique_ptr<unqlite_serialization_reader> reader(this->query_serialization_reader(name, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_RECORD));

        return this->derived_product_record_factory(reader.get());
      }


    // Enumerate the output groups available from a named integration task
    template <typename number>
    std::list< std::shared_ptr< output_group_record<integration_payload> > >
    repository_unqlite<number>::enumerate_integration_task_content(const std::string& name)
      {
        std::unique_ptr< task_record<number> > record(query_task(name));

        if(record->get_type() != task_record<number>::integration)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        std::list< std::shared_ptr< output_group_record<integration_payload> > > list;

        array_extraction_functor f = std::bind(&repository_unqlite<number>::array_extract_content_groups<integration_payload>, this,
                                               std::placeholders::_1, std::placeholders::_2, &list);

        scoped_transaction scoped_xn = this->scoped_transaction_factory();
        this->array_apply_functor(name, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, f, __CPP_TRANSPORT_NODE_OUTPUTGROUP_TASK_NAME);

        // sort the output groups into descending order of creation date, so the first element in the
        // list is the most recent data group.
        // This is usually what would be required.
        list.sort(&output_group_helper::comparator<integration_payload>);

        return(list);
      }


    // Enumerate the output groups available from a named output task
    template <typename number>
    std::list< std::shared_ptr< output_group_record<output_payload> > >
    repository_unqlite<number>::enumerate_output_task_content(const std::string& name)
      {
        std::unique_ptr< task_record<number> > record(query_task(name));

        if(record->get_type() != task_record<number>::output)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_OUTPUT << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        std::list< std::shared_ptr< output_group_record<output_payload> > > list;

        array_extraction_functor f = std::bind(&repository_unqlite<number>::array_extract_content_groups<output_payload>, this,
                                               std::placeholders::_1, std::placeholders::_2, &list);

        scoped_transaction scoped_xn = this->scoped_transaction_factory();
        this->array_apply_functor(name, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, f, __CPP_TRANSPORT_NODE_OUTPUTGROUP_TASK_NAME);

        // sort the output groups into descending order of creation date, so the first element in the
        // list is the most recent data group.
        // This is usually what would be required.
        list.sort(&output_group_helper::comparator<output_payload>);

        return(list);
      }


    // ADD CONTENT ASSOCIATED WITH A TASK


    template <typename number>
    std::shared_ptr< integration_writer<number> >
    repository_unqlite<number>::new_integration_task_content(integration_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker)
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
        boost::filesystem::path task_path = output_path / __CPP_TRANSPORT_REPO_TASKFILE_LEAF;
        boost::filesystem::path temp_path = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        typename generic_writer< integration_writer<number> >::callback_group callbacks;
        callbacks.commit = std::bind(&repository_unqlite<number>::close_integration_writer, this, std::placeholders::_1);
        callbacks.abort  = std::bind(&repository_unqlite<number>::abort_integration_writer, this, std::placeholders::_1);

        typename generic_writer< integration_writer<number> >::metadata_group metadata;
        metadata.tags          = tags;
        metadata.creation_time = now;

        typename generic_writer< integration_writer<number> >::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.data   = sql_path;
        paths.log    = log_path;
        paths.task   = task_path;
        paths.temp   = temp_path;

        // integration_writer constructor takes a copy of the integration_task_record
        return std::shared_ptr< integration_writer<number> >(new integration_writer<number>(rec, callbacks, metadata, paths, worker));
      }


    template <typename number>
    std::shared_ptr< derived_content_writer<number> >
    repository_unqlite<number>::new_output_task_content(output_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker)
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
        boost::filesystem::path task_path = output_path / __CPP_TRANSPORT_REPO_TASKFILE_LEAF;
        boost::filesystem::path temp_path = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        typename generic_writer< derived_content_writer<number> >::callback_group callbacks;
        callbacks.commit = std::bind(&repository_unqlite<number>::close_derived_content_writer, this, std::placeholders::_1);
        callbacks.abort  = std::bind(&repository_unqlite<number>::abort_derived_content_writer, this, std::placeholders::_1);

        typename generic_writer< derived_content_writer<number> >::metadata_group metadata;
        metadata.tags          = tags;
        metadata.creation_time = now;

        typename generic_writer< derived_content_writer<number> >::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.log    = log_path;
        paths.task   = task_path;
        paths.temp   = temp_path;

        return std::shared_ptr< derived_content_writer<number> >(new derived_content_writer<number>(rec, callbacks, metadata, paths, worker));
      }


    template <typename number>
    std::shared_ptr< postintegration_writer<number> >
    repository_unqlite<number>::new_postintegration_task_content(postintegration_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker)
      {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        // construct paths for output files and directories
        std::string output_leaf = boost::posix_time::to_iso_string(now);

        boost::filesystem::path output_path = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_TASKOUTPUT_LEAF) / rec->get_name() / output_leaf;
        boost::filesystem::path sql_path     = output_path / __CPP_TRANSPORT_REPO_DATABASE_LEAF;

        // temporary stuff, location not recorded in the database
        boost::filesystem::path log_path    = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path task_path   = output_path / __CPP_TRANSPORT_REPO_TASKFILE_LEAF;
        boost::filesystem::path temp_path   = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        typename generic_writer< postintegration_writer<number> >::callback_group callbacks;
        callbacks.commit        = std::bind(&repository_unqlite<number>::close_postintegration_writer, this, std::placeholders::_1);
        callbacks.abort         = std::bind(&repository_unqlite<number>::abort_postintegration_writer, this, std::placeholders::_1);

        typename generic_writer< postintegration_writer<number> >::metadata_group metadata;
        metadata.tags          = tags;
        metadata.creation_time = now;

        typename generic_writer< postintegration_writer<number> >::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.data   = sql_path;
        paths.log    = log_path;
        paths.task   = task_path;
        paths.temp   = temp_path;

        return std::shared_ptr< postintegration_writer<number> >(new postintegration_writer<number>(rec, callbacks, metadata, paths, worker));
      }


    // WRITER CALLBACKS


    template <typename number>
    template <typename Payload>
    void repository_unqlite<number>::advise_commit(output_group_record<Payload>* group)
      {
        std::ostringstream msg;

        msg << __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_A << " '" << group->get_name() << "' "
            << __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_B << " '" << group->get_task_name() << "'";

        this->message(msg.str());
      }


    template <typename number>
    template <typename Payload>
    void repository_unqlite<number>::advise_commit(postintegration_task_record<number>* rec, output_group_record<Payload>* group)
	    {
        std::ostringstream msg;

        msg << __CPP_TRANSPORT_REPO_WRITEBACK_POSTINT_GROUP_A << " '" << group->get_name() << "' "
	          << __CPP_TRANSPORT_REPO_WRITEBACK_POSTINT_GROUP_B << " '" << rec->get_name() << "' ("
	          << __CPP_TRANSPORT_REPO_WRITEBACK_POSTINT_GROUP_C << " '" << group->get_task_name() << "')";

        this->message(msg.str());
	    }


		template <typename number>
		void repository_unqlite<number>::close_integration_writer(base_writer& gwriter)
			{
        integration_writer<number>& writer = dynamic_cast< integration_writer<number>& >(gwriter);

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
    void repository_unqlite<number>::abort_integration_writer(base_writer& gwriter)
      {
        integration_writer<number>& writer = dynamic_cast< integration_writer<number>& >(gwriter);

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
    void repository_unqlite<number>::close_postintegration_writer(base_writer& gwriter)
      {
        postintegration_writer<number>& writer = dynamic_cast< postintegration_writer<number>& >(gwriter);

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
    void repository_unqlite<number>::abort_postintegration_writer(base_writer& gwriter)
      {
        postintegration_writer<number>& writer = dynamic_cast< postintegration_writer<number>& >(gwriter);

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
		void repository_unqlite<number>::close_derived_content_writer(base_writer& gwriter)
			{
        derived_content_writer<number>& writer = dynamic_cast< derived_content_writer<number>& >(gwriter);

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
		void repository_unqlite<number>::abort_derived_content_writer(base_writer& gwriter)
			{
        derived_content_writer<number>& writer = dynamic_cast< derived_content_writer<number>& >(gwriter);

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
    repository_unqlite<number>::find_integration_task_output(const std::string& name, const std::list<std::string>& tags)
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
    std::string repository_unqlite<number>::export_JSON_package_record(const std::string& name)
      {
        // open a new transaction, if necessary. After this we can assume the database handles are live
        this->begin_transaction();

        // check a suitable record exists
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);

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
        std::string document = unqlite_operations::extract_json(this->db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);

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
        unsigned int A = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);
        unsigned int B = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);
        unsigned int C = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_TASKS_POSTINTEGRATION_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);

        unsigned int total = A + B + C;

        std::string rval;

        if(total == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_RECORD << " '" << name << "'";
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
            if     (A==1) collection = __CPP_TRANSPORT_UNQLITE_TASKS_INTEGRATION_COLLECTION;
            else if(B==1) collection = __CPP_TRANSPORT_UNQLITE_TASKS_OUTPUT_COLLECTION;
            else          collection = __CPP_TRANSPORT_UNQLITE_TASKS_POSTINTEGRATION_COLLECTION;

            rval = unqlite_operations::extract_json(this->db, collection, name, __CPP_TRANSPORT_NODE_RECORD_NAME);
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
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);

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
        std::string document = unqlite_operations::extract_json(this->db, __CPP_TRANSPORT_UNQLITE_DERIVED_PRODUCT_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);

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
        unsigned int count = unqlite_operations::query_count(this->db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);

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
        std::string document = unqlite_operations::extract_json(this->db, __CPP_TRANSPORT_UNQLITE_CONTENT_COLLECTION, name, __CPP_TRANSPORT_NODE_RECORD_NAME);

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
