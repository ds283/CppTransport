//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __generic_batcher_H_
#define __generic_batcher_H_

#include <vector>
#include <list>
#include <functional>
#include <memory>

#include "transport-runtime-api/defaults.h"

#include "transport-runtime-api/utilities/host_information.h"
#include "transport-runtime-api/utilities/formatter.h"

#include "boost/filesystem/operations.hpp"
#include "boost/timer/timer.hpp"
#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/attributes.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"


namespace transport
	{

    // Batcher objects, used by integration workers to push results into a container

    //! Abstract batcher object, from which the concrete two- and threepf-batchers are derived.
    //! The batcher has a log directory, used for logging all transaction written into it,
    //! and also has a container replacement mechanism which writes all cached data into
    //! a data_manager-managed temporary file, and then pushes it to the master process.
    class generic_batcher
	    {

      public:

        //! Internal flag indicating whether flushes occur whenever the batcher becomes full,
        //! or whether we wait until the end-of-integration is reported.
        //! To unwind integrations, we need the delayed mode.
        typedef enum { flush_immediate, flush_delayed } flush_mode;

        //! Internal flag indicating the action which should be taken by
        //! a batcher when its temporary container is replaced.
        typedef enum { action_replace, action_close } replacement_action;

        //! data-manager callback to close a temporary container and replace it with another one
        typedef std::function<void(generic_batcher* batcher, replacement_action)> container_replacement_function;

        //! task-manager callback to push a container to the master node
        typedef std::function<void(generic_batcher* batcher)> container_dispatch_function;

        //! Logging severity level
        typedef enum { datapipe_pull, normal, warning, error, critical } log_severity_level;
        typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend > sink_t;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        template <typename handle_type>
        generic_batcher(unsigned int cap,
                        const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                        container_dispatch_function d, container_replacement_function r,
                        handle_type h, unsigned int w, bool no_log=false);

        virtual ~generic_batcher();


        // ADMINISTRATION

      public:

        //! Return the maximum memory available for batchers on this worker
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
        void get_manager_handle(handle_type* h) const { *h = static_cast<handle_type>(this->manager_handle); }

        //! Return worker numbers
        unsigned int get_worker_number() const { return(this->worker_number); }

        //! Return host information
        const host_information& get_host_information() const { return(this->host_info); }

        //! Close batcher
        virtual void close();


        // LOGGING

      public:

        //! Return logger
        boost::log::sources::severity_logger<log_severity_level>& get_log() { return(this->log_source); }


        // FLUSH INTERFACE

      public:

        //! Get flush mode
        flush_mode get_flush_mode() const { return(this->mode); }

        //! Set flush mode
        void set_flush_mode(flush_mode f) { this->mode = f; }

      protected:

        //! Compute the size of all currently-batched results
        virtual size_t storage() const = 0;

        //! Flush currently-batched results into the database, and then send to the master process
        virtual void flush(replacement_action action) = 0;

        //! Check if the batcher is ready for flush
        void check_for_flush();


        // INTERNAL DATA

      protected:

        //! Host information
        host_information                                         host_info;

        //! Worker number associated with this batcher
        unsigned int                                             worker_number;


        // OTHER INTERNAL DATA

        //! Capacity available
        unsigned int                                             capacity;

        //! Container path
        boost::filesystem::path                                  container_path;

        //! Log directory path
        boost::filesystem::path                                  logdir_path;

        //! Data manager handle
        void*                                                    manager_handle;

        //! Callback for dispatching a container
        container_dispatch_function                              dispatcher;

        //! Callback for obtaining a replacement container
        container_replacement_function                           replacer;


        // FLUSH HANDLING

        //! Needs flushing at next opportunity?
        bool                                                     flush_due;

        //! Flushing mode
        flush_mode                                               mode;


        // LOGGING

        //! Logger source
        boost::log::sources::severity_logger<log_severity_level> log_source;

        //! Logger sink
        boost::shared_ptr< sink_t >                              log_sink;

	    };


    // GENERIC BATCHER METHODS


    template <typename handle_type>
    generic_batcher::generic_batcher(unsigned int cap,
                                     const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                     container_dispatch_function d, container_replacement_function r,
                                     handle_type h, unsigned int w, bool no_log)
	    : capacity(cap),
	      container_path(cp),
	      logdir_path(lp),
	      dispatcher(d),
	      replacer(r),
	      worker_number(w),
	      manager_handle(static_cast<void*>(h)),
	      mode(flush_immediate),
	      flush_due(false)
	    {
        // set up logging

        std::ostringstream log_file;
        log_file << __CPP_TRANSPORT_LOG_FILENAME_A << worker_number << __CPP_TRANSPORT_LOG_FILENAME_B;

        boost::filesystem::path log_path = logdir_path / log_file.str();

        if(!no_log)
	        {
            boost::shared_ptr<boost::log::core> core = boost::log::core::get();

//		    core->set_filter(boost::log::trivial::severity >= normal);

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
        else
	        {
            this->log_sink.reset();
	        }

        BOOST_LOG_SEV(this->log_source, normal) << "** Instantiated batcher (capacity " << format_memory(capacity) << ")"
	        << " on MPI host " << host_info.get_host_name()
	        << ", OS = " << host_info.get_os_name()
	        << ", Version = " << host_info.get_os_version()
	        << " (Release = " << host_info.get_os_release()
	        << ") | " << host_info.get_architecture()
	        << " | CPU vendor = " << host_info.get_cpu_vendor_id() << std::endl;
	    }


    generic_batcher::~generic_batcher()
	    {
        if(this->log_sink)    // implicitly converts to bool, value true if not null
	        {
            boost::shared_ptr< boost::log::core > core = boost::log::core::get();
            core->remove_sink(this->log_sink);
	        }
	    }


    void generic_batcher::close()
	    {
        this->flush(action_close);
	    }


    void generic_batcher::check_for_flush()
	    {
        if(this->storage() > this->capacity)
	        {
            if(this->mode == flush_immediate) this->flush(action_replace);
            else                              this->flush_due = true;
	        }
	    }

	}   // namespace transport


#endif //__generic_batcher_H_
