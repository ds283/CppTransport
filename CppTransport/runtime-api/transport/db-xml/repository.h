//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __repository_H_
#define __repository_H_


#include <string>
#include <sstream>
#include <stdexcept>

#include "transport/manager/instance_manager.h"
#include "transport/models/model.h"
#include "transport/tasks/task.h"
#include "transport/messages_en.h"
#include "transport/exceptions.h"

#include "dbxml/db.h"
#include "dbxml/dbxml/DbXml.hpp"

#include "boost/filesystem/operations.hpp"
#include "exceptions.h"


#define __CPP_TRANSPORT_REPO_ENVIRONMENT_LEAF   "env"
#define __CPP_TRANSPORT_REPO_CONTAINERS_LEAF    "containers"
#define __CPP_TRANSPORT_CNTR_PACKAGES_LEAF      "packages.dbxml"
#define __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF  "integrations.dbxml"

#define __CPP_TRANSPORT_NODE_PACKAGE_SPEC       "package-specification"
#define __CPP_TRANSPORT_NODE_PACKAGE_MODEL      "package-model"
#define __CPP_TRANSPORT_NODE_PACKAGE_ICS        "package-ics"

#define __CPP_TRANSPORT_NODE_TWOPF_SPEC         "twopf-specification"
#define __CPP_TRANSPORT_NODE_THREEPF_SPEC       "threepf-specification"
#define __CPP_TRANSPORT_NODE_INTGRTN_PACKAGE    "integration-package"
#define __CPP_TRANSPORT_NODE_INTGRTN_TASK       "integration-task"


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
        typedef enum { node_storage, document_storage } storage_type;

        // CONSTRUCTOR, DESTRUCTOR

      public:
        //! Open a repository with a specific pathname
        repository(const std::string& path);
        //! Create a repository with a specific pathname
        repository(const std::string& path, const repository_creation_key& key, storage_type type=node_storage);

        //! Close a repository, including the corresponding containers and environment
        ~repository();

        // INTERFACE -- PUSH TASKS TO THE REPOSITORY DATABASE

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the model database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        void write_package(const initial_conditions<number>& ics, const model<number>* m);

        //! Write a threepf integration task to the integration database.
        //! Delegates write_integration_task() to do the work
        void write_integration(const twopf_task<number>& t, const model<number>* m) { this->write_integration_task(t, m, __CPP_TRANSPORT_NODE_TWOPF_SPEC); }
        //! Write a twopf integration task to the integration database
        //! Delegates write_integration_task() to do the work
        void write_integration(const threepf_task<number>& t, const model<number>* m) { this->write_integration_task(t, m, __CPP_TRANSPORT_NODE_THREEPF_SPEC); }

      protected:
        //! Write a generic task to the integration database, using a supplied node tag
        void write_integration_task(const task<number>& t, const model<number>* m, const std::string& node);

        // INTERFACE -- PULL TASKS FROM THE REPOSITORY DATABASE

      public:
        //! Query the database for a named task, and reconstruct it if present
        task<number>& query_task(const std::string& name, typename instance_manager<number>::model_finder finder);

      protected:
        //! Query the database for a named model, returned as an XmlValue
        DbXml::XmlValue get_package_by_name(const std::string& name);
        //! Query the database for a named integration task, returned as an XmlValue
        DbXml::XmlValue get_integration_by_name(const std::string& name);
        //! Given an XmlValue representing an integration schema, extract the associated package name.
        //! A handle to the associated container is not needed explicitly, but is expected to be held open.
        std::string get_package_from_integration(DbXml::XmlValue& value, const std::string& name);
        //! Given an XmlValue representing a package schema, extract the associated model uid.
        //! A handle to the associated container is not needed explicitly, but is expected to be held open.
        std::string get_model_uid_from_package(DbXml::XmlValue& value, const std::string& name);

        // INTERNAL DATA

      protected:
        //! BOOST path to repository root directory
        boost::filesystem::path root_path;
        //! BOOST path to repository environment
        boost::filesystem::path env_path;
        //! BOOST path to containers
        boost::filesystem::path containers_path;
        //! BOOST path to model container
        boost::filesystem::path packages_path;
        //! BOOST path to integrations container
        boost::filesystem::path integrations_path;

        // DATABASE ENVIRONMENT

        //! Berkeley DB XML environment object corresponding to the open repository
        DB_ENV* env;
        //! Berkeley DB XML XmlManager object corresponding to the open repository
        DbXml::XmlManager* mgr;
      };


    // Open a named repository
    template <typename number>
    repository<number>::repository(const std::string& path)
      : env(nullptr), mgr(nullptr)
      {
        root_path = path;
        if(!boost::filesystem::is_directory(root_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_ROOT << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        env_path = root_path / __CPP_TRANSPORT_REPO_ENVIRONMENT_LEAF;
        if(!boost::filesystem::is_directory(env_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_ENV << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        containers_path = root_path / __CPP_TRANSPORT_REPO_CONTAINERS_LEAF;
        if(!boost::filesystem::is_directory(containers_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_CNTR << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        packages_path = containers_path / __CPP_TRANSPORT_CNTR_PACKAGES_LEAF;
        if(!boost::filesystem::is_regular_file(packages_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_PACKAGES << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        integrations_path = containers_path / __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF;
        if(!boost::filesystem::is_regular_file(integrations_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_INTGRTNS << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        int dberr;
        if((dberr = ::db_env_create(&env, 0)) > 0)
          {
            if(env != nullptr) env->close(env, 0);
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_ENV << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        // set up environment to enable logging, transactional support
        // and locking (so multiple processes can access the repository safely)
        u_int32_t env_flags = DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN;
        env->open(env, env_path.string().c_str(), env_flags, 0);

        // set up XmlManager object
        mgr = new DbXml::XmlManager(env, 0);
      }


    // Create a named repository
    template <typename number>
    repository<number>::repository(const std::string& path, const repository_creation_key& key, storage_type type)
      : env(nullptr), mgr(nullptr)
      {
        // check whether root directory already exists
        root_path = path;
        if(boost::filesystem::exists(root_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_ROOT_EXISTS << " '" << path << "'";
            throw std::runtime_error(msg.str());
          }

        env_path = root_path / __CPP_TRANSPORT_REPO_ENVIRONMENT_LEAF;
        containers_path = root_path / __CPP_TRANSPORT_REPO_CONTAINERS_LEAF;
        packages_path = containers_path / __CPP_TRANSPORT_CNTR_PACKAGES_LEAF;
        integrations_path = containers_path / __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF;

        // create directories
        boost::filesystem::create_directories(root_path);
        boost::filesystem::create_directories(env_path);
        boost::filesystem::create_directories(containers_path);

        int dberr;
        if((dberr = ::db_env_create(&env, 0)) > 0)
          {
            if(env != nullptr) env->close(env, 0);
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_ENV << " '" << path << "'";
            throw std::runtime_error(msg.str());
          }

        // set up environment to enable logging, transactional support
        // and locking (so multiple processes can access the repository safely)
        u_int32_t env_flags = DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN;
        env->open(env, env_path.string().c_str(), env_flags, 0);

        // set up XmlManager object
        mgr = new DbXml::XmlManager(env, DbXml::DBXML_ADOPT_DBENV);

        // create database containers
        switch(type)
          {
            case node_storage:
              mgr->setDefaultContainerType(DbXml::XmlContainer::NodeContainer);
              break;

            case document_storage:
              mgr->setDefaultContainerType(DbXml::XmlContainer::WholedocContainer);
              break;

            default:
              assert(false);
          }
        DbXml::XmlContainer packages = this->mgr->createContainer(packages_path.string().c_str());
        DbXml::XmlContainer integrations = this->mgr->createContainer(integrations_path.string().c_str());
      }


    // Destroy a respository object, closing the associated repository
    template <typename number>
    repository<number>::~repository()
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        delete this->mgr;
      }


    // Write a model/initial conditions/parameters combination to the repository
    template <typename number>
    void repository<number>::write_package(const initial_conditions<number>& ics, const model<number>* m)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);
        assert(m != nullptr);

        if(m == nullptr) throw std::runtime_error(__CPP_TRANSPORT_REPO_NULL_MODEL);

        try
          {
            // open database container
            DbXml::XmlContainer packages = this->mgr->openContainer(this->packages_path.string().c_str());

            DbXml::XmlUpdateContext ctx = this->mgr->createUpdateContext();

            // begin XML document representing this model
            DbXml::XmlDocument doc = this->mgr->createDocument();
            doc.setName(ics.get_name());

            DbXml::XmlEventWriter& writer = packages.putDocumentAsEventWriter(doc, ctx);
            writer.writeStartDocument(nullptr, nullptr, nullptr);

            // write root node
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_SPEC), nullptr, nullptr, 0, false);

            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_MODEL), nullptr, nullptr, 0, false);
            m->serialize_xml(writer);
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_MODEL), nullptr, nullptr);

            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_ICS), nullptr, nullptr, 0, false);
            ics.serialize_xml(writer);
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_ICS), nullptr, nullptr);

            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_SPEC), nullptr, nullptr);

            // finalize XML document
            writer.writeEndDocument();
            writer.close();
          }
        catch (DbXml::XmlException& xe)
          {
            if(xe.getExceptionCode() == DbXml::XmlException::UNIQUE_ERROR)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_MODEL_EXISTS << " '" << ics.get_name() << "'";
                throw std::runtime_error(msg.str());
              }
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_INSERT_ERROR << xe.getExceptionCode() << ": '" << xe.what() << "')";
                throw std::runtime_error(msg.str());
              }
          }
      }


    // Write a task to the repository
    template <typename number>
    void repository<number>::write_integration_task(const task<number>& t, const model<number>* m, const std::string& node)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);
        assert(m != nullptr);

        if(m == nullptr) throw std::runtime_error(__CPP_TRANSPORT_REPO_NULL_MODEL);

        try
          {
            // open database container
            DbXml::XmlContainer models = this->mgr->openContainer(this->packages_path.string().c_str());
            DbXml::XmlContainer integrations = this->mgr->openContainer(this->integrations_path.string().c_str());

            // check whether XML document corresponding to our initial_conditions object is in the database
            try
              {
                DbXml::XmlDocument doc = models.getDocument(t.get_ics().get_name());
              }
            catch (DbXml::XmlException& xe)
              {
                if(xe.getExceptionCode() == DbXml::XmlException::DOCUMENT_NOT_FOUND)
                  {
                    this->write_package(t.get_ics(), m);
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_INSERT_ERROR << xe.getExceptionCode() << ": '" << xe.what() << "')";
                    throw std::runtime_error(msg.str());
                  }
              }

            // now insert task information into the integrations database
            DbXml::XmlUpdateContext ctx = this->mgr->createUpdateContext();

            // begin XML document representing this integration
            DbXml::XmlDocument doc = this->mgr->createDocument();
            doc.setName(t.get_name());

            DbXml::XmlEventWriter& writer = integrations.putDocumentAsEventWriter(doc, ctx);
            writer.writeStartDocument(nullptr, nullptr, nullptr);

            // write root node
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(node.c_str()), nullptr, nullptr, 0, false);

            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_PACKAGE), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(t.get_ics().get_name().c_str()), t.get_ics().get_name().length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_PACKAGE), nullptr, nullptr);

            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_TASK), nullptr, nullptr, 0, false);
            t.serialize_xml(writer);
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_TASK), nullptr, nullptr);

            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(node.c_str()), nullptr, nullptr);

            // finalize XML document
            writer.writeEndDocument();
            writer.close();
          }
        catch (DbXml::XmlException& xe)
          {
            if(xe.getExceptionCode() == DbXml::XmlException::UNIQUE_ERROR)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_INTGRTN_EXISTS << " '" << t.get_name() << "'";
                throw std::runtime_error(msg.str());
              }
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_INSERT_ERROR << xe.getExceptionCode() << ": '" << xe.what() << "')";
                throw std::runtime_error(msg.str());
              }
          }
      }


    // Query the database for a named task
    template <typename number>
    task<number>& repository<number>::query_task(const std::string& name, typename instance_manager<number>::model_finder finder)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        // open handles to database containers
        DbXml::XmlContainer integrations = this->mgr->openContainer(this->integrations_path.string().c_str());
        DbXml::XmlContainer models = this->mgr->openContainer(this->packages_path.string().c_str());

        // lookup record for the task
        DbXml::XmlValue task = this->get_integration_by_name(name);

        // extract name of record for initial conditions/parameters and lookup the corresponding record
        std::string package_name = this->get_package_from_integration(task, name);
        DbXml::XmlValue package = this->get_package_by_name(package_name);

        // extract uid for model
        std::string model_uid = this->get_model_uid_from_package(package, package_name);

        std::cerr << "Extracted model uid = " << model_uid << std::endl;

        // use the supplied finder to recover the model
        // throws an exception if the model cannot be found, which should be caught higher up in the task handler
        model<number>* model = finder(model_uid);

        std::cerr << "Matched model: name = " << model->get_name() << ", " << model->get_author() << std::endl;
      }


    // Extract package name from an integration schema
    // -- NOTE THAT THE CORRESPONDING CONTAINER IS EXPECTED TO BE OPEN
    template <typename number>
    std::string repository<number>::get_package_from_integration(DbXml::XmlValue& value, const std::string& name)
      {
        // run a query to find the XML record for the corresponding initial conditions/parameter package
        std::ostringstream query;
        query << __CPP_TRANSPORT_XQUERY_VALUES << "(" << __CPP_TRANSPORT_XQUERY_SEPARATOR
          << __CPP_TRANSPORT_XQUERY_WILDCARD << __CPP_TRANSPORT_XQUERY_SEPARATOR
          << __CPP_TRANSPORT_NODE_INTGRTN_PACKAGE << ")";

        // create a context for the query
        DbXml::XmlQueryContext ctx = this->mgr->createQueryContext();

        // compile the query
        DbXml::XmlQueryExpression expr = this->mgr->prepare(query.str(), ctx);

        // execute it and obtain a result set
        DbXml::XmlResults results = expr.execute(value, ctx);

        if(results.size() != 1)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_BADLY_FORMED_INTGRTN << " '" << name << "'" << __CPP_TRANSPORT_RUN_REPAIR;
            throw runtime_exception(runtime_exception::BADLY_FORMED_XML, msg.str());
          }

        DbXml::XmlValue node;
        results.next(node);

        return(node.asString());
      }


    // Extract uid name from a package schema
    // -- NOTE THAT THE CORRESPONDING CONTAINER IS EXPECTED TO BE OPEN
    template <typename number>
    std::string repository<number>::get_model_uid_from_package(DbXml::XmlValue& value, const std::string& name)
      {
        std::ostringstream query;
        query << __CPP_TRANSPORT_XQUERY_VALUES << "(" << __CPP_TRANSPORT_XQUERY_SEPARATOR
          << __CPP_TRANSPORT_NODE_PACKAGE_SPEC << __CPP_TRANSPORT_XQUERY_SEPARATOR
          << __CPP_TRANSPORT_NODE_PACKAGE_MODEL << ")";

        // create a context for the query
        DbXml::XmlQueryContext ctx = this->mgr->createQueryContext();

        // compile the query
        DbXml::XmlQueryExpression expr = this->mgr->prepare(query.str(), ctx);

        // execute it and obtain a result set
        DbXml::XmlResults results = expr.execute(value, ctx);

        if(results.size() != 1)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_BADLY_FORMED_PACKAGE << " '" << name << "'" << __CPP_TRANSPORT_RUN_REPAIR;
            throw runtime_exception(runtime_exception::BADLY_FORMED_XML, msg.str());
          }

        DbXml::XmlValue node;
        results.next(node);

        return(model_delegate::extract_uid(this->mgr, node));
      }


    template <typename number>
    DbXml::XmlValue repository<number>::get_integration_by_name(const std::string& name)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        // obtain handles to database container
        DbXml::XmlContainer integrations = this->mgr->openContainer(this->integrations_path.string().c_str());

        DbXml::XmlDocument integration_document;
        try
          {
            // find XML document corresponding to our integration name; will throw a DOCUMENT_NOT_FOUND EXCEPTION if it does not exist
            integration_document = integrations.getDocument(name);
          }
        catch (DbXml::XmlException& xe)
          {
            if(xe.getExceptionCode() == DbXml::XmlException::DOCUMENT_NOT_FOUND)
              {
                throw runtime_exception(runtime_exception::TASK_NOT_FOUND, name);
              }
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_QUERY_ERROR << xe.getExceptionCode() << ": " << xe.what() << "')";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
              }
          }

        DbXml::XmlValue integration(integration_document);

        return(integration);
      }


    template <typename number>
    DbXml::XmlValue repository<number>::get_package_by_name(const std::string& name)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        // obtain handles to database container
        DbXml::XmlContainer packages = this->mgr->openContainer(this->packages_path.string().c_str());

        DbXml::XmlDocument package_document;
        try
          {
            // find XML document corresponding to our model name; will throw a DOCUMENT_NOT_FOUND EXCEPTION if it does not exist
            package_document = packages.getDocument(name);
          }
        catch (DbXml::XmlException& xe)
          {
            if(xe.getExceptionCode() == DbXml::XmlException::DOCUMENT_NOT_FOUND)
              {
                throw runtime_exception(runtime_exception::MODEL_NOT_FOUND, name);
              }
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_QUERY_ERROR << xe.getExceptionCode() << ": " << xe.what() << "')";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
              }
          }

        DbXml::XmlValue package(package_document);

        return(package);
      }


  }   // namespace transport



#endif //__repository_H_
