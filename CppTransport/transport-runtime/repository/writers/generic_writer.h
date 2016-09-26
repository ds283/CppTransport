//
// Created by David Seery on 25/03/15.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_GENERIC_WRITER_H
#define CPPTRANSPORT_GENERIC_WRITER_H


#include <iostream>
#include <list>
#include <string>
#include <memory>
#include <functional>

#include "transport-runtime/serialization/serializable.h"

#include "transport-runtime/exceptions.h"
#include "transport-runtime/localizations/messages_en.h"

#include "transport-runtime/utilities/host_information.h"

#include "boost/filesystem.hpp"
#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/attributes.hpp"
#include "boost/log/sources/channel_logger.hpp"
#include "boost/log/sources/severity_channel_logger.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/log/attributes.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/support/date_time.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/shared_ptr.hpp"


namespace transport
	{

    // GENERIC WRITER

    //! base_writer is a dummy class at the bottom of the *_writer hierarchy.
    //! It is only a placeholder to allow complete instantiation of generic_writer
    class base_writer
	    {

      public:

        //! Types needed for logging
        enum class log_severity_level { normal, notification, warning, error, critical };

        //! log sink; we use a multifile to split output into logs and reports
        typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> sink_t;

        //! logging source
        typedef boost::log::sources::severity_channel_logger<log_severity_level, std::string> logger;
        
        //! reporting source
        typedef boost::log::sources::channel_logger<std::string> reporter;

	      // CONSTRUCTOR, DESTRUCTOR

      public:

		    //! constructor
        base_writer() = default;

		    //! destructor
        virtual ~base_writer() = default;

	    };
    
    
    // overload << to push log_severity_level to stream
    std::ostream& operator<<(std::ostream& stream, base_writer::log_severity_level level)
      {
        static const std::map< base_writer::log_severity_level, std::string > stringize_map =
          {
            { base_writer::log_severity_level::normal, "normal" },
            { base_writer::log_severity_level::notification, "notification" },
            { base_writer::log_severity_level::warning, "warning" },
            { base_writer::log_severity_level::error, "error" },
            { base_writer::log_severity_level::critical, "CRITICAL" }
          };
        
        stream << stringize_map.at(level);
        
        return stream;
      }
    
    
    // overload << to push log_severity_level to Boost.Log log
    struct base_writer_severity_tag;
    boost::log::formatting_ostream& operator<<(boost::log::formatting_ostream& stream,
                                               const boost::log::to_log_manip<base_writer::log_severity_level, base_writer_severity_tag> manip)
      {
        static const std::map< base_writer::log_severity_level, std::string > stringize_map =
          {
            { base_writer::log_severity_level::normal, "norm" },
            { base_writer::log_severity_level::notification, "ntfy" },
            { base_writer::log_severity_level::warning, "warn" },
            { base_writer::log_severity_level::error, "err " },
            { base_writer::log_severity_level::critical, "CRIT" }
          };
        
        base_writer::log_severity_level level = manip.get();
        stream << stringize_map.at(level);

        return stream;
      }
    

    //! 'generic_writer' supplies generic services for writers
    class generic_writer: public base_writer
	    {

      public:

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
            boost::filesystem::path temp;
	        };

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a generic writer object
        generic_writer(const std::string& n, const metadata_group& m, const paths_group& p, unsigned int w);

        //! destroy a generic writer object
        virtual ~generic_writer();


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

        //! Get name
        const std::string& get_name() const { return(this->name); }

        //! Return tags
        const std::list<std::string>& get_tags() const { return(this->generic_metadata.tags); }

        //! Get creation time
        const boost::posix_time::ptime& get_creation_time() const { return(this->generic_metadata.creation_time); }

        //! Set failed status
        void set_fail(bool g) { this->fail = g; }

        //! Query failed status
        bool is_failed() const { return(this->fail); }


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

        //! Return path to log directory
        boost::filesystem::path get_relative_logdir_path() const { return(this->paths.log); }

        //! Return path to temporary directory
        boost::filesystem::path get_relative_tempdir_path() const { return(this->paths.temp); }


        // LOGGING AND REPORTING

      public:

        //! Return logger
        logger& get_log() { return this->log_source; }
        
        //! Return reporter
        reporter& get_report() { return this->report_source; }


        // INTERNAL DATA

      protected:

        //! Host information
        host_information host_info;

        //! name of content group we are writing to
        std::string name;


        // SUCCESS FLAG - USED TO DETERMINE WHETHER TO ABORT/ROLLBACK WHEN WINDING UP

        //! has this written been committed to the repository?
        bool committed;


        // PATHS

        //! paths associated with this writer
        const paths_group paths;


        // GENERIC METADATA

        //! metadata
        metadata_group generic_metadata;

        //! fail flag (eg. failed integrations, failed content generation)
        bool fail;


        // MISCELLANEOUS

        //! our MPI worker number
        const unsigned int worker_number;

        //! internal handle used by data_manager to associate this writer with an integration database
        void* data_manager_handle;


        // LOGGING

        //! Logger source
        boost::log::sources::severity_channel_logger<log_severity_level, std::string> log_source;
        
        //! Reporting source
        boost::log::sources::channel_logger<std::string> report_source;

        //! Logger sink; note we are forced to use boost::shared_ptr<> because this is what the
        //! Boost.Log API expects
        boost::shared_ptr<sink_t> log_sink;
        
        //! Reporting sink; note we are forced to use boost::shared_ptr<> because this is what the
        //! Boost.Log API expects
        boost::shared_ptr<sink_t> report_sink;

	    };


    // GENERIC WRITER METHODS


    generic_writer::generic_writer(const std::string& n, const generic_writer::metadata_group& m,
                                   const generic_writer::paths_group& p, unsigned int w)
	    : generic_metadata(m),
	      paths(p),
        name(n),
	      worker_number(w),
	      data_manager_handle(nullptr),
	      committed(false),
        fail(false),
        log_source(boost::log::keywords::channel = "log"),
        report_source(boost::log::keywords::channel = "report")
	    {
        // set up logging

        std::ostringstream log_file;
        log_file << CPPTRANSPORT_LOG_FILENAME_A << worker_number << CPPTRANSPORT_LOG_FILENAME_B;
        boost::filesystem::path log_path = paths.root / paths.log / log_file.str();
        
        boost::filesystem::path report_path = paths.root / paths.log / std::string(CPPTRANSPORT_REPORT_FILENAME_A);

        boost::shared_ptr<boost::log::core> core = boost::log::core::get();
    
        boost::shared_ptr<boost::log::sinks::text_file_backend> logging_backend =
          boost::make_shared<boost::log::sinks::text_file_backend>(
            boost::log::keywords::file_name = log_path.string(),
            boost::log::keywords::open_mode = std::ios::app
          );

        // enable auto-flushing of log entries
        // this degrades performance, but we are not writing many entries and they
        // will not be lost in the event of a crash
        logging_backend->auto_flush(true);
        
        boost::shared_ptr<boost::log::sinks::text_file_backend> reporting_backend =
          boost::make_shared<boost::log::sinks::text_file_backend>(
            boost::log::keywords::file_name = report_path.string(),
            boost::log::keywords::open_mode = std::ios::app
          );

        // Generate logging sink and register it in the core.
        // The logging sink filters only for the log channel.
        this->log_sink = boost::make_shared<sink_t>(logging_backend);
        this->log_sink->set_formatter(
          boost::log::expressions::stream
            << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
            << " | "
            << boost::log::expressions::attr< base_writer::log_severity_level, base_writer_severity_tag >("Severity")
            << " | "
            << boost::log::expressions::smessage
        );
        this->log_sink->set_filter(
          boost::log::expressions::attr< std::string >("Channel") == "log"
        );
        
        core->add_sink(this->log_sink);
        
        // Generate reporting sink and register it in the core.
        // The reporting sink filters only for the report channel.
        this->report_sink = boost::make_shared<sink_t>(reporting_backend);
        this->report_sink->set_formatter(
          // no specific formatting required
          boost::log::expressions::stream << boost::log::expressions::smessage
        );
        this->report_sink->set_filter(
          boost::log::expressions::attr< std::string >("Channel") == "report"
        );
        
        core->add_sink(this->report_sink);

        boost::log::add_common_attributes();

        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "** Instantiated writer on MPI host " << host_info.get_host_name();

        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "** Host details: OS = " << host_info.get_os_name()
            << ", version = " << host_info.get_os_version()
            << " (release = " << host_info.get_os_release()
            << ") | " << host_info.get_architecture()
            << " | CPU vendor = " << host_info.get_cpu_vendor_id();
	    }


    generic_writer::~generic_writer()
	    {
        // remove logging objects
        boost::shared_ptr<boost::log::core> core = boost::log::core::get();

        core->remove_sink(this->log_sink);
	    }


    template <typename data_manager_type>
    void generic_writer::set_data_manager_handle(data_manager_type data)
	    {
        this->data_manager_handle = static_cast<void*>(data);  // will fail if data_manager_type not (static-)castable to void*
	    }


    template <typename data_manager_type>
    void generic_writer::get_data_manager_handle(data_manager_type* data)
	    {
        if(this->data_manager_handle == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_OUTPUT_WRITER_UNSETHANDLE);
        *data = static_cast<data_manager_type>(this->data_manager_handle);
	    }


	}   // namespace transport


#endif //CPPTRANSPORT_GENERIC_WRITER_H
