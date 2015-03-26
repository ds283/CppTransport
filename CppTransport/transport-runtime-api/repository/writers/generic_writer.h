//
// Created by David Seery on 25/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __generic_writer_H_
#define __generic_writer_H_


#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/localizations/messages_en.h"

#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"


// log file name
#define __CPP_TRANSPORT_LOG_FILENAME_A "worker_"
#define __CPP_TRANSPORT_LOG_FILENAME_B "_%3N.log"


namespace transport
	{

    // GENERIC WRITER

    //! base_writer is a dummy class at the bottom of the *_writer hierarchy.
    //! It is only a placeholder to allow complete instantiation of generic_writer<>
    class base_writer
	    {

      public:

        //! Types needed for logging
        typedef enum { normal, notification, warning, error, critical } log_severity_level;

        typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> sink_t;


	      // CONSTRUCTOR, DESTRUCTOR

      public:

		    //! constructor
        base_writer() = default;

		    //! destructor
        virtual ~base_writer() = default;

	    };

    //! 'generic_writer' supplies generic services for writers
    template <typename WriterObject>
    class generic_writer: public base_writer
	    {

      public:

        // In these callbacks, we would ideally like them to take an argument of type WriterObject&.
        // However, that can't be done because when the WriterObject template is instantiated,
        // causing instantiation of generic_writer<WriterObject>, WriterObject itself will be
        // an incomplete type. That doesn't matter inside method declarations (they are only
        // instantiated when they are used), but it *does* matter for typedefs inside generic_writer<>
        // which must be resolved when instantiated.

        // The ultimate solution might be to extract the *_writer classes from within repository<number>
        // and declare at namespace scope. That allows forward declarations and explicit specializations,
        // and the problem can be solved using traits classes.

        //! Define a commit callback object. Used to commit data products to the repository
        typedef std::function<void(base_writer&)> commit_callback;

        //! Define an abort callback object. Used to abort storage of data products
        typedef std::function<void(base_writer&)> abort_callback;

        //! Define an aggregation callback object. Used to aggregate results from worker processes
        typedef std::function<bool(base_writer&, const std::string&)> aggregate_callback;

        class callback_group
	        {
          public:
            commit_callback commit;
            abort_callback  abort;
	        };

        class metadata_group
	        {
          public:
            std::list<std::string> tags;
            boost::posix_time::ptime creation_time;
	        };

        class paths_group
	        {
          public:
            boost::filesystem::path root;
            boost::filesystem::path output;
            boost::filesystem::path data;
            boost::filesystem::path log;
            boost::filesystem::path task;
            boost::filesystem::path temp;
	        };

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a generic writer object
        generic_writer(const callback_group& c, const metadata_group& m, const paths_group& p, unsigned int w);

        //! destroy a generic writer object
        virtual ~generic_writer();


        // AGGREGATION

      public:

        //! Set aggregator
        void set_aggregation_handler(aggregate_callback c) { this->aggregator = c; }

        //! Aggregate a product
        bool aggregate(const std::string& product);


        // DATABASE FUNCTIONS

      public:

        //! Commit contents of this integration_writer to the database
        void commit() { this->callbacks.commit(static_cast<WriterObject&>(*this)); this->committed = true; }


        // ADMINISTRATION

      public:

        //! Set data_manager handle for data container
        template <typename data_manager_type>
        void set_data_manager_handle(data_manager_type data);

        //! Return data_manager handle for data container

        //! Throws a REPOSITORY_ERROR exception if the handle is unset
        template <typename data_manager_type>
        void get_data_manager_handle(data_manager_type* data);


        // METADATA

      public:

        //! Return tags
        const std::list<std::string>& get_tags() const { return(this->generic_metadata.tags); }

        //! Get creation time
        const boost::posix_time::ptime& get_creation_time() const { return(this->generic_metadata.creation_time); }


        // ABSOLUTE PATHS

      public:

        //! Return path to output directory
        boost::filesystem::path get_abs_output_path() const { return(this->paths.root/this->paths.output); }

        //! Return path to data container
        boost::filesystem::path get_abs_container_path() const { return(this->paths.root/this->paths.data); }

        //! Return path to log directory
        boost::filesystem::path get_abs_logdir_path() const { return(this->paths.root/this->paths.log); }

        //! Return path to directory for temporary files
        boost::filesystem::path get_abs_tempdir_path() const { return(this->paths.root/this->paths.temp); }


        // RELATIVE PATHS

      public:

        //! Return path to output directory
        boost::filesystem::path get_relative_output_path() const { return(this->paths.output); }

        //! Return path to data container
        boost::filesystem::path get_relative_container_path() const { return(this->paths.data); }


        // LOGGING

      public:

        //! Return logger
        boost::log::sources::severity_logger<log_severity_level>& get_log() { return (this->log_source); }


        // INTERNAL DATA

      protected:

        // SUCCESS FLAG - USED TO DETERMINE WHETHER TO ABORT/ROLLBACK WHEN WINDING UP

        bool committed;


        // REPOSITORY CALLBACK FUNCTIONS

        //! Repository callbacks
        callback_group callbacks;


        // DATA MANAGER CALLBACK FUNCTIONS

        //! Aggregate callback
        aggregate_callback aggregator;


        // PATHS

        //! paths associated with this writer
        const paths_group paths;


        // GENERIC METADATA

        //! metadata
        metadata_group generic_metadata;


        // MISCELLANEOUS

        //! our MPI worker number
        const unsigned int worker_number;

        //! internal handle used by data_manager to associate this writer with an integration database
        void* data_manager_handle;


        // LOGGING

        //! Logger source
        boost::log::sources::severity_logger<log_severity_level> log_source;

        //! Logger sink
        boost::shared_ptr<sink_t> log_sink;

	    };


    // GENERIC WRITER METHODS


    template <typename WriterObject>
    generic_writer<WriterObject>::generic_writer(const typename generic_writer<WriterObject>::callback_group& c,
                                                 const typename generic_writer<WriterObject>::metadata_group& m,
                                                 const typename generic_writer<WriterObject>::paths_group& p,
                                                 unsigned int w)
	    : callbacks(c),
	      generic_metadata(m),
	      paths(p),
	      worker_number(w),
	      data_manager_handle(nullptr),
	      committed(false)
	    {
        // set up logging

        std::ostringstream log_file;
        log_file << __CPP_TRANSPORT_LOG_FILENAME_A << worker_number << __CPP_TRANSPORT_LOG_FILENAME_B;
        boost::filesystem::path logfile_path = paths.root / paths.log / log_file.str();

        boost::shared_ptr<boost::log::core> core = boost::log::core::get();

        boost::shared_ptr<boost::log::sinks::text_file_backend> backend =
	                                                                boost::make_shared<boost::log::sinks::text_file_backend>(boost::log::keywords::file_name = logfile_path.string());

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


    template <typename WriterObject>
    generic_writer<WriterObject>::~generic_writer()
	    {
        // remove logging objects
        boost::shared_ptr<boost::log::core> core = boost::log::core::get();

        core->remove_sink(this->log_sink);
	    }


    template <typename WriterObject>
    bool generic_writer<WriterObject>::aggregate(const std::string& product)
	    {
        if(!this->aggregator)
	        {
            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_WRITER_AGGREGATOR_UNSET);
	        }

        return this->aggregator(static_cast<WriterObject&>(*this), product);
	    }


    template <typename WriterObject>
    template <typename data_manager_type>
    void generic_writer<WriterObject>::set_data_manager_handle(data_manager_type data)
	    {
        this->data_manager_handle = static_cast<void*>(data);  // will fail if data_manager_type not (static-)castable to void*
	    }


    template <typename WriterObject>
    template <typename data_manager_type>
    void generic_writer<WriterObject>::get_data_manager_handle(data_manager_type* data)
	    {
        if(this->data_manager_handle == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_OUTPUT_WRITER_UNSETHANDLE);
        *data = static_cast<data_manager_type>(this->data_manager_handle);
	    }


	}   // namespace transport


#endif //__generic_writer_H_
