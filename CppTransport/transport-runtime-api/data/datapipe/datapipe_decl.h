//
// Created by David Seery on 26/03/15.
//


#ifndef CPPTRANSPORT_DATAPIPE_DECL_H
#define CPPTRANSPORT_DATAPIPE_DECL_H


#include <vector>
#include <list>
#include <functional>
#include <memory>

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/enumerations.h"

#include "transport-runtime-api/utilities/formatter.h"
#include "transport-runtime-api/utilities/linecache.h"
#include "transport-runtime-api/utilities/host_information.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"
#include "transport-runtime-api/repository/records/content_finder_decl.h"

// forward-declare derived content if needed
#include "transport-runtime-api/derived-products/derived_product_forward_declare.h"
#include "transport-runtime-api/derived-products/derived-content/correlation-functions/template_types.h"

// forward declare tags if needed
#include "transport-runtime-api/data/datapipe/tags_forward_declare.h"

#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"

#include "transport-runtime-api/data/batchers/integration_items.h"
#include "transport-runtime-api/data/batchers/postintegration_items.h"

#include "transport-runtime-api/data/datapipe/linecache_specializations.h"

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


//#define CPPTRANSPORT_DEBUG_DATAPIPE


namespace transport
	{

    // default size of line cache hash table
    constexpr unsigned int CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE = 1024;

    // forward-declare data_manager
    template <typename number> class data_manager;


    //! Data pipe, used when generating derived content to extract data from an integration database.
    //! The datapipe has a log directory, used for logging transactions on the pipe.
		template <typename number>
    class datapipe
	    {

      public:

        //! Logging severity level
        enum class log_severity_level { datapipe_pull, normal, warning, error, critical };

        typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend > sink_t;


      public:

        //! Push derived content
        typedef std::function<void(datapipe<number>*, typename derived_data::derived_product<number>*, const std::list<std::string>&)> dispatch_function;


        class utility_callbacks
	        {

          public:

            //! constructor
            utility_callbacks(integration_content_finder<number>& i, postintegration_content_finder<number>& p, dispatch_function& d)
              : integration_finder(i),
                postintegration_finder(p),
                dispatch(d)
              {
              }

            integration_content_finder<number>      integration_finder;
            postintegration_content_finder<number>  postintegration_finder;
            dispatch_function                       dispatch;

	        };


      protected:

		    enum class attachment_type { none_attached, integration_attached, postintegration_attached };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a datapipe
        datapipe(unsigned int cap, const boost::filesystem::path& lp, const boost::filesystem::path& tp, unsigned int w,
                 data_manager<number>& dm, utility_callbacks& u, bool no_log=false);

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
        void commit(typename derived_data::derived_product<number>* product, const std::list<std::string>& groups) { this->utilities.dispatch(this, product, groups); }


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

        //! Get total statistics cache hits
        unsigned int get_stats_cache_hits() const { return(this->statistics_cache.get_hits()); }


        //! Get total time-config cache unloads
        unsigned int get_time_config_cache_unloads() const { return(this->time_config_cache.get_unloads()); }

        //! Get total twopf k-config cache unloads
        unsigned int get_twopf_kconfig_cache_unloads() const { return(this->twopf_kconfig_cache.get_unloads()); }

        //! Get total threepf k-config cache unloads
        unsigned int get_threepf_kconfig_cache_unloads() const { return(this->threepf_kconfig_cache.get_unloads()); }

        //! Get total data cache unloads
        unsigned int get_data_cache_unloads() const { return(this->data_cache.get_unloads()); }

        //! Get total statistics cache unloads
        unsigned int get_stats_cache_unloads() const { return(this->statistics_cache.get_unloads()); }


        //! Get total eviction time for time-config cache
        boost::timer::nanosecond_type get_time_config_cache_evictions() const { return(this->time_config_cache.get_eviction_timer()); }

        //! Get total eviction time for twopf k-config cache unloads
        boost::timer::nanosecond_type get_twopf_kconfig_cache_evictions() const { return(this->twopf_kconfig_cache.get_eviction_timer()); }

        //! Get total eviction time for threepf k-config cache unloads
        boost::timer::nanosecond_type get_threepf_kconfig_cache_evictions() const { return(this->threepf_kconfig_cache.get_eviction_timer()); }

        //! Get total eviction time for data cache
        boost::timer::nanosecond_type get_data_cache_evictions() const { return(this->data_cache.get_eviction_timer()); }

        //! Get total eviction time for statistics cache
        boost::timer::nanosecond_type get_stats_cache_evictions() const { return(this->statistics_cache.get_eviction_timer()); }


        // ATTACH, DETACH OUTPUT GROUPS

      public:

        //! Attach a content group to the datapipe, ready for reading. Returns name of attached content group.
        std::string attach(derivable_task<number>* tk, const std::list<std::string>& tags);

        //! Detach a content group from the datapipe
        void detach(void);

        //! Is this datapipe attached to an output group?
        bool is_attached() const { return(this->type != attachment_type::none_attached); }

		    //! Is this datapipe attached to an integration output group?
		    bool is_integration_attached() const { return(this->type == attachment_type::integration_attached); }

		    //! Is this datapipe attached to a postintegration output group?
		    bool is_postintegration_attached() const { return(this->type == attachment_type::postintegration_attached); }

      protected:

		    //! set up cache tables for a newly-attached content group
		    template <typename Payload>
		    void attach_cache_tables(Payload& payload);


        // PROPERTIES OF CURRENTLY-ATTACHED GROUP

      public:

        //! Get number of fields associated with currently attached group.
        //! Output is meaningful only when a group is attached.
        unsigned int get_N_fields() const { return(this->N_fields); }

		    //! Get payload record if an integration group is attached; returns nullptr if an integration group is not attached
        //! Raw pointers are used because there is no notion of ownership transfer
		    output_group_record<integration_payload>* get_attached_integration_record();

		    //! Get payload record if a postintegration group is attached; returns nullptr if a postintegration group is not attached
        //! Raw pointers are used because there is no notion of ownership transfer
		    output_group_record<postintegration_payload>* get_attached_postintegration_record();


        // PULL DATA


		    // HANDLE FACTORIES

      public:

        typedef linecache::serial_group< std::vector<time_config>, time_config_tag<number>, derived_data::SQL_time_config_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_config_handle;
        typedef linecache::serial_group< std::vector<twopf_kconfig>, twopf_kconfig_tag<number>, derived_data::SQL_twopf_kconfig_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE > twopf_kconfig_handle;
        typedef linecache::serial_group< std::vector<threepf_kconfig>, threepf_kconfig_tag<number>, derived_data::SQL_threepf_kconfig_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE > threepf_kconfig_handle;
		    typedef linecache::serial_group< std::vector<kconfiguration_statistics>, k_statistics_tag<number>, derived_data::SQL_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE > k_statistics_handle;
        typedef linecache::serial_group< std::vector<number>, data_tag<number>, derived_data::SQL_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_data_handle;
        typedef linecache::serial_group< std::vector<number>, data_tag<number>, derived_data::SQL_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE > kconfig_data_handle;
        typedef linecache::serial_group< std::vector<number>, data_tag<number>, derived_data::SQL_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_zeta_handle;
        typedef linecache::serial_group< std::vector<number>, data_tag<number>, derived_data::SQL_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE > kconfig_zeta_handle;

        //! Generate a serial-group handle for a set of time-configuration serial numbers
        time_config_handle& new_time_config_handle(const derived_data::SQL_time_config_query& query) const;

        //! Generate a serial-group handle for a set of twopf-kconfiguration serial numbers
        twopf_kconfig_handle& new_twopf_kconfig_handle(const derived_data::SQL_twopf_kconfig_query& query) const;

        //! Generate a serial-group handle for a set of threepf-kconfiguration serial numbers
        threepf_kconfig_handle& new_threepf_kconfig_handle(const derived_data::SQL_threepf_kconfig_query& query) const;

		    //! Generate a serial-group handle for a set of k-configuration statistics; accepts any SQL_query object, but
		    //! it needs to be a query over k-modes otherwise the result will be garbage
		    k_statistics_handle& new_k_statistics_handle(const derived_data::SQL_query& query) const;

        //! Generate a serial-group handle for a set of time-data serial numbers
        time_data_handle& new_time_data_handle(const derived_data::SQL_time_config_query& query) const;

        //! Generate a serial-group handle for a set of kconfig-data serial numbers
        kconfig_data_handle& new_kconfig_data_handle(const derived_data::SQL_twopf_kconfig_query& query) const;
        kconfig_data_handle& new_kconfig_data_handle(const derived_data::SQL_threepf_kconfig_query& query) const;

        //! Generate a serial-group handle for a set of zeta time-serial numbers
        time_zeta_handle& new_time_zeta_handle(const derived_data::SQL_time_config_query& query) const;

        //! Generate a serial-group handle fora set of zeta kconfig-serial numbers
        kconfig_zeta_handle& new_kconfig_zeta_handle(const derived_data::SQL_twopf_kconfig_query& query) const;
        kconfig_zeta_handle& new_kconfig_zeta_handle(const derived_data::SQL_threepf_kconfig_query& query) const;

		    // TAG FACTORIES

      public:

        //! Generate a new time-configuration tag
        time_config_tag<number> new_time_config_tag();

        //! Generate a new twopf-kconfiguration tag
        twopf_kconfig_tag<number> new_twopf_kconfig_tag();

        //! Generate a new threepf-kconfiguration tag
        threepf_kconfig_tag<number> new_threepf_kconfig_tag();

		    //! Generate a new k-configuration statistics tag
		    k_statistics_tag<number> new_k_statistics_tag();

        //! Generate a new background tag
        background_time_data_tag<number> new_background_time_data_tag(unsigned int id);

        //! Generate a new correlation-function time tag
        cf_time_data_tag<number> new_cf_time_data_tag(cf_data_type type, unsigned int id, unsigned int kserial);

        //! Generate a new correlation-function kconfig tag
        cf_kconfig_data_tag<number> new_cf_kconfig_data_tag(cf_data_type type, unsigned int id, unsigned int tserial);

        //! Generate a new zeta-correlation-function time tag
        zeta_twopf_time_data_tag<number> new_zeta_twopf_time_data_tag(const twopf_kconfig& kdata);

        //! Generate a new zeta-correlationn-function time tag
        zeta_threepf_time_data_tag<number> new_zeta_threepf_time_data_tag(const threepf_kconfig& kdata);

        //! Generate a new reduced bispectrum time tag
        zeta_reduced_bispectrum_time_data_tag<number> new_zeta_reduced_bispectrum_time_data_tag(const threepf_kconfig& kdata);

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
        friend class k_statistics_tag<number>;


        //! Host information
        host_information host_info;


        // CACHES

        //! time configuration cache
        linecache::cache<std::vector<time_config>, time_config_tag<number>, derived_data::SQL_time_config_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE> time_config_cache;

        //! twopf k-config cache
        linecache::cache<std::vector<twopf_kconfig>, twopf_kconfig_tag<number>, derived_data::SQL_twopf_kconfig_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE> twopf_kconfig_cache;

        //! threepf k-config cache
        linecache::cache<std::vector<threepf_kconfig>, threepf_kconfig_tag<number>, derived_data::SQL_threepf_kconfig_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE> threepf_kconfig_cache;

		    //! statistics cache
		    linecache::cache<std::vector<kconfiguration_statistics>, k_statistics_tag<number>, derived_data::SQL_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE> statistics_cache;

        //! data cache
        linecache::cache<std::vector<number>, data_tag<number>, derived_data::SQL_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE> data_cache;


        // CACHE TABLES

        //! Time configuration cache table for currently-attached group; null if no group is attached
        linecache::table<std::vector<time_config>, time_config_tag<number>, derived_data::SQL_time_config_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE>* time_config_cache_table;

        //! twopf k-config cache table for currently-attached group; null if no group is attached
        linecache::table<std::vector<twopf_kconfig>, twopf_kconfig_tag<number>, derived_data::SQL_twopf_kconfig_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE>* twopf_kconfig_cache_table;

        //! threepf k-config cache table for currently-attached group; null if no group is attached
        linecache::table<std::vector<threepf_kconfig>, threepf_kconfig_tag<number>, derived_data::SQL_threepf_kconfig_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE>* threepf_kconfig_cache_table;

		    //! statistics cache table for currently-attached group; null if no group is attached
		    linecache::table<std::vector<kconfiguration_statistics>, k_statistics_tag<number>, derived_data::SQL_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE>* statistics_cache_table;

        //! data cache table for currently-attached group; null if no group is attached
        linecache::table<std::vector<number>, data_tag<number>, derived_data::SQL_query, CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE>* data_cache_table;


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
        std::unique_ptr< output_group_record<integration_payload> > attached_integration_group;

		    //! Attachment point for a postintegration_payload output group; null if none is attached
		    std::unique_ptr< output_group_record<postintegration_payload> > attached_postintegration_group;

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


        // REFERENCE TO PARENT DATA_MANAGER OBJECT

        //! parent data_manager
        data_manager<number>& data_mgr;


        // CALLBACKS

        //! Utility functions: find output groups, attach/detach output groups, etc.
        utility_callbacks utilities;

	    };


	}   // namespace transport


// pull in tags definitions
#include "transport-runtime-api/data/datapipe/tags.h"


#endif //CPPTRANSPORT_DATAPIPE_DECL_H
