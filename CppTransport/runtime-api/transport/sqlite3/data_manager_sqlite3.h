//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __data_manager_sqlite3_H_
#define __data_manager_sqlite3_H_


#include <list>

#include "transport/manager/repository.h"
#include "transport/manager/data_manager.h"

#include "transport/messages_en.h"
#include "transport/exceptions.h"

#include "boost/filesystem/operations.hpp"

#include "sqlite3.h"
#include "transport/sqlite3/sqlite3_operations.h"

namespace transport
  {

    namespace
      {

        const std::string default_excpt_a = __CPP_TRANSPORT_DATACTR_OPEN_A;
        const std::string default_excpt_b = __CPP_TRANSPORT_DATACTR_OPEN_B;

      }   // unnamed namespace

    // implements the data_manager interface using sqlite3
    template <typename number>
    class data_manager_sqlite3: public data_manager<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:
        //! Destroy a data_manager_sqlite3 instance
        ~data_manager_sqlite3();


        // INTERFACE -- CONTAINER HANDLING

      public:
        //! Create a new container. Never overwrites existing data; if the container already exists, an exception is thrown
        void create_container(repository<number>* repo, typename repository<number>::integration_container& ctr);

        //! Open an existing container
        void open_container(repository<number>* repo, typename repository<number>::integration_container& ctr);

        //! Close an open container

        //! Any open sqlite3 handles are closed, meaning that any integration_container objects will be invalidated.
        //! Attempting to read or write from an integration_container will lead to unsubtle errors.
        void close_container(typename repository<number>::integration_container& ctr);

      protected:
        void backend_open_container(repository<number>* repo, typename repository<number>::integration_container& ctr, int flags=SQLITE_OPEN_READWRITE,
                                    const std::string& excpt_a=default_excpt_a,
                                    const std::string& excpt_b=default_excpt_b);


        // INTERFACE -- WRITE INDEX TABLES

      public:
        //! Create the table of sample times
        void create_time_sample_table(typename repository<number>::integration_container& ctr, task<number>* tk);

        //! Create the table of twopf sample configurations
        void create_twopf_sample_table(typename repository<number>::integration_container& ctr, twopf_list_task<number>* tk);

        //! Create the table of threepf sample configurations
        void create_threepf_sample_table(typename repository<number>::integration_container& ctr, threepf_task<number>* tk);


        // INTERNAL DATA

      private:
        std::list< sqlite3* > open_containers;
      };


    // CONSTRUCTOR, DESTRUCTOR

    template <typename number>
    data_manager_sqlite3<number>::~data_manager_sqlite3()
      {
        for(std::list<sqlite3*>::iterator t = this->open_containers.begin(); t != this->open_containers.end(); t++)
          {
            int status = sqlite3_close(*t);

            if(status != SQLITE_OK)
              {
                if(status == SQLITE_BUSY) throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, __CPP_TRANSPORT_DATACTR_NOT_CLOSED);
                else
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATACTR_CLOSE << status << ")";
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }
              }
          }
      }


    // Create a new container
    template <typename number>
    void data_manager_sqlite3<number>::create_container(repository<number>* repo, typename repository<number>::integration_container& ctr)
      {
        assert(repo != nullptr);
        this->backend_open_container(repo, ctr, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                     __CPP_TRANSPORT_DATACTR_CREATE_A, __CPP_TRANSPORT_DATACTR_CREATE_B);
      }


    // Open an existing container
    template <typename number>
    void data_manager_sqlite3<number>::open_container(repository<number>* repo, typename repository<number>::integration_container& ctr)
      {
        assert(repo != nullptr);
        this->backend_open_container(repo, ctr);
      }


    // Backend create/open container
    template <typename number>
    void data_manager_sqlite3<number>::backend_open_container(repository<number>* repo, typename repository<number>::integration_container& ctr, int flags,
                                                              const std::string& excpt_a, const std::string& excpt_b)
      {
        assert(repo != nullptr);

        sqlite3* db = nullptr;

        boost::filesystem::path repo_root = repo->get_root_path();
        boost::filesystem::path ctr_relative = ctr.data_container_path();

        boost::filesystem::path ctr_path = repo_root / ctr_relative;

        int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

        if(status == SQLITE_OK)
          {
            // remember this connexion
            this->open_containers.push_back(db);
            ctr.set_data_manager_data(db);
          }
        else
          {
            std::ostringstream msg;
            if(db != nullptr)
              {
                msg << excpt_a << " '" << ctr_path.string() << "' " << excpt_b << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(db);
              }
            else
              {
                msg << excpt_a << " '" << ctr_path.string() << "' " << excpt_b << status << ")";
              }
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
          }

        // enable foreign key constraints
        char* errmsg;
        sqlite3_exec(db, "PRAGMA foreign_keys = ON", nullptr, nullptr, &errmsg);
      }


    // Close a container
    template <typename number>
    void data_manager_sqlite3<number>::close_container(typename repository<number>::integration_container& ctr)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_data(&db);

        if(db != nullptr)
          {
            this->open_containers.remove(db);
            sqlite3_close(db);
          }
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATACTR_NULLPTR << " '" << ctr.data_container_path().string() << "' ('close_container')";
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
          }
      }


    // Create the table of sample times
    template <typename number>
    void data_manager_sqlite3<number>::create_time_sample_table(typename repository<number>::integration_container& ctr, task<number>* tk)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_data(&db);

        if(db != nullptr)
          {
            sqlite3_operations::create_time_sample_table(db, tk);
          }
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATACTR_NULLPTR << " '" << ctr.data_container_path().string() << "' ('create_time_sample_table')";
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
          }
      }


    // Create the table of twopf sample configurations
    template <typename number>
    void data_manager_sqlite3<number>::create_twopf_sample_table(typename repository<number>::integration_container& ctr, twopf_list_task<number>* tk)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_data(&db);

        if(db != nullptr)
          {
            sqlite3_operations::create_twopf_sample_table(db, tk);
          }
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATACTR_NULLPTR << " '" << ctr.data_container_path().string() << "' ('create_twopf_sample_table')";
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
          }
      }


    // Create the table of threepf sample configurations
    template <typename number>
    void data_manager_sqlite3<number>::create_threepf_sample_table(typename repository<number>::integration_container& ctr, threepf_task<number>* tk)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_data(&db);

        if(db != nullptr)
          {
            sqlite3_operations::create_threepf_sample_table(db, tk);
          }
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATACTR_NULLPTR << " '" << ctr.data_container_path().string() << "' ('create_threepf_sample_table')";
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
          }
      }


    // FACTORY FUNCTIONS TO BUILD A DATA_MANAGER

    template <typename number>
    data_manager<number>* data_manager_factory()
      {
        return new data_manager_sqlite3<number>();
      }


  }   // namespace transport



#endif //__data_manager_sqlite3_H_
