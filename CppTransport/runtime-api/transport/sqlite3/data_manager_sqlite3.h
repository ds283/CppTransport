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
#import "data_manager.h"


#define __CPP_TRANSPORT_TEMPORARY_CONTAINER_STEM "worker"
#define __CPP_TRANSPORT_TEMPORARY_CONTAINER_XTN  ".sqlite"

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
        //! Create a data_manager_sqlite3 instance
        data_manager_sqlite3(unsigned int cp)
          : data_manager<number>(cp)
          {
          }

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
        //! Create tables needed for a twopf container
        void create_tables(typename repository<number>::integration_container& ctr, twopf_task<number>* tk,
                           unsigned int Nfields);

        //! Create tables needed for a threepf container
        void create_tables(typename repository<number>::integration_container& ctr, threepf_task<number>* tk,
                           unsigned int Nfields);


        // INTERFACE - TASK FILES

      public:
        //! Create a list of task assignments, over a number of devices, from a work queue of twopf_kconfig-s
        void create_taskfile(typename repository<number>::integration_container& ctr, const work_queue<twopf_kconfig>& queue);

        //! Create a list of task assignments, over a number of devices, from a work queue of threepf_kconfig-s
        void create_taskfile(typename repository<number>::integration_container& ctr, const work_queue<threepf_kconfig>& queue);

        //! Read a list of task assignments for a particular worker
        std::set<unsigned int> read_taskfile(const boost::filesystem::path& taskfile, unsigned int worker);


        // INTERFACE -- TEMPORARY CONTAINERS

      public:
        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        typename data_manager<number>::twopf_batcher create_temp_twopf_container(const boost::filesystem::path& tempdir,
                                                                                 unsigned int worker, unsigned int Nfields,
                                                                                 typename data_manager<number>::container_dispatch_function dispatcher);

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        typename data_manager<number>::threepf_batcher create_temp_threepf_container(const boost::filesystem::path& tempdir,
                                                                                     unsigned int worker, unsigned int Nfields,
                                                                                     typename data_manager<number>::container_dispatch_function dispatcher);


      protected:
        //! Replace a temporary twopf container with a new one
        void replace_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                          unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                          typename data_manager<number>::replacement_action action);

        //! Replace a temporary threepf container with a new one
        void replace_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                            unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                            typename data_manager<number>::replacement_action action);

        // INTERNAL DATA

      private:
        //! List of open sqlite3 connexions
        std::list< sqlite3* > open_containers;

        //! Serial number used to distinguish temporary containers
        unsigned int          temporary_container_serial;
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
        sqlite3* taskfile = nullptr;

        // get root directory of repository
        boost::filesystem::path repo_root = repo->get_root_path();

        // get paths of the data container and taskfile relative to the repository root
        boost::filesystem::path ctr_relative = ctr.data_container_path();
        boost::filesystem::path taskfile_relative = ctr.taskfile_path();

        // construct absolute paths to the data container and repository root
        boost::filesystem::path ctr_path = repo_root / ctr_relative;
        boost::filesystem::path taskfile_path = repo_root / taskfile_relative;

        int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, flags, nullptr);

        if(status != SQLITE_OK)
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

        // remember this connexion
        this->open_containers.push_back(db);
        ctr.set_data_manager_handle(db);

        status = sqlite3_open_v2(taskfile_path.string().c_str(), &taskfile, flags, nullptr);

        if(status != SQLITE_OK)
          {
            sqlite3_close(db);

            std::ostringstream msg;
            if(taskfile != nullptr)
              {
                msg << excpt_a << " '" << ctr_path.string() << "' " << excpt_b << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(taskfile);
              }
            else
              {
                msg << excpt_a << " '" << ctr_path.string() << "' " << excpt_b << status << ")";
              }
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
          }

        // enable foreign key constraints
        sqlite3_exec(taskfile, "PRAGMA foreign_keys = ON", nullptr, nullptr, &errmsg);

        // remember this connexion
        this->open_containers.push_back(taskfile);
        ctr.set_data_manager_taskfile(taskfile);
      }


    // Close a container
    template <typename number>
    void data_manager_sqlite3<number>::close_container(typename repository<number>::integration_container& ctr)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception is handle is unset, so the return value is guaranteed not to be nullptr

        this->open_containers.remove(db);
        sqlite3_close(db);

        sqlite3* taskfile = nullptr;
        ctr.get_data_manager_taskfile(&taskfile); // throws an exception is handle is unset, so the return value is guaranteed not to be nullptr

        this->open_containers.remove(taskfile);
        sqlite3_close(taskfile);
      }


    // INDEX TABLE MANAGEMENT

    template <typename number>
    void data_manager_sqlite3<number>::create_tables(typename repository<number>::integration_container& ctr,
                                                     twopf_task<number>* tk, unsigned int Nfields)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception is handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_twopf_sample_table(db, tk);
        sqlite3_operations::create_backg_table(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table(db, Nfields, sqlite3_operations::real_twopf, sqlite3_operations::foreign_keys);
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(typename repository<number>::integration_container& ctr,
                                                     threepf_task<number>* tk, unsigned int Nfields)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception is handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_twopf_sample_table(db, tk);
        sqlite3_operations::create_threepf_sample_table(db, tk);
        sqlite3_operations::create_backg_table(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table(db, Nfields, sqlite3_operations::real_twopf, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table(db, Nfields, sqlite3_operations::imag_twopf, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_threepf_table(db, Nfields, sqlite3_operations::foreign_keys);
      }


    // TASKFILE MANAGEMENT

    // Create a tasklist based on a work queue of twopf_kconfig-s
    template <typename number>
    void data_manager_sqlite3<number>::create_taskfile(typename repository<number>::integration_container& ctr, const work_queue<twopf_kconfig>& queue)
      {
        sqlite3* taskfile = nullptr;
        ctr.get_data_manager_taskfile(&taskfile); // throws an exception is handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_taskfile(taskfile, queue);
      }

    // Create a tasklist based on a work queue of threepf_kconfig-s
    template <typename number>
    void data_manager_sqlite3<number>::create_taskfile(typename repository<number>::integration_container& ctr, const work_queue<threepf_kconfig>& queue)
      {
        sqlite3* taskfile = nullptr;
        ctr.get_data_manager_taskfile(&taskfile); // throws an exception is handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_taskfile(taskfile, queue);
      }

    // Read a taskfile
    template <typename number>
    std::set<unsigned int> data_manager_sqlite3<number>::read_taskfile(const boost::filesystem::path& taskfile, unsigned int worker)
      {
        return sqlite3_operations::read_taskfile(taskfile.string(), worker);
      }


    // TEMPORARY CONTAINERS

    template <typename number>
    typename data_manager<number>::twopf_batcher data_manager_sqlite3<number>::create_temp_twopf_container(const boost::filesystem::path& tempdir,
                                                                                                           unsigned int worker,
                                                                                                           unsigned int Nfields,
                                                                                                           typename data_manager<number>::container_dispatch_function dispatcher)
      {
        std::ostringstream container_name;
        container_name << __CPP_TRANSPORT_TEMPORARY_CONTAINER_STEM << worker << __CPP_TRANSPORT_TEMPORARY_CONTAINER_XTN;

        boost::filesystem::path container = tempdir / container_name;
        sqlite3* db = sqlite3_operations::create_temp_twopf_container(container, Nfields);

        // set up writers
        typename data_manager<number>::twopf_writer_group writers;
        writers.backg = std::bind(&sqlite3_operations::write_backg, std::placeholders::_1, std::placeholders::_2);
        writers.twopf = std::bind(&sqlite3_operations::write_twopf, sqlite3_operations::real_twopf, std::placeholders::_1, std::placeholders::_2);

        // set up a replacement function
        typename data_manager<number>::container_replacement_function replacer =
                                                                        std::bind(&data_manager<number>::replace_temp_twopf_container,
                                                                                  this, tempdir, worker, Nfields,
                                                                                  std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        typename data_manager<number>::twopf_batcher batcher(this->capacity, Nfields, container, writers, dispatcher, replacer, db);

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }

    template <typename number>
    typename data_manager<number>::threepf_batcher data_manager_sqlite3<number>::create_temp_threepf_container(const boost::filesystem::path& tempdir,
                                                                                                               unsigned int worker,
                                                                                                               unsigned int Nfields,
                                                                                                               typename data_manager<number>::container_dispatch_function dispatcher)
      {
        std::ostringstream container_name;
        container_name << __CPP_TRANSPORT_TEMPORARY_CONTAINER_STEM << worker << "_" << this->temporary_container_serial++ << __CPP_TRANSPORT_TEMPORARY_CONTAINER_XTN;

        boost::filesystem::path container = tempdir / container_name;

        sqlite3* db = sqlite3_operations::create_temp_threepf_container(container, Nfields);

        // set up writers
        typename data_manager<number>::threepf_writer_group writers;
        writers.backg    = std::bind(&sqlite3_operations::write_backg, std::placeholders::_1, std::placeholders::_2);
        writers.twopf_re = std::bind(&sqlite3_operations::write_twopf, sqlite3_operations::real_twopf, std::placeholders::_1, std::placeholders::_2);
        writers.twopf_im = std::bind(&sqlite3_operations::write_twopf, sqlite3_operations::imag_twopf, std::placeholders::_1, std::placeholders::_2);
        writers.threepf  = std::bind(&sqlite3_operations::write_threepf, std::placeholders::_1, std::placeholders::_2);

        // set up a replacement function
        typename data_manager<number>::container_replacement_function replacer =
                                                                        std::bind(&data_manager<number>::replace_temp_threepf_container,
                                                                                  this, tempdir, worker, Nfields,
                                                                                  std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        typename data_manager<number>::threepf_batcher batcher(this->capacity, Nfields, container, writers, dispatcher, replacer, db);

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    void data_manager<number>::replace_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                                            unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                                            typename data_manager<number>::replacement_action action)
      {
        sqlite3* db = nullptr;

        batcher.get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        if(action == data_manager<number>::replace)
          {
            std::ostringstream container_name;
            container_name << __CPP_TRANSPORT_TEMPORARY_CONTAINER_STEM << worker << "_" << this->temporary_container_serial++ << __CPP_TRANSPORT_TEMPORARY_CONTAINER_XTN;

            boost::filesystem::path container = tempdir / container_name;

            sqlite3* db = sqlite3_operations::create_temp_twopf_container(container, Nfields);

            batcher->set_container_name(container);
            batcher->set_manager_handle(db);

            this->open_containers.push_back(db);
          }
      }


    template <typename number>
    void data_manager<number>::replace_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                                              unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                                              typename data_manager<number>::replacement_action action)
      {
        sqlite3* db = nullptr;

        batcher.get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        if(action == data_manager<number>::replace_container)
          {
            std::ostringstream container_name;
            container_name << __CPP_TRANSPORT_TEMPORARY_CONTAINER_STEM << worker << "_" << this->temporary_container_serial++ << __CPP_TRANSPORT_TEMPORARY_CONTAINER_XTN;

            boost::filesystem::path container = tempdir / container_name;

            sqlite3* db = sqlite3_operations::create_temp_threepf_container(container, Nfields);

            batcher->set_container_name(container);
            batcher->set_manager_handle(db);

            this->open_containers.push_back(db);
          }
      }


    // FACTORY FUNCTIONS TO BUILD A DATA_MANAGER

    template <typename number>
    data_manager<number>* data_manager_factory(unsigned int capacity)
      {
        return new data_manager_sqlite3<number>(capacity);
      }


  }   // namespace transport



#endif //__data_manager_sqlite3_H_
