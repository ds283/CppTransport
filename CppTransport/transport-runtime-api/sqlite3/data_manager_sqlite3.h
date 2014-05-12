//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __data_manager_sqlite3_H_
#define __data_manager_sqlite3_H_


#include <list>

#include "transport-runtime-api/manager/repository.h"
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/models/model.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"
#include "boost/timer/timer.hpp"

#include "sqlite3.h"
#include "transport-runtime-api/sqlite3/sqlite3_operations.h"


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
          : data_manager<number>(cp), temporary_container_serial(0)
          {
          }

        //! Destroy a data_manager_sqlite3 instance
        ~data_manager_sqlite3();


        // INTERFACE -- CONTAINER HANDLING (implements a 'data_manager' interface)

      public:

        //! Create a new container associated with an integration_writer object.
        //! Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void create_container(typename repository<number>::integration_writer& ctr) override;

        //! Close an open container associated with an integration_writer object.

        //! Any open sqlite3 handles are closed, meaning that any integration_writer objects will be invalidated.
        //! After closing, attempting to write using an integration_writer will lead to unsubtle errors.
        virtual void close_container(typename repository<number>::integration_writer& ctr) override;

		    //! Open an existing container associated with an integration_reader object.
		    virtual void open_container(typename repository<number>::integration_reader& ctr) override;

		    //! Close an open container associated with an integration_reader object.

		    //! Any open sqlite3 handles are closed. Attempting to read from the container after closing
		    //! will lead to unsubtle errors.
		    virtual void close_container(typename repository<number>::integration_reader& ctr) override;


        // INTERFACE -- WRITE INDEX TABLES (implements a 'data_manager' interface)

      public:

        //! Create tables needed for a twopf container
        virtual void create_tables(typename repository<number>::integration_writer& ctr, twopf_task<number>* tk,
                                   unsigned int Nfields) override;

        //! Create tables needed for a threepf container
        virtual void create_tables(typename repository<number>::integration_writer& ctr, threepf_task<number>* tk,
                                   unsigned int Nfields) override;


        // INTERFACE - TASK FILES (implements a 'data_manager' interface)

      public:

        //! Create a list of task assignments, over a number of devices, from a work queue of twopf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_writer& ctr, const work_queue<twopf_kconfig>& queue) override;

        //! Create a list of task assignments, over a number of devices, from a work queue of threepf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_writer& ctr, const work_queue<threepf_kconfig>& queue) override;

        //! Read a list of task assignments for a particular worker
        virtual std::set<unsigned int> read_taskfile(const boost::filesystem::path& taskfile, unsigned int worker) override;


        // INTERFACE -- TEMPORARY CONTAINERS (implements a 'data_manager' interface)

      public:

        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual typename data_manager<number>::twopf_batcher create_temp_twopf_container(const boost::filesystem::path& tempdir,
                                                                                         const boost::filesystem::path& logdir,
                                                                                         unsigned int worker, unsigned int Nfields,
                                                                                         typename data_manager<number>::container_dispatch_function dispatcher,
                                                                                         boost::timer::cpu_timer& integration_timer) override;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual typename data_manager<number>::threepf_batcher create_temp_threepf_container(const boost::filesystem::path& tempdir,
                                                                                             const boost::filesystem::path& logdir,
                                                                                             unsigned int worker, unsigned int Nfields,
                                                                                             typename data_manager<number>::container_dispatch_function dispatcher,
                                                                                             boost::timer::cpu_timer& integration_timer) override;

        //! Aggregate a temporary twopf container into a principal container
        virtual void aggregate_twopf_batch(typename repository<number>::integration_writer& ctr,
                                           const std::string& temp_ctr, model<number>* m, integration_task<number>* tk) override;

        //! Aggregate a temporary threepf container into a principal container
        virtual void aggregate_threepf_batch(typename repository<number>::integration_writer& ctr,
                                             const std::string& temp_ctr, model<number>* m, integration_task<number>* tk) override;


		    // INTERNAL UTILITY FUNCTIONS

      protected:

        //! Replace a temporary twopf container with a new one
        void replace_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                          unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                          typename data_manager<number>::replacement_action action);

        //! Replace a temporary threepf container with a new one
        void replace_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                            unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                            typename data_manager<number>::replacement_action action);

        //! Generate the name for a temporary container
        boost::filesystem::path generate_temporary_container_path(const boost::filesystem::path& tempdir, unsigned int worker);

        // INTERNAL DATA

      private:

        //! List of open sqlite3 connexions
        std::list< sqlite3* > open_containers;

        //! Serial number used to distinguish
        //! the set of temporary containers associated with this worker.
		    //! Begins at zero and is incremented as temporary containers are generated.
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
    void data_manager_sqlite3<number>::create_container(typename repository<number>::integration_writer& ctr)
      {
        sqlite3* db = nullptr;
        sqlite3* taskfile = nullptr;

        // get paths of the data container and taskfile
        boost::filesystem::path ctr_path = ctr.data_container_path();
        boost::filesystem::path taskfile_path = ctr.taskfile_path();

        // open the main container

        int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

        if(status != SQLITE_OK)
	        {
            std::ostringstream msg;
            if(db != nullptr)
	            {
                msg << __CPP_TRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << __CPP_TRANSPORT_DATACTR_CREATE_B << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(db);
	            }
            else
	            {
                msg << __CPP_TRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << __CPP_TRANSPORT_DATACTR_CREATE_B << status << ")";
	            }
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
	        }

        // enable foreign key constraints
        char* errmsg;
        sqlite3_exec(db, "PRAGMA foreign_keys = ON", nullptr, nullptr, &errmsg);

        // remember this connexion
        this->open_containers.push_back(db);
        ctr.set_data_manager_handle(db);

        // open the taskfile associated with this container

        status = sqlite3_open_v2(taskfile_path.string().c_str(), &taskfile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

        if(status != SQLITE_OK)
	        {
            sqlite3_close(db);

            std::ostringstream msg;
            if(taskfile != nullptr)
	            {
                msg << __CPP_TRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << __CPP_TRANSPORT_DATACTR_CREATE_B << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(taskfile);
	            }
            else
	            {
                msg << __CPP_TRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << __CPP_TRANSPORT_DATACTR_CREATE_B << status << ")";
	            }
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
	        }

        // remember this connexion
        this->open_containers.push_back(taskfile);
        ctr.set_data_manager_taskfile(taskfile);
      }


    // Close a container after writing
    template <typename number>
    void data_manager_sqlite3<number>::close_container(typename repository<number>::integration_writer& ctr)
      {
        // close sqlite3 handle to principal database
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        this->open_containers.remove(db);
        sqlite3_close(db);

        // close sqlite3 handle to taskfile
        sqlite3* taskfile = nullptr;
        ctr.get_data_manager_taskfile(&taskfile); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        this->open_containers.remove(taskfile);
        sqlite3_close(taskfile);

        // physically remove the taskfile from the disc; it isn't needed any more
        boost::filesystem::remove(ctr.taskfile_path());

        // physically remove the tempfiles directory
        boost::filesystem::remove(ctr.temporary_files_path());
      }


		// Open a container for reading
		template <typename number>
		void data_manager_sqlite3<number>::open_container(typename repository<number>::integration_reader& ctr)
			{
			}


		// Close a container after reading
		template <typename number>
		void data_manager_sqlite3<number>::close_container(typename repository<number>::integration_reader& ctr)
			{
			}


    // INDEX TABLE MANAGEMENT

    template <typename number>
    void data_manager_sqlite3<number>::create_tables(typename repository<number>::integration_writer& ctr,
                                                     twopf_task<number>* tk, unsigned int Nfields)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_twopf_sample_table(db, tk);
        sqlite3_operations::create_backg_table(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table(db, Nfields, sqlite3_operations::real_twopf, sqlite3_operations::foreign_keys);
//        sqlite3_operations::create_dN_table(db, Nfields, sqlite3_operations::foreign_keys);
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(typename repository<number>::integration_writer& ctr,
                                                     threepf_task<number>* tk, unsigned int Nfields)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_twopf_sample_table(db, tk);
        sqlite3_operations::create_threepf_sample_table(db, tk);
        sqlite3_operations::create_backg_table(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table(db, Nfields, sqlite3_operations::real_twopf, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table(db, Nfields, sqlite3_operations::imag_twopf, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_threepf_table(db, Nfields, sqlite3_operations::foreign_keys);
//        sqlite3_operations::create_dN_table(db, Nfields, sqlite3_operations::foreign_keys);
//        sqlite3_operations::create_ddN_table(db, Nfields, sqlite3_operations::foreign_keys);
      }


    // TASKFILE MANAGEMENT

    // Create a tasklist based on a work queue of twopf_kconfig-s
    template <typename number>
    void data_manager_sqlite3<number>::create_taskfile(typename repository<number>::integration_writer& ctr, const work_queue<twopf_kconfig>& queue)
      {
        sqlite3* taskfile = nullptr;
        ctr.get_data_manager_taskfile(&taskfile); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_taskfile(taskfile, queue);
      }

    // Create a tasklist based on a work queue of threepf_kconfig-s
    template <typename number>
    void data_manager_sqlite3<number>::create_taskfile(typename repository<number>::integration_writer& ctr, const work_queue<threepf_kconfig>& queue)
      {
        sqlite3* taskfile = nullptr;
        ctr.get_data_manager_taskfile(&taskfile); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

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
                                                                                                           const boost::filesystem::path& logdir,
                                                                                                           unsigned int worker,
                                                                                                           unsigned int Nfields,
                                                                                                           typename data_manager<number>::container_dispatch_function dispatcher,
                                                                                                           boost::timer::cpu_timer& integration_timer)
      {
        boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

        sqlite3* db = sqlite3_operations::create_temp_twopf_container(container, Nfields);

        // set up writers
        typename data_manager<number>::twopf_writer_group writers;
        writers.backg = std::bind(&sqlite3_operations::write_backg<number>, std::placeholders::_1, std::placeholders::_2);
        writers.twopf = std::bind(&sqlite3_operations::write_twopf<number>, sqlite3_operations::real_twopf, std::placeholders::_1, std::placeholders::_2);

        // set up a replacement function
        typename data_manager<number>::container_replacement_function replacer =
                                                                        std::bind(&data_manager_sqlite3<number>::replace_temp_twopf_container,
                                                                                  this, tempdir, worker, Nfields,
                                                                                  std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        typename data_manager<number>::twopf_batcher batcher(this->capacity, Nfields, container, logdir, writers, dispatcher, replacer, db, worker, integration_timer);

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "** Created new temporary twopf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }

    template <typename number>
    typename data_manager<number>::threepf_batcher data_manager_sqlite3<number>::create_temp_threepf_container(const boost::filesystem::path& tempdir,
                                                                                                               const boost::filesystem::path& logdir,
                                                                                                               unsigned int worker,
                                                                                                               unsigned int Nfields,
                                                                                                               typename data_manager<number>::container_dispatch_function dispatcher,
                                                                                                               boost::timer::cpu_timer& integration_timer)
      {
        boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

        sqlite3* db = sqlite3_operations::create_temp_threepf_container(container, Nfields);

        // set up writers
        typename data_manager<number>::threepf_writer_group writers;
        writers.backg    = std::bind(&sqlite3_operations::write_backg<number>, std::placeholders::_1, std::placeholders::_2);
        writers.twopf_re = std::bind(&sqlite3_operations::write_twopf<number>, sqlite3_operations::real_twopf, std::placeholders::_1, std::placeholders::_2);
        writers.twopf_im = std::bind(&sqlite3_operations::write_twopf<number>, sqlite3_operations::imag_twopf, std::placeholders::_1, std::placeholders::_2);
        writers.threepf  = std::bind(&sqlite3_operations::write_threepf<number>, std::placeholders::_1, std::placeholders::_2);

        // set up a replacement function
        typename data_manager<number>::container_replacement_function replacer =
                                                                        std::bind(&data_manager_sqlite3<number>::replace_temp_threepf_container,
                                                                                  this, tempdir, worker, Nfields,
                                                                                  std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        typename data_manager<number>::threepf_batcher batcher(this->capacity, Nfields, container, logdir, writers, dispatcher, replacer, db, worker, integration_timer);

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "** Created new temporary threepf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                                                    unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                                                    typename data_manager<number>::replacement_action action)
      {
        sqlite3* db = nullptr;

        batcher->get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        if(action == data_manager<number>::action_replace)
          {
            boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

            sqlite3* new_db = sqlite3_operations::create_temp_twopf_container(container, Nfields);

            batcher->set_container_path(container);
            batcher->set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                                                      unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                                                      typename data_manager<number>::replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal)
            << "** " << (action == data_manager<number>::action_replace ? "Replacing" : "Closing")
            << " temporary threepf container " << batcher->get_container_path();

        batcher->get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal) << "** Closed sqlite3 handle for " << batcher->get_container_path();

        if(action == data_manager<number>::action_replace)
          {
            boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

            BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal) << "** Opening new threepf container " << container;

            sqlite3* new_db = sqlite3_operations::create_temp_threepf_container(container, Nfields);

            batcher->set_container_path(container);
            batcher->set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    boost::filesystem::path data_manager_sqlite3<number>::generate_temporary_container_path(const boost::filesystem::path& tempdir, unsigned int worker)
      {
        std::ostringstream container_name;
        container_name << __CPP_TRANSPORT_TEMPORARY_CONTAINER_STEM << worker << "_" << this->temporary_container_serial++ << __CPP_TRANSPORT_TEMPORARY_CONTAINER_XTN;

        boost::filesystem::path container = tempdir / container_name.str();

        return(container);
      }


    template <typename number>
    void data_manager_sqlite3<number>::aggregate_twopf_batch(typename repository<number>::integration_writer& ctr,
                                                             const std::string& temp_ctr, model<number>* m, integration_task<number>* tk)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::aggregate_backg<number>(db, ctr, temp_ctr, m, tk, sqlite3_operations::gauge_xfm_1);
        sqlite3_operations::aggregate_twopf<number>(db, ctr, temp_ctr, sqlite3_operations::real_twopf);
      }


    template <typename number>
    void data_manager_sqlite3<number>::aggregate_threepf_batch(typename repository<number>::integration_writer& ctr,
                                                               const std::string& temp_ctr, model<number>* m, integration_task<number>* tk)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::aggregate_backg<number>(db, ctr, temp_ctr, m, tk, sqlite3_operations::gauge_xfm_2);
        sqlite3_operations::aggregate_twopf<number>(db, ctr, temp_ctr, sqlite3_operations::real_twopf);
        sqlite3_operations::aggregate_twopf<number>(db, ctr, temp_ctr, sqlite3_operations::imag_twopf);
        sqlite3_operations::aggregate_threepf<number>(db, ctr, temp_ctr);
      }


    // FACTORY FUNCTIONS TO BUILD A DATA_MANAGER

    template <typename number>
    data_manager<number>* data_manager_factory(unsigned int capacity)
      {
        return new data_manager_sqlite3<number>(capacity);
      }


  }   // namespace transport



#endif //__data_manager_sqlite3_H_
