//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __datapipe_detail_H_
#define __datapipe_detail_H_


#include <vector>
#include <list>
#include <functional>
#include <memory>

#include "transport-runtime-api/defaults.h"

#include "transport-runtime-api/utilities/formatter.h"
#include "transport-runtime-api/utilities/linecache.h"
#include "transport-runtime-api/utilities/host_information.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

// forward-declare derived content if needed
#include "transport-runtime-api/derived-products/derived_product_forward_declare.h"
#include "transport-runtime-api/derived-products/template_types.h"

// forward-declare compute gadgets if needed
#include "transport-runtime-api/derived-products/derived-content/compute_forward_declare.h"

// forward declare tags if needed
#include "transport-runtime-api/data/datapipe/tags_forward_declare.h"

#include "transport-runtime-api/data/batchers/integration_items.h"
#include "transport-runtime-api/data/batchers/postintegration_items.h"

#include "transport-runtime-api/data/datapipe/configurations.h"

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


//#define __CPP_TRANSPORT_DEBUG_DATAPIPE


namespace transport
	{

    //! Data pipe, used when generating derived content to extract data from an integration database.
    //! The datapipe has a log directory, used for logging transactions on the pipe.
		template <typename number>
    class datapipe
	    {

      public:

        //! Logging severity level
        typedef enum { datapipe_pull, normal, warning, error, critical } log_severity_level;

        typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend > sink_t;


      public:

        typedef enum { time_config_group, twopf_kconfig_group, threepf_kconfig_group, time_serial_group, kconfig_serial_group } serial_group_tag;

        typedef enum { twopf_real, twopf_imag } twopf_type;

        //! Find integration content -- serivce provided by a repository implementation
        typedef std::function<std::shared_ptr< output_group_record<integration_payload> >(const std::string& name, const std::list<std::string>&)> integration_content_finder;

        //! Find postintegration content -- serivce provided by a repository implementation
        typedef std::function<std::shared_ptr< output_group_record<postintegration_payload> >(const std::string& name, const std::list<std::string>&)> postintegration_content_finder;

        //! Attach to integration content group
        typedef std::function<std::shared_ptr< output_group_record<integration_payload> >(datapipe<number>*, integration_content_finder&, const std::string&, const std::list<std::string>&)> integration_attach_callback;

        //! Attach to postintegration content group
        typedef std::function<std::shared_ptr< output_group_record<postintegration_payload> >(datapipe<number>*, postintegration_content_finder&, const std::string&, const std::list<std::string>&)> postintegration_attach_callback;

        //! Detach function for a datapipe
        typedef std::function<void(datapipe<number>*)> detach_callback;

        //! Push derived content
        typedef std::function<void(datapipe<number>*, typename derived_data::derived_product<number>*)> dispatch_function;

        //! Extract a set of time sample-points from a datapipe
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, std::vector<double>&)> time_config_callback;

        //! Extract a set of 2pf k-configuration sample points from a datapipe
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, std::vector<twopf_configuration>&)> kconfig_twopf_callback;

        //! Extract a set of 3pf k-configuration sample points from a datapipe
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, std::vector<threepf_configuration>&)> kconfig_threepf_callback;

        //! Extract a background field at a set of time sample-points
        typedef std::function<void(datapipe<number>*, unsigned int, const std::vector<unsigned int>&, std::vector<number>&)> background_time_callback;

        //! Extract a twopf component at fixed k-configuration for a set of time sample-points
        typedef std::function<void(datapipe<number>*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&, twopf_type)> twopf_time_callback;

        //! Extract a threepf component at fixed k-configuration for a set of time sample-point
        typedef std::function<void(datapipe<number>*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> threepf_time_callback;

        //! Extract a tensor twopf component at fixed k-configuration for a set of time sample-points
        typedef std::function<void(datapipe<number>*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> tensor_twopf_time_callback;

        //! Extract the zeta twopf at fixed k-configuration for a set of time sample-points
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_twopf_time_callback;

        //! Extract the zeta threepf at fixed k-configuration for a set of time sample-points
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_threepf_time_callback;

        //! Extract the zeta reduced bispectrum at fixed k-configuration for a set of time sample-points
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_redbsp_time_callback;

        //! Extract an fNL for a set of time sample-points
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, std::vector<number>&, derived_data::template_type)> fNL_time_callback;

        //! Extract bispectrum.template data for a set of time sample-points
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, std::vector<number>&, derived_data::template_type)> BT_time_callback;

        //! Extract template.template data for a set of time sample-points
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, std::vector<number>&, derived_data::template_type)> TT_time_callback;

        //! Extract a twopf component at fixed time for a set of k-configuration sample-points
        typedef std::function<void(datapipe<number>*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&, twopf_type)> twopf_kconfig_callback;

        //! Extract a threepf component at fixed time for a set of k-configuration sample-points
        typedef std::function<void(datapipe<number>*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> threepf_kconfig_callback;

        //! Extract a tensor twopf component at fixed time for a set of k-configuration sample-points
        typedef std::function<void(datapipe<number>*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> tensor_twopf_kconfig_callback;

        //! Extract the zeta twopf at fixed time for a set of k-configuration sample-points
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_twopf_kconfig_callback;

        //! Extract the zeta threepf at fixed time for a set of k-configuration sample-points
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_threepf_kconfig_callback;

        //! Extract the zeta reduced bispectrum at fixed time for a set of k-configuration sample-points
        typedef std::function<void(datapipe<number>*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_redbsp_kconfig_callback;


        class utility_callbacks
	        {
          public:
            integration_content_finder      integration_finder;
            integration_attach_callback     integration_attach;
            postintegration_content_finder  postintegration_finder;
            postintegration_attach_callback postintegration_attach;
            detach_callback                 detach;
            dispatch_function               dispatch;
	        };


        class config_cache
	        {
          public:
            time_config_callback     time;
            kconfig_twopf_callback   twopf;
            kconfig_threepf_callback threepf;
	        };


        class timeslice_cache
	        {
          public:
            background_time_callback   background;
            twopf_time_callback        twopf;
            threepf_time_callback      threepf;
            tensor_twopf_time_callback tensor_twopf;
            zeta_twopf_time_callback   zeta_twopf;
            zeta_threepf_time_callback zeta_threepf;
            zeta_redbsp_time_callback  zeta_redbsp;
            fNL_time_callback          fNL;
            BT_time_callback           BT;
            TT_time_callback           TT;
	        };


        class kslice_cache
	        {
          public:
            twopf_kconfig_callback        twopf;
            threepf_kconfig_callback      threepf;
            tensor_twopf_kconfig_callback tensor_twopf;
            zeta_twopf_kconfig_callback   zeta_twopf;
            zeta_threepf_kconfig_callback zeta_threepf;
            zeta_redbsp_kconfig_callback  zeta_redbsp;
	        };

      protected:

		    typedef enum { none_attached, integration_attached, postintegration_attached } attachment_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a datapipe
        datapipe(unsigned int dcap, unsigned int zcap,
                 const boost::filesystem::path& lp, const boost::filesystem::path& tp, unsigned int w,
                 utility_callbacks& u, config_cache& cf, timeslice_cache& t, kslice_cache& k, bool no_log=false);

        //! Destroy a datapipe
        ~datapipe();


        // PIPE MANAGEMENT

      public:

        //! Close this datapipe
        void close() { ; }

        //! Set an implementation-dependent handle
        template <typename handle_type>
        void set_manager_handle(handle_type h)  { this->manager_handle = static_cast<void*>(h); }

        //! Return an implementation-dependent handle
        template <typename handle_type>
        void get_manager_handle(handle_type* h) const { *h = static_cast<handle_type>(this->manager_handle); }

        //! Get worker numbers
        unsigned int get_worker_number() const { return(this->worker_number); }

        //! Validate that the pipe is attached to a container
        bool validate_attached(void) const;
		    bool validate_attached(attachment_type) const;

        //! Validate that the pipe is *not* attached to a container
        bool validate_unattached(void) const;


        // ABSOLUTE PATHS

      public:

        //! Return temporary files path
        const boost::filesystem::path& get_abs_tempdir_path() const { return(this->temporary_path); }


        // LOGGING

      public:

        //! Return logger
        boost::log::sources::severity_logger<log_severity_level>& get_log() { return(this->log_source); }


        // COMMIT AN OUTPUT

      public:

        //! Commit an output
        void commit(typename derived_data::derived_product<number>* product) { this->utilities.dispatch(this, product); }


        // CACHE STATISTICS

      public:

        //! Get total time spent reading database
        boost::timer::nanosecond_type get_database_time() const { return(this->database_timer.elapsed().wall); }

        //! Get total time-config cache hits
        unsigned int get_time_config_cache_hits() const { return(this->time_config_cache.get_hits()); }

        //! Get total twopf k-config cache hits
        unsigned int get_twopf_kconfig_cache_hits() const { return(this->twopf_kconfig_cache.get_hits()); }

        //! Get total threepf k-config cache hits
        unsigned int get_threepf_kconfig_cache_hits() const { return(this->threepf_kconfig_cache.get_hits()); }

        //! Get total data cache hits
        unsigned int get_data_cache_hits() const { return(this->data_cache.get_hits()); }

        //! Get total zeta cache hits
        unsigned int get_zeta_cache_hits() const { return(this->zeta_cache.get_hits()); }


        //! Get total time-config cache unloads
        unsigned int get_time_config_cache_unloads() const { return(this->time_config_cache.get_unloads()); }

        //! Get total twopf k-config cache unloads
        unsigned int get_twopf_kconfig_cache_unloads() const { return(this->twopf_kconfig_cache.get_unloads()); }

        //! Get total threepf k-config cache unloads
        unsigned int get_threepf_kconfig_cache_unloads() const { return(this->threepf_kconfig_cache.get_unloads()); }

        //! Get total data cache unloads
        unsigned int get_data_cache_unloads() const { return(this->data_cache.get_unloads()); }

        //! Get total zeta cache unloads
        unsigned int get_zeta_cache_unloads() const { return(this->zeta_cache.get_unloads()); }


        //! Get total eviction time for time-config cache
        boost::timer::nanosecond_type get_time_config_cache_evictions() const { return(this->time_config_cache.get_eviction_timer()); }

        //! Get total eviction time for twopf k-config cache unloads
        boost::timer::nanosecond_type get_twopf_kconfig_cache_evictions() const { return(this->twopf_kconfig_cache.get_eviction_timer()); }

        //! Get total eviction time for threepf k-config cache unloads
        boost::timer::nanosecond_type get_threepf_kconfig_cache_evictions() const { return(this->threepf_kconfig_cache.get_eviction_timer()); }

        //! Get total eviction time for data cache
        boost::timer::nanosecond_type get_data_cache_evictions() const { return(this->data_cache.get_eviction_timer()); }

        //! Get total eviction time for zeta cache
        boost::timer::nanosecond_type get_zeta_cache_evictions() const { return(this->zeta_cache.get_eviction_timer()); }


        // ATTACH, DETACH OUTPUT GROUPS

      public:

        //! Attach a content group to the datapipe, ready for reading
        void attach(derivable_task<number>* tk, const std::list<std::string>& tags);

        //! Detach a content group from the datapipe
        void detach(void);

        //! Is this datapipe attached to an output group?
        bool is_attached() const { return(this->type != none_attached); }

      protected:

		    //! set up cache tables for a newly-attached content group
		    template <typename Payload>
		    void attach_cache_tables(Payload& payload);


        // PROPERTIES OF CURRENTLY-ATTACHED GROUP

      public:

        //! Get number of fields associated with currently attached group.
        //! Output is meaningful only when a group is attached.
        unsigned int get_N_fields() const { return(this->N_fields); }


        // PULL DATA


		    // HANDLE FACTORIES

      public:

        typedef linecache::serial_group< std::vector<double>, time_config_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_config_handle;
        typedef linecache::serial_group< std::vector<twopf_configuration>, twopf_kconfig_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > twopf_kconfig_handle;
        typedef linecache::serial_group< std::vector<threepf_configuration>, threepf_kconfig_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > threepf_kconfig_handle;
        typedef linecache::serial_group< std::vector<number>, data_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_data_handle;
        typedef linecache::serial_group< std::vector<number>, data_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > kconfig_data_handle;
        typedef linecache::serial_group< std::vector<number>, data_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_zeta_handle;
        typedef linecache::serial_group< std::vector<number>, data_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > kconfig_zeta_handle;

        //! Generate a serial-group handle for a set of time-configuration serial numbers
        time_config_handle& new_time_config_handle(const std::vector<unsigned int>& sns) const;

        //! Generate a serial-group handle for a set of twopf-kconfiguration serial numbers
        twopf_kconfig_handle& new_twopf_kconfig_handle(const std::vector<unsigned int>& sns) const;

        //! Generate a serial-group handle for a set of threepf-kconfiguration serial numbers
        threepf_kconfig_handle& new_threepf_kconfig_handle(const std::vector<unsigned int>& sns) const;

        //! Generate a serial-group handle for a set of time-data serial numbers
        time_data_handle& new_time_data_handle(const std::vector<unsigned int>& sns) const;

        //! Generate a serial-group handle for a set of kconfig-data serial numbers
        kconfig_data_handle& new_kconfig_data_handle(const std::vector<unsigned int>& sns) const;

        //! Generate a serial-group handle for a set of zeta time-serial numbers
        time_zeta_handle& new_time_zeta_handle(const std::vector<unsigned int>& sns) const;

        //! Generate a serial-group handle fora set of zeta kconfig-serial numbers
        kconfig_zeta_handle& new_kconfig_zeta_handle(const std::vector<unsigned int>& sns) const;


		    // TAG FACTORIES

      public:

        //! Generate a new time-configuration tag
        time_config_tag<number> new_time_config_tag();

        //! Generate a new twopf-kconfiguration tag
        twopf_kconfig_tag<number> new_twopf_kconfig_tag();

        //! Generate a new threepf-kconfiguration tag
        threepf_kconfig_tag<number> new_threepf_kconfig_tag();

        //! Generate a new background tag
        background_time_data_tag<number> new_background_time_data_tag(unsigned int id);

        //! Generate a new correlation-function time tag
        cf_time_data_tag<number> new_cf_time_data_tag(typename data_tag<number>::cf_data_type type, unsigned int id, unsigned int kserial);

        //! Generate a new correlation-function kconfig tag
        cf_kconfig_data_tag<number> new_cf_kconfig_data_tag(typename data_tag<number>::cf_data_type type, unsigned int id, unsigned int tserial);

        //! Generate a new zeta-correlation-function time tag
        zeta_twopf_time_data_tag<number> new_zeta_twopf_time_data_tag(const twopf_configuration& kdata);

        //! Generate a new zeta-correlationn-function time tag
        zeta_threepf_time_data_tag<number> new_zeta_threepf_time_data_tag(const threepf_configuration& kdata);

        //! Generate a new reduced bispectrum time tag
        zeta_reduced_bispectrum_time_data_tag<number> new_zeta_reduced_bispectrum_time_data_tag(const threepf_configuration& kdata);

        //! Generate a new zeta-correlation-function kconfig tag
        zeta_twopf_kconfig_data_tag<number> new_zeta_twopf_kconfig_data_tag(unsigned int tserial);

        //! Generate a new zeta-correlation-function kconfig tag
        zeta_threepf_kconfig_data_tag<number> new_zeta_threepf_kconfig_data_tag(unsigned int tserial);

        //! Generate a new reduced bispectrum kconfig tag
        zeta_reduced_bispectrum_kconfig_data_tag<number> new_zeta_reduced_bispectrum_kconfig_data_tag(unsigned int tserial);

        //! Generate a new fNL time tag
        fNL_time_data_tag<number> new_fNL_time_data_tag(derived_data::template_type type);

        //! Generate a new bispectrum.template time tag
        BT_time_data_tag<number> new_BT_time_data_tag(derived_data::template_type type);

        //! Generate a new template.template time tag
        TT_time_data_tag<number> new_TT_time_data_tag(derived_data::template_type type);


        // INTERNAL DATA

      private:

        // friend the tag classes, so they can use our data
        friend class time_config_tag<number>;
        friend class twopf_kconfig_tag<number>;
        friend class threepf_kconfig_tag<number>;
        friend class background_time_data_tag<number>;
        friend class cf_time_data_tag<number>;
        friend class cf_kconfig_data_tag<number>;
        friend class zeta_twopf_time_data_tag<number>;
        friend class zeta_threepf_time_data_tag<number>;
        friend class zeta_reduced_bispectrum_time_data_tag<number>;
        friend class zeta_twopf_kconfig_data_tag<number>;
        friend class zeta_threepf_kconfig_data_tag<number>;
        friend class zeta_reduced_bispectrum_kconfig_data_tag<number>;
		    friend class fNL_time_data_tag<number>;
		    friend class BT_time_data_tag<number>;
		    friend class TT_time_data_tag<number>;


        //! Host information
        host_information host_info;


        // CACHES

        //! time configuration cache
        linecache::cache<std::vector<double>, time_config_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> time_config_cache;

        //! twopf k-config cache
        linecache::cache<std::vector<twopf_configuration>, twopf_kconfig_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> twopf_kconfig_cache;

        //! threepf k-config cache
        linecache::cache<std::vector<threepf_configuration>, threepf_kconfig_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> threepf_kconfig_cache;

        //! data cache
        linecache::cache<std::vector<number>, data_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> data_cache;

        //! zeta cache
        linecache::cache<std::vector<number>, data_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> zeta_cache;


        // CACHE TABLES

        //! Time configuration cache table for currently-attached group; null if no group is attached
        linecache::table<std::vector<number>, time_config_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* time_config_cache_table;

        //! twopf k-config cache table for currently-attached group; null if no group is attached
        linecache::table<std::vector<twopf_configuration>, twopf_kconfig_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* twopf_kconfig_cache_table;

        //! threepf k-config cache table for currently-attached group; null if no group is attached
        linecache::table<std::vector<threepf_configuration>, threepf_kconfig_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* threepf_kconfig_cache_table;

        //! data cache table for currently-attached group; null if no group is attached
        linecache::table<std::vector<number>, data_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* data_cache_table;

        //! zeta cache table for currently-attached group; null if no group is attached
        linecache::table<std::vector<number>, data_tag<number>, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* zeta_cache_table;


        // PROPERTIES

        //! Maximum capacity to use (approximately--we don't try to do a detailed accounting of memory use)
        unsigned int capacity;

        //! Unique serial number identifying the worker process owning this datapipe
        const unsigned int worker_number;

        //! Implementation-dependent handle used by data_manager
        void* manager_handle;


        // CURRENTLY ATTACHED OUTPUT GROUP

		    //! what sort of group is currently attached?
		    attachment_type type;

        //! Attachment point for an integration_payload output group; null if none is attached
        std::shared_ptr< output_group_record<integration_payload> > attached_integration_group;

		    //! Attachment point for a postintegration_payload output group; null if none is attached
		    std::shared_ptr< output_group_record<postintegration_payload> > attached_postintegration_group;

        //! Number of fields associated with currently attached group
        unsigned int N_fields;


        // PATHS

        //! Path to logging directory
        const boost::filesystem::path logdir_path;

        //! Path to temporary files
        const boost::filesystem::path temporary_path;


        // TIMERS

        //! Database access timer
        boost::timer::cpu_timer database_timer;


        // LOGGING

        //! Logger source
        boost::log::sources::severity_logger<log_severity_level> log_source;

        //! Logger sink
        boost::shared_ptr<sink_t> log_sink;


        // CALLBACKS

        //! Utility functions: find output groups, attach/detach output groups, etc.
        utility_callbacks utilities;

        //! Pull configuration data from the cache
        config_cache pull_config;

        //! Pull time-series data from the cache
        timeslice_cache pull_timeslice;

        //! Pull kconfig-series data from the cache
        kslice_cache pull_kslice;

	    };


    // DATAPIPE METHODS


    template <typename number>
    datapipe<number>::datapipe(unsigned int dcap, unsigned int zcap,
                                             const boost::filesystem::path& lp, const boost::filesystem::path& tp, unsigned int w,
                                             utility_callbacks& u, config_cache& cf, timeslice_cache& t, kslice_cache& k, bool no_log)
	    : logdir_path(lp),
	      temporary_path(tp),
	      worker_number(w),
	      utilities(u),
	      pull_config(cf),
	      pull_timeslice(t),
	      pull_kslice(k),
	      time_config_cache_table(nullptr),
	      twopf_kconfig_cache_table(nullptr),
	      threepf_kconfig_cache_table(nullptr),
	      data_cache_table(nullptr),
	      zeta_cache_table(nullptr),
	      time_config_cache(__CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
	      twopf_kconfig_cache(__CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
	      threepf_kconfig_cache(__CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
	      data_cache(dcap),
	      zeta_cache(zcap),
	      type(none_attached),
        N_fields(0)
	    {
        this->database_timer.stop();

        std::ostringstream log_file;
        log_file << __CPP_TRANSPORT_LOG_FILENAME_A << worker_number << __CPP_TRANSPORT_LOG_FILENAME_B;

        boost::filesystem::path log_path = logdir_path / log_file.str();

        if(!no_log)
	        {
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
        else
	        {
            this->log_sink.reset();
	        }

        BOOST_LOG_SEV(this->log_source, normal) << "** Instantiated datapipe (main cache " << format_memory(dcap) << ", zeta cache " << format_memory(zcap) << ")"
	        << " on MPI host " << host_info.get_host_name()
	        << ", OS = " << host_info.get_os_name()
	        << ", Version = " << host_info.get_os_version()
	        << " (Release = " << host_info.get_os_release()
	        << ") | " << host_info.get_architecture()
	        << " | CPU vendor = " << host_info.get_cpu_vendor_id() << std::endl;
	    }


    template <typename number>
    datapipe<number>::~datapipe()
	    {
//		    assert(this->database_timer.is_stopped());

        if(!(this->database_timer.is_stopped()))
	        {
            BOOST_LOG_SEV(this->log_source, error) << ":: Error: datapipe being destroyed, but database timer is still running";
	        }

        BOOST_LOG_SEV(this->log_source, normal) << "";
        BOOST_LOG_SEV(this->log_source, normal) << "-- Closing datapipe: final usage statistics:";
        BOOST_LOG_SEV(this->log_source, normal) << "--   time spent querying database       = " << format_time(this->database_timer.elapsed().wall);
        BOOST_LOG_SEV(this->log_source, normal) << "--   time-configuration cache hits      = " << this->time_config_cache.get_hits() << " | unloads = " << this->time_config_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, normal) << "--   twopf k-configuration cache hits   = " << this->twopf_kconfig_cache.get_hits() << " | unloads = " << this->twopf_kconfig_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, normal) << "--   threepf k-configuration cache hits = " << this->threepf_kconfig_cache.get_hits() << " | unloads = " << this->threepf_kconfig_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, normal) << "--   data cache hits:                   = " << this->data_cache.get_hits() << " | unloads = " << this->data_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, normal) << "--   zeta cache hits:                   = " << this->zeta_cache.get_hits() << " | unloads = " << this->zeta_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, normal) << "";
        BOOST_LOG_SEV(this->log_source, normal) << "--   time-configuration evictions       = " << format_time(this->time_config_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, normal) << "--   twopf k-configuration evictions    = " << format_time(this->twopf_kconfig_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, normal) << "--   threepf k-configuration evictions  = " << format_time(this->threepf_kconfig_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, normal) << "--   data evictions                     = " << format_time(this->data_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, normal) << "--   zeta evictions                     = " << format_time(this->zeta_cache.get_eviction_timer());

        // detach any attached output group, if necessary
		    switch(this->type)
			    {
		        case integration_attached:
			        if(this->attached_integration_group.get() != nullptr) this->detach();
				      break;

		        case postintegration_attached:
			        if(this->attached_postintegration_group.get() != nullptr) this->detach();
				      break;

		        case none_attached:
		        default:
			        break;
			    }

        if(this->log_sink)    // implicitly converts to bool, value true if not null
	        {
            boost::shared_ptr<boost::log::core> core = boost::log::core::get();
            core->remove_sink(this->log_sink);
	        }
	    }


    template <typename number>
    bool datapipe<number>::validate_unattached(void) const
	    {
		    if(this->type != none_attached) return(false);

		    if(this->attached_integration_group.get() != nullptr
			       || this->attached_postintegration_group.get() != nullptr) return(false);

		    if(this->time_config_cache_table != nullptr
				    || this->twopf_kconfig_cache_table != nullptr
				    || this->threepf_kconfig_cache_table != nullptr
				    || this->data_cache_table != nullptr
				    || this->zeta_cache_table != nullptr) return(false);

		    return(true);
	    }


    template <typename number>
    bool datapipe<number>::validate_attached(void) const
	    {
		    switch(this->type)
			    {
		        case none_attached:
			        return(false);

		        case integration_attached:
			        if(this->attached_integration_group.get() == nullptr) return(false);
				      break;

		        case postintegration_attached:
			        if(this->attached_postintegration_group.get() == nullptr) return(false);
				      break;
			    }

		    if(this->time_config_cache_table == nullptr
			       || this->twopf_kconfig_cache_table == nullptr
			       || this->threepf_kconfig_cache_table == nullptr
			       || this->data_cache_table == nullptr
			       || this->zeta_cache_table == nullptr) return(false);

		    return(true);
	    }


		template <typename number>
		bool datapipe<number>::validate_attached(attachment_type t) const
			{
				if(this->type != t) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_WRONG_CONTENT);
				return(this->validate_attached());
			}


    template <typename number>
    void datapipe<number>::attach(derivable_task<number>* tk, const std::list<std::string>& tags)
	    {
        assert(this->validate_unattached());
        if(!this->validate_unattached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_ATTACH_PIPE_ALREADY_ATTACHED);

        assert(tk != nullptr);
        if(tk == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NULL_TASK);

        // work out what sort of content group we are trying to attach
        integration_task<number>    * itk = nullptr;
        postintegration_task<number>* ptk = nullptr;

        if((itk = dynamic_cast< integration_task<number>* >(tk)) != nullptr)    // trying to attach to an integration content group
	        {
            this->type                       = integration_attached;
            this->attached_integration_group = this->utilities.integration_attach(this, this->utilities.integration_finder, tk->get_name(), tags);

            // remember number of fields associated with this container
            this->N_fields = itk->get_model()->get_N_fields();

            integration_payload& payload = this->attached_integration_group->get_payload();
            this->attach_cache_tables(payload);

            BOOST_LOG_SEV(this->get_log(), normal) << "** ATTACH integration content group " << boost::posix_time::to_simple_string(this->attached_integration_group->get_creation_time())
		            << " (from integration task '" << tk->get_name() << "', generated using integration backend '" << payload.get_backend() << "')";
	        }
        else if((ptk = dynamic_cast< postintegration_task<number>* >(ptk)) != nullptr)      // trying to attach to a postintegration content group
	        {
            this->type                           = postintegration_attached;
            this->attached_postintegration_group = this->utilities.postintegration_attach(this, this->utilities.postintegration_finder, tk->get_name(), tags);

            this->N_fields = 0;

            postintegration_payload& payload = this->attached_postintegration_group->get_payload();
            this->attach_cache_tables(payload);

            BOOST_LOG_SEV(this->get_log(), normal) << "** ATTACH postintegration content group " << boost::posix_time::to_simple_string(this->attached_postintegration_group->get_creation_time())
		            << " (from postintegration task '" << tk->get_name() << "'";
	        }
        else
	        {
            std::stringstream msg;
            msg << __CPP_TRANSPORT_DATAMGR_UNKNOWN_DERIVABLE_TASK << " '" << tk->get_name() << "'";
            throw runtime_exception(runtime_exception::DATAPIPE_ERROR, msg.str());
	        }
	    }


		template <typename number>
		template <typename Payload>
		void datapipe<number>::attach_cache_tables(Payload& payload)
			{
        // attach new cache tables for this container

        this->time_config_cache_table = &(this->time_config_cache.get_table_handle(payload.get_container_path().string()));
        this->twopf_kconfig_cache_table = &(this->twopf_kconfig_cache.get_table_handle(payload.get_container_path().string()));
        this->threepf_kconfig_cache_table = &(this->threepf_kconfig_cache.get_table_handle(payload.get_container_path().string()));
        this->data_cache_table = &(this->data_cache.get_table_handle(payload.get_container_path().string()));
        this->zeta_cache_table = &(this->zeta_cache.get_table_handle(payload.get_container_path().string()));
	    }


    template <typename number>
    void datapipe<number>::detach(void)
	    {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_DETACH_PIPE_NOT_ATTACHED);

        this->utilities.detach(this);

		    switch(this->type)
			    {
		        case integration_attached:
			        BOOST_LOG_SEV(this->get_log(), datapipe<number>::normal) << "** DETACH output group " << boost::posix_time::to_simple_string(this->attached_integration_group->get_creation_time());
			        this->attached_integration_group.reset();
				      break;

		        case postintegration_attached:
			        BOOST_LOG_SEV(this->get_log(), datapipe<number>::normal) << "** DETACH output group " << boost::posix_time::to_simple_string(this->attached_postintegration_group->get_creation_time());
			        this->attached_postintegration_group.reset();
				      break;

		        default:
			        break;
			    }

        this->time_config_cache_table = nullptr;
        this->twopf_kconfig_cache_table = nullptr;
        this->threepf_kconfig_cache_table = nullptr;
        this->data_cache_table = nullptr;
        this->zeta_cache_table = nullptr;
	    }


    template <typename number>
    typename datapipe<number>::time_config_handle& datapipe<number>::new_time_config_handle(const std::vector<unsigned int>& sns) const
	    {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->time_config_cache_table->get_serial_handle(sns, time_config_group));
	    }


    template <typename number>
    typename datapipe<number>::twopf_kconfig_handle& datapipe<number>::new_twopf_kconfig_handle(const std::vector<unsigned int>& sns) const
	    {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->twopf_kconfig_cache_table->get_serial_handle(sns, twopf_kconfig_group));
	    }


    template <typename number>
    typename datapipe<number>::threepf_kconfig_handle& datapipe<number>::new_threepf_kconfig_handle(const std::vector<unsigned int>& sns) const
	    {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->threepf_kconfig_cache_table->get_serial_handle(sns, threepf_kconfig_group));
	    }


    template <typename number>
    typename datapipe<number>::time_data_handle& datapipe<number>::new_time_data_handle(const std::vector<unsigned int>& sns) const
	    {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->data_cache_table->get_serial_handle(sns, time_serial_group));
	    }


    template <typename number>
    typename datapipe<number>::kconfig_data_handle& datapipe<number>::new_kconfig_data_handle(const std::vector<unsigned int>& sns) const
	    {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->data_cache_table->get_serial_handle(sns, kconfig_serial_group));
	    }


    template <typename number>
    typename datapipe<number>::time_zeta_handle& datapipe<number>::new_time_zeta_handle(const std::vector<unsigned int>& sns) const
	    {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->zeta_cache_table->get_serial_handle(sns, time_serial_group));
	    }


    template <typename number>
    typename datapipe<number>::kconfig_zeta_handle& datapipe<number>::new_kconfig_zeta_handle(const std::vector<unsigned int>& sns) const
	    {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->zeta_cache_table->get_serial_handle(sns, kconfig_serial_group));
	    }


    template <typename number>
    time_config_tag<number> datapipe<number>::new_time_config_tag()
	    {
        return time_config_tag<number>(this);
	    }


    template <typename number>
    twopf_kconfig_tag<number> datapipe<number>::new_twopf_kconfig_tag()
	    {
        return twopf_kconfig_tag<number>(this);
	    }


    template <typename number>
    threepf_kconfig_tag<number> datapipe<number>::new_threepf_kconfig_tag()
	    {
        return threepf_kconfig_tag<number>(this);
	    }


    template <typename number>
    background_time_data_tag<number> datapipe<number>::new_background_time_data_tag(unsigned int id)
	    {
        return background_time_data_tag<number>(this, id);
	    }


    template <typename number>
    cf_time_data_tag<number> datapipe<number>::new_cf_time_data_tag(typename data_tag<number>::cf_data_type type, unsigned int id, unsigned int kserial)
	    {
        return cf_time_data_tag<number>(this, type, id, kserial);
	    }


    template <typename number>
    cf_kconfig_data_tag<number> datapipe<number>::new_cf_kconfig_data_tag(typename data_tag<number>::cf_data_type type, unsigned int id, unsigned int tserial)
	    {
        return cf_kconfig_data_tag<number>(this, type, id, tserial);
	    }


    template <typename number>
    zeta_twopf_time_data_tag<number> datapipe<number>::new_zeta_twopf_time_data_tag(const twopf_configuration& kdata)
	    {
        return zeta_twopf_time_data_tag<number>(this, kdata);
	    }


    template <typename number>
    zeta_threepf_time_data_tag<number> datapipe<number>::new_zeta_threepf_time_data_tag(const threepf_configuration& kdata)
	    {
        return zeta_threepf_time_data_tag<number>(this, kdata);
	    }


    template <typename number>
    zeta_reduced_bispectrum_time_data_tag<number> datapipe<number>::new_zeta_reduced_bispectrum_time_data_tag(const threepf_configuration& kdata)
	    {
        return zeta_reduced_bispectrum_time_data_tag<number>(this, kdata);
	    }


    template <typename number>
    zeta_twopf_kconfig_data_tag<number> datapipe<number>::new_zeta_twopf_kconfig_data_tag(unsigned int tserial)
	    {
        return zeta_twopf_kconfig_data_tag<number>(this, tserial);
	    }


    template <typename number>
    zeta_threepf_kconfig_data_tag<number> datapipe<number>::new_zeta_threepf_kconfig_data_tag(unsigned int tserial)
	    {
        return zeta_threepf_kconfig_data_tag<number>(this, tserial);
	    }


    template <typename number>
    zeta_reduced_bispectrum_kconfig_data_tag<number> datapipe<number>::new_zeta_reduced_bispectrum_kconfig_data_tag(unsigned int tserial)
	    {
        return zeta_reduced_bispectrum_kconfig_data_tag<number>(this, tserial);
	    }


    template <typename number>
    fNL_time_data_tag<number> datapipe<number>::new_fNL_time_data_tag(derived_data::template_type type)
	    {
        return fNL_time_data_tag<number>(this, type);
	    }


    template <typename number>
    BT_time_data_tag<number> datapipe<number>::new_BT_time_data_tag(derived_data::template_type type)
	    {
        return BT_time_data_tag<number>(this, type);
	    }


    template <typename number>
    TT_time_data_tag<number> datapipe<number>::new_TT_time_data_tag(derived_data::template_type type)
	    {
        return TT_time_data_tag<number>(this, type);
	    }


	}   // namespace transport



#endif //__datapipe_detail_H_
