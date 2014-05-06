//
// Created by David Seery on 04/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __repository_unqlite_H_
#define __repository_unqlite_H_

#include <string>
#include <sstream>

#include "transport-runtime-api/manager/repository.h"

#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "transport-runtime-api/unqlite/unqlite_serializable.h"
#include "transport-runtime-api/unqlite/unqlite_operations.h"

#include "unqlite/unqlite.h"

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

        // CONSTRUCTOR, DESTRUCTOR

      public:
        //! Open a repository with a specific pathname
        repository_unqlite(const std::string& path);
        //! Create a repository with a specific pathname
        repository_unqlite(const std::string& path, const repository_creation_key& key);

        //! Close a repository, including any corresponding containers
        ~repository_unqlite();


        // INTERFACE -- PATHS (implements a 'repository' interface)


      public:
        //! Get path to root of repository
        const boost::filesystem::path& get_root_path() { return(this->root_path); }


        // INTERFACE -- PUSH TASKS TO THE REPOSITORY DATABASE (implements a 'repository' interface)


      public:
        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        void write_package(const initial_conditions<number>& ics, const model<number>* m);

        //! Write a threepf integration to the integration database.
        //! Delegates write_integration_task() to do the work.
        void write_integration(const threepf_task<number>& t, const model<number>* m)
          { this->write_integration_task(t, m, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION); }

        //! Write a twopf integration to the integration database.
        //! Delegates write_integration_task() to do the work.
        void write_integration(const twopf_task<number>& t, const model<number>* m)
          { this->write_integration_task(t, m, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION); }

      protected:
        //! Write a generic integration task to the database, using a supplied node tag
        void write_integration_task(const task<number>& t, const model<number>* m, const std::string& collection);


        // INTERFACE -- PULL TASKS FROM THE REPOSITORY DATABASE (implements a 'repository' interface)


      public:
        //! Query the database for a named task, and reconstruct it if present
        task<number>* query_task(const std::string& name, model<number>*& m, typename instance_manager<number>::model_finder finder);


        // EXTRA FUNCTIONS, NOT DEFINED BY INTERFACE

        //! Extract the JSON document for a named package
        std::string extract_package_document(const std::string& name);
        //! Extract the JSON document for a named integration
        std::string extract_integration_document(const std::string& name);


        // INTERFACE -- ADD OUTPUT TO TASKS (implements a 'repository' interface)


      public:
        //! Insert a record for new twopf output in the task database, and set up paths to a suitable data container.
        //! Delegates insert_output to do the work.
        integration_container integration_new_output(twopf_task<number>* tk,
                                                     const std::string& backend, unsigned int worker);
        //! Insert a record for a new threepf output in the task database, and set up paths to a suitable data container.
        //! Delegats insert_output to do the work.
        integration_container integration_new_output(threepf_task<number>* tk,
                                                     const std::string& backend, unsigned int worker);

      protected:
        //! Insert a record for a specified task, in a specified container
        integration_container insert_output(const std::string& collection, task<number>* tk, const std::string& backend, unsigned int worker);
        //! Compute list of used serial numbers from an output record
        std::list<unsigned int> compute_used_serial_numbers(unqlite_value* recs);


        // INTERNAL DATA

      private:
        //! BOOST path to the repository root directory
        boost::filesystem::path root_path;

        //! BOOST path to data containers (parent directory for the following containers)
        boost::filesystem::path containers_path;
        //! BOOST path to container for packages
        boost::filesystem::path packages_path;
        //! BOOST path to container for integrations
        boost::filesystem::path integrations_path;


        // UNQLITE DATABASE HANDLES


      private:
        unqlite* package_db;
        unqlite* integration_db;
      };


    // IMPLEMENTATION


    // Open a named repository
    template <typename number>
    repository_unqlite<number>::repository_unqlite(const std::string& path)
      : package_db(nullptr), integration_db(nullptr)
      {
        root_path = path;

        // supplied path should be a directory which exists
        if(!boost::filesystem::is_directory(root_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_ROOT << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        // database containers should be present in an existing directory
        containers_path = root_path / __CPP_TRANSPORT_REPO_CONTAINERS_LEAF;
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

        // integrations database should be present inside the container direcotry
        integrations_path = containers_path / __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF;
        if(!boost::filesystem::is_regular_file(integrations_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_INTGRTNS << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        // open containers for reading/writings
        int err;

        if((err = unqlite_open(&package_db, packages_path.c_str(), UNQLITE_OPEN_READWRITE)) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_PKG << " '" << packages_path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        if((err = unqlite_open(&integration_db, integrations_path.c_str(), UNQLITE_OPEN_READWRITE)) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_INTGN << " '" << integrations_path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // ensure default collections are present within each conatiners
        unqlite_operations::ensure_collection(package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);
        unqlite_operations::ensure_collection(integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION);
        unqlite_operations::ensure_collection(integration_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION);
      }


    // Create a named repository
    template <typename number>
    repository_unqlite<number>::repository_unqlite(const std::string& path, const repository_creation_key& key)
      : package_db(nullptr), integration_db(nullptr)
      {
        // check whether root directory for the repository already exists -- it shouldn't
        root_path = path;

        if(boost::filesystem::exists(root_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_ROOT_EXISTS << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        containers_path = root_path / __CPP_TRANSPORT_REPO_CONTAINERS_LEAF;
        packages_path = containers_path / __CPP_TRANSPORT_CNTR_PACKAGES_LEAF;
        integrations_path = containers_path / __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF;

        // create directory structure
        boost::filesystem::create_directories(root_path);
        boost::filesystem::create_directories(containers_path);

        // create containers
        int err;

        if((err = unqlite_open(&package_db, packages_path.c_str(), UNQLITE_OPEN_CREATE)) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_PKG << " '" << packages_path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        if((err = unqlite_open(&integration_db, integrations_path.c_str(), UNQLITE_OPEN_CREATE)) != UNQLITE_OK)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_INTGN << " '" << integrations_path << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // create default collection within each conatiners
        unqlite_operations::create_collection(package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION);
        unqlite_operations::create_collection(integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION);
        unqlite_operations::create_collection(integration_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION);
      }


    // Destroy a repository_unqlite object, closing the associated repository
    template <typename number>
    repository_unqlite<number>::~repository_unqlite()
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);

        unqlite_close(this->package_db);
        unqlite_close(this->integration_db);
      }


    // Write a model/initial-conditions/parameters combination to the repository
    template <typename number>
    void repository_unqlite<number>::write_package(const initial_conditions<number>& ics, const model<number>* m)
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);
        assert(m != nullptr);

        if(this->package_db == nullptr || this->integration_db == nullptr)
          throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_MISSING_DB);
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

        writer.write_value(__CPP_TRANSPORT_NODE_PKGDATA_NAME, m->get_name());
        writer.write_value(__CPP_TRANSPORT_NODE_PKGDATA_AUTHOR, m->get_author());
        writer.write_value(__CPP_TRANSPORT_NODE_PKGDATA_TAG, m->get_tag());

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        writer.write_value(__CPP_TRANSPORT_NODE_PKGDATA_CREATED, boost::posix_time::to_simple_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_PKGDATA_EDITED, boost::posix_time::to_simple_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_PKGDATA_RUNTIMEAPI, __CPP_TRANSPORT_RUNTIME_API_VERSION);

        writer.end_element(__CPP_TRANSPORT_NODE_PACKAGE_DATA);

        // commit initial conditions
        writer.start_node(__CPP_TRANSPORT_NODE_PACKAGE_ICS);
        ics.serialize(writer);
        writer.end_element(__CPP_TRANSPORT_NODE_PACKAGE_ICS);

        // insert this record in the package database
        unqlite_operations::store(this->package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, writer.get_contents());
      }


    // Write a task to the repository
    template <typename number>
    void repository_unqlite<number>::write_integration_task(const task<number>& t, const model<number>* m, const std::string& collection)
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);
        assert(m != nullptr);

        if(this->package_db == nullptr || this->integration_db == nullptr)
          throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_MISSING_DB);
        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        // check if an integration task with this name already exists
        unsigned int twopf_count   = unqlite_operations::query_count(this->integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, t.get_name(), __CPP_TRANSPORT_NODE_INTGRTN_NAME);
        unsigned int threepf_count = unqlite_operations::query_count(this->integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, t.get_name(), __CPP_TRANSPORT_NODE_INTGRTN_NAME);
        if(twopf_count + threepf_count > 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_INTGRTN_EXISTS << " '" << t.get_name() << "'";
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
        writer.write_value(__CPP_TRANSPORT_NODE_INTGRTN_NAME, t.get_name());

        // commit data block
        writer.start_node(__CPP_TRANSPORT_NODE_INTGRTN_DATA);

        writer.write_value(__CPP_TRANSPORT_NODE_INTDATA_PACKAGE, t.get_ics().get_name());

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        writer.write_value(__CPP_TRANSPORT_NODE_INTDATA_CREATED, boost::posix_time::to_simple_string(now));
        writer.write_value(__CPP_TRANSPORT_NODE_INTDATA_EDITED, boost::posix_time::to_simple_string(now));

        writer.write_value(__CPP_TRANSPORT_NODE_INTDATA_RUNTIMEAPI, __CPP_TRANSPORT_RUNTIME_API_VERSION);

        writer.end_element(__CPP_TRANSPORT_NODE_INTGRTN_DATA);

        // commit task block
        writer.start_node(__CPP_TRANSPORT_NODE_INTGRTN_TASK);
        t.serialize(writer);
        writer.end_element(__CPP_TRANSPORT_NODE_INTGRTN_TASK);

        // write empty array of output blocks: will be populated when integrations are run
        writer.start_array(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT, true);
        writer.end_array(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);

        // insert this record in the task database
        unqlite_operations::store(this->integration_db, collection, writer.get_contents());
      }


    // Query the database for a named task
    template <typename number>
    task<number>* repository_unqlite<number>::query_task(const std::string& name, model<number>*& m, typename instance_manager<number>::model_finder finder)
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);
        assert(m != nullptr);

        if(this->package_db == nullptr || this->integration_db == nullptr)
          throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_MISSING_DB);
        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        // check if a suitable record exists
        unsigned int twopf_count   = unqlite_operations::query_count(this->integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);
        unsigned int threepf_count = unqlite_operations::query_count(this->integration_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);

        if(twopf_count + threepf_count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
        else if(twopf_count + threepf_count > 1)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_TASK << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        std::string collection;
        if(twopf_count == 1) collection = __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION;
        else                 collection = __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION;

        unqlite_vm* vm = nullptr;
        unqlite_value* recs = unqlite_operations::query(this->integration_db, collection, name, __CPP_TRANSPORT_NODE_INTGRTN_NAME, &vm);

        unqlite_vm_release(vm);
      }


    // Extract package database record as a JSON document
    template <typename number>
    std::string repository_unqlite<number>::extract_package_document(const std::string& name)
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);

        if(this->package_db == nullptr || this->integration_db == nullptr)
          throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_MISSING_DB);

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

        return(document);
      }


    // Extract integration task database record as a JSON document
    template <typename number>
    std::string repository_unqlite<number>::extract_integration_document(const std::string& name)
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);

        if(this->package_db == nullptr || this->integration_db == nullptr)
          throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_MISSING_DB);

        // check a suitable record exists
        unsigned int twopf_count   = unqlite_operations::query_count(this->integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);
        unsigned int threepf_count = unqlite_operations::query_count(this->integration_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);

        std::string rval;

        if(twopf_count + threepf_count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
        else if(twopf_count + threepf_count > 1)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_TASK << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
        else
          {
            std::string collection;
            if(twopf_count == 1) collection = __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION;
            else                 collection = __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION;

            rval = unqlite_operations::extract_json(this->package_db, collection, name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);
          }

        return(rval);
      }


    // Add output for a twopf task
    template <typename number>
    typename repository<number>::integration_container
    repository_unqlite<number>::integration_new_output(twopf_task<number>* tk,
                                                       const std::string& backend, unsigned int worker)
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);
        assert(tk != nullptr);

        if(this->package_db == nullptr || this->integration_db == nullptr)
          throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_MISSING_DB);
        if(tk == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_TASK);

        // check this task name exists in the database
        unsigned int count = unqlite_operations::query_count(this->integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, tk->get_name(), __CPP_TRANSPORT_NODE_INTGRTN_NAME);
        if(count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // insert a new output record, and return the corresponding integration_container handle
        typename repository<number>::integration_container ctr = this->insert_output(__CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, tk, backend, worker);

        unqlite_operations::update_task_edit_time(this->integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, tk->get_name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);

        return(ctr);
      }


    // Add output for a threepf task
    template <typename number>
    typename repository<number>::integration_container
    repository_unqlite<number>::integration_new_output(threepf_task<number>* tk,
                                                       const std::string& backend, unsigned int worker)
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);
        assert(tk != nullptr);

        if(this->package_db == nullptr || this->integration_db == nullptr)
          throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_MISSING_DB);
        if(tk == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_TASK);

        // check this task name exists in the database
        unsigned int count = unqlite_operations::query_count(this->integration_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, tk->get_name(), __CPP_TRANSPORT_NODE_INTGRTN_NAME);
        if(count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // insert a new output record, and return the corresponding integration_container handle
        typename repository<number>::integration_container ctr = this->insert_output(__CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, tk, backend, worker);

        unqlite_operations::update_task_edit_time(this->integration_db, __CPP_TRANSPORT_UNQLITE_THREEPFPF_COLLECTION, tk->get_name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);

        return(ctr);
      }


    // Add output for a specified task
    // We are allowed to assume that all handles exist, tk is not null, and the task exists in the database collection specified



    // Compute list of used serial numbers

    namespace
      {

        int extract_serial_numbers(unqlite_value* key, unqlite_value* value, void* handle)
          {
            std::list<unsigned int>* serial_numbers = static_cast< std::list<unsigned int>* >(handle);

            if(value == nullptr || !unqlite_value_is_json_object(value)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

            unqlite_value* serial = unqlite_array_fetch(value, __CPP_TRANSPORT_NODE_OUTPUT_ID, -1);

            if(serial == nullptr || !unqlite_value_is_int(serial)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

            int sn = unqlite_value_to_int(serial);

            if(sn < 0) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

            serial_numbers->push_back(static_cast<unsigned int>(sn));

            return(UNQLITE_OK);
          }

        std::list<unsigned int> compute_used_serial_numbers(unqlite_value* recs, const std::string& name)
          {
            if(recs == nullptr || !unqlite_value_is_json_array(recs)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

            if(unqlite_array_count(recs) != 1)   // shouldn't happen because checked for above
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_DUPLICATE_TASK << " '" << name << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
              }

            // get JSON object representing our record; this is the only component of the output array
            unqlite_value* rec = unqlite_array_fetch(recs, "1", -1);

            if(rec == nullptr || !unqlite_value_is_json_object(rec)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

            // get array corresponding to the output list
            unqlite_value* output = unqlite_array_fetch(rec, __CPP_TRANSPORT_NODE_INTGRTN_OUTPUT, -1);

            if(output == nullptr || !unqlite_value_is_json_array(recs)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

            // walk over output list, getting serial numbers
            std::list<unsigned int> serial_numbers;

            int err = unqlite_array_walk(output, extract_serial_numbers, &serial_numbers);

            return(serial_numbers);
          }

      }   // anonymous namespace


    template <typename number>
    typename repository<number>::integration_container
    repository_unqlite<number>::insert_output(const std::string& collection, task<number>* tk, const std::string& backend, unsigned int worker)
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);
        assert(tk != nullptr);

        // check if a suitable record exists
        unsigned int twopf_count   = unqlite_operations::query_count(this->integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);
        unsigned int threepf_count = unqlite_operations::query_count(this->integration_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);

        if(twopf_count + threepf_count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
        else if(twopf_count + threepf_count > 1)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_TASK << " '" << name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        std::string collection;
        if(twopf_count == 1) collection = __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION;
        else                 collection = __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION;

        unqlite_vm* vm = nullptr;
        unqlite_value* recs = unqlite_operations::query(this->integration_db, collection, tk->get_name(), __CPP_TRANSPORT_NODE_INTGRTN_NAME, &vm);

        // find serial numbers for all output groups associated with this task
        std::list<unsigned int> serial_numbers = compute_used_serial_numbers(recs, tk->get_name());

        // allocate a new serial number

        unsigned int serial_number;
        for(serial_number = 0;
            serial_number < serial_numbers.size() && std::find(serial_numbers.begin(), serial_numbers.end(), serial_number) != serial_numbers.end();
            serial_number++)
          ;

        if(serial_number > serial_numbers.size()) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NO_SERIALNO);

        unqlite_vm_release(vm);
      }


    // FACTORY FUNCTIONS TO BUILD A REPOSITORY


    template <typename number>
    repository<number>* repository_factory(const std::string& path)
      {
        return new repository_unqlite<number>(path);
      }


    template <typename number>
    repository<number>* repository_factory(const std::string& path, const repository_creation_key& key)
      {
        return new repository_unqlite<number>(path, key);
      }


  }   // namespace transport


#endif //__repository_unqlite_H_