//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __data_manager_H_
#define __data_manager_H_


#include <set>
#include <vector>
#include <list>

#include <math.h>

#include "transport-runtime-api/scheduler/work_queue.h"
#include "transport-runtime-api/manager/repository.h"

#include "transport-runtime-api/utilities/formatter.h"

#include "boost/filesystem/operations.hpp"
#include "boost/timer/timer.hpp"
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

    // forward-declare model to avoid circular references:
		// model.h includes data_manager.h, and conversely
		// data_manager needs to know about model objects.
		// The compromise is that data_manager.h should be included before model.h
    template <typename number> class model;

		// forward-declare task types
		template <typename number> class task;
		template <typename number> class integration_task;
		template <typename number> class twopf_task;
		template <typename number> class threepf_task;
		template <typename number> class output_task;

		// forward-declare task elements
		class twopf_kconfig;
		class threepf_kconfig;
		template <typename number> class output_task_element;

//		// forward-declare work queue
//		template <typename ItemType> class work_queue;

    template <typename number>
    class data_manager
      {

      public:

        // data structures for storing individual sample points from each integration

		    //! Stores a background field configuration associated with single time-point
        class backg_item
          {
          public:
            unsigned int        time_serial;
            std::vector<number> coords;
          };

		    //! Stores a twopf configuration associated with a single time-point and k-configuration
        class twopf_item
          {
          public:
            unsigned int        time_serial;
            unsigned int        kconfig_serial;
            std::vector<number> elements;
          };

		    //! Stores a threepf configuration associated with a single time-point and k-configuration
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

        //! Background writer function
        typedef std::function<void(generic_batcher*, const std::vector<backg_item>&)> backg_writer;

        //! Two-point function writer function
        typedef std::function<void(generic_batcher*, const std::vector<twopf_item>&)> twopf_writer;

        //! Three-point function writer function
        typedef std::function<void(generic_batcher*, const std::vector<threepf_item>&)> threepf_writer;


				//! Aggregation of writers for a two-point function integration
        class twopf_writer_group
          {
          public:
            backg_writer backg;
            twopf_writer twopf;
          };


		    //! Aggregation of writers for a three-point function integration
        class threepf_writer_group
          {
          public:
            backg_writer   backg;
            twopf_writer   twopf_re;
            twopf_writer   twopf_im;
            threepf_writer threepf;
          };


		    // data pipe, used by derived content providers to extract content from an output group
		    class datapipe;

		    //! Internal flag indicating the action which should be taken by
		    //! a batcher when its temporary container is replaced.
        typedef enum { action_replace, action_close } replacement_action;

		    //! data-manager callback to close a temporary container and replace it with another one
        typedef std::function<void(generic_batcher* batcher, replacement_action)> container_replacement_function;

		    //! task-manager callback to push a container to the master node
        typedef std::function<void(generic_batcher* batcher)> container_dispatch_function;

		    //! task-manager callback to push new derived content to the master node
		    typedef std::function<void(datapipe* pipe)> derived_content_dispatch_function;

		    //! Logging severity level
        typedef enum { normal, notification, warning, error, critical } log_severity_level;
        typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend > sink_t;

        // Batcher objects, used by integration workers to push results into a container

		    //! Abstract batcher object, from which the concrete two- and threepf-batchers are derived.
		    //! The batcher has a log directory, used for logging all transaction written into it,
		    //! and also has a container replacement mechanism which writes all cached data into
		    //! a data_manager-managed temporary file, and then pushes it to the master process.
        class generic_batcher
          {

          public:

            template <typename handle_type>
            generic_batcher(unsigned int cap, unsigned int Nf,
                            const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                            container_dispatch_function d, container_replacement_function r,
                            handle_type h, unsigned int w, boost::timer::cpu_timer& tm)
              : capacity(cap), Nfields(Nf), container_path(cp), logdir_path(lp), num_backg(0),
                dispatcher(d), replacer(r), worker_number(w),
                manager_handle(static_cast<void*>(h)), integration_timer(tm)
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

            const unsigned int                                       capacity;

            const unsigned int                                       Nfields;

            unsigned int                                             num_backg;
            std::vector<backg_item>                                  backg_batch;

            boost::filesystem::path                                  container_path;
            boost::filesystem::path                                  logdir_path;

            void*                                                    manager_handle;
            container_dispatch_function                              dispatcher;
            container_replacement_function                           replacer;

            unsigned int                                             worker_number;

            //! Logger source
            boost::log::sources::severity_logger<log_severity_level> log_source;

            //! Logger sink
            boost::shared_ptr< sink_t >                              log_sink;

            //! Integration timer - should be stopped while batching
            boost::timer::cpu_timer&                                 integration_timer;
          };


        class twopf_batcher: public generic_batcher
          {
          public:
            template <typename handle_type>
            twopf_batcher(unsigned int cap, unsigned int Nf,
                          const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                          const twopf_writer_group& w,
                          container_dispatch_function d, container_replacement_function r,
                          handle_type h, unsigned int wn, boost::timer::cpu_timer& tm)
              : generic_batcher(cap, Nf, cp, lp, d, r, h, wn, tm), writers(w), num_twopf(0)
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
                // pause integration timer
                this->integration_timer.stop();

                BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing twopf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

                // set up a timer to measure how long it takes to flush
                boost::timer::cpu_timer flush_timer;

                this->writers.backg(this, this->backg_batch);
                this->writers.twopf(this, this->twopf_batch);

                flush_timer.stop();
                BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

                this->backg_batch.clear();
                this->twopf_batch.clear();
                this->num_backg = this->num_twopf = 0;

                // push a message to the master node, indicating that new data is available
                // note that the order of calls to 'dispatcher' and 'replacer' is important
                // because 'dispatcher' needs the current path name, not the one created by
                // 'replacer'
                this->dispatcher(this);

                // close current container, and replace with a new one if required
                this->replacer(this, action);

                // restart integration timer
                this->integration_timer.resume();
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
                            handle_type h, unsigned int wn, boost::timer::cpu_timer& tm)
              : generic_batcher(cap, Nf, cp, lp, d, r, h, wn, tm), writers(w), num_twopf_re(0), num_twopf_im(0), num_threepf(0)
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
                // pause integration timer
                this->integration_timer.stop();

                BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing threepf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

                // set up a timer to measure how long it takes to flush
                boost::timer::cpu_timer flush_timer;

                this->writers.backg(this, this->backg_batch);
                this->writers.twopf_re(this, this->twopf_re_batch);
                this->writers.twopf_im(this, this->twopf_im_batch);
                this->writers.threepf(this, this->threepf_batch);

                flush_timer.stop();
                BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

                this->backg_batch.clear();
                this->twopf_re_batch.clear();
                this->twopf_im_batch.clear();
                this->threepf_batch.clear();
                this->num_backg = this->num_twopf_re = this->num_twopf_im = this->num_threepf = 0;

                // push a message to the master node, indicating that new data is available
                // note that the order of calls to 'dispatcher' and 'replacer' is important
                // because 'dispatcher' needs the current path name, not the one created by
                // 'replacer'
                this->dispatcher(this);

                // close current container, and replace with a new one if required
                this->replacer(this, action);

                // restart integration timer
                this->integration_timer.resume();
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


		    // Data pipe objects

		    //! Attach function for a datapipe
		    typedef std::function<void(datapipe*,typename repository<number>::output_group&)> datapipe_attach_function;

		    //! Detach function for a datapipe
		    typedef std::function<void(datapipe*)> datapipe_detach_function;

		    //! Extract a set of time sample-points from a datapipe
		    typedef std::function<void(datapipe*,const std::vector<unsigned int>&,std::vector<double>&)> datapipe_time_sample_function;

		    //! Extract a background field at a set of time sample-points
		    typedef std::function<void(datapipe*,unsigned int,const std::vector<unsigned int>&,std::vector<number>&)> datapipe_background_time_sample_function;

		    //! Data pipe, used when generating derived content to extract data froman integration database.
		    //! The datapipe has a log directory, used for logging transactions on the pipe.
		    class datapipe
			    {

		      public:

				    //! Construct a datapipe
				    datapipe(const boost::filesystem::path& lp, const boost::filesystem::path& tp,
				             unsigned int w, boost::timer::cpu_timer& tm,
				             datapipe_attach_function& at, datapipe_detach_function& dt,
				             datapipe_time_sample_function& tsf,
				             datapipe_background_time_sample_function& btsf);

				    //! Destroy a datapipe
		        ~datapipe();


		        // MANAGEMENT

		      public:

		        //! Close this datapipe
		        void close() { ; }

		        //! Set an implementation-dependent handle
		        template <typename handle_type>
		        void set_manager_handle(handle_type h)  { this->manager_handle = static_cast<void*>(h); }

		        //! Return an implementation-dependent handle
		        template <typename handle_type>
		        void get_manager_handle(handle_type* h) const { *h = static_cast<handle_type>(this->manager_handle); }

				    //! Validate that the pipe is attached to a container
				    bool validate(void);

				    //! Return temporary files path
				    const boost::filesystem::path& get_temporary_files_path() const { return(this->temporary_path); }

		        //! Return logger
		        boost::log::sources::severity_logger<log_severity_level>& get_log() { return(this->log_source); }


				    // ATTACH, DETACH OUTPUT GROUPS

		      public:

				    //! Attach an output-group to the datapipe, ready for reading
				    void attach(typename repository<number>::output_group& group,
				                typename data_manager<number>::derived_content_dispatch_function& dispatcher);

				    //! Detach an output-group from the datapipe
				    void detach(void);

				    //! Get attached output group
				    typename repository<number>::output_group* get_attached_output_group(void) const;


				    // PULL DATA

		      public:

				    //! Pull a set of time serial numbers from the database
				    void pull_time_sample(const std::vector<unsigned int>& serial_numbers, std::vector<double>& sample);

				    //! Pull a sample of a background field from the database
				    void pull_background_time_sample(unsigned id, const std::vector<unsigned int>& serial_numbers, std::vector<number>& sample);


				    // INTERNAL DATA

		      private:

				    //! Path to logging directory
				    const boost::filesystem::path                                     logdir_path;

				    //! Path to temporary files
				    const boost::filesystem::path                                     temporary_path;

						//! Unique serial number identifying the worker process which owns this datapipe
				    const unsigned int                                                worker_number;

		        //! Logger source
		        boost::log::sources::severity_logger<log_severity_level>          log_source;

		        //! Logger sink
		        boost::shared_ptr< sink_t >                                       log_sink;

				    //! Timer, used to track how long the datapipe is kept open
				    boost::timer::cpu_timer&                                          timer;

				    //! Currently-attached output group; null is no group is attached
				    typename repository<number>::output_group*                        attached_group;

				    //! Currently-attached dispatch function; null is no dispatcher
				    typename data_manager<number>::derived_content_dispatch_function* attached_dispatcher;

				    //! Implementation-dependent handle
				    void*                                                             manager_handle;

				    // CALLBACKS

				    //! Callback: attach a datapipe
				    datapipe_attach_function                                          attach_callback;

				    //! Callback: detach a datapipe
				    datapipe_detach_function                                          detach_callback;

				    //! Callback: extract a time sample
				    datapipe_time_sample_function                                     time_sample_callback;

				    //! Callback: extract a time sample of a background field
				    datapipe_background_time_sample_function                          background_time_sample_callback;
			    };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a data_manager instance with a nominated capacity per batcher
        data_manager(unsigned int cp)
          : capacity(cp)
          {
          }

        //! Destroy the data_manager instance. In practice this would always be delegated to an implementation class
        virtual ~data_manager() = default;


        // WRITER HANDLING

      public:

        //! Create data files for an integration_writer object.
        //! Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void create_writer(typename repository<number>::integration_writer& ctr) = 0;

        //! Close an open container integration_writer object.
        virtual void close_writer(typename repository<number>::integration_writer& ctr) = 0;

        //! Create data files for a new derived_content_writer object.
        virtual void create_writer(typename repository<number>::derived_content_writer& ctr) = 0;

        //! Close an open derived_content_writer object.
        virtual void close_writer(typename repository<number>::derived_content_writer& ctr) = 0;


        // WRITE INDEX TABLES FOR A DATA CONTAINER

      public:

        //! Create tables needed for a twopf container
        virtual void create_tables(typename repository<number>::integration_writer& ctr, twopf_task<number>* tk,
                                   unsigned int Nfields) = 0;

        //! Create tables needed for a threepf container
        virtual void create_tables(typename repository<number>::integration_writer& ctr, threepf_task<number>* tk,
                                   unsigned int Nfields) = 0;


        // TASK FILES

      public:

        //! Create a list of task assignments, over a number of devices, from a work queue of twopf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_writer& writer, const work_queue<twopf_kconfig>& queue) = 0;

        //! Create a list of task assignments, over a number of devices, from a work queue of threepf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_writer& writer, const work_queue<threepf_kconfig>& queue) = 0;

		    //! Create a list of task assignments, over a number of devices, for a work queue of output_task_element-s
		    virtual void create_taskfile(typename repository<number>::derived_content_writer& writer, const work_queue< output_task_element<number> >& queue) = 0;

        //! Read a list of task assignments for a particular worker
        virtual std::set<unsigned int> read_taskfile(const boost::filesystem::path& taskfile, unsigned int worker) = 0;


        // TEMPORARY CONTAINERS

      public:

        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual twopf_batcher create_temp_twopf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                          unsigned int worker, unsigned int Nfields,
                                                          container_dispatch_function dispatcher,
                                                          boost::timer::cpu_timer& integration_timer) = 0;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual threepf_batcher create_temp_threepf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                              unsigned int worker, unsigned int Nfields,
                                                              container_dispatch_function dispatcher,
                                                              boost::timer::cpu_timer& integration_timer) = 0;

        //! Aggregate a temporary twopf container into a principal container
        virtual void aggregate_twopf_batch(typename repository<number>::integration_writer& ctr,
                                           const std::string& temp_ctr, model<number>* m, integration_task<number>* tk) = 0;

        //! Aggregate a temporary threepf container into a principal container
        virtual void aggregate_threepf_batch(typename repository<number>::integration_writer& ctr,
                                             const std::string& temp_ctr, model<number>* m, integration_task<number>* tk) = 0;


		    // DATA PIPES AND DATA ACCESS

      public:

		    //! Create a datapipe
		    virtual datapipe create_datapipe(const boost::filesystem::path& logdir, const boost::filesystem::path& tempdir,
		                                     unsigned int worker, boost::timer::cpu_timer& timer) = 0;

		    //! Pull a set of time sample-points from a datapipe
		    virtual void pull_time_sample(datapipe* pipe, const std::vector<unsigned int>& serial_numbers, std::vector<double>& sample) = 0;

		    //! Pull a time sample of a background field from a datapipe
		    virtual void pull_background_time_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& t_serials, std::vector<number>& sample) = 0;

        // INTERNAL DATA

      protected:

        //! Maximum memory available to each worker process
        unsigned int capacity;
      };


		// DATAPIPE METHODS



    template <typename number>
    data_manager<number>::datapipe::datapipe(const boost::filesystem::path& lp, const boost::filesystem::path& tp,
                                             unsigned int w, boost::timer::cpu_timer& tm,
                                             datapipe_attach_function& at, datapipe_detach_function& dt,
                                             datapipe_time_sample_function& tsf,
                                             datapipe_background_time_sample_function& btsf)
	    : logdir_path(lp), temporary_path(tp), worker_number(w), timer(tm),
	      attach_callback(at), detach_callback(dt), time_sample_callback(tsf),
	      background_time_sample_callback(btsf),
	      attached_group(nullptr), attached_dispatcher(nullptr)
	    {
        std::ostringstream log_file;
        log_file << __CPP_TRANSPORT_LOG_FILENAME_A << worker_number << __CPP_TRANSPORT_LOG_FILENAME_B;

        boost::filesystem::path log_path = logdir_path / log_file.str();

        boost::shared_ptr<boost::log::core> core = boost::log::core::get();

        boost::shared_ptr<boost::log::sinks::text_file_backend> backend =
	                                                                boost::make_shared<boost::log::sinks::text_file_backend>(boost::log::keywords::file_name = log_path.string());

        // enable auto-flushing of log entries
        // this degrades performance, but we are not writing many entries and they
        // will not be lost in the event of a crash
        backend->auto_flush(true);

        // Wrap it into the frontend and register in the core.
        // The backend requires synchronization in the frontend.
        this->log_sink = boost::shared_ptr<sink_t>(new sink_t(backend));

        core->add_sink(this->log_sink);

        boost::log::add_common_attributes();
	    }


    template <typename number>
    data_manager<number>::datapipe::~datapipe()
	    {
        boost::shared_ptr<boost::log::core> core = boost::log::core::get();

        core->remove_sink(this->log_sink);

        // detach any attached output group, if necessary
        if(this->attached_group != nullptr) this->detach();
	    }


    template <typename number>
    void data_manager<number>::datapipe::attach(typename repository<number>::output_group& group,
                                                typename data_manager<number>::derived_content_dispatch_function& dispatcher)
	    {
        assert(this->attached_group == nullptr);
        if(this->attached_group != nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_ATTACH_PIPE_ALREADY_ATTACHED);

        assert(this->attached_dispatcher == nullptr);
        if(this->attached_dispatcher != nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_ATTACH_PIPE_ALREADY_ATTACHED);

        // take copy of output group and dispatcher
        this->attached_group      = new typename repository<number>::output_group(group);
        this->attached_dispatcher = new typename data_manager<number>::derived_content_dispatch_function(dispatcher);

        BOOST_LOG_SEV(this->get_log(), data_manager<number>::normal) << "** DATAPIPE ATTACH output group " << boost::posix_time::to_simple_string(group.get_creation_time());

		    this->attach_callback(this, group);
	    }


		template <typename number>
    void data_manager<number>::datapipe::detach(void)
	    {
		    assert(this->attached_group != nullptr);
		    if(this->attached_group == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_DETACH_PIPE_NOT_ATTACHED);

		    assert(this->attached_dispatcher != nullptr);
		    if(this->attached_dispatcher == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR,  __CPP_TRANSPORT_DATAMGR_DETACH_PIPE_NOT_ATTACHED);

		    BOOST_LOG_SEV(this->get_log(), data_manager<number>::normal) << "** DATAPIPE DETACH output group " << boost::posix_time::to_simple_string(this->attached_group->get_creation_time());

				this->detach_callback(this);

		    delete this->attached_group;
		    delete this->attached_dispatcher;

		    this->attached_group = nullptr;
		    this->attached_dispatcher = nullptr;
	    }


    template <typename number>
    typename repository<number>::output_group* data_manager<number>::datapipe::get_attached_output_group(void) const
	    {
        assert(this->attached_group != nullptr);
        if(this->attached_group == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(this->attached_dispatcher != nullptr);
        if(this->attached_dispatcher == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR,  __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

		    return(this->attached_group);
	    }


    template <typename number>
    void data_manager<number>::datapipe::pull_time_sample(const std::vector<unsigned int>& serial_numbers, std::vector<double>& sample)
	    {
		    assert(this->attached_group != nullptr);
        if(this->attached_group == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(this->attached_dispatcher != nullptr);
        if(this->attached_dispatcher == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR,  __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

		    BOOST_LOG_SEV(this->get_log(), data_manager<number>::normal) << "** DATAPIPE pull time sample request";

		    this->time_sample_callback(this, serial_numbers, sample);
	    }


    template <typename number>
    void data_manager<number>::datapipe::pull_background_time_sample(unsigned int id, const std::vector<unsigned int>& serial_numbers,
                                                                     std::vector<number>& sample)
	    {
				assert(this->attached_group != nullptr);
        if(this->attached_group == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(this->attached_dispatcher != nullptr);
        if(this->attached_dispatcher == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR,  __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

		    BOOST_LOG_SEV(this->get_log(), data_manager<number>::normal) << "** DATAPIPE pull background time sample request";

		    this->background_time_sample_callback(this, id, serial_numbers, sample);
	    }


	  template <typename number>
		bool data_manager<number>::datapipe::validate(void)
		  {
			  return(this->attached_group != nullptr && this->attached_dispatcher != nullptr);
		  }


  }   // namespace transport



#endif //__data_manager_H_
