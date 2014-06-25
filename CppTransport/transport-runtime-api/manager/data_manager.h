//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __data_manager_H_
#define __data_manager_H_


#include <set>
#include <vector>
#include <list>
#include <functional>

#include <math.h>

#include "transport-runtime-api/scheduler/work_queue.h"
#include "transport-runtime-api/manager/repository.h"

#include "transport-runtime-api/utilities/formatter.h"
#include "transport-runtime-api/utilities/linecache.h"


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
#define __CPP_TRANSPORT_LOG_FILENAME_A  "worker_"
#define __CPP_TRANSPORT_LOG_FILENAME_B  "_%3N.log"


// default size of line cache hash table
#define __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE (1024)

// default size of the k-configuration caches - 1 Mb
#define __CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE (1*1024*1024)

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

		        //! kconfig serial number for the integration which produced these values. Used when unwinding a  batch
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

        //! Per-configuration statistics writer function
        typedef std::function<void(generic_batcher*, const std::vector<configuration_statistics>&)> stats_writer;


				//! Aggregation of writers for a two-point function integration
        class twopf_writer_group
          {
          public:
            backg_writer backg;
            twopf_writer twopf;
            stats_writer stats;
          };


		    //! Aggregation of writers for a three-point function integration
        class threepf_writer_group
          {
          public:
            backg_writer   backg;
            twopf_writer   twopf_re;
            twopf_writer   twopf_im;
            threepf_writer threepf;
            stats_writer   stats;
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

          public:

            template <typename handle_type>
            generic_batcher(unsigned int cap, unsigned int Nf,
                            const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                            container_dispatch_function d, container_replacement_function r,
                            handle_type h, unsigned int w, bool s);

            virtual ~generic_batcher();


            // ADMINISTRATION

          public:

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
            void get_manager_handle(handle_type* h) const { *h = static_cast<handle_type>(this->manager_handle); }

            //! Return number of fields
            unsigned int get_number_fields() const { return(this->Nfields); }

            //! Return worker numbers
            unsigned int get_worker_number() const { return(this->worker_number); }

            //! Close this batcher -- called at the end of an integration
            void close();


            // INTEGRATION MANAGEMENT

          public:

            //! Add integration details
            void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching);

            //! Add integration details, plus report a k-configuration serial number for storing per-configuration statistics
            void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching, unsigned int kserial);

            //! Report a failed integration
            void report_integration_failure();

		        //! Report an integration which required mesh refinement
		        void report_refinement();

            //! Query whether any integrations failed
            bool integrations_failed() const { return(this->failures > 0); }

		        //! Query how many refinements occurred
		        unsigned int number_refinements() const { return(this->refinements); }


            // STATISTICS

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


		        // UNBATCH

          public:

		        //! Unbatch a given configuration
		        virtual void unbatch(unsigned int source_serial) = 0;


            // INTERNAL DATA

          protected:

            // CACHES

            //! Cache of background pushes
            std::vector<backg_item>                                  backg_batch;

            //! Cache of per-configuration statistics
            std::vector<configuration_statistics>                    stats_batch;


            // OTHER INTERNAL DATA

            //! Capacity of this batcher; when the capacity is exceeded, the batcher
            //! flushes its data to a temporary database and pushes it to the
            //! master process for aggregation
            const unsigned int                                       capacity;

            //! Number of fields associated with this integration
            const unsigned int                                       Nfields;

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

            //! Worker number associated with this batcher
            unsigned int                                             worker_number;


		        // FLUSH HANDLING

		        //! Needs flushing at next opportunity?
		        bool flush_due;

            //! Flushing mode
            flush_mode                                               mode;


            // EVENT TRACKING

            //! number of integrations which have failed
            unsigned int                                             failures;

		        //! number of integrations which required refinement
		        unsigned int                                             refinements;


            // STATISTICS

            //! Are we collecting per-configuration statistics?
            bool                                                     collect_statistics;

            //! Number of integrations handled by this batcher
            unsigned int                                             num_integrations;

            //! Aggregate integration time
            boost::timer::nanosecond_type                            integration_time;

            //! Aggregate batching time
            boost::timer::nanosecond_type                            batching_time;

            //! Longest integration time
            boost::timer::nanosecond_type                            max_integration_time;

            //! Shortest integration time
            boost::timer::nanosecond_type                            min_integration_time;

            //! Longest batching time
            boost::timer::nanosecond_type                            max_batching_time;

            //! Shortest batching time
            boost::timer::nanosecond_type                            min_batching_time;


            // LOGGING

            //! Logger source
            boost::log::sources::severity_logger<log_severity_level> log_source;

            //! Logger sink
            boost::shared_ptr< sink_t >                              log_sink;

          };


        class twopf_batcher: public generic_batcher
          {

          public:

            template <typename handle_type>
            twopf_batcher(unsigned int cap, unsigned int Nf,
                          const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                          const twopf_writer_group& w,
                          container_dispatch_function d, container_replacement_function r,
                          handle_type h, unsigned int wn, bool s);

            void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

		        virtual void unbatch(unsigned int source_serial) override;

          protected:

            virtual size_t storage() const override;

            virtual void flush(replacement_action action) override;

          protected:

            const twopf_writer_group writers;

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
                            handle_type h, unsigned int wn, bool s);

            void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values, twopf_type t=real_twopf);

            void push_threepf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

		        virtual void unbatch(unsigned int source_serial) override;

          protected:

            virtual size_t storage() const override;

            virtual void flush(replacement_action action) override;

          protected:

            const threepf_writer_group writers;

            std::vector<twopf_item>    twopf_re_batch;
            std::vector<twopf_item>    twopf_im_batch;
            std::vector<threepf_item>  threepf_batch;

          };


        // DATAPIPE OBJECTS


		    //! Data pipe, used when generating derived content to extract data from an integration database.
		    //! The datapipe has a log directory, used for logging transactions on the pipe.
		    class datapipe
			    {

		      public:

				    typedef enum { time_config_group, twopf_kconfig_group, threepf_kconfig_group, time_serial_group, kconfig_serial_group } serial_group_tag;

				    typedef enum { cf_twopf_re, cf_twopf_im, cf_threepf } cf_data_type;

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

		        //! Extract a twopf component at fixed time for a set of k-configuration sample-points
		        typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&, twopf_type)> twopf_kconfig_callback;

		        //! Extract a threepf component at fixed time for a set of k-configuration sample-point
		        typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, unsigned int, std::vector<number>&)> threepf_kconfig_callback;

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
						    background_time_callback background;
						    twopf_time_callback      twopf;
						    threepf_time_callback    threepf;
					    };

				    class kslice_cache
					    {
				      public:
						    twopf_kconfig_callback   twopf;
						    threepf_kconfig_callback threepf;
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

								//! get priority
								constexpr unsigned int get_priority() const { return(0); }


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

				        //! get priority
				        constexpr unsigned int get_priority() const { return(0); }


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

				        //! get priority
				        constexpr unsigned int get_priority() const { return(0); }


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

				        //! get priority
				        virtual unsigned int get_priority() const = 0;

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

				        //! get priority
				        virtual unsigned int get_priority() const override { return(0); }


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

				        //! get priority
				        virtual unsigned int get_priority() const override { return(0); }


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

				        //! get priority
				        virtual unsigned int get_priority() const override { return(0); }


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

						    zeta_twopf_time_data_tag(datapipe* p, const twopf_configuration& k, integration_task<number>* t, unsigned int N)
							    : kdata(k), data_tag(p), tk(t), N_fields(N)
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
						    virtual std::string name() const override { std::ostringstream msg; msg << "zeta two-point function, kserial =  " << kdata.serial; return(msg.str()); };

				        //! get priority
				        virtual unsigned int get_priority() const override { return(1); }


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

		            zeta_threepf_time_data_tag(datapipe* p, const threepf_configuration& k, integration_task<number>* t, unsigned int N)
			            : kdata(k), data_tag(p), tk(t), N_fields(N)
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
		            virtual std::string name() const override { std::ostringstream msg; msg << "zeta three-point function, kserial =  " << kdata.serial; return(msg.str()); };

		            //! get priority
		            virtual unsigned int get_priority() const override { return(1); }


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

		            zeta_reduced_bispectrum_time_data_tag(datapipe* p, const threepf_configuration& k, integration_task<number>* t, unsigned int N)
			            : kdata(k), data_tag(p), tk(t), N_fields(N)
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
		            virtual std::string name() const override { std::ostringstream msg; msg << "zeta reduced bispectrum, kserial =  " << kdata.serial; return(msg.str()); };

		            //! get priority
		            virtual unsigned int get_priority() const override { return(1); }


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

						    zeta_twopf_kconfig_data_tag(datapipe* p, unsigned int ts, integration_task<number>* t, unsigned int N)
							    : tserial(ts), data_tag(p), tk(t), N_fields(N)
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
						    virtual std::string name() const override { std::ostringstream msg; msg << "zeta two-point function, tserial =  " << tserial; return(msg.str()); };;

				        //! get priority
				        virtual unsigned int get_priority() const override { return(1); }


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

		            zeta_threepf_kconfig_data_tag(datapipe* p, unsigned int ts, integration_task<number>* t, unsigned int N)
			            : tserial(ts), data_tag(p), tk(t), N_fields(N)
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
		            virtual std::string name() const override { std::ostringstream msg; msg << "zeta three-point function, tserial =  " << tserial; return(msg.str()); };;

		            //! get priority
		            virtual unsigned int get_priority() const override { return(1); }


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

		            zeta_reduced_bispectrum_kconfig_data_tag(datapipe* p, unsigned int ts, integration_task<number>* t, unsigned int N)
			            : tserial(ts), data_tag(p), tk(t), N_fields(N)
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
		            virtual std::string name() const override { std::ostringstream msg; msg << "zeta reduced bispectrum, tserial =  " << tserial; return(msg.str()); };;

		            //! get priority
		            virtual unsigned int get_priority() const override { return(1); }


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

		            //! tserial - controls which t serial number is sampled
		            const unsigned int tserial;

		            //! compute delegate
		            derived_data::zeta_kseries_compute<number> computer;

		            //! pointer to task
		            integration_task<number>* tk;

		            //! number of fields
		            unsigned int N_fields;

			        };


				    // CONSTRUCTOR, DESTRUCTOR

		      public:

            //! Construct a datapipe
            datapipe(unsigned int cap, const boost::filesystem::path& lp, const boost::filesystem::path& tp,
                     unsigned int w, boost::timer::cpu_timer& tm,
                     utility_callbacks& u, config_cache& cf, timeslice_cache& t, kslice_cache& k);

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


		        //! Get total time-config cache unloads
		        unsigned int get_time_config_cache_unloads() const { return(this->time_config_cache.get_unloads()); }

		        //! Get total twopf k-config cache unloads
		        unsigned int get_twopf_kconfig_cache_unloads() const { return(this->twopf_kconfig_cache.get_unloads()); }

		        //! Get total threepf k-config cache unloads
		        unsigned int get_threepf_kconfig_cache_unloads() const { return(this->threepf_kconfig_cache.get_unloads()); }

		        //! Get total data cache unloads
		        unsigned int get_data_cache_unloads() const { return(this->data_cache.get_unloads()); }


				    //! Get total eviction time for time-config cache
				    boost::timer::nanosecond_type get_time_config_cache_evictions() const { return(this->time_config_cache.get_eviction_timer()); }

				    //! Get total eviction time for twopf k-config cache unloads
				    boost::timer::nanosecond_type get_twopf_kconfig_cache_evictions() const { return(this->twopf_kconfig_cache.get_eviction_timer()); }

				    //! Get total eviction time for threepf k-config cache unloads
				    boost::timer::nanosecond_type get_threepf_kconfig_cache_evictions() const { return(this->threepf_kconfig_cache.get_eviction_timer()); }

				    //! Get total eviction time for data cache
				    boost::timer::nanosecond_type get_data_cache_evictions() const { return(this->data_cache.get_unloads()); }


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


				    // CACHES

		        //! time configuration cache
		        linecache::cache<std::vector<double>, time_config_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> time_config_cache;

		        //! twopf k-config cache
		        linecache::cache<std::vector<twopf_configuration>, twopf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> twopf_kconfig_cache;

		        //! threepf k-config cache
		        linecache::cache<std::vector<threepf_configuration>, threepf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> threepf_kconfig_cache;

		        //! data cache
		        linecache::cache<std::vector<number>, data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE> data_cache;


				    // CACHE TABLES

		        //! Time configuration cache table for currently-attached group; null if no group is attached
		        linecache::table<std::vector<number>, time_config_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* time_config_cache_table;

		        //! twopf k-config cache table for currently-attached group; null if no group is attached
		        linecache::table<std::vector<twopf_configuration>, twopf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* twopf_kconfig_cache_table;

		        //! threepf k-config cache table for currently-attached group; null if no group is attached
		        linecache::table<std::vector<threepf_configuration>, threepf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* threepf_kconfig_cache_table;

		        //! data cache table for currently-attached group; null if no group is attached
		        linecache::table<std::vector<number>, data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>* data_cache_table;


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

		        //! Timer, used to track how long the datapipe is kept open
		        boost::timer::cpu_timer& timer;


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
        virtual void initialize_writer(typename repository<number>::integration_writer& writer) = 0;

        //! Close an open container integration_writer object.
        virtual void close_writer(typename repository<number>::integration_writer& writer) = 0;

        //! Create data files for a new derived_content_writer object.
        virtual void initialize_writer(typename repository<number>::derived_content_writer& writer) = 0;

        //! Close an open derived_content_writer object.
        virtual void close_writer(typename repository<number>::derived_content_writer& writer) = 0;


        // WRITE INDEX TABLES FOR A DATA CONTAINER

      public:

        //! Create tables needed for a twopf container
        virtual void create_tables(typename repository<number>::integration_writer& writer, twopf_task<number>* tk) = 0;

        //! Create tables needed for a threepf container
        virtual void create_tables(typename repository<number>::integration_writer& writer, threepf_task<number>* tk) = 0;


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
                                                          unsigned int worker, model<number>* m,
                                                          container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual threepf_batcher create_temp_threepf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                              unsigned int worker, model<number>* m,
                                                              container_dispatch_function dispatcher) = 0;

        //! Aggregate a temporary twopf container into a principal container
        virtual void aggregate_twopf_batch(typename repository<number>::integration_writer& writer, const std::string& temp_ctr) = 0;

        //! Aggregate a temporary threepf container into a principal container
        virtual void aggregate_threepf_batch(typename repository<number>::integration_writer& writer, const std::string& temp_ctr) = 0;


		    // DATA PIPES AND DATA ACCESS

      public:

		    //! Create a datapipe
		    virtual datapipe create_datapipe(const boost::filesystem::path& logdir, const boost::filesystem::path& tempdir,
                                         typename datapipe::output_group_finder finder, typename datapipe::dispatch_function dispatcher,
		                                     unsigned int worker, boost::timer::cpu_timer& timer) = 0;

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

        //! Pull a kconfig sample of a twopf component at fixed time from a datapipe
        virtual void pull_twopf_kconfig_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& k_serials,
                                               unsigned int t_serial, std::vector<number>& sample, typename datapipe::twopf_type type) = 0;

        //! Pull a kconfig of a threepf at fixed time from a datapipe
        virtual void pull_threepf_kconfig_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& k_serials,
                                                 unsigned int t_serial, std::vector<number>& sample) = 0;

        // INTERNAL DATA

      protected:

        //! Maximum memory available to each worker process
        unsigned int capacity;

      };


    // GENERIC BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    data_manager<number>::generic_batcher::generic_batcher(unsigned int cap, unsigned int Nf,
                                                           const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                           typename data_manager<number>::container_dispatch_function d,
                                                           typename data_manager<number>::container_replacement_function r,
                                                           handle_type h, unsigned int w, bool s)
      : capacity(cap), Nfields(Nf), container_path(cp), logdir_path(lp),
        dispatcher(d), replacer(r), worker_number(w),
        manager_handle(static_cast<void*>(h)),
        num_integrations(0),
        integration_time(0),
        max_integration_time(0), min_integration_time(0),
        batching_time(0),
        max_batching_time(0), min_batching_time(0),
        collect_statistics(s), failures(0), refinements(0),
        mode(data_manager<number>::generic_batcher::flush_immediate),
        flush_due(false)
      {
        std::ostringstream log_file;
        log_file << __CPP_TRANSPORT_LOG_FILENAME_A << worker_number << __CPP_TRANSPORT_LOG_FILENAME_B;

        boost::filesystem::path log_path = logdir_path / log_file.str();

        boost::shared_ptr< boost::log::core > core = boost::log::core::get();

//		    core->set_filter(boost::log::trivial::severity >= data_manager<number>::normal);

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


    template <typename number>
    data_manager<number>::generic_batcher::~generic_batcher()
      {
        boost::shared_ptr< boost::log::core > core = boost::log::core::get();

        core->remove_sink(this->log_sink);
      }


    template <typename number>
    void data_manager<number>::generic_batcher::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching)
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
				    flush_due = false;
				    this->flush(action_replace);
			    }
      }


    template <typename number>
    void data_manager<number>::generic_batcher::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching,
    unsigned int kserial)
      {
        this->report_integration_success(integration, batching);

        configuration_statistics stats;
        stats.serial     = kserial;
        stats.integration = integration;
        stats.batching    = batching;

        this->stats_batch.push_back(stats);

		    if(this->flush_due)
			    {
				    flush_due = false;
				    this->flush(action_replace);
			    }
      }


    template <typename number>
    void data_manager<number>::generic_batcher::push_backg(unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values)
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
    void data_manager<number>::generic_batcher::report_integration_failure()
      {
        this->failures++;
        if(this->flush_due)
          {
            flush_due = false;
            this->flush(action_replace);
          }
      }


		template <typename number>
		void data_manager<number>::generic_batcher::report_refinement()
			{
				this->refinements++;
			}


    template <typename number>
    void data_manager<number>::generic_batcher::close()
      {
        this->flush(action_close);

        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "";
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "-- Closing batcher: final integration statistics";
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   processed " << this->num_integrations << " individual integrations";
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   total integration time          = " << format_time(this->integration_time);
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
				    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "-- " << this->refinements << " triangles required mesh refinement";
			    }
		    if(this->failures > 0)
			    {
				    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "-- " << this->failures << " triangles failed to integrate";
			    }
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


		template <typename number>
		template <typename handle_type>
		data_manager<number>::twopf_batcher::twopf_batcher(unsigned int cap, unsigned int Nf,
		                                                   const boost::filesystem::path& cp, const boost::filesystem::path& lp,
		                                                   const twopf_writer_group& w,
		                                                   container_dispatch_function d, container_replacement_function r,
		                                                   handle_type h, unsigned int wn, bool s)
			: generic_batcher(cap, Nf, cp, lp, d, r, h, wn, s), writers(w)
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
    size_t data_manager<number>::twopf_batcher::storage() const
	    {
		    return((sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->backg_batch.size()
	             + (2*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*this->twopf_batch.size());
			}


		template <typename number>
    void data_manager<number>::twopf_batcher::flush(replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing twopf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        if(this->collect_statistics) this->writers.stats(this, this->stats_batch);
        this->writers.backg(this, this->backg_batch);
        this->writers.twopf(this, this->twopf_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->stats_batch.clear();
        this->backg_batch.clear();
        this->twopf_batch.clear();

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
	    }


    template <typename number>
    template <typename handle_type>
    data_manager<number>::threepf_batcher::threepf_batcher(unsigned int cap, unsigned int Nf,
                                                           const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                           const threepf_writer_group& w,
                                                           container_dispatch_function d, container_replacement_function r,
                                                           handle_type h, unsigned int wn, bool s)
	    : generic_batcher(cap, Nf, cp, lp, d, r, h, wn, s), writers(w)
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
		size_t data_manager<number>::threepf_batcher::storage() const
			{ return((sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->backg_batch.size()
							 + (2*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*(this->twopf_re_batch.size() + this->twopf_im_batch.size())
							 + (2*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*2*this->Nfields*sizeof(number))*this->threepf_batch.size());
			}


		template <typename number>
    void data_manager<number>::threepf_batcher::flush(replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing threepf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        if(this->collect_statistics) this->writers.stats(this, this->stats_batch);
        this->writers.backg(this, this->backg_batch);
        this->writers.twopf_re(this, this->twopf_re_batch);
        this->writers.twopf_im(this, this->twopf_im_batch);
        this->writers.threepf(this, this->threepf_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->stats_batch.clear();
        this->backg_batch.clear();
        this->twopf_re_batch.clear();
        this->twopf_im_batch.clear();
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

		    this->threepf_batch.erase(std::remove_if(this->threepf_batch.begin(), this->threepf_batch.end(),
		                                             [ & ](const threepf_item& item) -> bool
		                                             {
		                                               return (item.source_serial == source_serial);
		                                             }),
		                              this->threepf_batch.end());
			}



    // DATAPIPE METHODS


    template <typename number>
    data_manager<number>::datapipe::datapipe(unsigned int cap, const boost::filesystem::path& lp, const boost::filesystem::path& tp,
                                             unsigned int w, boost::timer::cpu_timer& tm,
                                             typename data_manager<number>::datapipe::utility_callbacks& u,
                                             typename data_manager<number>::datapipe::config_cache& cf,
                                             typename data_manager<number>::datapipe::timeslice_cache& t,
                                             typename data_manager<number>::datapipe::kslice_cache& k)
	    : logdir_path(lp), temporary_path(tp), worker_number(w), timer(tm),
	      utilities(u), pull_config(cf), pull_timeslice(t), pull_kslice(k),
	      time_config_cache_table(nullptr),
	      twopf_kconfig_cache_table(nullptr),
	      threepf_kconfig_cache_table(nullptr),
	      data_cache_table(nullptr),
	      time_config_cache(__CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
	      twopf_kconfig_cache(__CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
	      threepf_kconfig_cache(__CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
	      data_cache(cap),
        attached_task(nullptr)
	    {
        this->database_timer.stop();

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
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "";
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   time-configuration evictions       = " << format_time(this->time_config_cache.get_eviction_timer());
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   twopf k-configuration evictions    = " << format_time(this->twopf_kconfig_cache.get_eviction_timer());
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   threepf k-configuration evictions  = " << format_time(this->threepf_kconfig_cache.get_eviction_timer());
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   data evictions                     = " << format_time(this->data_cache.get_eviction_timer());

        boost::shared_ptr<boost::log::core> core = boost::log::core::get();

        core->remove_sink(this->log_sink);

        // detach any attached output group, if necessary
        if(this->attached_group.get() != nullptr) this->detach();
	    }

 
    template <typename number>
    bool data_manager<number>::datapipe::validate_unattached(void) const
      {
        return(this->attached_group.get() == nullptr &&
               this->attached_task == nullptr &&
               this->time_config_cache_table == nullptr &&
               this->twopf_kconfig_cache_table == nullptr &&
               this->threepf_kconfig_cache_table == nullptr &&
               this->data_cache_table == nullptr);
      }
 
 
    template <typename number>
    bool data_manager<number>::datapipe::validate_attached(void) const
      {
        return(this->attached_group.get() != nullptr &&
               this->attached_task != nullptr &&
               this->time_config_cache_table != nullptr &&
               this->twopf_kconfig_cache_table != nullptr &&
               this->threepf_kconfig_cache_table != nullptr &&
               this->data_cache_table != nullptr);
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

        typename repository<number>::integration_payload& payload = this->attached_group.get()->get_payload();

        BOOST_LOG_SEV(this->get_log(), data_manager<number>::normal) << "** ATTACH output group " << boost::posix_time::to_simple_string(this->attached_group.get()->get_creation_time())
          << " (from task '" << tk->get_name() << "', generated using integration backend '" << payload.get_backend() << "')";

		     // attach new cache tables

		    this->time_config_cache_table = &(this->time_config_cache.get_table_handle(payload.get_container_path().string()));
		    this->twopf_kconfig_cache_table = &(this->twopf_kconfig_cache.get_table_handle(payload.get_container_path().string()));
		    this->threepf_kconfig_cache_table = &(this->threepf_kconfig_cache.get_table_handle(payload.get_container_path().string()));
		    this->data_cache_table = &(this->data_cache.get_table_handle(payload.get_container_path().string()));

		    // remember number of fields associated with this container
		    this->N_fields = Nf;
	    }


		template <typename number>
    void data_manager<number>::datapipe::detach(void)
	    {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_DETACH_PIPE_NOT_ATTACHED);

		    BOOST_LOG_SEV(this->get_log(), data_manager<number>::normal) << "** DETACH output group " << boost::posix_time::to_simple_string(this->attached_group.get()->get_creation_time());

				this->utilities.detach(this);
        this->attached_group.reset();
				this->attached_task = nullptr;

		    this->time_config_cache_table = nullptr;
		    this->twopf_kconfig_cache_table = nullptr;
		    this->threepf_kconfig_cache_table = nullptr;
		    this->data_cache_table = nullptr;
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
				return data_manager<number>::datapipe::zeta_twopf_time_data_tag(this, kdata, this->attached_task, this->N_fields);
			}


    template <typename number>
    typename data_manager<number>::datapipe::zeta_threepf_time_data_tag data_manager<number>::datapipe::new_zeta_threepf_time_data_tag(const threepf_configuration& kdata)
	    {
        return data_manager<number>::datapipe::zeta_threepf_time_data_tag(this, kdata, this->attached_task, this->N_fields);
	    }


    template <typename number>
    typename data_manager<number>::datapipe::zeta_reduced_bispectrum_time_data_tag data_manager<number>::datapipe::new_zeta_reduced_bispectrum_time_data_tag(const threepf_configuration& kdata)
	    {
        return data_manager<number>::datapipe::zeta_reduced_bispectrum_time_data_tag(this, kdata, this->attached_task, this->N_fields);
	    }


		template <typename number>
		typename data_manager<number>::datapipe::zeta_twopf_kconfig_data_tag data_manager<number>::datapipe::new_zeta_twopf_kconfig_data_tag(unsigned int tserial)
			{
				return data_manager<number>::datapipe::zeta_twopf_kconfig_data_tag(this, tserial, this->attached_task, this->N_fields);
			}


    template <typename number>
    typename data_manager<number>::datapipe::zeta_threepf_kconfig_data_tag data_manager<number>::datapipe::new_zeta_threepf_kconfig_data_tag(unsigned int tserial)
	    {
        return data_manager<number>::datapipe::zeta_threepf_kconfig_data_tag(this, tserial, this->attached_task, this->N_fields);
	    }


    template <typename number>
    typename data_manager<number>::datapipe::zeta_reduced_bispectrum_kconfig_data_tag data_manager<number>::datapipe::new_zeta_reduced_bispectrum_kconfig_data_tag(unsigned int tserial)
	    {
        return data_manager<number>::datapipe::zeta_reduced_bispectrum_kconfig_data_tag(this, tserial, this->attached_task, this->N_fields);
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
				else if (this->type == cf_threepf)
					{
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
				    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL threepf time sample request for element " << this->id << ", k-configuration " << this->kserial;
#endif

						this->pipe->database_timer.resume();
				    this->pipe->pull_timeslice.threepf(this->pipe, this->id, sns, this->kserial, sample);
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
        else if (this->type == cf_threepf)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL threepf kconfig sample request for element " << this->id << ", t-serial " << this->tserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.threepf(this->pipe, this->id, sns, this->tserial, sample);
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

		    // look up time values corresponding to these serial numbers
        time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
        time_config_tag tc_tag = this->pipe->new_time_config_tag();

        const std::vector<double>& time_values = tc_handle.lookup_tag(tc_tag);

        // set up handle for compute delegate
        std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields);
        this->computer.twopf(handle, sample, this->kdata);
	    }


    template <typename number>
    void data_manager<number>::datapipe::zeta_threepf_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta threepf time sample request, type = real, k-configuration " << this->kdata.kserial;
#endif

        // look up time values corresponding to these serial numbers
        time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
        time_config_tag tc_tag = this->pipe->new_time_config_tag();

        const std::vector<double>& time_values = tc_handle.lookup_tag(tc_tag);

        // set up handle for compute delegate
        std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields);
        this->computer.threepf(handle, sample, this->kdata);
	    }


    template <typename number>
    void data_manager<number>::datapipe::zeta_reduced_bispectrum_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta reduced bispectrum time sample request, type = real, k-configuration " << this->kdata.kserial;
#endif

        // look up time values corresponding to these serial numbers
        time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
        time_config_tag tc_tag = this->pipe->new_time_config_tag();

        const std::vector<double>& time_values = tc_handle.lookup_tag(tc_tag);

        // set up handle for compute delegate
        std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields);
        this->computer.reduced_bispectrum(handle, sample, this->kdata);
	    }


		template <typename number>
		void data_manager<number>::datapipe::zeta_twopf_kconfig_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
			{
				assert(this->pipe->validate_attached());
		    if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta twopf kconfig sample request, t-serial " << this->tserial;
#endif

				// pull information on this time-value from the database -- not efficient! but is there a better way?
		    std::vector<unsigned int> time_sn;
				time_sn.push_back(this->tserial);

				time_config_handle& tc_handle = this->pipe->new_time_config_handle(time_sn);
				time_config_tag tc_tag = this->pipe->new_time_config_tag();

				const std::vector<double>& time_values = tc_handle.lookup_tag(tc_tag);

				// set up handle for compute delegate
		    std::shared_ptr<typename derived_data::zeta_kseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_sn, time_values, this->N_fields);
				this->computer.twopf(handle, sample, 0);
			}


    template <typename number>
    void data_manager<number>::datapipe::zeta_threepf_kconfig_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta threepf kconfig sample request, t-serial " << this->tserial;
#endif

        // pull information on this time-value from the database -- as above, not efficient
        std::vector<unsigned int> time_sn;
        time_sn.push_back(this->tserial);

        time_config_handle& tc_handle = this->pipe->new_time_config_handle(time_sn);
        time_config_tag tc_tag = this->pipe->new_time_config_tag();

        const std::vector<double>& time_values = tc_handle.lookup_tag(tc_tag);

        // set up handle for compute delegate
        std::shared_ptr<typename derived_data::zeta_kseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_sn, time_values, this->N_fields);
        this->computer.threepf(handle, sample, 0);
	    }


    template <typename number>
    void data_manager<number>::datapipe::zeta_reduced_bispectrum_kconfig_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::datapipe_pull) << "** PULL zeta reduced bispectrum kconfig sample request, t-serial " << this->tserial;
#endif

        // pull information on this time-value from the database -- as above, not efficient
        std::vector<unsigned int> time_sn;
        time_sn.push_back(this->tserial);

        time_config_handle& tc_handle = this->pipe->new_time_config_handle(time_sn);
        time_config_tag tc_tag = this->pipe->new_time_config_tag();

        const std::vector<double>& time_values = tc_handle.lookup_tag(tc_tag);

        // set up handle for compute delegate
        std::shared_ptr<typename derived_data::zeta_kseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_sn, time_values, this->N_fields);
        this->computer.reduced_bispectrum(handle, sample, 0);
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
