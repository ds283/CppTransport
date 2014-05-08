//
// Created by David Seery on 04/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __repository_unqlite_H_
#define __repository_unqlite_H_

#include <string>
#include <sstream>
#include <list>
#include <functional>
#include <utility>

#include "transport-runtime-api/manager/repository.h"

#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "transport-runtime-api/unqlite/unqlite_serializable.h"
#include "transport-runtime-api/unqlite/unqlite_operations.h"

#include "unqlite/unqlite.h"


#define __CPP_TRANSPORT_UNQLITE_RECORD_ID "__id"


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
        //! Delegates insert_output to do the work.
        integration_container integration_new_output(threepf_task<number>* tk,
                                                     const std::string& backend, unsigned int worker);


		    // INTERNAL UTILITY FUNCTIONS

      protected:

		    //! Convert a named database task into a serialization reader, returned as a pointer.
		    //! It's up to the calling function to destroy the pointer which is returned.
        unqlite_serialization_reader* deserialize_task(const std::string& name, int& unqlite_id);

		    //! Convert a named database package into a serialization reader, returned as a pointer.
		    //! It's up to the calling function to destroy the pointer which is returned.
		    unqlite_serialization_reader* deserialize_package(const std::string& name, int& unqlite_id);

        //! Insert a record for a specified task, in a specified container.
        integration_container insert_output(const std::string& collection, task<number>* tk, const std::string& backend, unsigned int worker);

        //! Allocate a new serial number based on the list of used serial numbers from an output record
        unsigned int allocate_new_serial_number(unqlite_serialization_reader& reader);


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
        writer.end_element(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);

        // insert this record in the task database
        unqlite_operations::store(this->integration_db, collection, writer.get_contents());
      }


		// Query the database for a named task, returned as a serialization_reader
		template <typename number>
		unqlite_serialization_reader* repository_unqlite<number>::deserialize_task(const std::string& name, int& unqlite_id)
			{
		    assert(this->package_db != nullptr);
		    assert(this->integration_db != nullptr);

		    if(this->package_db == nullptr || this->integration_db == nullptr)
			    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_MISSING_DB);

		    // check if a suitable record exists
		    unqlite_vm* vm_twopf   = nullptr;
		    unqlite_vm* vm_threepf = nullptr;

		    unqlite_value* twopf_recs   = unqlite_operations::query(this->integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);
		    unqlite_value* threepf_recs = unqlite_operations::query(this->integration_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, name, __CPP_TRANSPORT_NODE_INTGRTN_NAME);

		    if(twopf_recs == nullptr || threepf_recs == nullptr || !unqlite_value_is_json_array(twopf_recs) || !unqlite_value_is_json_array(threepf_recs))
			    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

		    unsigned int twopf_count   = static_cast<unsigned int>(unqlite_array_count(twopf_recs));
		    unsigned int threepf_count = static_cast<unsigned int>(unqlite_array_count(threepf_recs));

		    if(twopf_count + threepf_count == 0)
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << name << "'";
		        throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }
		    else if(twopf_count + threepf_count > 1)
			    {
		        std::ostringstream msg;
		        msg << __CPP_TRANSPORT_REPO_DUPLICATE_TASK << " '" << name << "'" __CPP_TRANSPORT_RUN_REPAIR;
		        throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
			    }

		    unqlite_value* recs = nullptr;

		    if(twopf_count == 1) recs = twopf_recs;
		    else                 recs = threepf_recs;

		    if(unqlite_array_count(recs) != 1)   // shouldn't happen because checked for above
			    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

		    // get JSON object representing our record; this is the only component of the output array
		    unqlite_value* rec = unqlite_array_fetch(recs, "1", -1);

		    // convert this JSON object to a serialization reader
		    unqlite_serialization_reader* reader = new unqlite_serialization_reader(rec);

				// extract internal UnQLite ID for this record
				unqlite_value* id = unqlite_array_fetch(rec, __CPP_TRANSPORT_UNQLITE_RECORD_ID, -1);

				if(id == nullptr || !unqlite_value_is_int(id))
					throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_NO_ID);

				unqlite_id = unqlite_value_to_int(id);

				unqlite_vm_release(vm_twopf);
				unqlite_vm_release(vm_threepf);

				return(reader);
			}


    // Query the database for a named package, returned as a serialization_reader
    template <typename number>
    unqlite_serialization_reader* repository_unqlite<number>::deserialize_package(const std::string& name, int& unqlite_id)
	    {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);

        if(this->package_db == nullptr || this->integration_db == nullptr)
	        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_MISSING_DB);

        // check if a suitable record exists
        unqlite_vm* vm = nullptr;

        unqlite_value* recs = unqlite_operations::query(this->package_db, __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION, name, __CPP_TRANSPORT_NODE_PACKAGE_NAME);

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
	        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

        // get JSON object representing our record; this is the only component of the output array
        unqlite_value* rec = unqlite_array_fetch(recs, "1", -1);

        // convert this JSON object to a serialization reader
        unqlite_serialization_reader* reader = new unqlite_serialization_reader(rec);

        // extract internal UnQLite ID for this record
        unqlite_value* id = unqlite_array_fetch(rec, __CPP_TRANSPORT_UNQLITE_RECORD_ID, -1);

        if(id == nullptr || !unqlite_value_is_int(id))
	        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_NO_ID);

        unqlite_id = unqlite_value_to_int(id);

        unqlite_vm_release(vm);

        return(reader);
	    }


    // Query the database for a named task, which is reconstructed and returned as a task<> object
    template <typename number>
    task<number>* repository_unqlite<number>::query_task(const std::string& name, model<number>*& m, typename instance_manager<number>::model_finder finder)
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);
        assert(m != nullptr);

        if(this->package_db == nullptr || this->integration_db == nullptr)
          throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_MISSING_DB);
        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

		    // get serialization_reader for the named task record
		    int task_id = 0;
		    unqlite_serialization_reader* task_reader = this->deserialize_task(name, task_id);

		    // extract data:
        task_reader->start_node(__CPP_TRANSPORT_NODE_INTGRTN_DATA);

				// extract the model/initial-conditions/parameters package corresponding to this task
        std::string package_name;
		    task_reader->read_value(__CPP_TRANSPORT_NODE_INTDATA_PACKAGE, name);

		    // get serialization_reader for the named package
		    int package_id = 0;
		    unqlite_serialization_reader* package_reader = this->deserialize_package(name, package_id);

		    // extract UID for model
        std::string uid;
		    package_reader->read_value(__CPP_TRANSPORT_NODE_PACKAGE_MODELUID, uid);

		    // use the supplied finder to recover a model object for this UID
		    m = finder(uid);

		    // move the package serialization reader to the initial conditions block, and use it to
		    // reconstruct a initial_conditions<> object
		    package_reader->start_node(__CPP_TRANSPORT_NODE_PACKAGE_ICS);

		    package_reader->push_bookmark();
		    initial_conditions<number> ics = initial_conditions::deserialize(package_reader, m, package_name);
		    package_reader->pop_bookmark();

		    package_reader->end_element(__CPP_TRANSPORT_NODE_PACKAGE_ICS);

		    // reset read data from the task record, and use it to reconstruct
		    // a task<> object
				task_reader->reset();

		    task_reader->push_bookmark();
		    task<number> tk = task::deserialize(task_reader, ics, m, name);
		    task_reader->pop_bookmark();

		    task_reader->end_element(__CPP_TRANSPORT_NODE_INTGRTN_DATA);

		    delete task_reader;
		    delete package_reader;

		    task<number>* rval = new task<number>(tk);
        return(rval);
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
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_TASK << " '" << name << "'" __CPP_TRANSPORT_RUN_REPAIR;
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

        return(ctr);
      }


    // Add output for a specified task
    // We are allowed to assume that all handles exist, tk is not null, and the task exists in the database collection specified


    template <typename number>
    typename repository<number>::integration_container
    repository_unqlite<number>::insert_output(const std::string& collection, task<number>* tk, const std::string& backend, unsigned int worker)
      {
        assert(this->package_db != nullptr);
        assert(this->integration_db != nullptr);
        assert(tk != nullptr);

        // check if a suitable record exists
		    unqlite_vm* vm_twopf   = nullptr;
		    unqlite_vm* vm_threepf = nullptr;

		    unqlite_value* twopf_recs   = unqlite_operations::query(this->integration_db, __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION, tk->get_name(), __CPP_TRANSPORT_NODE_INTGRTN_NAME);
		    unqlite_value* threepf_recs = unqlite_operations::query(this->integration_db, __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION, tk->get_name(), __CPP_TRANSPORT_NODE_INTGRTN_NAME);

		    if(twopf_recs == nullptr || threepf_recs == nullptr || !unqlite_value_is_json_array(twopf_recs) || !unqlite_value_is_json_array(threepf_recs))
			    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

		    unsigned int twopf_count   = static_cast<unsigned int>(unqlite_array_count(twopf_recs));
		    unsigned int threepf_count = static_cast<unsigned int>(unqlite_array_count(threepf_recs));

        if(twopf_count + threepf_count == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }
        else if(twopf_count + threepf_count > 1)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_DUPLICATE_TASK << " '" << tk->get_name() << "'" __CPP_TRANSPORT_RUN_REPAIR;
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

		    unqlite_value* recs = nullptr;

        if(twopf_count == 1) recs = twopf_recs;
        else                 recs = threepf_recs;

        if(unqlite_array_count(recs) != 1)   // shouldn't happen because checked for above
	        throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);

        // get JSON object representing our record; this is the only component of the output array
        unqlite_value* rec = unqlite_array_fetch(recs, "1", -1);

		    // convert this JSON object to a serialization reader
		    unqlite_serialization_reader reader(rec);

		    // allocate a new serial number
		    unsigned int serial_number = this->allocate_new_serial_number(reader);

        unqlite_vm_release(vm_twopf);
		    unqlite_vm_release(vm_threepf);
      }


    unsigned int repository_unqlite::allocate_new_serial_number(unqlite_serialization_reader& reader)
	    {
				// populate list of serial numbers
        std::list<unsigned int> serial_numbers;

		    // reset reader to the beginning
		    reader.reset();

		    // start reading from integration output block
		    unsigned int entries = reader.start_array(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);
		    for(unsigned int i = 0; i < entries; i++)
			    {
				    reader.start_node("");  // name doesn't matter; array elements have no name
				    unsigned int sn;
				    reader.read_value(__CPP_TRANSPORT_NODE_OUTPUT_ID, sn);
				    serial_numbers.push_back(sn);
				    reader.end_element("");
			    }

        unsigned int serial_number;

        for(serial_number = 0;
            serial_number < serial_numbers.size() && std::find(serial_numbers.begin(), serial_numbers.end(), serial_number) != serial_numbers.end();
            serial_number++)
	        ;

        if(serial_number > serial_numbers.size()) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NO_SERIALNO);

        return(serial_number);
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
