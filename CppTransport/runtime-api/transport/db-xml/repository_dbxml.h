//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __repository_dbxml_H_
#define __repository_dbxml_H_

#include <string>
#include <sstream>

#include "transport/manager/repository.h"

#include "transport/version.h"
#include "transport/messages_en.h"
#include "transport/exceptions.h"

#include "transport/db-xml/dbxml_operations.h"


namespace transport
  {
    // forward-declare 'key' class used to create repositories
    // the complete declaration is in a separate file,
    // which must be included to allow creation of repositories
    class repository_creation_key;

    // class 'repository_dbxml' implements the 'repository' interface using a DBXML backend
    template <typename number>
    class repository_dbxml: public repository<number>
      {

      // CONSTRUCTOR, DESTRUCTOR

        public:
      //! Open a repository with a specific pathname
      repository_dbxml(const std::string& path, bool recovery=false);
      //! Create a repository with a specific pathname
      repository_dbxml(const std::string& path, const repository_creation_key& key, typename repository<number>::storage_type type=repository<number>::node_storage);

      //! Close a repository, including the corresponding containers and environment
      ~repository_dbxml();


      // INTERFACE -- PATHS

      //! Get path to root of repository
      const boost::filesystem::path& get_root_path() { return(this->root_path); }

      // INTERFACE -- PUSH TASKS TO THE REPOSITORY DATABASE

      //! Write a 'model/initial conditions/parameters' combination (a 'package') to the model database.
      //! No combination with the supplied name should already exist; if it does, this is considered an error.
      void write_package(const initial_conditions<number>& ics, const model<number>* m);

      //! Write a threepf integration task to the integration database.
      //! Delegates write_integration_task() to do the work
      void write_integration(const twopf_task<number>& t, const model<number>* m) { this->write_integration_task(t, m, __CPP_TRANSPORT_NODE_TWOPF_ROOT); }
      //! Write a twopf integration task to the integration database
      //! Delegates write_integration_task() to do the work
      void write_integration(const threepf_task<number>& t, const model<number>* m) { this->write_integration_task(t, m, __CPP_TRANSPORT_NODE_THREEPF_ROOT); }

        protected:
      //! Write a generic task to the integration database, using a supplied node tag
      void write_integration_task(const task<number>& t, const model<number>* m, const std::string& root_node_name);

      // INTERFACE -- PULL TASKS FROM THE REPOSITORY DATABASE

        public:
      //! Query the database for a named task, and reconstruct it if present
      task<number>* query_task(const std::string& name, model<number>*& m, typename instance_manager<number>::model_finder finder);

      //! Extract the XML document for a named package
      std::string extract_package_document(const std::string& name);
      //! Extract the XML document for a named integration
      std::string extract_integration_document(const std::string& name);


      // INTERFACE -- ADD OUTPUT TO TASKS

        public:
      //! Insert a record for new twopf output in the task XML database, and set up paths to a suitable SQL container
      typename repository<number>::integration_container integration_new_output(twopf_task<number>* tk, unsigned int worker);
      //! Insert a record for new threepf output in the task XML database, and set up paths to a suitable SQL container
      typename repository<number>::integration_container integration_new_output(threepf_task<number>* tk, unsigned int worker);


      // INTERNAL DATA

        private:
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

        private:
      //! Berkeley DB XML environment object corresponding to the open repository
      DB_ENV* env;
      //! Berkeley DB XML XmlManager object corresponding to the open repository
      DbXml::XmlManager* mgr;
      };


    // Open a named repository
    template <typename number>
    repository_dbxml<number>::repository_dbxml(const std::string& path, bool recovery)
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
          throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
        }

        // set up environment
        u_int32_t env_flags = DB_JOINENV;
        if(recovery || env->open(env, env_path.string().c_str(), env_flags, 0) != 0)
          {
            // that failed
            // instead, try to create a new environment
            // enable logging, transactional support and locking (so multiple processes can access the repository safely)

            env_flags = DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN;
            if(recovery) env_flags = env_flags | DB_RECOVER | DB_CREATE;
            env->open(env, env_path.string().c_str(), env_flags, 0);

          }

        // set up XmlManager object
        // we have to allow external access in order for XQuery updates to be processed
        mgr = new DbXml::XmlManager(env, DbXml::DBXML_ADOPT_DBENV | DbXml::DBXML_ALLOW_EXTERNAL_ACCESS);
      }


    // Create a named repository
    template <typename number>
    repository_dbxml<number>::repository_dbxml(const std::string& path, const repository_creation_key& key, typename repository<number>::storage_type type)
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
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        // set up environment to enable logging, transactional support
        // and locking (so multiple processes can access the repository safely)
        u_int32_t env_flags = DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN;
        env->open(env, env_path.string().c_str(), env_flags, 0);

        // set up XmlManager object
        // we have to allow external access in order for XQuery updates to be processed
        mgr = new DbXml::XmlManager(env, DbXml::DBXML_ADOPT_DBENV | DbXml::DBXML_ALLOW_EXTERNAL_ACCESS);

        // create database containers
        switch(type)
          {
            case repository<number>::node_storage:
              mgr->setDefaultContainerType(DbXml::XmlContainer::NodeContainer);
            break;

            case repository<number>::document_storage:
              mgr->setDefaultContainerType(DbXml::XmlContainer::WholedocContainer);
            break;

            default:
              assert(false);
          }
        DbXml::XmlContainer packages = this->mgr->createContainer(packages_path.string().c_str());
        DbXml::XmlContainer integrations = this->mgr->createContainer(integrations_path.string().c_str());
      }


    // Destroy a repository object, closing the associated repository
    template <typename number>
    repository_dbxml<number>::~repository_dbxml()
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        // delete XmlManager object: this should also close the DBENV environment, any open containers, etc.
        delete this->mgr;
      }


    // Write a model/initial conditions/parameters combination to the repository
    template <typename number>
    void repository_dbxml<number>::write_package(const initial_conditions<number>& ics, const model<number>* m)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);
        assert(m != nullptr);

        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

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
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_ROOT), nullptr, nullptr, 0, false);

            // write model uid
            std::string uid = m->get_identity_string();
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_MODELUID), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(uid.c_str()), uid.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_MODELUID), nullptr, nullptr);

            // write data block
            dbxml_operations::write_package_data_block(writer, m);

            // write initial conditions block
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_ICS), nullptr, nullptr, 0, false);
            ics.serialize_xml(writer);
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_ICS), nullptr, nullptr);

            // end root node
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_ROOT), nullptr, nullptr);

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
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
              }
          }
      }


    // Write a task to the repository
    template <typename number>
    void repository_dbxml<number>::write_integration_task(const task<number>& t, const model<number>* m, const std::string& root_node_name)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);
        assert(m != nullptr);

        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

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
                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
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
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(root_node_name.c_str()), nullptr, nullptr, 0, false);

            // write data block
            dbxml_operations::write_integration_data_block(writer, t.get_ics().get_name());

            // write task block
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_TASK), nullptr, nullptr, 0, false);
            t.serialize_xml(writer);
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_TASK), nullptr, nullptr);

            // write (empty) output block -- will be populated when integrations are run
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT), nullptr, nullptr, 0, true);

            // end root node
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(root_node_name.c_str()), nullptr, nullptr);

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
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
              }
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_INSERT_ERROR << xe.getExceptionCode() << ": '" << xe.what() << "')";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
              }
          }
      }


    // Query the database for a named task
    template <typename number>
    task<number>* repository_dbxml<number>::query_task(const std::string& name, model<number>*& m, typename instance_manager<number>::model_finder finder)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        // open handles to database containers
        DbXml::XmlContainer integrations = this->mgr->openContainer(this->integrations_path.string().c_str());
        DbXml::XmlContainer models = this->mgr->openContainer(this->packages_path.string().c_str());

        // lookup record for the task
        DbXml::XmlValue integration = dbxml_operations::get_integration_by_name(name, integrations);
        if(!(integration.getType() == DbXml::XmlValue::NODE && integration.getNodeType() == DbXml::XmlValue::DOCUMENT_NODE)) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);

        // extract name of record for initial conditions/parameters and lookup the corresponding record
        // note that dbxml_operations::get_integration_by_name() returns an XmlDocument
        // when this is cast to an XmlValue, the node type is a DOCUMENT_NODE
        // we want the root note for doing queries, so have to descend to the first child
        DbXml::XmlValue integration_root = integration.getFirstChild();
        std::string package_name = dbxml_operations::get_package_from_integration(this->mgr, integration_root, name);
        DbXml::XmlValue package = dbxml_operations::get_package_by_name(package_name, models);
        if(!(package.getType() == DbXml::XmlValue::NODE && package.getNodeType() == DbXml::XmlValue::DOCUMENT_NODE)) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_PACKAGE);

        // extract uid for model
        DbXml::XmlValue package_root = package.getFirstChild();
        std::string model_uid = dbxml_operations::get_model_uid_from_package(this->mgr, package_root, package_name);

        // use the supplied finder to recover the model
        // throws an exception if the model cannot be found, which should be caught higher up in the task handler
        m = finder(model_uid);

        // get XML schema describing initial conditions/parameters package
        // this comes from the initial_conditions<number> serialization, and contains an
        // embedded parameters<number> serialization
        DbXml::XmlValue ics_group = dbxml_operations::get_ics_group(this->mgr, package_root, package_name);

        // build initial_conditions<> object from this schema
        initial_conditions<number> ics = dbxml_operations::build_ics_object(this->mgr, ics_group, package_name, m);

        // build task<> object from the original task schema
        task<number>* tk = dbxml_operations::build_task_object(this->mgr, integration, ics, m, name);

        return(tk);
      }


    template <typename number>
    std::string repository_dbxml<number>::extract_package_document(const std::string& name)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        DbXml::XmlContainer models = this->mgr->openContainer(this->packages_path.string().c_str());

        DbXml::XmlDocument document = dbxml_operations::get_package_by_name(name, models);

        std::string content;
        document.getContent(content);
        return(content);
      }


    template <typename number>
    std::string repository_dbxml<number>::extract_integration_document(const std::string& name)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        DbXml::XmlContainer integrations = this->mgr->openContainer(this->integrations_path.string().c_str());

        DbXml::XmlDocument document = dbxml_operations::get_integration_by_name(name, integrations);

        std::string content;
        document.getContent(content);
        return(content);
      }


    template <typename number>
    typename repository<number>::integration_container repository_dbxml<number>::integration_new_output(twopf_task<number>* tk, unsigned int worker)
      {
        assert(tk != nullptr);

        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        // TODO: implement rollback if the integration is not successful

        // insert a new output record, and return the corresponding integration_container handle
        typename repository<number>::integration_container ctr =
                                                             dbxml_operations::insert_integration_output<number>(this->mgr, tk->get_name(),
                                                                                                                 this->root_path, this->integrations_path,
                                                                                                                 worker);

        dbxml_operations::update_integration_edit_time(this->mgr, tk->get_name(), this->integrations_path);

        return(ctr);
      }


    template <typename number>
    typename repository<number>::integration_container repository_dbxml<number>::integration_new_output(threepf_task<number>* tk, unsigned int worker)
      {
        assert(tk != nullptr);

        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        // TODO: implement rollback if the integration is not successful

        // insert a new output record, and return the corresponding integration container handle
        typename repository<number>::integration_container ctr =
                                                             dbxml_operations::insert_integration_output<number>(this->mgr, tk->get_name(),
                                                                                                                 this->root_path, this->integrations_path,
                                                                                                                 worker);

        dbxml_operations::update_integration_edit_time(this->mgr, tk->get_name(), this->integrations_path);

//        std::string content = this->extract_integration_document(tk->get_name());
//        std::cerr << "Updated XML document:" << std::endl << content << std::endl << std::endl;

        return(ctr);
      }


    // FACTORY FUNCTIONS TO BUILD A REPOSITORY

    template <typename number>
    repository<number>* repository_factory(const std::string& path, bool recovery=false)
      {
        return new repository_dbxml<number>(path, recovery);
      }


    template <typename number>
    repository<number>* repository_factory(const std::string& path, const repository_creation_key& key, typename repository<number>::storage_type type=repository<number>::node_storage)
      {
        return new repository_dbxml<number>(path, key, type);
      }

  }   // namespace transport


#endif //__repository_dbxml_H_
