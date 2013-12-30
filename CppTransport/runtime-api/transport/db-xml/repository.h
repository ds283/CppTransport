//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __repository_H_
#define __repository_H_


#include <string>
#include <sstream>
#include <stdexcept>

#include "transport/tasks/task.h"
#include "transport/messages_en.h"

#include "dbxml/db.h"
#include "dbxml/dbxml/DbXml.hpp"

#include "boost/filesystem/operations.hpp"


#define __CPP_TRANSPORT_REPO_ENVIRONMENT_LEAF   "env"
#define __CPP_TRANSPORT_REPO_CONTAINERS_LEAF    "containers"
#define __CPP_TRANSPORT_CNTR_MODELS_LEAF        "models.bdbxml"
#define __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF  "integrations.bdbxml"


namespace transport
  {
    // forward-declare 'key' class used to create repositories
    // the complete declaration is in a separate file,
    // which must be included to allow creation of repositories
    class repository_creation_key;

    class repository
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:
        //! open a repository with a specific pathname
        repository(const std::string& path);
        //! create a repository with a specific pathname
        repository(const std::string& path, const repository_creation_key& key);

        //! close a repository, including the corresponding containers and environment
        ~repository();



        // INTERNAL DATA

      protected:
        //! BOOST path to repository root directory
        boost::filesystem::path root_path;
        //! BOOST path to repository environment
        boost::filesystem::path env_path;
        //! BOOST path to containers
        boost::filesystem::path containers_path;
        //! BOOST path to model container
        boost::filesystem::path models_path;
        //! BOOST path to integrations container
        boost::filesystem::path integrations_path;

        // DATABASE ENVIRONMENT

        //! Berkeley DB XML environment object corresponding to the open repository
        DbXml::DB_ENV* env;
        //! Berkeley DB XML XmlManager object corresponding to the open repository
        DbXml::XmlManager* mgr;

        // DATABASE CONTAINERS

        //! XML container object for models (=choice of model, choice of parameters, choice of initial conditions)
        DbXml::XmlContainer models;
        //! XML container object for integrations (=choice of n-point function)
        DbXml::XmlContainer integrations;
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
            throw std::runtime_error(msg.str());
          }

        env_path = root_path / __CPP_TRANSPORT_REPO_ENVIRONMENT_LEAF;
        if(!boost::filesystem::is_directory(env_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_ENV << " '" << path << "'";
            throw std::runtime_error(msg.str());
          }

        containers_path = root_path / __CPP_TRANSPORT_REPO_CONTAINERS_LEAF;
        if(!boost::filesystem::is_directory(containers_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_CNTR << " '" << path << "'";
            throw std::runtime_error(msg.str());
          }

        models_path = containers_path / __CPP_TRANSPORT_CNTR_MODELS_LEAF;
        if(!boost::filesystem::is_regular_file(models_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_MODELS << " '" << path << "'";
            throw std::runtime_error(msg.str());
          }

        integrations_path = containers_path / __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF;
        if(!boost::filesystem::is_regular_file(integrations_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_INTGRTNS << " '" << path << "'";
            throw std::runtime_error(msg.str());
          }

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
        u_int32_t env_flags = DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN;
        env->open(env, env_path.string().c_str(), env_flags, 0);

        // set up XmlManager object
        mgr = new DbXml::XmlManager(env, 0);

        // open database containers
        models = DbXml::openContainer(models_path.string().c_str());
        integrations = DbXml::openContainer(integrations_path.string().c_str());
      }


    // Create a named repository
    template <typename number>
    repository<number>::repository(const std::string& path, const repository_creation_key& key)
      : env(nullptr), mgr(nullptr)
      {
        root_path = path;
        if(boost::filesystem::exists(root_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_ROOT_EXISTS << " '" << path << "'";
            throw std::runtime_error(msg.str());
          }

        env_path = root_path / __CPP_TRANSPORT_REPO_ENVIRONMENT_LEAF;
        containers_path = root_path / __CPP_TRANSPORT_REPO_CONTAINERS_LEAF;
        models_path = containers_path / __CPP_TRANSPORT_CNTR_MODELS_LEAF;
        integrations_path = containers_path / __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF;

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
        mgr = new DbXml::XmlManager(env, 0);

        // open database containers
        models = DbXml::createContainer(models_path.string().c_str());
        integrations = DbXml::createContainer(integrations_path.string().c_str());

      }


    // Destroy a respository
    template <typename number>
    repository<number>::~repository()
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        if(this->mgr != nullptr) delete this->mgr;
        if(this->env != nullptr)
          {
            this->env->close(this->env, 0);
          }
      }


  }   // namespace transport



#endif //__repository_H_
