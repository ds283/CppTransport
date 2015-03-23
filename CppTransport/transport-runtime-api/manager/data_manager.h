//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __data_manager_H_
#define __data_manager_H_


#include <set>
#include <vector>
#include <list>
#include <functional>
#include <memory>

#include <math.h>

#include "transport-runtime-api/scheduler/work_queue.h"
#include "transport-runtime-api/manager/repository.h"

#include "transport-runtime-api/utilities/formatter.h"
#include "transport-runtime-api/utilities/linecache.h"
#include "transport-runtime-api/utilities/host_information.h"

#include "transport-runtime-api/derived-products/template_types.h"

#include "transport-runtime-api/defaults.h"


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
#include "boost/phoenix/bind/bind_function_object.hpp"

// log file name
#define __CPP_TRANSPORT_LOG_FILENAME_A "worker_"
#define __CPP_TRANSPORT_LOG_FILENAME_B "_%3N.log"


// default size of line cache hash table
#define __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE (1024)

//#define __CPP_TRANSPORT_DEBUG_DATAPIPE


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

		// forward declare compute gadgets
		namespace derived_data
			{

				template <typename number> class zeta_timeseries_compute;
				template <typename number> class zeta_kseries_compute;
				template <typename number> class fNL_timeseries_compute;

			}   // derived_data


    template <typename number>
    class data_manager
      {

      public:

		    // data structures for storing k-configurations

		    //! Stores a twopf k-configuration
		    class twopf_configuration
			    {
		      public:
				    unsigned serial;
				    double k_conventional;
				    double k_comoving;
			    };

		    //! Stores a threepf k-configuration
		    class threepf_configuration
			    {
		      public:
				    unsigned serial;
				    double kt_conventional;
				    double kt_comoving;
				    double alpha;
				    double beta;
				    double k1_conventional;
				    double k2_conventional;
				    double k3_conventional;
				    double k1_comoving;
				    double k2_comoving;
				    double k3_comoving;
				    unsigned int k1_serial;
				    unsigned int k2_serial;
				    unsigned int k3_serial;
			    };

        // data structures for storing individual sample points from each integration

		    //! Stores a background field configuration associated with single time-point
        class backg_item
          {
          public:

		        //! time serial number for this configuration
            unsigned int        time_serial;

		        //! values
            std::vector<number> coords;

		        //! kconfig serial number for the integration which produced this. Used when unwinding a batch.
            unsigned int        source_serial;

          };

		    //! Stores a twopf configuration associated with a single time-point and k-configuration
        class twopf_item
          {
          public:

		        //! time serial number for this configuration
            unsigned int        time_serial;

		        //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

						// values
            std::vector<number> elements;

		        //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
		        unsigned int        source_serial;

          };

        //! Stores a tensor two-point function configuration, associated with a single time-point and k-configuration
        class tensor_twopf_item
          {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            // values
            std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int        source_serial;
          };

		    //! Stores a threepf configuration associated with a single time-point and k-configuration
        class threepf_item
          {
          public:

		        //! time serial number for this configuration
            unsigned int        time_serial;

		        //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

		        //! values
            std::vector<number> elements;

		        //! kconfig serial number for the integration which produced these values. Used when unwinding a batch
		        unsigned int        source_serial;
          };

        //! Stores per-configuration statistics about the performance of the integrator.
        //! Not used by all backends
        class configuration_statistics
          {
          public:

		        //! kconfig serial number for this configuration
            unsigned int                  serial;

		        //! time spent integrating, in nanoseconds
            boost::timer::nanosecond_type integration;

		        //! time spent batching, in nanoseconds
            boost::timer::nanosecond_type batching;

		        //! number of mesh refinements needed for this configuration
		        unsigned int                  refinements;
          };

        //! Stores a zeta twopf configuration
        class zeta_twopf_item
          {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number of this configuration
            unsigned int kconfig_serial;

            // value
            number value;
          };

        //! Stores a zeta threepf configuration
        class zeta_threepf_item
          {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number of this configuration
            unsigned int kconfig_serial;

            // value
            number value;
          };

        //! Stores an fNL configuration
        class fNL_item
          {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            // value
            number BT;
            number TT;
          };

        // writer functions, used by the compute backends to store the output of each integration
        // in a temporary container

        class generic_batcher;
        class integration_batcher;
        class postintegration_batcher;

        //! Background writer function
        typedef std::function<void(integration_batcher*, const std::vector<backg_item>&)> backg_writer;

        //! Two-point function writer function
        typedef std::function<void(integration_batcher*, const std::vector<twopf_item>&)> twopf_writer;

        //! Tensor two-point function writer function
        typedef std::function<void(integration_batcher*, const std::vector<tensor_twopf_item>&)> tensor_twopf_writer;

        //! Three-point function writer function
        typedef std::function<void(integration_batcher*, const std::vector<threepf_item>&)> threepf_writer;

        //! Per-configuration statistics writer function
        typedef std::function<void(integration_batcher*, const std::vector<configuration_statistics>&)> stats_writer;

		    //! Host information writer function
		    typedef std::function<void(integration_batcher*)> host_info_writer;

        //! Zeta 2pf writer function
        typedef std::function<void(postintegration_batcher*, const std::vector<zeta_twopf_item>&)> zeta_twopf_writer;

        //! Zeta 3pf writer function
        typedef std::function<void(postintegration_batcher*, const std::vector<zeta_threepf_item>&)> zeta_threepf_writer;

        //! fNL writer function
        typedef std::function<void(postintegration_batcher*, const std::vector<fNL_item>&, derived_data::template_type)> fNL_writer;


		    // data pipe, used by derived content providers to extract content from an output group
		    class datapipe;

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

        // Batcher objects, used by integration workers to push results into a container

		    //! Abstract batcher object, from which the concrete two- and threepf-batchers are derived.
		    //! The batcher has a log directory, used for logging all transaction written into it,
		    //! and also has a container replacement mechanism which writes all cached data into
		    //! a data_manager-managed temporary file, and then pushes it to the master process.
        class generic_batcher
          {

          public:

            typedef enum { flush_immediate, flush_delayed } flush_mode;


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


        class integration_batcher: public generic_batcher
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            template <typename handle_type>
            integration_batcher(unsigned int cap, unsigned int Nf,
                                const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                container_dispatch_function d, container_replacement_function r,
                                handle_type h, unsigned int w, bool s);

            virtual ~integration_batcher() = default;


            // ADMINISTRATION

          public:

            //! Return number of fields
            unsigned int get_number_fields() const { return(this->Nfields); }

            //! Close this batcher -- called at the end of an integration
            virtual void close() override;


            // INTEGRATION MANAGEMENT

          public:

            //! Add integration details
            void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching);

            //! Add integration details, plus report a k-configuration serial number and mesh refinement level for storing per-configuration statistics
            void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching, unsigned int kserial, unsigned int refinement);

            //! Report a failed integration
            void report_integration_failure();

            //! Report an integration which required mesh refinement
            void report_refinement();

            //! Query whether any integrations failed
            bool integrations_failed() const { return(this->failures > 0); }

            //! Query how many refinements occurred
            unsigned int number_refinements() const { return(this->refinements); }

            //! Prepare for new work assignment
            void begin_assignment();

		        //! Tidy up after a work assignment
		        void end_assignment();


            // INTEGRATION STATISTICS

          public:

            //! Get aggregate integration time
            boost::timer::nanosecond_type get_integration_time() const { return(this->integration_time); }

            //! Get longest integration time
            boost::timer::nanosecond_type get_max_integration_time() const { return(this->max_integration_time); }

            //! Get shortest integration time
            boost::timer::nanosecond_type get_min_integration_time() const { return(this->min_integration_time); }

            //! Get aggegrate batching time
            boost::timer::nanosecond_type get_batching_time() const { return(this->batching_time); }

            //! Get longest batching time
            boost::timer::nanosecond_type get_max_batching_time() const { return(this->max_batching_time); }

            //! Get shortest batching time
            boost::timer::nanosecond_type get_min_batching_time() const { return(this->min_batching_time); }

            //! Get total number of reported integrations
            unsigned int get_reported_integrations() const { return(this->num_integrations); }


            // PUSH BACKGROUND

          public:

            //! Push a background sample
            void push_backg(unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values);


            // UNBATCH

          public:

            //! Unbatch a given configuration
            virtual void unbatch(unsigned int source_serial) = 0;


            // INTERNAL DATA

          protected:


            // CACHES

            //! Cache of background pushes
            std::vector<backg_item>               backg_batch;

            //! Cache of per-configuration statistics
            std::vector<configuration_statistics> stats_batch;


            // OTHER INTERNAL DATA

            //! Number of fields associated with this integration
            const unsigned int                    Nfields;


            // INTEGRATION EVENT TRACKING

            //! number of integrations which have failed
            unsigned int                          failures;

            //! number of integrations which required refinement
            unsigned int                          refinements;


            // INTEGRATION STATISTICS

            //! Are we collecting per-configuration statistics?
            bool                                  collect_statistics;

            //! Number of integrations handled by this batcher
            unsigned int                          num_integrations;

            //! Aggregate integration time
            boost::timer::nanosecond_type         integration_time;

            //! Aggregate batching time
            boost::timer::nanosecond_type         batching_time;

            //! Longest integration time
            boost::timer::nanosecond_type         max_integration_time;

            //! Shortest integration time
            boost::timer::nanosecond_type         min_integration_time;

            //! Longest batching time
            boost::timer::nanosecond_type         max_batching_time;

            //! Shortest batching time
            boost::timer::nanosecond_type         min_batching_time;

          };


        class postintegration_batcher: public generic_batcher
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            template <typename handle_type>
            postintegration_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                    container_dispatch_function d, container_replacement_function r,
                                    handle_type h, unsigned int w);

            virtual ~postintegration_batcher() = default;


            // MANAGEMENT

          public:

            //! Report finished block
            void report_finished_item(boost::timer::nanosecond_type time);

            //! Prepare for new work assignment
            void begin_assignment();

            //! Tidy up after a work assignment
            void end_assignment();


            // STATISTICS

          public:

            //! Get aggregate processing time
            boost::timer::nanosecond_type get_processing_time() const { return(this->total_time); }

            //! Get longest item processing time
            boost::timer::nanosecond_type get_max_processing_time() const { return(this->longest_time); }

            //! Get shortest item processing time
            boost::timer::nanosecond_type get_min_processing_time() const { return(this->shortest_time); }

		        //! Get number of items processed
		        unsigned int get_items_processed() const { return(this->items_processed); }

		        // INTERNAL DATA

          private:

		        //! Number of work items processed
		        unsigned int items_processed;

		        //! Aggregate processing time
		        boost::timer::nanosecond_type total_time;

		        //! Longest individual processing time
		        boost::timer::nanosecond_type longest_time;

		        //! Shortest individual processing time
		        boost::timer::nanosecond_type shortest_time;

          };


        class twopf_batcher: public integration_batcher
          {

          public:

            class writer_group
              {
              public:
                backg_writer        backg;
                twopf_writer        twopf;
                tensor_twopf_writer tensor_twopf;
                stats_writer        stats;
		            host_info_writer    host_info;
              };

          public:

            template <typename handle_type>
            twopf_batcher(unsigned int cap, unsigned int Nf,
                          const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                          const writer_group& w,
                          container_dispatch_function d, container_replacement_function r,
                          handle_type h, unsigned int wn, bool s);

            void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

            void push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

		        virtual void unbatch(unsigned int source_serial) override;

          protected:

            virtual size_t storage() const override;

            virtual void flush(replacement_action action) override;

          protected:

            const writer_group writers;

            std::vector<twopf_item> twopf_batch;
            std::vector<tensor_twopf_item> tensor_twopf_batch;

          };


        class threepf_batcher: public integration_batcher
          {

          public:

            class writer_group
              {
              public:
                backg_writer        backg;
                twopf_writer        twopf_re;
                twopf_writer        twopf_im;
                tensor_twopf_writer tensor_twopf;
                threepf_writer      threepf;
                stats_writer        stats;
		            host_info_writer    host_info;
              };

          public:

            typedef enum { real_twopf, imag_twopf } twopf_type;

            template <typename handle_type>
            threepf_batcher(unsigned int cap, unsigned int Nf,
                            const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                            const writer_group& w,
                            container_dispatch_function d, container_replacement_function r,
                            handle_type h, unsigned int wn, bool s);

            void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values, twopf_type t=real_twopf);

            void push_threepf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

            void push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

		        virtual void unbatch(unsigned int source_serial) override;

          protected:

            virtual size_t storage() const override;

            virtual void flush(replacement_action action) override;

          protected:

            const writer_group writers;

            std::vector<twopf_item>        twopf_re_batch;
            std::vector<twopf_item>        twopf_im_batch;
            std::vector<tensor_twopf_item> tensor_twopf_batch;
            std::vector<threepf_item>      threepf_batch;

          };


        class zeta_twopf_batcher: public postintegration_batcher
          {

          public:

            class writer_group
              {
              public:
                zeta_twopf_writer twopf;
              };

          public:

            template <typename handle_type>
            zeta_twopf_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                               const writer_group& w, container_dispatch_function d, container_replacement_function r,
                               handle_type h, unsigned int wn);

            void push_twopf(unsigned int time_serial, unsigned int k_serial, number value);

          protected:

            virtual size_t storage() const override;

            virtual void flush(replacement_action action) override;

          protected:

            const writer_group writers;

            std::vector<zeta_twopf_item> twopf_batch;

          };


        class zeta_threepf_batcher: public postintegration_batcher
          {

          public:

            class writer_group
              {
              public:
                zeta_twopf_writer   twopf;
                zeta_threepf_writer threepf;
                zeta_threepf_writer redbsp;
              };

          public:

            template <typename handle_type>
            zeta_threepf_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                 const writer_group& w, container_dispatch_function d, container_replacement_function r,
                                 handle_type h, unsigned int wn);

            void push_twopf(unsigned int time_serial, unsigned int k_serial, number value);

            void push_threepf(unsigned int time_serial, unsigned int k_serial, number value);

            void push_reduced_bispectrum(unsigned int time_serial, unsigned int k_serial, number value);

          protected:

            virtual size_t storage() const override;

            virtual void flush(replacement_action action) override;

          protected:

            const writer_group writers;

            std::vector<zeta_twopf_item> twopf_batch;
            std::vector<zeta_threepf_item> threepf_batch;
            std::vector<zeta_threepf_item> redbsp_batch;

          };


        class fNL_batcher: public postintegration_batcher
          {

          public:

            class writer_group
              {
              public:
                fNL_writer fNL;
              };

          public:

            template <typename handle_type>
            fNL_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                        const writer_group& w, container_dispatch_function d, container_replacement_function r,
                        handle_type h, unsigned int wn, derived_data::template_type t);

            void push_fNL(unsigned int time_serial, number BT, number TT);

          protected:

            virtual size_t storage() const override;

            virtual void flush(replacement_action action) override;

          protected:

            const writer_group writers;

            std::vector<fNL_item> fNL_batch;

            derived_data::template_type type;

          };


        // DATAPIPE OBJECTS


		    //! Data pipe, used when generating derived content to extract data from an integration database.
		    //! The datapipe has a log directory, used for logging transactions on the pipe.
		    class datapipe
			    {

		      public:

				    typedef enum { time_config_group, twopf_kconfig_group, threepf_kconfig_group, time_serial_group, kconfig_serial_group } serial_group_tag;

				    typedef enum { cf_twopf_re, cf_twopf_im, cf_threepf, cf_tensor_twopf } cf_data_type;

		        typedef enum { twopf_real, twopf_imag } twopf_type;

		        //! Output-group finder function -- serivce provided by a repository implementation
		        typedef std::function<std::shared_ptr< typename repository<number>::template output_group_record< typename repository<number>::integration_payload > >(const std::string& name, const std::list<std::string>&)> output_group_finder;

		        //! Attach function for a datapipe
		        typedef std::function<std::shared_ptr< typename repository<number>::template output_group_record< typename repository<number>::integration_payload > >(datapipe*, output_group_finder&, const std::string&, const std::list<std::string>&)> attach_callback;

		        //! Detach function for a datapipe
		        typedef std::function<void(datapipe*)> detach_callback;

		        //! Push derived content
		        typedef std::function<void(datapipe*, typename derived_data::derived_product<number>*)> dispatch_function;

		        //! Extract a set of time sample-points from a datapipe
		        typedef std::function<void(datapipe*, const std::vector<unsigned int>&, std::vector<double>&)> time_config_callback;

		        //! Extract a set of 2pf k-configuration sample points from a datapipe
		        typedef std::function<void(datapipe*, const std::vector<unsigned int>&, std::vector<twopf_configuration>&)> kconfig_twopf_callback;

		        //! Extract a set of 3pf k-configuration sample points from a datapipe
		        typedef std::function<void(datapipe*, const std::vector<unsigned int>&, std::vector<threepf_configuration>&)> kconfig_threepf_callback;

		        //! Extract a background field at a set of time sample-points
		        typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, std::vector<number>&)> background_time_callback;

		        //! Extract a twopf component at fixed k-configuration for a set of time sample-points
		        typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&, twopf_type)> twopf_time_callback;

		        //! Extract a threepf component at fixed k-configuration for a set of time sample-point
		        typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> threepf_time_callback;

            //! Extract a tensor twopf component at fixed k-configuration for a set of time sample-points
            typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> tensor_twopf_time_callback;

				    //! Extract the zeta twopf at fixed k-configuration for a set of time sample-points
				    typedef std::function<void(datapipe*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_twopf_time_callback;

				    //! Extract the zeta threepf at fixed k-configuration for a set of time sample-points
				    typedef std::function<void(datapipe*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_threepf_time_callback;

				    //! Extract the zeta reduced bispectrum at fixed k-configuration for a set of time sample-points
				    typedef std::function<void(datapipe*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_redbsp_time_callback;

				    //! Extract an fNL for a set of time sample-points
				    typedef std::function<void(datapipe*, const std::vector<unsigned int>&, std::vector<number>&, derived_data::template_type)> fNL_time_callback;

				    //! Extract bispectrum.template data for a set of time sample-points
				    typedef std::function<void(datapipe*, const std::vector<unsigned int>&, std::vector<number>&, derived_data::template_type)> BT_time_callback;

				    //! Extract template.template data for a set of time sample-points
				    typedef std::function<void(datapipe*, const std::vector<unsigned int>&, std::vector<number>&, derived_data::template_type)> TT_time_callback;

		        //! Extract a twopf component at fixed time for a set of k-configuration sample-points
		        typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&, twopf_type)> twopf_kconfig_callback;

		        //! Extract a threepf component at fixed time for a set of k-configuration sample-points
		        typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> threepf_kconfig_callback;

            //! Extract a tensor twopf component at fixed time for a set of k-configuration sample-points
            typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> tensor_twopf_kconfig_callback;

		        //! Extract the zeta twopf at fixed time for a set of k-configuration sample-points
		        typedef std::function<void(datapipe*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_twopf_kconfig_callback;

		        //! Extract the zeta threepf at fixed time for a set of k-configuration sample-points
		        typedef std::function<void(datapipe*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_threepf_kconfig_callback;

		        //! Extract the zeta reduced bispectrum at fixed time for a set of k-configuration sample-points
		        typedef std::function<void(datapipe*, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> zeta_redbsp_kconfig_callback;

				    class utility_callbacks
					    {
				      public:
						    output_group_finder finder;
						    attach_callback     attach;
						    detach_callback     detach;
						    dispatch_function   dispatch;
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


				    //! time configuration data group tag
						class time_config_tag
							{

								// CONSTRUCTOR, DESTRUCTOR

						  public:

								time_config_tag(datapipe* p)
									: pipe(p)
									{
									}

								~time_config_tag() = default;


								// INTERFACE

						  public:

								//! check for tag equality
								bool operator==(const time_config_tag& obj) const { return(true); }   // nothing to check

								//! pull data corresponding to this tag
								void pull(const std::vector<unsigned int>& sns, std::vector<double>& data);

								//! emit a log item for this tag
								void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << log_item; }

								//! identify this tag
								std::string name() const { return(std::string("time config")); }


								// CLONE

						  public:

								//! copy this object
								time_config_tag* clone() const { return new time_config_tag(static_cast<const time_config_tag&>(*this)); }


								// HASH

						  public:

								//! hash
								unsigned int hash() const { return(0); }


								// INTERNAL DATA

						  protected:

								//! parent datapipe
								datapipe* pipe;

							};


				    //! twopf k-configuration data group tag
				    class twopf_kconfig_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

						    twopf_kconfig_tag(datapipe* p)
							    : pipe(p)
							    {
							    }

						    ~twopf_kconfig_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    bool operator==(const twopf_kconfig_tag& obj) const { return(true); }     // nothing to check

						    //! pull data corresponding to this tag
						    void pull(const std::vector<unsigned int>& sns, std::vector<twopf_configuration>& data);

				        //! emit a log item for this tag
				        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << log_item; }

				        //! identify this tag
				        std::string name() const { return(std::string("twopf k-config")); }


				        // CLONE

				      public:

				        //! copy this object
				        twopf_kconfig_tag* clone() const { return new twopf_kconfig_tag(static_cast<const twopf_kconfig_tag&>(*this)); }


				        // HASH

				      public:

				        //! hash
				        unsigned int hash() const { return(0); }


						    // INTERNAL DATA

				      protected:

						    //! parent pipe
						    datapipe* pipe;

					    };


				    //! threepf k-configuration data group tag
				    class threepf_kconfig_tag
					    {

						    // CONTRUCTOR, DESTRUCTOR

				      public:

						    threepf_kconfig_tag(datapipe* p)
							    : pipe(p)
							    {
							    }

						    ~threepf_kconfig_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    bool operator==(const threepf_kconfig_tag& obj) const { return(true); }     // nothing to check

						    //! pull data corresponding to this tag
						    void pull(const std::vector<unsigned int>& sns, std::vector<threepf_configuration>& data);

				        //! emit a log item for this tag
				        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << log_item; }

				        //! identify this tag
				        std::string name() const { return(std::string("threepf k-config")); }


				        // CLONE

				      public:

				        //! copy this object
				        threepf_kconfig_tag* clone() const { return new threepf_kconfig_tag(static_cast<const threepf_kconfig_tag&>(*this)); }


				        // HASH

				      public:

				        //! hash
				        unsigned int hash() const { return(0); }


						    // INTERNAL DATA

				      protected:

						    //! sampler function
						    datapipe* pipe;

					    };


				    //! data group tag -- abstract group used to derive background and field tags
				    class data_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

						    data_tag(datapipe* p)
							    : pipe(p)
							    {
								    assert(pipe != nullptr);
							    }

						    virtual ~data_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    virtual bool operator==(const data_tag& obj) const = 0;

								//! virtual function to pull a cache line
						    virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) = 0;

				        //! emit a log item for this tag
				        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << log_item; }

						    //! virtual function to identify this tag
						    virtual std::string name() const = 0;

				        // CLONE

				      public:

				        //! copy this object
				        virtual data_tag* clone() const = 0;


				        // HASH

				      public:

				        //! hash
				        virtual unsigned int hash() const = 0;


				        // INTERNAL DATA

				      protected:

				        //! parent datapipe
				        datapipe* pipe;

					    };


				    //! background time data group tag
				    class background_time_data_tag: public data_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

				        background_time_data_tag(datapipe* p, unsigned int i)
					        : id(i), data_tag(p)
					        {
					        }

						    virtual ~background_time_data_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    virtual bool operator==(const data_tag& obj) const override;

						    //! pull data corresponding to this tag
						    virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

				        //! identify this tag
				        virtual std::string name() const override { std::ostringstream msg; msg << "background field " << id; return(msg.str()); }


				        // CLONE

				      public:

				        //! copy this object
				        data_tag* clone() const { return new background_time_data_tag(static_cast<const background_time_data_tag&>(*this)); }


				        // HASH

				      public:

				        //! hash
				        virtual unsigned int hash() const override { return((this->id*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


						    // INTERNAL DATA

				      protected:

						    //! data id - controls which background field is sampled
						    const unsigned int id;

					    };


				    //! field correlation-function time data group tag
				    class cf_time_data_tag: public data_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

						    cf_time_data_tag(datapipe* p, cf_data_type t, unsigned int i, unsigned int k)
						      : type(t), id(i), kserial(k), data_tag(p)
							    {
							    }

						    virtual ~cf_time_data_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    virtual bool operator==(const data_tag& obj) const override;

						    //! pull data corresponding to this tag
						    virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

				        //! identify this tag
				        virtual std::string name() const override;


				        // CLONE

				      public:

				        //! copy this object
				        data_tag* clone() const { return new cf_time_data_tag(static_cast<const cf_time_data_tag&>(*this)); }


				        // HASH

				      public:

				        //! hash
				        virtual unsigned int hash() const override { return((this->kserial*8761 + this->id*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


						    // INTERNAL DATA

				      protected:

						    //! type
						    const cf_data_type type;

						    //! data id - controls which element id is sampled
						    const unsigned int id;

						    //! kserial - controls which k serial number is sampled
						    const unsigned int kserial;

					    };


				    //! field correlation-function kconfig data group tag
				    class cf_kconfig_data_tag: public data_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

						    cf_kconfig_data_tag(datapipe* p, cf_data_type t, unsigned int i, unsigned int ts)
							    : type(t), id(i), tserial(ts), data_tag(p)
							    {
							    }

						    virtual ~cf_kconfig_data_tag() = default;


						    // INTERFACE

				      public:

				        //! check for tag equality
				        virtual bool operator==(const data_tag& obj) const override;

						    //! pull data corresponding to this tag
						    virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

						    //! identify this tag
						    virtual std::string name() const override;


						    // CLONE

				      public:

						    //! copy this object
						    data_tag* clone() const { return new cf_kconfig_data_tag(static_cast<const cf_kconfig_data_tag&>(*this)); }


						    // HASH

				      public:

						    //! hash
						    virtual unsigned int hash() const override { return((this->tserial*8761 + this->id*2131) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


						    // INTERNAL DATA

				      protected:

						    //! type
						    const cf_data_type type;

						    //! data id - controls which element id is samples
						    const unsigned int id;

						    //! tserial - controls which t serial number is sampled
						    const unsigned int tserial;

					    };


				    //! zeta two-point function time data tag
				    class zeta_twopf_time_data_tag: public data_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

						    zeta_twopf_time_data_tag(datapipe* p, const twopf_configuration& k, integration_task<number>* t, unsigned int N, bool c)
							    : kdata(k), data_tag(p), tk(t), N_fields(N), cached(c)
							    {
							    }

						    virtual ~zeta_twopf_time_data_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    virtual bool operator==(const data_tag& obj) const override;

						    //! pull data corresponding to this tag
						    virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

						    //! identify this tag
						    virtual std::string name() const override { std::ostringstream msg; msg << "zeta two-point function, kserial =  " << kdata.serial; return(msg.str()); }


						    // CLONE

				      public:

						    //! copy this object
						    data_tag* clone() const { return new zeta_twopf_time_data_tag(static_cast<const zeta_twopf_time_data_tag&>(*this)); }


						    // HASH

				      public:

						    //! hash
						    virtual unsigned int hash() const override { return((this->kdata.serial*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


						    // INTERNAL DATA

				      protected:

						    //! cached version available?
						    bool cached;

						    //! kserial - controls which k serial number is sampled
						    const twopf_configuration kdata;

						    //! compute delegate
						    derived_data::zeta_timeseries_compute<number> computer;

				        //! pointer to task
				        integration_task<number>* tk;

				        //! number of fields
				        unsigned int N_fields;

					    };


		        //! zeta two-point function time data tag
		        class zeta_threepf_time_data_tag: public data_tag
			        {

		            // CONSTRUCTOR, DESTRUCTOR

		          public:

		            zeta_threepf_time_data_tag(datapipe* p, const threepf_configuration& k, integration_task<number>* t, unsigned int N, bool c)
			            : kdata(k), data_tag(p), tk(t), N_fields(N), cached(c)
			            {
			            }

		            virtual ~zeta_threepf_time_data_tag() = default;


		            // INTERFACE

		          public:

		            //! check for tag equality
		            virtual bool operator==(const data_tag& obj) const override;

		            //! pull data corresponding to this tag
		            virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

		            //! identify this tag
		            virtual std::string name() const override { std::ostringstream msg; msg << "zeta three-point function, kserial =  " << kdata.serial; return(msg.str()); }


		            // CLONE

		          public:

		            //! copy this object
		            data_tag* clone() const { return new zeta_threepf_time_data_tag(static_cast<const zeta_threepf_time_data_tag&>(*this)); }


		            // HASH

		          public:

		            //! hash
		            virtual unsigned int hash() const override { return((this->kdata.serial*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


		            // INTERNAL DATA

		          protected:

		            //! cached version available?
		            bool cached;

		            //! kserial - controls which k serial number is sampled
		            const threepf_configuration kdata;

				        //! compute delegate
				        derived_data::zeta_timeseries_compute<number> computer;

		            //! pointer to task
		            integration_task<number>* tk;

		            //! number of fields
		            unsigned int N_fields;

			        };


		        //! zeta two-point function time data tag
		        class zeta_reduced_bispectrum_time_data_tag: public data_tag
			        {

		            // CONSTRUCTOR, DESTRUCTOR

		          public:

		            zeta_reduced_bispectrum_time_data_tag(datapipe* p, const threepf_configuration& k, integration_task<number>* t, unsigned int N, bool c)
			            : kdata(k), data_tag(p), tk(t), N_fields(N), cached(c)
			            {
			            }

		            virtual ~zeta_reduced_bispectrum_time_data_tag() = default;


		            // INTERFACE

		          public:

		            //! check for tag equality
		            virtual bool operator==(const data_tag& obj) const override;

		            //! pull data corresponding to this tag
		            virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

		            //! identify this tag
		            virtual std::string name() const override { std::ostringstream msg; msg << "zeta reduced bispectrum, kserial =  " << kdata.serial; return(msg.str()); }


		            // CLONE

		          public:

		            //! copy this object
		            data_tag* clone() const { return new zeta_reduced_bispectrum_time_data_tag(static_cast<const zeta_reduced_bispectrum_time_data_tag&>(*this)); }


		            // HASH

		          public:

		            //! hash
		            virtual unsigned int hash() const override { return((this->kdata.serial*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


		            // INTERNAL DATA

		          protected:

		            //! cached version available?
		            bool cached;

		            //! kserial - controls which k serial number is sampled
		            const threepf_configuration kdata;

		            //! compute delegate
		            derived_data::zeta_timeseries_compute<number> computer;

		            //! pointer to task
		            integration_task<number>* tk;

		            //! number of fields
		            unsigned int N_fields;

			        };


				    //! zeta correlation-function kconfig data tag
				    class zeta_twopf_kconfig_data_tag: public data_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

						    zeta_twopf_kconfig_data_tag(datapipe* p, unsigned int ts, integration_task<number>* t, unsigned int N, bool c)
							    : tserial(ts), data_tag(p), tk(t), N_fields(N), cached(c)
							    {
							    }

						    virtual ~zeta_twopf_kconfig_data_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    virtual bool operator==(const data_tag& obj) const override;

						    //! pull data corresponding to this tag
						    virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

						    //! identify this tag
						    virtual std::string name() const override { std::ostringstream msg; msg << "zeta two-point function, tserial =  " << tserial; return(msg.str()); }


						    // CLONE

				      public:

						    //! copy this object
						    data_tag* clone() const { return new zeta_twopf_kconfig_data_tag(static_cast<const zeta_twopf_kconfig_data_tag&>(*this)); }


						    // HASH

				      public:

						    //! hash
						    virtual unsigned int hash() const override { return((this->tserial*2131) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


						    // INTERNAL DATA

				      protected:

				        //! cached version available?
				        bool cached;

						    //! tserial - controls which t serial number is sampled
						    const unsigned int tserial;

						    //! compute delegate
						    derived_data::zeta_kseries_compute<number> computer;

				        //! pointer to task
				        integration_task<number>* tk;

				        //! number of fields
				        unsigned int N_fields;

					    };


		        //! zeta correlation-function kconfig data tag
		        class zeta_threepf_kconfig_data_tag: public data_tag
			        {

		            // CONSTRUCTOR, DESTRUCTOR

		          public:

		            zeta_threepf_kconfig_data_tag(datapipe* p, unsigned int ts, integration_task<number>* t, unsigned int N, bool c)
			            : tserial(ts), data_tag(p), tk(t), N_fields(N), cached(c)
			            {
			            }

		            virtual ~zeta_threepf_kconfig_data_tag() = default;


		            // INTERFACE

		          public:

		            //! check for tag equality
		            virtual bool operator==(const data_tag& obj) const override;

		            //! pull data corresponding to this tag
		            virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

		            //! identify this tag
		            virtual std::string name() const override { std::ostringstream msg; msg << "zeta three-point function, tserial =  " << tserial; return(msg.str()); }


		            // CLONE

		          public:

		            //! copy this object
		            data_tag* clone() const { return new zeta_threepf_kconfig_data_tag(static_cast<const zeta_threepf_kconfig_data_tag&>(*this)); }


		            // HASH

		          public:

		            //! hash
		            virtual unsigned int hash() const override { return((this->tserial*2131) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


		            // INTERNAL DATA

		          protected:

		            //! cached version available?
		            bool cached;

		            //! tserial - controls which t serial number is sampled
		            const unsigned int tserial;

				        //! compute delegate
				        derived_data::zeta_kseries_compute<number> computer;

				        //! pointer to task
				        integration_task<number>* tk;

				        //! number of fields
				        unsigned int N_fields;

			        };


		        //! zeta correlation-function kconfig data tag
		        class zeta_reduced_bispectrum_kconfig_data_tag: public data_tag
			        {

		            // CONSTRUCTOR, DESTRUCTOR

		          public:

		            zeta_reduced_bispectrum_kconfig_data_tag(datapipe* p, unsigned int ts, integration_task<number>* t, unsigned int N, bool c)
			            : tserial(ts), data_tag(p), tk(t), N_fields(N), cached(c)
			            {
			            }

		            virtual ~zeta_reduced_bispectrum_kconfig_data_tag() = default;


		            // INTERFACE

		          public:

		            //! check for tag equality
		            virtual bool operator==(const data_tag& obj) const override;

		            //! pull data corresponding to this tag
		            virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

		            //! identify this tag
		            virtual std::string name() const override { std::ostringstream msg; msg << "zeta reduced bispectrum, tserial =  " << tserial; return(msg.str()); }


		            // CLONE

		          public:

		            //! copy this object
		            data_tag* clone() const { return new zeta_reduced_bispectrum_kconfig_data_tag(static_cast<const zeta_reduced_bispectrum_kconfig_data_tag&>(*this)); }


		            // HASH

		          public:

		            //! hash
		            virtual unsigned int hash() const override { return((this->tserial*2131) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


		            // INTERNAL DATA

		          protected:

		            //! cached version available?
		            bool cached;

		            //! tserial - controls which t serial number is sampled
		            const unsigned int tserial;

		            //! compute delegate
		            derived_data::zeta_kseries_compute<number> computer;

		            //! pointer to task
		            integration_task<number>* tk;

		            //! number of fields
		            unsigned int N_fields;

			        };


		        // fNL time-series tag
				    class fNL_time_data_tag: public data_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

						    fNL_time_data_tag(datapipe* p, integration_task<number>* t, unsigned int N, derived_data::template_type ty, bool c)
							    : data_tag(p), tk(t), N_fields(N), type(ty), cached(c)
							    {
							    }

						    virtual ~fNL_time_data_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    virtual bool operator==(const data_tag& obj) const override;

						    //! pull data corresponding to this tag
						    virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

						    //! identify this tag
						    virtual std::string name() const override { std::ostringstream msg; msg << "fNL, template =  " << template_name(this->type); return(msg.str()); }


						    // CLONE

				      public:

						    //! copy this object
						    data_tag* clone() const { return new fNL_time_data_tag(static_cast<const fNL_time_data_tag&>(*this)); }


						    // HASH

				      public:

						    //! hash
						    virtual unsigned int hash() const override { return((static_cast<unsigned int>(this->type)*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


						    // INTERNAL DATA

				      protected:

				        //! cached version available?
				        bool cached;

						    //! compute delegate
						    derived_data::fNL_timeseries_compute<number> computer;

						    //! pointer to task
						    integration_task<number>* tk;

						    //! number of fields
						    unsigned int N_fields;

						    //! template type
						    typename derived_data::template_type type;

					    };


            // bispectrum.template time-series tag
            class BT_time_data_tag: public data_tag
              {

                // CONSTRUCTOR, DESTRUCTOR

              public:

                BT_time_data_tag(datapipe* p, integration_task<number>* t, unsigned int N, derived_data::template_type ty, bool c)
                  : data_tag(p), tk(t), N_fields(N), type(ty), cached(c),
                    restrict_triangles(false)
                  {
                  }

                BT_time_data_tag(datapipe* p, integration_task<number>* t, unsigned int N, derived_data::template_type ty,
                                 const std::vector<unsigned int>& kc, bool c)
                  : data_tag(p), tk(t), N_fields(N), type(ty), cached(c),
                    restrict_triangles(true),
                    kconfig_sns(kc)
                  {
                  }

                virtual ~BT_time_data_tag() = default;


                // INTERFACE

              public:

                //! check for tag equality
                virtual bool operator==(const data_tag& obj) const override;

                //! pull data corresponding to this tag
                virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

                //! identify this tag
                virtual std::string name() const override { std::ostringstream msg; msg << "bispectrum.template, template =  " << template_name(this->type); return(msg.str()); }


                // CLONE

              public:

                //! copy this object
                data_tag* clone() const { return new BT_time_data_tag(static_cast<const BT_time_data_tag&>(*this)); }


                // HASH

              public:

                //! hash
                virtual unsigned int hash() const override { return((static_cast<unsigned int>(this->type)*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


                // INTERNAL DATA

              protected:

                //! compute delegate
                derived_data::fNL_timeseries_compute<number> computer;

                //! cached version available?
                bool cached;

                //! pointer to task
                integration_task<number>* tk;

                //! number of fields
                unsigned int N_fields;

                //! template type
                typename derived_data::template_type type;

                //! restrict integration to supplied set of triangles?
                bool restrict_triangles;

                //! set of kconfig serial numbers to restrict to, if used
                std::vector<unsigned int> kconfig_sns;

              };


            // template.template time-series tag
            class TT_time_data_tag: public data_tag
              {

                // CONSTRUCTOR, DESTRUCTOR

              public:

                TT_time_data_tag(datapipe* p, integration_task<number>* t, unsigned int N, derived_data::template_type ty, bool c)
                  : data_tag(p), tk(t), N_fields(N), type(ty), cached(c),
                    restrict_triangles(false)
                  {
                  }

                TT_time_data_tag(datapipe* p, integration_task<number>* t, unsigned int N, derived_data::template_type ty,
                                 const std::vector<unsigned int>& kc, bool c)
                  : data_tag(p), tk(t), N_fields(N), type(ty), cached(c),
                    restrict_triangles(true),
                    kconfig_sns(kc)
                  {
                  }

                virtual ~TT_time_data_tag() = default;


                // INTERFACE

              public:

                //! check for tag equality
                virtual bool operator==(const data_tag& obj) const override;

                //! pull data corresponding to this tag
                virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

                //! identify this tag
                virtual std::string name() const override { std::ostringstream msg; msg << "template.template, template =  " << template_name(this->type); return(msg.str()); }


                // CLONE

              public:

                //! copy this object
                data_tag* clone() const { return new TT_time_data_tag(static_cast<const TT_time_data_tag&>(*this)); }


                // HASH

              public:

                //! hash
                virtual unsigned int hash() const override { return((static_cast<unsigned int>(this->type)*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


                // INTERNAL DATA

              protected:

                //! compute delegate
                derived_data::fNL_timeseries_compute<number> computer;

                //! cached version available?
                bool cached;

                //! pointer to task
                integration_task<number>* tk;

                //! number of fields
                unsigned int N_fields;

                //! template type
                typename derived_data::template_type type;

                //! restrict integration to supplied set of triangles?
                bool restrict_triangles;

                //! set of kconfig serial numbers to restrict to, if used
                std::vector<unsigned int> kconfig_sns;

              };


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

				    //! Attach an output-group to the datapipe, ready for reading
				    void attach(integration_task<number>* tk, unsigned int Nf, const std::list<std::string>& tags);

				    //! Detach an output-group from the datapipe
				    void detach(void);

            //! Is this datapipe attached to an output group?
            bool is_attached() const { return(this->attached_group.get() != nullptr); }


				    // PROPERTIES OF CURRENTLY-ATTACHED GROUP

		      public:

				    //! Get attached output group
				    std::shared_ptr< typename repository<number>::template output_group_record< typename repository<number>::integration_payload > >
              get_attached_output_group(void) const;

				    //! Get number of fields associated with currently attached group.
				    //! Output is meaningful only when a group is attached.
				    unsigned int get_N_fields() const { return(this->N_fields); }


				    // PULL DATA

		      public:

				    typedef linecache::serial_group< std::vector<double>, time_config_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_config_handle;
				    typedef linecache::serial_group< std::vector<twopf_configuration>, twopf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > twopf_kconfig_handle;
				    typedef linecache::serial_group< std::vector<threepf_configuration>, threepf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > threepf_kconfig_handle;
				    typedef linecache::serial_group< std::vector<number>, data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_data_handle;
				    typedef linecache::serial_group< std::vector<number>, data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > kconfig_data_handle;
            typedef linecache::serial_group< std::vector<number>, data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_zeta_handle;
            typedef linecache::serial_group< std::vector<number>, data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > kconfig_zeta_handle;

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

				    //! Generate a new time-configuration tag
				    time_config_tag new_time_config_tag();

				    //! Generate a new twopf-kconfiguration tag
				    twopf_kconfig_tag new_twopf_kconfig_tag();

				    //! Generate a new threepf-kconfiguration tag
				    threepf_kconfig_tag new_threepf_kconfig_tag();

				    //! Generate a new background tag
				    background_time_data_tag new_background_time_data_tag(unsigned int id);

				    //! Generate a new correlation-function time tag
				    cf_time_data_tag new_cf_time_data_tag(cf_data_type type, unsigned int id, unsigned int kserial);

				    //! Generate a new correlation-function kconfig tag
				    cf_kconfig_data_tag new_cf_kconfig_data_tag(cf_data_type type, unsigned int id, unsigned int tserial);

				    //! Generate a new zeta-correlation-function time tag
				    zeta_twopf_time_data_tag new_zeta_twopf_time_data_tag(const twopf_configuration& kdata);

				    //! Generate a new zeta-correlationn-function time tag
				    zeta_threepf_time_data_tag new_zeta_threepf_time_data_tag(const threepf_configuration& kdata);

				    //! Generate a new reduced bispectrum time tag
				    zeta_reduced_bispectrum_time_data_tag new_zeta_reduced_bispectrum_time_data_tag(const threepf_configuration& kdata);

				    //! Generate a new zeta-correlation-function kconfig tag
				    zeta_twopf_kconfig_data_tag new_zeta_twopf_kconfig_data_tag(unsigned int tserial);

				    //! Generate a new zeta-correlation-function kconfig tag
				    zeta_threepf_kconfig_data_tag new_zeta_threepf_kconfig_data_tag(unsigned int tserial);

				    //! Generate a new reduced bispectrum kconfig tag
				    zeta_reduced_bispectrum_kconfig_data_tag new_zeta_reduced_bispectrum_kconfig_data_tag(unsigned int tserial);

				    //! Generate a new fNL time tag
				    fNL_time_data_tag new_fNL_time_data_tag(derived_data::template_type type);

            //! Generate a new bispectrum.template time tag
            BT_time_data_tag new_BT_time_data_tag(derived_data::template_type type);

            //! Generate a new bispectrum.template time tag, with restriction on integral over triangles
            BT_time_data_tag new_BT_time_data_tag(derived_data::template_type type, const std::vector<unsigned int>& kc);

            //! Generate a new template.template time tag
            TT_time_data_tag new_TT_time_data_tag(derived_data::template_type type);

            //! Generate a new template.template time tag, with restriction on integral over triangles
            TT_time_data_tag new_TT_time_data_tag(derived_data::template_type type, const std::vector<unsigned int>& kc);

				    // friend sample tag classes, so they can use our data
				    friend class time_config_tag;
				    friend class twopf_kconfig_tag;
				    friend class threepf_kconfig_tag;
				    friend class background_time_data_tag;
				    friend class cf_time_data_tag;
				    friend class cf_kconfig_data_tag;
				    friend class zeta_twopf_time_data_tag;
				    friend class zeta_threepf_time_data_tag;
				    friend class zeta_reduced_bispectrum_time_data_tag;
				    friend class zeta_twopf_kconfig_data_tag;
				    friend class zeta_threepf_kconfig_data_tag;
				    friend class zeta_reduced_bispectrum_kconfig_data_tag;


		        // INTERNAL DATA

		      private:

		        //! Host information
		        host_information host_info;


				    // CACHES

		        //! time configuration cache
		        linecache::cache<std::vector<double>, time_config_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> time_config_cache;

		        //! twopf k-config cache
		        linecache::cache<std::vector<twopf_configuration>, twopf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> twopf_kconfig_cache;

		        //! threepf k-config cache
		        linecache::cache<std::vector<threepf_configuration>, threepf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> threepf_kconfig_cache;

		        //! data cache
		        linecache::cache<std::vector<number>, data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> data_cache;

            //! zeta cache
            linecache::cache<std::vector<number>, data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> zeta_cache;


				    // CACHE TABLES

		        //! Time configuration cache table for currently-attached group; null if no group is attached
		        linecache::table<std::vector<number>, time_config_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* time_config_cache_table;

		        //! twopf k-config cache table for currently-attached group; null if no group is attached
		        linecache::table<std::vector<twopf_configuration>, twopf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* twopf_kconfig_cache_table;

		        //! threepf k-config cache table for currently-attached group; null if no group is attached
		        linecache::table<std::vector<threepf_configuration>, threepf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* threepf_kconfig_cache_table;

		        //! data cache table for currently-attached group; null if no group is attached
		        linecache::table<std::vector<number>, data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* data_cache_table;

            //! zeta cache table for currently-attached group; null if no group is attached
            linecache::table<std::vector<number>, data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* zeta_cache_table;


				    // PROPERTIES

		        //! Maximum capacity to use (approximately--we don't try to do a detailed accounting of memory use)
		        unsigned int capacity;

		        //! Unique serial number identifying the worker process owning this datapipe
		        const unsigned int worker_number;

		        //! Implementation-dependent handle used by data_manager
		        void* manager_handle;


				    // CURRENTLY ATTACHED OUTPUT GROUP

		        //! Currently-attached output group; null if no group is attached
		        std::shared_ptr< typename repository<number>::template output_group_record<typename repository<number>::integration_payload> > attached_group;

				    //! Currently attached task
				    integration_task<number>* attached_task;

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


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a data_manager instance with a nominated capacity per batcher
        data_manager(unsigned int bcap, unsigned int dcap, unsigned int zcap)
          : batcher_capacity(bcap),
            pipe_data_capacity(dcap),
            pipe_zeta_capacity(zcap)
          {
          }

        //! Destroy the data_manager instance. In practice this would always be delegated to an implementation class
        virtual ~data_manager() = default;


        // CAPACITY ADMIN

      public:

        //! Return the maximum memory available for batchers on this worker
        size_t get_batcher_capacity() const { return(this->batcher_capacity); }

        //! Set the maximum memory avilable for batchers on this worker
        void set_batcher_capacity(size_t size) { this->batcher_capacity = size; }

        //! Return the maximum memory available for data cache on this worker
        size_t get_data_capacity() const { return(this->pipe_data_capacity); }

        //! Set capacity available for data cache on this worker
        void set_data_capacity(size_t size) { this->pipe_data_capacity = size; }

        //! Return the maximum memory available for zeta cache on this worker
        size_t get_zeta_capacity() const { return(this->pipe_zeta_capacity); }

        //! Set capacity available for zeta cache on this worker
        void set_zeta_capacity(size_t size) { this->pipe_zeta_capacity = size; }


        // WRITER HANDLING

      public:

        //! Initialize an integration_writer object.
        //! Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void initialize_writer(std::shared_ptr<typename repository<number>::integration_writer>& writer) = 0;

        //! Close an integration_writer object.
        virtual void close_writer(std::shared_ptr<typename repository<number>::integration_writer>& writer) = 0;

        //! Initialize a derived_content_writer object.
        virtual void initialize_writer(std::shared_ptr<typename repository<number>::derived_content_writer>& writer) = 0;

        //! Close an open derived_content_writer object.
        virtual void close_writer(std::shared_ptr<typename repository<number>::derived_content_writer>& writer) = 0;

        //! Initialize a postintegration_writer object.
        virtual void initialize_writer(std::shared_ptr<typename repository<number>::postintegration_writer>& writer) = 0;

        //! Close an open postintegration_writer object.
        virtual void close_writer(std::shared_ptr<typename repository<number>::postintegration_writer>& writer) = 0;


        // WRITE INDEX TABLES FOR A DATA CONTAINER

      public:

        //! Create tables needed for a twopf container
        virtual void create_tables(std::shared_ptr<typename repository<number>::integration_writer>& writer, twopf_task<number>* tk) = 0;

        //! Create tables needed for a threepf container
        virtual void create_tables(std::shared_ptr<typename repository<number>::integration_writer>& writer, threepf_task<number>* tk) = 0;

        //! Create tables needed for a zeta twopf container
        virtual void create_tables(std::shared_ptr<typename repository<number>::postintegration_writer>& writer, zeta_twopf_task<number>* tk) = 0;

        //! Create tables needed for a zeta threepf container
        virtual void create_tables(std::shared_ptr<typename repository<number>::postintegration_writer>& writer, zeta_threepf_task<number>* tk) = 0;

        //! Create tables needed for an fNL container
        virtual void create_tables(std::shared_ptr<typename repository<number>::postintegration_writer>& writer, fNL_task<number>* tk) = 0;


        // TEMPORARY CONTAINERS

      public:

        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual twopf_batcher create_temp_twopf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                          unsigned int worker, model<number>* m,
                                                          container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual threepf_batcher create_temp_threepf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                              unsigned int worker, model<number>* m,
                                                              container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for zeta twopf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_twopf_batcher create_temp_zeta_twopf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                                    unsigned int worker, container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for zeta threepf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_threepf_batcher create_temp_zeta_threepf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                                        unsigned int worker, container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for fNL data. Returns a batcher which can be used for writing to the container.
        virtual fNL_batcher create_temp_fNL_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                      unsigned int worker, container_dispatch_function dispatcher, derived_data::template_type type) = 0;


		    // DATA PIPES AND DATA ACCESS

      public:

		    //! Create a datapipe
		    virtual datapipe create_datapipe(const boost::filesystem::path& logdir, const boost::filesystem::path& tempdir,
                                         typename datapipe::output_group_finder finder, typename datapipe::dispatch_function dispatcher,
		                                     unsigned int worker, bool no_log=false) = 0;

        //! Pull a set of time sample-points from a datapipe
        virtual void pull_time_config(datapipe* pipe, const std::vector<unsigned int>& serial_numbers, std::vector<double>& sample) = 0;

        //! Pull a set of 2pf k-configuration serial numbers from a datapipe
        virtual void pull_kconfig_twopf(datapipe* pipe, const std::vector<unsigned int>& serial_numbers, std::vector<twopf_configuration>& sample) = 0;

        //! Pull a set of 3pd k-configuration serial numbesr from a datapipe
        //! Simultaneously, populates three lists (k1, k2, k3) with serial numbers for the 2pf k-configurations
        //! corresponding to k1, k2, k3
        virtual void pull_kconfig_threepf(datapipe* pipe, const std::vector<unsigned int>& serial_numbers, std::vector<threepf_configuration>& sample) = 0;

        //! Pull a time sample of a background field from a datapipe
        virtual void pull_background_time_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& t_serials, std::vector<number>& sample) = 0;

        //! Pull a time sample of a twopf component at fixed k-configuration from a datapipe
        virtual void pull_twopf_time_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& t_serials,
                                            unsigned int k_serial, std::vector<number>& sample, typename datapipe::twopf_type type) = 0;

        //! Pull a sample of a threepf at fixed k-configuration from a datapipe
        virtual void pull_threepf_time_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& t_serials,
                                              unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of a tensor twopf at fixed k-configuration from a datapipe
        virtual void pull_tensor_twopf_time_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& t_serials,
                                                   unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of the zeta twopf at fixed k-configuration from a datapipe
        virtual void pull_zeta_twopf_time_sample(typename data_manager<number>::datapipe*, const std::vector<unsigned int>& t_serials,
                                                 unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of the zeta threepf at fixed k-configuration from a datapipe
        virtual void pull_zeta_threepf_time_sample(typename data_manager<number>::datapipe*, const std::vector<unsigned int>& t_serials,
                                                   unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of the zeta reduced bispectrum at fixed k-configuration from a datapipe
        virtual void pull_zeta_redbsp_time_sample(typename data_manager<number>::datapipe*, const std::vector<unsigned int>& t_serials,
                                                  unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of fNL from a datapipe
        virtual void pull_fNL_time_sample(typename data_manager<number>::datapipe*, const std::vector<unsigned int>& t_serials,
                                          std::vector<number>& sample, derived_data::template_type type) = 0;

        //! Pull a sample of bispectrum.template from a datapipe
        virtual void pull_BT_time_sample(typename data_manager<number>::datapipe*, const std::vector<unsigned int>& t_serials,
                                         std::vector<number>& sample, derived_data::template_type type) = 0;

        //! Pull a sample of template.template from a datapipe
        virtual void pull_TT_time_sample(typename data_manager<number>::datapipe*, const std::vector<unsigned int>& t_serials,
                                         std::vector<number>& sample, derived_data::template_type type) = 0;

        //! Pull a kconfig sample of a twopf component at fixed time from a datapipe
        virtual void pull_twopf_kconfig_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& k_serials,
                                               unsigned int t_serial, std::vector<number>& sample, typename datapipe::twopf_type type) = 0;

        //! Pull a kconfig sample of a threepf at fixed time from a datapipe
        virtual void pull_threepf_kconfig_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& k_serials,
                                                 unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of a tensor twopf component at fixed time from a datapipe
        virtual void pull_tensor_twopf_kconfig_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& k_serials,
                                                      unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of the zeta twopf at fixed time from a datapipe
        virtual void pull_zeta_twopf_kconfig_sample(typename data_manager<number>::datapipe*, const std::vector<unsigned int>& k_serials,
                                                    unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of the zeta threepf at fixed time from a datapipe
        virtual void pull_zeta_threepf_kconfig_sample(typename data_manager<number>::datapipe*, const std::vector<unsigned int>& k_serials,
                                                      unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of the zeta reduced bispectrum at fixed time from a datapipe
        virtual void pull_zeta_redbsp_kconfig_sample(typename data_manager<number>::datapipe*, const std::vector<unsigned int>& k_serials,
                                                     unsigned int t_serial, std::vector<number>& sample) = 0;

        // INTERNAL DATA

      protected:

        //! Capacity available for batchers
        unsigned int                                             batcher_capacity;

        //! Capacity available for data cache
        unsigned int                                             pipe_data_capacity;

        //! Capacity available for zeta cache
        unsigned int                                             pipe_zeta_capacity;

      };


    // GENERIC BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    data_manager<number>::generic_batcher::generic_batcher(unsigned int cap,
                                                           const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                           typename data_manager<number>::container_dispatch_function d,
                                                           typename data_manager<number>::container_replacement_function r,
                                                           handle_type h, unsigned int w, bool no_log)
      : capacity(cap),
        container_path(cp),
        logdir_path(lp),
        dispatcher(d),
        replacer(r),
        worker_number(w),
        manager_handle(static_cast<void*>(h)),
        mode(data_manager<number>::generic_batcher::flush_immediate),
        flush_due(false)
      {
		    // set up logging

        std::ostringstream log_file;
        log_file << __CPP_TRANSPORT_LOG_FILENAME_A << worker_number << __CPP_TRANSPORT_LOG_FILENAME_B;

        boost::filesystem::path log_path = logdir_path / log_file.str();

        if(!no_log)
          {
            boost::shared_ptr<boost::log::core> core = boost::log::core::get();

//		    core->set_filter(boost::log::trivial::severity >= data_manager<number>::normal);

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

        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "** Instantiated batcher (capacity " << format_memory(capacity) << ")"
            << " on MPI host " << host_info.get_host_name()
		        << ", OS = " << host_info.get_os_name()
		        << ", Version = " << host_info.get_os_version()
		        << " (Release = " << host_info.get_os_release()
		        << ") | " << host_info.get_architecture()
		        << " | CPU vendor = " << host_info.get_cpu_vendor_id() << std::endl;
      }


    template <typename number>
    data_manager<number>::generic_batcher::~generic_batcher()
      {
        if(this->log_sink)    // implicitly converts to bool, value true if not null
          {
            boost::shared_ptr< boost::log::core > core = boost::log::core::get();
            core->remove_sink(this->log_sink);
          }
      }


    template <typename number>
    void data_manager<number>::generic_batcher::close()
      {
        this->flush(action_close);
      }


    template <typename number>
    void data_manager<number>::generic_batcher::check_for_flush()
      {
        if(this->storage() > this->capacity)
          {
            if(this->mode == flush_immediate) this->flush(action_replace);
            else                              this->flush_due = true;
          }
      }


    // INTEGRATION BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    data_manager<number>::integration_batcher::integration_batcher(unsigned int cap, unsigned int Nf,
                                                                   const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                                   container_dispatch_function d, container_replacement_function r,
                                                                   handle_type h, unsigned int w, bool s)
      : generic_batcher(cap, cp, lp, d, r, h, w),
        Nfields(Nf),
        num_integrations(0),
        integration_time(0),
        max_integration_time(0),
        min_integration_time(0),
        batching_time(0),
        max_batching_time(0),
        min_batching_time(0),
        collect_statistics(s),
        failures(0),
        refinements(0)
      {
      }


    template <typename number>
    void data_manager<number>::integration_batcher::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching)
      {
        this->integration_time += integration;
        this->batching_time += batching;

        this->num_integrations++;

        if(this->max_integration_time == 0 || integration > this->max_integration_time) this->max_integration_time = integration;
        if(this->min_integration_time == 0 || integration < this->min_integration_time) this->min_integration_time = integration;

        if(this->max_batching_time == 0 || batching > this->max_batching_time) this->max_batching_time = batching;
        if(this->min_batching_time == 0 || batching < this->min_batching_time) this->min_batching_time = batching;

        if(this->flush_due)
          {
            this->flush_due = false;
            this->flush(action_replace);
          }
      }


    template <typename number>
    void data_manager<number>::integration_batcher::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching,
                                                                               unsigned int kserial, unsigned int refinements)
      {
        this->report_integration_success(integration, batching);

        configuration_statistics stats;
        stats.serial      = kserial;
        stats.integration = integration;
        stats.batching    = batching;
		    stats.refinements = refinements;

        this->stats_batch.push_back(stats);

        if(this->flush_due)
          {
            this->flush_due = false;
            this->flush(action_replace);
          }
      }


    template <typename number>
    void data_manager<number>::integration_batcher::push_backg(unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values)
      {
        if(values.size() != 2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_BACKG);
        backg_item item;
        item.time_serial   = time_serial;
        item.source_serial = source_serial;
        item.coords        = values;

        this->backg_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    void data_manager<number>::integration_batcher::report_integration_failure()
      {
        this->failures++;
		    this->check_for_flush();

        if(this->flush_due)
          {
            this->flush_due = false;
            this->flush(action_replace);
          }
      }


    template <typename number>
    void data_manager<number>::integration_batcher::report_refinement()
      {
        this->refinements++;
      }


		template <typename number>
    void data_manager<number>::integration_batcher::begin_assignment()
			{
				this->num_integrations = 0;
				this->integration_time = 0;
				this->batching_time = 0;
				this->max_integration_time = 0;
				this->min_integration_time = 0;
				this->max_batching_time = 0;
				this->min_batching_time = 0;

				this->failures = 0;
				this->refinements = 0;
			}


		template <typename number>
		void data_manager<number>::integration_batcher::end_assignment()
			{
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "";
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "-- Finished assignment: final integration statistics";
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   processed " << this->num_integrations << " individual integrations in " << format_time(this->integration_time);
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   mean integration time           = " << format_time(this->integration_time/this->num_integrations);
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   longest individual integration  = " << format_time(this->max_integration_time);
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   shortest individual integration = " << format_time(this->min_integration_time);
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   total batching time             = " << format_time(this->batching_time);
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   mean batching time              = " << format_time(this->batching_time/this->num_integrations);
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   longest individual batch        = " << format_time(this->max_batching_time);
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   shortest individual batch       = " << format_time(this->min_batching_time);

		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "";

		    if(this->refinements > 0)
			    {
		        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "-- " << this->refinements << " work items required mesh refinement";
			    }
		    if(this->failures > 0)
			    {
		        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "-- " << this->failures << " work items failed to integrate";
			    }
			}


    template <typename number>
    void data_manager<number>::integration_batcher::close()
      {
        this->generic_batcher::close();
      }


    // POSTINTEGRATION BATCHER METHODS


    template <typename number>
    template <typename handle_type>
    data_manager<number>::postintegration_batcher::postintegration_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                                           container_dispatch_function d, container_replacement_function r,
                                                                           handle_type h, unsigned int w)
      : generic_batcher(cap, cp, lp, d, r, h, w),
        items_processed(0),
        total_time(0),
        longest_time(0),
        shortest_time(0)
      {
      }


    template <typename number>
    void data_manager<number>::postintegration_batcher::report_finished_item(boost::timer::nanosecond_type time)
      {
		    this->items_processed++;
		    this->total_time += time;

		    if(this->longest_time == 0 || time > this->longest_time) this->longest_time = time;
		    if(this->shortest_time == 0 || time < this->shortest_time) this->shortest_time = time;
      }


		template <typename number>
		void data_manager<number>::postintegration_batcher::begin_assignment()
			{
				this->items_processed = 0;
				this->total_time = 0;
				this->longest_time = 0;
				this->shortest_time = 0;
			}


		template <typename number>
		void data_manager<number>::postintegration_batcher::end_assignment()
			{
				BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "";
				BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "-- Finished assignment: final statistics";
				BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   processed " << this->items_processed << " individual work items in " << format_time(this->total_time);
				BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   mean processing time                = " << format_time(this->total_time/this->items_processed);
				BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   longest individual processing time  = " << format_time(this->longest_time);
				BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   shortest individual processing time = " << format_time(this->shortest_time);

				BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "";
			}


    // TWOPF BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    data_manager<number>::twopf_batcher::twopf_batcher(unsigned int cap, unsigned int Nf,
                                                       const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                       const writer_group& w,
                                                       container_dispatch_function d, container_replacement_function r,
                                                       handle_type h, unsigned int wn, bool s)
      : integration_batcher(cap, Nf, cp, lp, d, r, h, wn, s),
        writers(w)
      {
      }


    template <typename number>
    void data_manager<number>::twopf_batcher::push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                         const std::vector<number>& values)
      {
        if(values.size() != 2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TWOPF);
        twopf_item item;
        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        this->twopf_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    void data_manager<number>::twopf_batcher::push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                                const std::vector<number>& values)
      {
        if(values.size() != 4) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TENSOR_TWOPF);
        tensor_twopf_item item;
        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        this->tensor_twopf_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    size_t data_manager<number>::twopf_batcher::storage() const
      {
        return((sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->backg_batch.size()
               + (3*sizeof(unsigned int) + 4*sizeof(number))*this->tensor_twopf_batch.size()
               + (3*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*this->twopf_batch.size()
               + (2*sizeof(unsigned int) + 2*sizeof(boost::timer::nanosecond_type))*this->stats_batch.size());
      }


    template <typename number>
    void data_manager<number>::twopf_batcher::flush(replacement_action action)
      {
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing twopf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

		    this->writers.host_info(this);
        if(this->collect_statistics) this->writers.stats(this, this->stats_batch);
        this->writers.backg(this, this->backg_batch);
        this->writers.twopf(this, this->twopf_batch);
        this->writers.tensor_twopf(this, this->tensor_twopf_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->stats_batch.clear();
        this->backg_batch.clear();
        this->twopf_batch.clear();
        this->tensor_twopf_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        this->dispatcher(this);

        // close current container, and replace with a new one if required
        this->replacer(this, action);
      }


    template <typename number>
    void data_manager<number>::twopf_batcher::unbatch(unsigned int source_serial)
      {
        this->backg_batch.erase(std::remove_if(this->backg_batch.begin(), this->backg_batch.end(),
                                               [ & ](const backg_item& item) -> bool
                                               {
                                                   return (item.source_serial == source_serial);
                                               }),
                                this->backg_batch.end());

        this->twopf_batch.erase(std::remove_if(this->twopf_batch.begin(), this->twopf_batch.end(),
                                               [ & ](const twopf_item& item) -> bool
                                               {
                                                   return (item.source_serial == source_serial);
                                               }),
                                this->twopf_batch.end());

        this->tensor_twopf_batch.erase(std::remove_if(this->tensor_twopf_batch.begin(), this->tensor_twopf_batch.end(),
                                                      [ & ](const tensor_twopf_item& item) -> bool
                                                      {
                                                        return (item.source_serial == source_serial);
                                                      }),
                                       this->tensor_twopf_batch.end());
      }


    // THREEPF BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    data_manager<number>::threepf_batcher::threepf_batcher(unsigned int cap, unsigned int Nf,
                                                           const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                           const writer_group& w,
                                                           container_dispatch_function d, container_replacement_function r,
                                                           handle_type h, unsigned int wn, bool s)
      : integration_batcher(cap, Nf, cp, lp, d, r, h, wn, s),
        writers(w)
      {
      }


    template <typename number>
    void data_manager<number>::threepf_batcher::push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                           const std::vector<number>& values, twopf_type t)
      {
        if(values.size() != 2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TWOPF);
        twopf_item item;
        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        if(t == real_twopf) this->twopf_re_batch.push_back(item);
        else                this->twopf_im_batch.push_back(item);

        this->check_for_flush();
      }


    template <typename number>
    void data_manager<number>::threepf_batcher::push_threepf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                             const std::vector<number>& values)
      {
        if(values.size() != 2*this->Nfields*2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_THREEPF);
        threepf_item item;
        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        this->threepf_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    void data_manager<number>::threepf_batcher::push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
    const std::vector<number>& values)
      {
        if(values.size() != 4) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TENSOR_TWOPF);
        tensor_twopf_item item;
        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        this->tensor_twopf_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    size_t data_manager<number>::threepf_batcher::storage() const
      {
        return((sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->backg_batch.size()
               + (3*sizeof(unsigned int) + 4*sizeof(number))*this->tensor_twopf_batch.size()
               + (3*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*(this->twopf_re_batch.size() + this->twopf_im_batch.size())
               + (3*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*2*this->Nfields*sizeof(number))*this->threepf_batch.size()
	             + (2*sizeof(unsigned int) + 2*sizeof(boost::timer::nanosecond_type))*this->stats_batch.size());
      }


    template <typename number>
    void data_manager<number>::threepf_batcher::flush(replacement_action action)
      {
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing threepf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.host_info(this);
        if(this->collect_statistics) this->writers.stats(this, this->stats_batch);
        this->writers.backg(this, this->backg_batch);
        this->writers.twopf_re(this, this->twopf_re_batch);
        this->writers.twopf_im(this, this->twopf_im_batch);
        this->writers.tensor_twopf(this, this->tensor_twopf_batch);
        this->writers.threepf(this, this->threepf_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->stats_batch.clear();
        this->backg_batch.clear();
        this->twopf_re_batch.clear();
        this->twopf_im_batch.clear();
        this->tensor_twopf_batch.clear();
        this->threepf_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        this->dispatcher(this);

        // close current container, and replace with a new one if required
        this->replacer(this, action);
      }


    template <typename number>
    void data_manager<number>::threepf_batcher::unbatch(unsigned int source_serial)
      {
        this->backg_batch.erase(std::remove_if(this->backg_batch.begin(), this->backg_batch.end(),
                                               [ & ](const backg_item& item) -> bool
                                               {
                                                   return (item.source_serial == source_serial);
                                               }),
                                this->backg_batch.end());

        this->twopf_re_batch.erase(std::remove_if(this->twopf_re_batch.begin(), this->twopf_re_batch.end(),
                                                  [ & ](const twopf_item& item) -> bool
                                                  {
                                                      return (item.source_serial == source_serial);
                                                  }),
                                   this->twopf_re_batch.end());

        this->twopf_im_batch.erase(std::remove_if(this->twopf_im_batch.begin(), this->twopf_im_batch.end(),
                                                  [ & ](const twopf_item& item) -> bool
                                                  {
                                                      return (item.source_serial == source_serial);
                                                  }),
                                   this->twopf_im_batch.end());

        this->tensor_twopf_batch.erase(std::remove_if(this->tensor_twopf_batch.begin(), this->tensor_twopf_batch.end(),
                                                      [ & ](const tensor_twopf_item& item) -> bool
                                                      {
                                                        return (item.source_serial == source_serial);
                                                      }),
                                       this->tensor_twopf_batch.end());

        this->threepf_batch.erase(std::remove_if(this->threepf_batch.begin(), this->threepf_batch.end(),
                                                 [ & ](const threepf_item& item) -> bool
                                                 {
                                                     return (item.source_serial == source_serial);
                                                 }),
                                  this->threepf_batch.end());
      }


    // ZETA TWOPF BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    data_manager<number>::zeta_twopf_batcher::zeta_twopf_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                                 const writer_group& w, container_dispatch_function d, container_replacement_function r,
                                                                 handle_type h, unsigned int wn)
      : postintegration_batcher(cap, cp, lp, d, r, h, wn),
        writers(w)
      {
      }


    template <typename number>
    void data_manager<number>::zeta_twopf_batcher::push_twopf(unsigned int time_serial, unsigned int k_serial, number value)
      {
        zeta_twopf_item item;
        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.value          = value;

        this->twopf_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    size_t data_manager<number>::zeta_twopf_batcher::storage() const
      {
        return((3*sizeof(unsigned int) + sizeof(number))*this->twopf_batch.size());
      }


    template <typename number>
    void data_manager<number>::zeta_twopf_batcher::flush(replacement_action action)
      {
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing zeta twopf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.twopf(this, this->twopf_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->twopf_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        this->dispatcher(this);

        // close current container, and replace with a new one if required
        this->replacer(this, action);
      }


    // ZETA THREEPF BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    data_manager<number>::zeta_threepf_batcher::zeta_threepf_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                                     const writer_group& w, container_dispatch_function d, container_replacement_function r,
                                                                     handle_type h, unsigned int wn)
      : postintegration_batcher(cap, cp, lp, d, r, h, wn),
        writers(w)
      {
      }


    template <typename number>
    void data_manager<number>::zeta_threepf_batcher::push_twopf(unsigned int time_serial, unsigned int k_serial, number value)
      {
        zeta_twopf_item item;
        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.value          = value;

        this->twopf_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    void data_manager<number>::zeta_threepf_batcher::push_threepf(unsigned int time_serial, unsigned int k_serial, number value)
      {
        zeta_threepf_item item;
        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.value          = value;

        this->threepf_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    void data_manager<number>::zeta_threepf_batcher::push_reduced_bispectrum(unsigned int time_serial, unsigned int k_serial, number value)
      {
        zeta_threepf_item item;
        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.value          = value;

        this->redbsp_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    size_t data_manager<number>::zeta_threepf_batcher::storage() const
      {
        return((2*sizeof(unsigned int) + sizeof(number))*this->twopf_batch.size()
               + (3*sizeof(unsigned int) + sizeof(number))*this->threepf_batch.size()
               + (3*sizeof(unsigned int) + sizeof(number))*this->redbsp_batch.size());
      }


    template <typename number>
    void data_manager<number>::zeta_threepf_batcher::flush(replacement_action action)
      {
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing zeta threepf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.twopf(this, this->twopf_batch);
        this->writers.threepf(this, this->threepf_batch);
        this->writers.redbsp(this, this->redbsp_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->twopf_batch.clear();
        this->threepf_batch.clear();
        this->redbsp_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        this->dispatcher(this);

        // close current container, and replace with a new one if required
        this->replacer(this, action);
      }


    // FNL BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    data_manager<number>::fNL_batcher::fNL_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                   const writer_group& w, container_dispatch_function d, container_replacement_function r,
                                                   handle_type h, unsigned int wn, derived_data::template_type t)
      : postintegration_batcher(cap, cp, lp, d, r, h, wn),
        writers(w),
        type(t)
      {
      }


    template <typename number>
    void data_manager<number>::fNL_batcher::push_fNL(unsigned int time_serial, number BT, number TT)
      {
        fNL_item item;
        item.time_serial = time_serial;
        item.BT          = BT;
        item.TT          = TT;

        this->fNL_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    size_t data_manager<number>::fNL_batcher::storage() const
      {
        return((2*sizeof(unsigned int) + sizeof(number))*this->fNL_batch.size());
      }


    template <typename number>
    void data_manager<number>::fNL_batcher::flush(replacement_action action)
      {
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing " << template_name(this->type) << " batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.fNL(this, this->fNL_batch, this->type);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->fNL_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        this->dispatcher(this);

        // close current container, and replace with a new one if required
        this->replacer(this, action);
      }


    // DATAPIPE METHODS


    template <typename number>
    data_manager<number>::datapipe::datapipe(unsigned int dcap, unsigned int zcap,
                                             const boost::filesystem::path& lp, const boost::filesystem::path& tp, unsigned int w,
                                             typename data_manager<number>::datapipe::utility_callbacks& u,
                                             typename data_manager<number>::datapipe::config_cache& cf,
                                             typename data_manager<number>::datapipe::timeslice_cache& t,
                                             typename data_manager<number>::datapipe::kslice_cache& k, bool no_log)
      : logdir_path(lp), temporary_path(tp), worker_number(w),
        utilities(u), pull_config(cf), pull_timeslice(t), pull_kslice(k),
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
        attached_task(nullptr)
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

		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "** Instantiated datapipe (main cache " << format_memory(dcap) << ", zeta cache " << format_memory(zcap) << ")"
				    << " on MPI host " << host_info.get_host_name()
				    << ", OS = " << host_info.get_os_name()
				    << ", Version = " << host_info.get_os_version()
				    << " (Release = " << host_info.get_os_release()
				    << ") | " << host_info.get_architecture()
				    << " | CPU vendor = " << host_info.get_cpu_vendor_id() << std::endl;
      }


    template <typename number>
    data_manager<number>::datapipe::~datapipe()
      {
//		    assert(this->database_timer.is_stopped());

        if(!(this->database_timer.is_stopped()))
          {
            BOOST_LOG_SEV(this->log_source, data_manager<number>::error) << ":: Error: datapipe being destroyed, but database timer is still running";
          }

        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "";
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "-- Closing datapipe: final usage statistics:";
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   time spent querying database       = " << format_time(this->database_timer.elapsed().wall);
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   time-configuration cache hits      = " << this->time_config_cache.get_hits() << " | unloads = " << this->time_config_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   twopf k-configuration cache hits   = " << this->twopf_kconfig_cache.get_hits() << " | unloads = " << this->twopf_kconfig_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   threepf k-configuration cache hits = " << this->threepf_kconfig_cache.get_hits() << " | unloads = " << this->threepf_kconfig_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   data cache hits:                   = " << this->data_cache.get_hits() << " | unloads = " << this->data_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   zeta cache hits:                   = " << this->zeta_cache.get_hits() << " | unloads = " << this->zeta_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "";
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   time-configuration evictions       = " << format_time(this->time_config_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   twopf k-configuration evictions    = " << format_time(this->twopf_kconfig_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   threepf k-configuration evictions  = " << format_time(this->threepf_kconfig_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   data evictions                     = " << format_time(this->data_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   zeta evictions                     = " << format_time(this->zeta_cache.get_eviction_timer());

        // detach any attached output group, if necessary
        if(this->attached_group.get() != nullptr) this->detach();

        if(this->log_sink)    // implicitly converts to bool, value true if not null
          {
            boost::shared_ptr<boost::log::core> core = boost::log::core::get();
            core->remove_sink(this->log_sink);
          }
      }


    template <typename number>
    bool data_manager<number>::datapipe::validate_unattached(void) const
      {
        return(this->attached_group.get() == nullptr &&
          this->attached_task == nullptr &&
          this->time_config_cache_table == nullptr &&
          this->twopf_kconfig_cache_table == nullptr &&
          this->threepf_kconfig_cache_table == nullptr &&
          this->data_cache_table == nullptr &&
          this->zeta_cache_table == nullptr);
      }


    template <typename number>
    bool data_manager<number>::datapipe::validate_attached(void) const
      {
        return(this->attached_group.get() != nullptr &&
          this->attached_task != nullptr &&
          this->time_config_cache_table != nullptr &&
          this->twopf_kconfig_cache_table != nullptr &&
          this->threepf_kconfig_cache_table != nullptr &&
          this->data_cache_table != nullptr &&
          this->zeta_cache_table != nullptr);
      }


    template <typename number>
    void data_manager<number>::datapipe::attach(integration_task<number>* tk, unsigned int Nf, const std::list<std::string>& tags)
      {
        assert(this->validate_unattached());
        if(!this->validate_unattached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_ATTACH_PIPE_ALREADY_ATTACHED);

        assert(tk != nullptr);
        if(tk == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NULL_TASK);

        this->attached_group = this->utilities.attach(this, this->utilities.finder, tk->get_name(), tags);
        this->attached_task  = tk;

        typename repository<number>::integration_payload& payload = this->attached_group->get_payload();

        BOOST_LOG_SEV(this->get_log(), data_manager<number>::normal) << "** ATTACH output group " << boost::posix_time::to_simple_string(this->attached_group->get_creation_time())
            << " (from task '" << tk->get_name() << "', generated using integration backend '" << payload.get_backend() << "')";

        // attach new cache tables

        this->time_config_cache_table = &(this->time_config_cache.get_table_handle(payload.get_container_path().string()));
        this->twopf_kconfig_cache_table = &(this->twopf_kconfig_cache.get_table_handle(payload.get_container_path().string()));
        this->threepf_kconfig_cache_table = &(this->threepf_kconfig_cache.get_table_handle(payload.get_container_path().string()));
        this->data_cache_table = &(this->data_cache.get_table_handle(payload.get_container_path().string()));
        this->zeta_cache_table = &(this->zeta_cache.get_table_handle(payload.get_container_path().string()));

        // remember number of fields associated with this container
        this->N_fields = Nf;
      }


    template <typename number>
    void data_manager<number>::datapipe::detach(void)
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_DETACH_PIPE_NOT_ATTACHED);

        BOOST_LOG_SEV(this->get_log(), data_manager<number>::normal) << "** DETACH output group " << boost::posix_time::to_simple_string(this->attached_group->get_creation_time());

        this->utilities.detach(this);
        this->attached_group.reset();
        this->attached_task = nullptr;

        this->time_config_cache_table = nullptr;
        this->twopf_kconfig_cache_table = nullptr;
        this->threepf_kconfig_cache_table = nullptr;
        this->data_cache_table = nullptr;
        this->zeta_cache_table = nullptr;
      }


    template <typename number>
    std::shared_ptr< typename repository<number>::template output_group_record< typename repository<number>::integration_payload > > data_manager<number>::datapipe::get_attached_output_group(void) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->attached_group);
      }


    template <typename number>
    typename data_manager<number>::datapipe::time_config_handle& data_manager<number>::datapipe::new_time_config_handle(const std::vector<unsigned int>& sns) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->time_config_cache_table->get_serial_handle(sns, time_config_group));
      }


    template <typename number>
    typename data_manager<number>::datapipe::twopf_kconfig_handle& data_manager<number>::datapipe::new_twopf_kconfig_handle(const std::vector<unsigned int>& sns) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->twopf_kconfig_cache_table->get_serial_handle(sns, twopf_kconfig_group));
      }


    template <typename number>
    typename data_manager<number>::datapipe::threepf_kconfig_handle& data_manager<number>::datapipe::new_threepf_kconfig_handle(const std::vector<unsigned int>& sns) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->threepf_kconfig_cache_table->get_serial_handle(sns, threepf_kconfig_group));
      }


    template <typename number>
    typename data_manager<number>::datapipe::time_data_handle& data_manager<number>::datapipe::new_time_data_handle(const std::vector<unsigned int>& sns) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->data_cache_table->get_serial_handle(sns, time_serial_group));
      }


    template <typename number>
    typename data_manager<number>::datapipe::kconfig_data_handle& data_manager<number>::datapipe::new_kconfig_data_handle(const std::vector<unsigned int>& sns) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->data_cache_table->get_serial_handle(sns, kconfig_serial_group));
      }


    template <typename number>
    typename data_manager<number>::datapipe::time_zeta_handle& data_manager<number>::datapipe::new_time_zeta_handle(const std::vector<unsigned int>& sns) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->zeta_cache_table->get_serial_handle(sns, time_serial_group));
      }


    template <typename number>
    typename data_manager<number>::datapipe::kconfig_zeta_handle& data_manager<number>::datapipe::new_kconfig_zeta_handle(const std::vector<unsigned int>& sns) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        assert(sns.size() > 0);

        return(this->zeta_cache_table->get_serial_handle(sns, kconfig_serial_group));
      }


    template <typename number>
    typename data_manager<number>::datapipe::time_config_tag data_manager<number>::datapipe::new_time_config_tag()
      {
        return data_manager<number>::datapipe::time_config_tag(this);
      }


    template <typename number>
    typename data_manager<number>::datapipe::twopf_kconfig_tag data_manager<number>::datapipe::new_twopf_kconfig_tag()
      {
        return data_manager<number>::datapipe::twopf_kconfig_tag(this);
      }


    template <typename number>
    typename data_manager<number>::datapipe::threepf_kconfig_tag data_manager<number>::datapipe::new_threepf_kconfig_tag()
      {
        return data_manager<number>::datapipe::threepf_kconfig_tag(this);
      }


    template <typename number>
    typename data_manager<number>::datapipe::background_time_data_tag data_manager<number>::datapipe::new_background_time_data_tag(unsigned int id)
      {
        return data_manager<number>::datapipe::background_time_data_tag(this, id);
      }


    template <typename number>
    typename data_manager<number>::datapipe::cf_time_data_tag data_manager<number>::datapipe::new_cf_time_data_tag(cf_data_type type, unsigned int id, unsigned int kserial)
      {
        return data_manager<number>::datapipe::cf_time_data_tag(this, type, id, kserial);
      }


    template <typename number>
    typename data_manager<number>::datapipe::cf_kconfig_data_tag data_manager<number>::datapipe::new_cf_kconfig_data_tag(cf_data_type type, unsigned int id, unsigned int tserial)
      {
        return data_manager<number>::datapipe::cf_kconfig_data_tag(this, type, id, tserial);
      }


    template <typename number>
    typename data_manager<number>::datapipe::zeta_twopf_time_data_tag data_manager<number>::datapipe::new_zeta_twopf_time_data_tag(const twopf_configuration& kdata)
      {
		    bool cached = this->attached_group->get_payload().get_precomputed_products().get_zeta_twopf();
        return data_manager<number>::datapipe::zeta_twopf_time_data_tag(this, kdata, this->attached_task, this->N_fields, cached);
      }


    template <typename number>
    typename data_manager<number>::datapipe::zeta_threepf_time_data_tag data_manager<number>::datapipe::new_zeta_threepf_time_data_tag(const threepf_configuration& kdata)
      {
        bool cached = this->attached_group->get_payload().get_precomputed_products().get_zeta_threepf();
        return data_manager<number>::datapipe::zeta_threepf_time_data_tag(this, kdata, this->attached_task, this->N_fields, cached);
      }


    template <typename number>
    typename data_manager<number>::datapipe::zeta_reduced_bispectrum_time_data_tag data_manager<number>::datapipe::new_zeta_reduced_bispectrum_time_data_tag(const threepf_configuration& kdata)
      {
        bool cached = this->attached_group->get_payload().get_precomputed_products().get_zeta_redbsp();
        return data_manager<number>::datapipe::zeta_reduced_bispectrum_time_data_tag(this, kdata, this->attached_task, this->N_fields, cached);
      }


    template <typename number>
    typename data_manager<number>::datapipe::zeta_twopf_kconfig_data_tag data_manager<number>::datapipe::new_zeta_twopf_kconfig_data_tag(unsigned int tserial)
      {
        bool cached = this->attached_group->get_payload().get_precomputed_products().get_zeta_twopf();
        return data_manager<number>::datapipe::zeta_twopf_kconfig_data_tag(this, tserial, this->attached_task, this->N_fields, cached);
      }


    template <typename number>
    typename data_manager<number>::datapipe::zeta_threepf_kconfig_data_tag data_manager<number>::datapipe::new_zeta_threepf_kconfig_data_tag(unsigned int tserial)
      {
        bool cached = this->attached_group->get_payload().get_precomputed_products().get_zeta_threepf();
        return data_manager<number>::datapipe::zeta_threepf_kconfig_data_tag(this, tserial, this->attached_task, this->N_fields, cached);
      }


    template <typename number>
    typename data_manager<number>::datapipe::zeta_reduced_bispectrum_kconfig_data_tag data_manager<number>::datapipe::new_zeta_reduced_bispectrum_kconfig_data_tag(unsigned int tserial)
      {
        bool cached = this->attached_group->get_payload().get_precomputed_products().get_zeta_redbsp();
        return data_manager<number>::datapipe::zeta_reduced_bispectrum_kconfig_data_tag(this, tserial, this->attached_task, this->N_fields, cached);
      }


		template <typename number>
		bool is_cached(typename repository<number>::integration_payload& payload, derived_data::template_type type)
			{
				bool rval = false;
				switch(type)
					{
				    case derived_data::fNLlocal:
					    rval = payload.get_precomputed_products().get_fNL_local();
							break;

				    case derived_data::fNLequi:
					    rval = payload.get_precomputed_products().get_fNL_equi();
							break;

				    case derived_data::fNLortho:
					    rval = payload.get_precomputed_products().get_fNL_ortho();
							break;

				    case derived_data::fNLDBI:
					    rval = payload.get_precomputed_products().get_fNL_DBI();
							break;
					}

				return(rval);
			}


    template <typename number>
    typename data_manager<number>::datapipe::fNL_time_data_tag data_manager<number>::datapipe::new_fNL_time_data_tag(derived_data::template_type type)
      {
		    bool cached = is_cached<number>(this->attached_group->get_payload(), type);
        return data_manager<number>::datapipe::fNL_time_data_tag(this, this->attached_task, this->N_fields, type, cached);
      }


    template <typename number>
    typename data_manager<number>::datapipe::BT_time_data_tag data_manager<number>::datapipe::new_BT_time_data_tag(derived_data::template_type type)
      {
        bool cached = is_cached<number>(this->attached_group->get_payload(), type);
        return data_manager<number>::datapipe::BT_time_data_tag(this, this->attached_task, this->N_fields, type, cached);
      }


    template <typename number>
    typename data_manager<number>::datapipe::BT_time_data_tag data_manager<number>::datapipe::new_BT_time_data_tag(derived_data::template_type type, const std::vector<unsigned int>& kc)
      {
        bool cached = is_cached<number>(this->attached_group->get_payload(), type);
        return data_manager<number>::datapipe::BT_time_data_tag(this, this->attached_task, this->N_fields, type, kc, cached);
      }


    template <typename number>
    typename data_manager<number>::datapipe::TT_time_data_tag data_manager<number>::datapipe::new_TT_time_data_tag(derived_data::template_type type)
      {
        bool cached = is_cached<number>(this->attached_group->get_payload(), type);
        return data_manager<number>::datapipe::TT_time_data_tag(this, this->attached_task, this->N_fields, type, cached);
      }


    template <typename number>
    typename data_manager<number>::datapipe::TT_time_data_tag data_manager<number>::datapipe::new_TT_time_data_tag(derived_data::template_type type, const std::vector<unsigned int>& kc)
      {
        bool cached = is_cached<number>(this->attached_group->get_payload(), type);
        return data_manager<number>::datapipe::TT_time_data_tag(this, this->attached_task, this->N_fields, type, kc, cached);
      }


    template <typename number>
    void data_manager<number>::datapipe::time_config_tag::pull(const std::vector<unsigned int>& sns, std::vector<double>& data)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL time sample request";
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_config.time(this->pipe, sns, data);
        this->pipe->database_timer.stop();
      }


    template <typename number>
    void data_manager<number>::datapipe::twopf_kconfig_tag::pull(const std::vector<unsigned int>& sns, std::vector<twopf_configuration>& data)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL 2pf k-configuration sample request";
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_config.twopf(this->pipe, sns, data);
        this->pipe->database_timer.stop();
      }


    template <typename number>
    void data_manager<number>::datapipe::threepf_kconfig_tag::pull(const std::vector<unsigned int>& sns, std::vector<threepf_configuration>& data)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL 3pf k-configuration sample request";
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_config.threepf(this->pipe, sns, data);
        this->pipe->database_timer.stop();
      }


    template <typename number>
    void data_manager<number>::datapipe::background_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL background time sample request for element " << this->id;
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_timeslice.background(this->pipe, this->id, sns, sample);
        this->pipe->database_timer.stop();
      }


    template <typename number>
    void data_manager<number>::datapipe::cf_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        if(this->type == cf_twopf_re)
          {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
				    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL twopf time sample request, type = real, for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.twopf(this->pipe, this->id, sns, this->kserial, sample, twopf_real);
            this->pipe->database_timer.stop();
          }
        else if(this->type == cf_twopf_im)
          {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
				    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL twopf time sample request, type = imaginary, for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.twopf(this->pipe, this->id, sns, this->kserial, sample, twopf_imag);
            this->pipe->database_timer.stop();
          }
        else if(this->type == cf_threepf)
          {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
				    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL threepf time sample request for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.threepf(this->pipe, this->id, sns, this->kserial, sample);
            this->pipe->database_timer.stop();
          }
        else if(this->type == cf_tensor_twopf)
          {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL tensor twopf time sample request for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.tensor_twopf(this->pipe, this->id, sns, this->kserial, sample);
            this->pipe->database_timer.stop();
          }
        else
          {
            assert(false);
            throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_UNKNOWN_CF_TYPE);
          }
      }


    template <typename number>
    std::string data_manager<number>::datapipe::cf_time_data_tag::name() const
      {
        std::ostringstream msg;

        switch(this->type)
          {
            case cf_twopf_re:
              msg << "real twopf (time series)";
              break;

            case cf_twopf_im:
              msg << "imaginary twopf (time series";
              break;

            case cf_threepf:
              msg << "threepf (time series)";
              break;

            case cf_tensor_twopf:
              msg << "tensor twopf (time series)";
              break;

            default:
              msg << "unknown";
          }

        msg << ", element = " << this->id << ", kserial = " << this->kserial;
        return(msg.str());
      }


    template <typename number>
    void data_manager<number>::datapipe::cf_kconfig_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        if(this->type == cf_twopf_re)
          {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL twopf kconfig sample request, type = real, for element " << this->id << ", t-serial " << this->tserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.twopf(this->pipe, this->id, sns, this->tserial, sample, twopf_real);
            this->pipe->database_timer.stop();
          }
        else if(this->type == cf_twopf_im)
          {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL twopf kconfig sample request, type = imaginary, for element " << this->id << ", t-serial " << this->tserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.twopf(this->pipe, this->id, sns, this->tserial, sample, twopf_imag);
            this->pipe->database_timer.stop();
          }
        else if(this->type == cf_threepf)
          {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL threepf kconfig sample request for element " << this->id << ", t-serial " << this->tserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.threepf(this->pipe, this->id, sns, this->tserial, sample);
            this->pipe->database_timer.stop();
          }
        else if(this->type == cf_tensor_twopf)
          {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL tensor twopf kconfig sample request for element " << this->id << ", t-serial " << this->tserial;
#endif
            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.tensor_twopf(this->pipe, this->id, sns, this->tserial, sample);
            this->pipe->database_timer.stop();
          }
        else
          {
            assert(false);
            throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_UNKNOWN_CF_TYPE);
          }
      }


    template <typename number>
    std::string data_manager<number>::datapipe::cf_kconfig_data_tag::name() const
      {
        std::ostringstream msg;

        switch(this->type)
          {
            case cf_twopf_re:
              msg << "real twopf (kconfig series)";
              break;

            case cf_twopf_im:
              msg << "imaginary twopf (kconfig series";
              break;

            case cf_threepf:
              msg << "threepf (kconfig series)";
              break;

            case cf_tensor_twopf:
              msg << "tensor twopf (kconfig series)";
              break;

            default:
              msg << "unknown";
          }

        msg << ", element = " << this->id << ", tserial = " << this->tserial;
        return(msg.str());
      }


    template <typename number>
    void data_manager<number>::datapipe::zeta_twopf_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta twopf time sample request, k-configuration " << this->kdata.kserial;
#endif

		    if(this->cached)  // extract from data container
			    {
				    this->pipe->database_timer.resume();
				    this->pipe->pull_timeslice.zeta_twopf(this->pipe, sns, this->kdata.serial, sample);
				    this->pipe->database_timer.stop();
			    }
		    else              // have to compute values on the fly
			    {
		        // look up time values corresponding to these serial numbers
		        time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
		        time_config_tag tc_tag = this->pipe->new_time_config_tag();

		        const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

		        // set up handle for compute delegate
		        std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields);
		        this->computer.twopf(handle, sample, this->kdata);
			    }
      }


    template <typename number>
    void data_manager<number>::datapipe::zeta_threepf_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta threepf time sample request, type = real, k-configuration " << this->kdata.kserial;
#endif

        if(this->cached)  // extract from data container
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.zeta_threepf(this->pipe, sns, this->kdata.serial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields);
            this->computer.threepf(handle, sample, this->kdata);
	        }
      }


    template <typename number>
    void data_manager<number>::datapipe::zeta_reduced_bispectrum_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta reduced bispectrum time sample request, type = real, k-configuration " << this->kdata.kserial;
#endif

        if(this->cached)  // extract from data container
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.zeta_redbsp(this->pipe, sns, this->kdata.serial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields);
            this->computer.reduced_bispectrum(handle, sample, this->kdata);
	        }
      }


    template <typename number>
    void data_manager<number>::datapipe::zeta_twopf_kconfig_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta twopf kconfig sample request, t-serial " << this->tserial;
#endif

        if(this->cached)  // extract from data container
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.zeta_twopf(this->pipe, sns, this->tserial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // pull information on this time-value from the database -- not efficient! but is there a better way?
            std::vector<unsigned int> time_sn;
            time_sn.push_back(this->tserial);

            time_config_handle& tc_handle = this->pipe->new_time_config_handle(time_sn);
            time_config_tag tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_kseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_sn, time_values, this->N_fields);
            this->computer.twopf(handle, sample, 0);
	        }
      }


    template <typename number>
    void data_manager<number>::datapipe::zeta_threepf_kconfig_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta threepf kconfig sample request, t-serial " << this->tserial;
#endif

        if(this->cached)  // extract from data contanier
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.zeta_threepf(this->pipe, sns, this->tserial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // pull information on this time-value from the database -- as above, not efficient
            std::vector<unsigned int> time_sn;
            time_sn.push_back(this->tserial);

            time_config_handle& tc_handle = this->pipe->new_time_config_handle(time_sn);
            time_config_tag tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_kseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_sn, time_values, this->N_fields);
            this->computer.threepf(handle, sample, 0);
	        }
      }


    template <typename number>
    void data_manager<number>::datapipe::zeta_reduced_bispectrum_kconfig_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta reduced bispectrum kconfig sample request, t-serial " << this->tserial;
#endif

        if(this->cached)  // extract from data contanier
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.zeta_redbsp(this->pipe, sns, this->tserial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // pull information on this time-value from the database -- as above, not efficient
            std::vector<unsigned int> time_sn;
            time_sn.push_back(this->tserial);

            time_config_handle& tc_handle = this->pipe->new_time_config_handle(time_sn);
            time_config_tag tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_kseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_sn, time_values, this->N_fields);
            this->computer.reduced_bispectrum(handle, sample, 0);
	        }
      }


    template <typename number>
    void data_manager<number>::datapipe::fNL_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL fNL sample request, template = " << template_type(this->type);
#endif

        if(this->cached)  // extract from data container
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.fNL(this->pipe, sns, sample, this->type);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields, this->type);
            this->computer.fNL(handle, sample);
	        }
      }


    template <typename number>
    void data_manager<number>::datapipe::BT_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL bispectrum.template sample request, template = " << template_type(this->type);
#endif

        if(!this->restrict_triangles && this->cached)  // extract from data contanier
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.BT(this->pipe, sns, sample, this->type);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            if(this->restrict_triangles)
	            {
                std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields, this->type, this->kconfig_sns);
                this->computer.BT(handle, sample);
	            }
            else
	            {
                std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields, this->type);
                this->computer.BT(handle, sample);
	            }
	        }
      }


    template <typename number>
    void data_manager<number>::datapipe::TT_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
      {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL template.template sample request, template = " << template_type(this->type);
#endif

        if(!this->restrict_triangles && this->cached)  // extract from data contanier
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.BT(this->pipe, sns, sample, this->type);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            if(this->restrict_triangles)
	            {
                std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields, this->type, this->kconfig_sns);
                this->computer.TT(handle, sample);
	            }
            else
	            {
                std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields, this->type);
                this->computer.TT(handle, sample);
	            }
	        }
      }


    template <typename number>
    bool data_manager<number>::datapipe::background_time_data_tag::operator==(const data_tag& obj) const
      {
        const background_time_data_tag* bg_tag = dynamic_cast<const background_time_data_tag*>(&obj);

        if(bg_tag == nullptr) return(false);
        return(this->id == bg_tag->id);
      }


    template <typename number>
    bool data_manager<number>::datapipe::cf_time_data_tag::operator==(const data_tag& obj) const
      {
        const cf_time_data_tag* cf_tag = dynamic_cast<const cf_time_data_tag*>(&obj);

        if(cf_tag == nullptr) return(false);
        return(this->type == cf_tag->type && this->id == cf_tag->id && this->kserial == cf_tag->kserial);
      }


    template <typename number>
    bool data_manager<number>::datapipe::cf_kconfig_data_tag::operator==(const data_tag& obj) const
      {
        const cf_kconfig_data_tag* cf_tag = dynamic_cast<const cf_kconfig_data_tag*>(&obj);

        if(cf_tag == nullptr) return(false);
        return(this->type == cf_tag->type && this->id == cf_tag->id && this->tserial == cf_tag->tserial);
      }


    template <typename number>
    bool data_manager<number>::datapipe::zeta_twopf_time_data_tag::operator==(const data_tag& obj) const
      {
        const zeta_twopf_time_data_tag* zeta_tag = dynamic_cast<const zeta_twopf_time_data_tag*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->kdata.serial == zeta_tag->kdata.serial);
      }


    template <typename number>
    bool data_manager<number>::datapipe::zeta_threepf_time_data_tag::operator==(const data_tag& obj) const
      {
        const zeta_threepf_time_data_tag* zeta_tag = dynamic_cast<const zeta_threepf_time_data_tag*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->kdata.serial == zeta_tag->kdata.serial);
      }


    template <typename number>
    bool data_manager<number>::datapipe::zeta_reduced_bispectrum_time_data_tag::operator==(const data_tag& obj) const
      {
        const zeta_reduced_bispectrum_time_data_tag* zeta_tag = dynamic_cast<const zeta_reduced_bispectrum_time_data_tag*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->kdata.serial == zeta_tag->kdata.serial);
      }


    template <typename number>
    bool data_manager<number>::datapipe::zeta_twopf_kconfig_data_tag::operator==(const data_tag& obj) const
      {
        const zeta_twopf_kconfig_data_tag* zeta_tag = dynamic_cast<const zeta_twopf_kconfig_data_tag*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->tserial == zeta_tag->tserial);
      }


    template <typename number>
    bool data_manager<number>::datapipe::zeta_threepf_kconfig_data_tag::operator==(const data_tag& obj) const
      {
        const zeta_threepf_kconfig_data_tag* zeta_tag = dynamic_cast<const zeta_threepf_kconfig_data_tag*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->tserial == zeta_tag->tserial);
      }


    template <typename number>
    bool data_manager<number>::datapipe::zeta_reduced_bispectrum_kconfig_data_tag::operator==(const data_tag& obj) const
      {
        const zeta_reduced_bispectrum_kconfig_data_tag* zeta_tag = dynamic_cast<const zeta_reduced_bispectrum_kconfig_data_tag*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->tserial == zeta_tag->tserial);
      }


    template <typename number>
    bool data_manager<number>::datapipe::fNL_time_data_tag::operator==(const data_tag& obj) const
      {
        const fNL_time_data_tag* fNL_tag = dynamic_cast<const fNL_time_data_tag*>(&obj);

        if(fNL_tag == nullptr) return(false);
        return(this->type == fNL_tag->type);
      }


    template <typename number>
    bool data_manager<number>::datapipe::BT_time_data_tag::operator==(const data_tag& obj) const
      {
        const BT_time_data_tag* BT_tag = dynamic_cast<const BT_time_data_tag*>(&obj);

        if(BT_tag == nullptr) return(false);
        return(this->type == BT_tag->type);
      }


    template <typename number>
    bool data_manager<number>::datapipe::TT_time_data_tag::operator==(const data_tag& obj) const
      {
        const TT_time_data_tag* TT_tag = dynamic_cast<const TT_time_data_tag*>(&obj);

        if(TT_tag == nullptr) return(false);
        return(this->type == TT_tag->type);
      }


    // Provide specializations for the size methods used in linecache to compute the size of data elements
    namespace linecache
      {

        template<>
        unsigned int sizeof_container_element< typename std::vector< typename data_manager<double>::twopf_configuration > >() { return(sizeof(data_manager<double>::twopf_configuration)); }

        template<>
        unsigned int elementsof_container(const std::vector< typename data_manager<double>::twopf_configuration >& container) { return(container.size()); }

        template<>
        unsigned int sizeof_container_element< typename std::vector< typename data_manager<double>::threepf_configuration > >() { return(sizeof(data_manager<double>::threepf_configuration)); }

        template<>
        unsigned int elementsof_container(const std::vector< typename data_manager<double>::threepf_configuration >& container) { return(container.size()); }

        template<>
        unsigned int sizeof_container_element< std::vector<double> >() { return(sizeof(double)); }

        template<>
        unsigned int elementsof_container(const std::vector<double>& container) { return(container.size()); }

      }   // namespace linecache -- specializations


  }   // namespace transport



#endif //__data_manager_H_
