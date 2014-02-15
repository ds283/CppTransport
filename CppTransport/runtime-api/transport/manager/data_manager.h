//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __data_manager_H_
#define __data_manager_H_


#include <set>

#include "transport/tasks/task.h"
#include "transport/scheduler/work_queue.h"
#include "transport/manager/repository.h"

#include "boost/filesystem/operations.hpp"
#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"


// log file name
#define __CPP_TRANSPORT_LOG_FILENAME_A  "worker_"
#define __CPP_TRANSPORT_LOG_FILENAME_B  "_%3N.log"


namespace transport
  {

    // forward-declare model to avoid circular references
    template <typename number> class model;

    template <typename number>
    class data_manager
      {

      public:

        // data structures for storing individual sample points from each integration
        class backg_item
          {
          public:
            unsigned int        time_serial;
            std::vector<number> coords;
          };

        class twopf_item
          {
          public:
            unsigned int        time_serial;
            unsigned int        kconfig_serial;
            std::vector<number> elements;
          };

        class threepf_item
          {
          public:
            unsigned int        time_serial;
            unsigned int        kconfig_serial;
            std::vector<number> elements;
          };

        // writer functions, used by the compute backends to store the output of each integration
        // in a temporary container

        class generic_batcher;
        class twopf_batcher;
        class threepf_batcher;

        // background writer
        typedef std::function<void(generic_batcher*, const std::vector<backg_item>&)> backg_writer;

        // twopf writer
        typedef std::function<void(generic_batcher*, const std::vector<twopf_item>&)> twopf_writer;

        // threepf writer requires: threepf writer
        typedef std::function<void(generic_batcher*, const std::vector<threepf_item>&)> threepf_writer;

        // group writers together in batches for twopf and threepf integrations
        class twopf_writer_group
          {
          public:
            backg_writer backg;
            twopf_writer twopf;
          };

        class threepf_writer_group
          {
          public:
            backg_writer   backg;
            twopf_writer   twopf_re;
            twopf_writer   twopf_im;
            threepf_writer threepf;
          };


        typedef enum { action_replace, action_close } replacement_action;

        // data_manager function to close the temporary container and replace it with another one
        typedef std::function<void(generic_batcher* batcher, replacement_action)> container_replacement_function;
        // task_manager function to push a container to the master node
        typedef std::function<void(generic_batcher* batcher)> container_dispatch_function;

        // Types needed for logging
        typedef enum { normal, notification, warning, error, critical } log_severity_level;
        typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend > sink_t;

        // Batcher objects, used by integration workers to push results into a container

        class generic_batcher
          {
          public:
            template <typename handle_type>
            generic_batcher(unsigned int cap, unsigned int Nf,
                            const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                            container_dispatch_function d, container_replacement_function r,
                            handle_type h, unsigned int w)
              : capacity(cap), Nfields(Nf), container_path(cp), logdir_path(lp), num_backg(0),
                dispatcher(d), replacer(r), worker_number(w),
                manager_handle(static_cast<void*>(h))
              {
                std::ostringstream log_file;
                log_file << __CPP_TRANSPORT_LOG_FILENAME_A << worker_number << __CPP_TRANSPORT_LOG_FILENAME_B;

                boost::filesystem::path log_path = logdir_path / log_file.str();

                boost::shared_ptr< boost::log::core > core = boost::log::core::get();

                boost::shared_ptr< boost::log::sinks::text_file_backend > backend =
                                     boost::make_shared< boost::log::sinks::text_file_backend >( boost::log::keywords::file_name = log_path.string() );

		            // enable auto-flushing of log entries
		            // this degrades performance, but we are not writing many entries and they
		            // will not be lost in the event of a crash
		            backend->auto_flush(true);

                // Wrap it into the frontend and register in the core.
                // The backend requires synchronization in the frontend.
                this->log_sink = boost::shared_ptr< sink_t >(new sink_t(backend));

                core->add_sink(this->log_sink);

                boost::log::add_common_attributes();
              }

            virtual ~generic_batcher()
              {
                boost::shared_ptr< boost::log::core > core = boost::log::core::get();

                core->remove_sink(this->log_sink);
              }

            //! Return the maximum memory available to this worker
            size_t get_capacity() const { return(this->capacity); }

            //! Set the path to the (new) container
            void set_container_path(const boost::filesystem::path& path) { this->container_path = path; }

            //! Return the path to the (current) container
            const boost::filesystem::path& get_container_path() const { return(this->container_path); }

            //! Set an implementation-dependent handle
            template <typename handle_type>
            void set_manager_handle(handle_type h)  { this->manager_handle = static_cast<void*>(h); }

            //! Return an implementation-dependent handle
            template <typename handle_type>
            void get_manager_handle(handle_type* h) { *h = static_cast<handle_type>(this->manager_handle); }

            //! Return number of fields
            unsigned int get_number_fields() { return(this->Nfields); }

            //! Close this batcher -- called at the end of an integration
            void close() { this->flush(action_close); }

            //! Push a background sample
            void push_backg(unsigned int time_serial, const std::vector<number>& values)
              {
                if(values.size() != 2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_BACKG);
                backg_item item;
                item.time_serial = time_serial;
                item.coords      = values;

                this->backg_batch.push_back(item), num_backg++;
                if(this->storage() > this->capacity) this->flush(action_replace);
              }

            //! Return logger
            boost::log::sources::severity_logger<log_severity_level>& get_log() { return(this->log_source); }

          protected:
            //! Compute the size of all currently-batched results
            virtual size_t storage() const = 0;

            //! Flush currently-batched results into the database, and then send to the master process
            virtual void flush(replacement_action action) = 0;

          protected:
            const unsigned int             capacity;

            const unsigned int             Nfields;

            unsigned int                   num_backg;
            std::vector<backg_item>        backg_batch;

            boost::filesystem::path        container_path;
            boost::filesystem::path        logdir_path;

            void*                          manager_handle;
            container_dispatch_function    dispatcher;
            container_replacement_function replacer;

            unsigned int                   worker_number;

            //! Logger source
            boost::log::sources::severity_logger<log_severity_level> log_source;

            //! Logger sink
            boost::shared_ptr< sink_t > log_sink;
          };

        class twopf_batcher: public generic_batcher
          {
          public:
            template <typename handle_type>
            twopf_batcher(unsigned int cap, unsigned int Nf,
                          const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                          const twopf_writer_group& w,
                          container_dispatch_function d, container_replacement_function r,
                          handle_type h, unsigned int wn)
              : generic_batcher(cap, Nf, cp, lp, d, r, h, wn), writers(w), num_twopf(0)
              {
              }

            void push_twopf(unsigned int time_serial, unsigned int k_serial, const std::vector<number>& values)
              {
                if(values.size() != 2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TWOPF);
                twopf_item item;
                item.time_serial    = time_serial;
                item.kconfig_serial = k_serial;
                item.elements       = values;

                this->twopf_batch.push_back(item), num_twopf++;
                if(this->storage() > this->capacity) this->flush(action_replace);
              }

          protected:
            size_t storage() const { return((sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->num_backg
                                            + (2*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*this->num_twopf); }

            void flush(replacement_action action)
              {
                BOOST_LOG_SEV(this->get_log(), normal) << "** flushing twopf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage()) << ", pushing to master";

                this->writers.backg(this, this->backg_batch);
                this->writers.twopf(this, this->twopf_batch);

                this->backg_batch.clear();
                this->twopf_batch.clear();
                this->num_backg = this->num_twopf = 0;

                // push a message to the master node, indicating that new data is available
                // note that the order of calls to 'dispatcher' and 'replacer' is important
                // because 'dispatcher' needs the current path name, not the one create by
                // 'replacer'
                this->dispatcher(this);

                // close current container, and replace with a new one if required
                this->replacer(this, action);
              }

          protected:
            const twopf_writer_group writers;

            unsigned int             num_twopf;

            std::vector<twopf_item>  twopf_batch;
          };


        class threepf_batcher: public generic_batcher
          {
          public:

            typedef enum { real_twopf, imag_twopf } twopf_type;

            template <typename handle_type>
            threepf_batcher(unsigned int cap, unsigned int Nf,
                            const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                            const threepf_writer_group& w,
                            container_dispatch_function d, container_replacement_function r,
                            handle_type h, unsigned int wn)
              : generic_batcher(cap, Nf, cp, lp, d, r, h, wn), writers(w), num_twopf_re(0), num_twopf_im(0), num_threepf(0)
              {
              }

            void push_twopf(unsigned int time_serial, unsigned int k_serial, const std::vector<number>& values, twopf_type type=real_twopf)
              {
                if(values.size() != 2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TWOPF);
                twopf_item item;
                item.time_serial    = time_serial;
                item.kconfig_serial = k_serial;
                item.elements       = values;

                if(type == real_twopf) this->twopf_re_batch.push_back(item), num_twopf_re++;
                else                   this->twopf_im_batch.push_back(item), num_twopf_im++;

                if(this->storage() > this->capacity) this->flush(action_replace);
              }

            void push_threepf(unsigned int time_serial, unsigned int k_serial, const std::vector<number>& values)
              {
                if(values.size() != 2*this->Nfields*2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_THREEPF);
                threepf_item item;
                item.time_serial    = time_serial;
                item.kconfig_serial = k_serial;
                item.elements       = values;

                this->threepf_batch.push_back(item), num_threepf++;
                if(this->storage() > this->capacity) this->flush(action_replace);
              }

          protected:
            size_t storage() const { return((sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->num_backg
                                            + (2*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*(this->num_twopf_re + this->num_twopf_im)
                                            + (2*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*2*this->Nfields*sizeof(number))*this->num_threepf); }

            void flush(replacement_action action)
              {
                BOOST_LOG_SEV(this->get_log(), normal) << "** flushing threepf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage()) << ", pushing to master";

                this->writers.backg(this, this->backg_batch);
                this->writers.twopf_re(this, this->twopf_re_batch);
                this->writers.twopf_im(this, this->twopf_im_batch);
                this->writers.threepf(this, this->threepf_batch);

                this->backg_batch.clear();
                this->twopf_re_batch.clear();
                this->twopf_im_batch.clear();
                this->threepf_batch.clear();
                this->num_backg = this->num_twopf_re = this->num_twopf_im = this->num_threepf = 0;

                // push a message to the master node, indicating that new data is available
                // note that the order of calls to 'dispatcher' and 'replacer' is important
                // because 'dispatcher' needs the current path name, not the one create by
                // 'replacer'
                this->dispatcher(this);

                // close current container, and replace with a new one if required
                this->replacer(this, action);
              }

          protected:
            const threepf_writer_group writers;

            unsigned int               num_twopf_re;
            unsigned int               num_twopf_im;
            unsigned int               num_threepf;

            std::vector<twopf_item>    twopf_re_batch;
            std::vector<twopf_item>    twopf_im_batch;
            std::vector<threepf_item>  threepf_batch;
          };


        // CONSTRUCTOR, DESTRUCTOR

      public:
        //! Create a data_manager instance
        data_manager(unsigned int cp)
          : capacity(cp)
          {
          }

        //! Destroy the data_manager instance. In practice this would always be delegated to an implementation class
        virtual ~data_manager()
          {
          }


        // INTERFACE -- WRITE CONTAINER HANDLING

      public:
        //! Create a new container. Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void create_container(typename repository<number>::integration_container& ctr) = 0;

        //! Open an existing container
        virtual void open_container(typename repository<number>::integration_container& ctr) = 0;

        //! Close an open container
        virtual void close_container(typename repository<number>::integration_container& ctr) = 0;


        // INTERFACE -- WRITE INDEX TABLES FOR A CONTAIER

      public:
        //! Create tables needed for a twopf container
        virtual void create_tables(typename repository<number>::integration_container& ctr, twopf_task<number>* tk,
                                   unsigned int Nfields) = 0;

        //! Create tables needed for a threepf container
        virtual void create_tables(typename repository<number>::integration_container& ctr, threepf_task<number>* tk,
                                   unsigned int Nfields) = 0;


        // INTERFACE -- TASK FILES FOR A CONTAINER

      public:
        //! Create a list of task assignments, over a number of devices, from a work queue of twopf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_container& ctr, const work_queue<twopf_kconfig>& queue) = 0;

        //! Create a list of task assignments, over a number of devices, from a work queue of threepf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_container& ctr, const work_queue<threepf_kconfig>& queue) = 0;

        //! Read a list of task assignments for a particular worker
        virtual std::set<unsigned int> read_taskfile(const boost::filesystem::path& taskfile, unsigned int worker) = 0;


        // INTERFACE -- TEMPORARY CONTAINERS

      public:
        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual twopf_batcher create_temp_twopf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                          unsigned int worker, unsigned int Nfields,
                                                          container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual threepf_batcher create_temp_threepf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                              unsigned int worker, unsigned int Nfields,
                                                              container_dispatch_function dispatcher) = 0;

        //! Aggregate a temporary twopf container into a principal container
        virtual void aggregate_twopf_batch(typename repository<number>::integration_container& ctr,
                                           const std::string& temp_ctr, model<number>* m, task<number>* tk) = 0;

        //! Aggregate a temporary threepf container into a principal container
        virtual void aggregate_threepf_batch(typename repository<number>::integration_container& ctr,
                                             const std::string& temp_ctr, model<number>* m, task<number>* tk) = 0;


        // INTERNAL DATA

      protected:
        //! Maximum memory available to each worker process
        unsigned int capacity;
      };

  }   // namespace transport



#endif //__data_manager_H_
