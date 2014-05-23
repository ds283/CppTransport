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
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"


// log file name
#define __CPP_TRANSPORT_LOG_FILENAME_A  "worker_"
#define __CPP_TRANSPORT_LOG_FILENAME_B  "_%3N.log"


// default size of line cache hash table
#define __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE (1024)

// default size of the k-configuration caches - 1 Mb
#define __CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE (1*1024*1024)


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


		    // DATAPIPE OBJECTS

		    typedef enum { twopf_real, twopf_imag } datapipe_twopf_type;

        //! Output-group finder function -- serivce provided by a repository implementation
        typedef std::function<typename repository<number>::template output_group< typename repository<number>::integration_payload >(integration_task<number>*, const std::list<std::string>&)>                                  output_group_finder;

        //! Attach function for a datapipe
        typedef std::function<typename repository<number>::template output_group< typename repository<number>::integration_payload >(datapipe*, output_group_finder&, integration_task<number>*, const std::list<std::string>&)> datapipe_attach_function;

        //! Detach function for a datapipe
        typedef std::function<void(datapipe*)>                                                                                                                                                                                   datapipe_detach_function;

        //! Push derived content
        typedef std::function<void(datapipe*)>                                                                                                                                                                                   datapipe_content_dispatch_function;

        //! Extract a set of time sample-points from a datapipe
        typedef std::function<void(datapipe*, const std::vector<unsigned int>&, std::vector<double>&, unsigned int)>                                                                                                             datapipe_time_sample_function;

        //! Extract a set of 2pf k-configuration sample points from a datapipe
        typedef std::function<void(datapipe*, const std::vector<unsigned int>&, std::vector<twopf_configuration>&, unsigned int)>                                                                                                datapipe_twopf_kconfig_sample_function;

        //! Extract a set of 3pf k-configuration sample points from a datapipe
        typedef std::function<void(datapipe*, const std::vector<unsigned int>&, std::vector<threepf_configuration>&, unsigned int)>                                                                                              datapipe_threepf_kconfig_sample_function;

        //! Extract a background field at a set of time sample-points
        typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, std::vector<number>&, unsigned int)>                                                                                               datapipe_background_time_sample_function;

        //! Extract a twopf component at fixed k-configuration for a set of time sample-points
        typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, unsigned int,
                                   std::vector<number>&, datapipe_twopf_type, unsigned int)>                                                                                                                                     datapipe_twopf_time_sample_function;

        //! Extract a threepf component at fixed k-configuration for a set of time sample-point
        typedef std::function<void(datapipe*, unsigned int, const std::vector<unsigned int>&, unsigned int,
                                   std::vector<number>&, unsigned int)>                                                                                                                                                          datapipe_threepf_time_sample_function;


		    //! Data pipe, used when generating derived content to extract data from an integration database.
		    //! The datapipe has a log directory, used for logging transactions on the pipe.
		    class datapipe
			    {

		      public:

				    typedef enum { time_config_group, twopf_kconfig_group, threepf_kconfig_group, time_serial_group } serial_group_tag;

				    typedef enum { cf_twopf_re, cf_twopf_im, cf_threepf } cf_time_data_type;


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


				    //! time data group tag -- abstract group used to derive background and field tags
				    class time_data_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

						    time_data_tag() = default;

						    virtual ~time_data_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    virtual bool operator==(const time_data_tag& obj) const = 0;

						    virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) = 0;


				        // CLONE

				      public:

				        //! copy this object
				        virtual time_data_tag* clone() const = 0;


				        // HASH

				      public:

				        //! hash
				        virtual unsigned int hash() const = 0;

					    };


				    //! background time data group tag
				    class background_time_data_tag: public time_data_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

				        background_time_data_tag(datapipe* p, unsigned int i)
					        : pipe(p), id(i)
					        {
					        }

						    virtual ~background_time_data_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    virtual bool operator==(const time_data_tag& obj) const override;

						    //! pull data corresponding to this tag
						    virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;


				        // CLONE

				      public:

				        //! copy this object
				        time_data_tag* clone() const { return new background_time_data_tag(static_cast<const background_time_data_tag&>(*this)); }


				        // HASH

				      public:

				        //! hash
				        virtual unsigned int hash() const override { return(id % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


						    // INTERNAL DATA

				      protected:

						    //! parent datapipe
						    datapipe* pipe;

						    //! data id - controls which background field is sampled
						    const unsigned int id;

					    };


				    //! correlation-function time data group tag
				    class cf_time_data_tag: public time_data_tag
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

						    cf_time_data_tag(datapipe* p, cf_time_data_type t, unsigned int i, unsigned int k)
						      : pipe(p), type(t), id(i), kserial(k)
							    {
							    }

						    virtual ~cf_time_data_tag() = default;


						    // INTERFACE

				      public:

						    //! check for tag equality
						    virtual bool operator==(const time_data_tag& obj) const override;

						    //! pull data corresponding to this tag
						    virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;


				        // CLONE

				      public:

				        //! copy this object
				        time_data_tag* clone() const { return new cf_time_data_tag(static_cast<const cf_time_data_tag&>(*this)); }


				        // HASH

				      public:

				        //! hash
				        virtual unsigned int hash() const override { return((kserial*15485863 + id) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


						    // INTERNAL DATA

				      protected:

						    //! parent datapipe
						    datapipe* pipe;

						    //! type
						    const cf_time_data_type type;

						    //! data id - controls which element id is sampled
						    const unsigned int id;

						    //! kserial - controls which k serial number is sampled
						    const unsigned int kserial;
					    };


				    // CONSTRUCTOR, DESTRUCTOR

		      public:

            //! Construct a datapipe
            datapipe(unsigned int cap,
                     const boost::filesystem::path& lp, const boost::filesystem::path& tp,
                     unsigned int w, boost::timer::cpu_timer& tm,
                     output_group_finder& fd,
                     datapipe_attach_function& at, datapipe_detach_function& dt,
                     datapipe_content_dispatch_function& df,
                     datapipe_time_sample_function& tsf,
                     datapipe_twopf_kconfig_sample_function& twopf_kcfg_sf,
                     datapipe_threepf_kconfig_sample_function& threepf_kcfg_sf,
                     datapipe_background_time_sample_function& btsf,
                     datapipe_twopf_time_sample_function& twopf_tsf,
                     datapipe_threepf_time_sample_function& threepf_tsf);

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

				    //! Get total time spent reading database
				    const boost::timer::nanosecond_type get_database_time() const { return(this->database_timer.elapsed().wall); }

				    //! Get total time-config cache hits
				    unsigned int get_time_config_cache_hits() const { return(this->time_config_cache.get_hits()); }

				    //! Get total twopf k-config cache hits
				    unsigned int get_twopf_kconfig_cache_hits() const { return(this->twopf_kconfig_cache.get_hits()); }

				    //! Get total threepf k-config cache hits
				    unsigned int get_threepf_kconfig_cache_hits() const { return(this->threepf_kconfig_cache.get_hits()); }

				    //! Get total time-data cache hits
				    unsigned int get_time_data_cache_hits() const { return(this->time_data_cache.get_hits()); }


				    // ATTACH, DETACH OUTPUT GROUPS

		      public:

				    //! Attach an output-group to the datapipe, ready for reading
				    void attach(integration_task<number>* tk, const std::list<std::string>& tags);

				    //! Detach an output-group from the datapipe
				    void detach(void);

            //! Is this datapipe attached to an output group?
            bool is_attached() const { return(this->attached_group != nullptr); }

            //!

				    //! Get attached output group
				    typename repository<number>::template output_group< typename repository<number>::integration_payload >*
              get_attached_output_group(void) const;


				    // PULL DATA

		      public:

				    typedef linecache::serial_group< std::vector<double>, time_config_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_config_handle;
				    typedef linecache::serial_group< std::vector<twopf_configuration>, twopf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > twopf_kconfig_handle;
				    typedef linecache::serial_group< std::vector<threepf_configuration>, threepf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > threepf_kconfig_handle;
				    typedef linecache::serial_group< std::vector<number>, time_data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE > time_data_handle;

				    //! Generate a serial-group handle for a set of time-configuration serial numbers
				    time_config_handle& new_time_config_handle(const std::vector<unsigned int>& sns) const;

				    //! Generate a serial-group handle for a set of twopf-kconfiguration serial numbers
				    twopf_kconfig_handle& new_twopf_kconfig_handle(const std::vector<unsigned int>& sns) const;

				    //! Generate a serial-group handle for a set of threepf-kconfiguration serial numbers
				    threepf_kconfig_handle& new_threepf_kconfig_handle(const std::vector<unsigned int>& sns) const;

				    //! Generate a serial-group handle for a set of time-data serial numbers
				    time_data_handle& new_time_data_handle(const std::vector<unsigned int>& sns) const;

				    //! Generate a new time-configuration tag
				    time_config_tag new_time_config_tag();

				    //! Generate a new twopf-kconfiguration tag
				    twopf_kconfig_tag new_twopf_kconfig_tag();

				    //! Generate a new threepf-kconfiguration tag
				    threepf_kconfig_tag new_threepf_kconfig_tag();

				    //! Generate a new background tag
				    background_time_data_tag new_background_time_data_tag(unsigned int id);

				    //! Generate a new correlation-function tag
				    cf_time_data_tag new_cf_time_data_tag(cf_time_data_type type, unsigned int id, unsigned int kserial);

				    // friend sample tag classes, so they can use our data
				    friend class time_config_tag;
				    friend class twopf_kconfig_tag;
				    friend class threepf_kconfig_tag;
				    friend class background_time_data_tag;
				    friend class cf_time_data_tag;


				    // INTERNAL DATA

		      private:

				    //! time configuration cache
				    linecache::cache< std::vector<double>, time_config_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE >                     time_config_cache;

				    //! twopf k-config cache
				    linecache::cache< std::vector<twopf_configuration>, twopf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE >      twopf_kconfig_cache;

				    //! threepf k-config cache
				    linecache::cache< std::vector<threepf_configuration>, threepf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE >  threepf_kconfig_cache;

				    //! time-line data cache
				    linecache::cache< std::vector<number>, time_data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE>                        time_data_cache;

				    //! Maximum capacity to use (approximately--we don't try to do a detailed accounting of memory use)
				    unsigned int                                                                                                                              capacity;

				    //! Path to logging directory
				    const boost::filesystem::path                                                                                                             logdir_path;

				    //! Path to temporary files
				    const boost::filesystem::path                                                                                                             temporary_path;

						//! Unique serial number identifying the worker process owning this datapipe
				    const unsigned int                                                                                                                        worker_number;

		        //! Logger source
		        boost::log::sources::severity_logger<log_severity_level>                                                                                  log_source;

		        //! Logger sink
		        boost::shared_ptr< sink_t >                                                                                                               log_sink;

				    //! Timer, used to track how long the datapipe is kept open
				    boost::timer::cpu_timer&                                                                                                                  timer;

				    //! Currently-attached output group; null if no group is attached
				    typename repository<number>::template output_group< typename repository<number>::integration_payload >*                                   attached_group;

		        //! Time configuration cache table for currently-attached group; null if no group is attached
				    linecache::table< std::vector<number>, time_config_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE >*                    time_config_cache_table;

				    //! twopf k-config cache table for currently-attached group; null if no group is attached
				    linecache::table< std::vector<twopf_configuration>, twopf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE >*     twopf_kconfig_cache_table;

				    //! threepf k-config cache table for currently-attached group; null if no group is attached
				    linecache::table< std::vector<threepf_configuration>, threepf_kconfig_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE >* threepf_kconfig_cache_table;

				    //! time data cache table for currently-attached group; null if no group is attached
				    linecache::table< std::vector<number>, time_data_tag, serial_group_tag, __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE >*                      time_data_cache_table;

				    //! Implementation-dependent handle
				    void*                                                                                                                                     manager_handle;

				    //! Database access timer
				    boost::timer::cpu_timer                                                                                                                   database_timer;

				    // CALLBACKS

            //! Callback: find an output group for a task
            output_group_finder                                                                                                                       output_finder;

				    //! Callback: attach a datapipe
				    datapipe_attach_function                                                                                                                  attach_callback;

				    //! Callback: detach a datapipe
				    datapipe_detach_function                                                                                                                  detach_callback;

            //! Callback: dispatch content to master process
            datapipe_content_dispatch_function                                                                                                        dispatch_callback;

				    //! Callback: extract a time sample
				    datapipe_time_sample_function                                                                                                             time_sample_callback;

				    //! Callback: extract a 2pf k-configuration sample
				    datapipe_twopf_kconfig_sample_function                                                                                                    twopf_kconfig_sample_callback;

				    //! Callback: extract a 3pf k-configuration sample
				    datapipe_threepf_kconfig_sample_function                                                                                                  threepf_kconfig_sample_callback;

				    //! Callback: extract a time sample of a background field
				    datapipe_background_time_sample_function                                                                                                  background_time_sample_callback;

				    //! Callback: extract a time sample of a component of a 2pf at fixed k-configuration
				    datapipe_twopf_time_sample_function                                                                                                       twopf_time_sample_callback;

				    //! Callback: extract a time sample of a component of a 3pf at fixed k-configuration
				    datapipe_threepf_time_sample_function                                                                                                     threepf_time_sample_callback;
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
                                         output_group_finder finder, derived_content_dispatch_function dispatcher,
		                                     unsigned int worker, boost::timer::cpu_timer& timer) = 0;

        //! Pull a set of time sample-points from a datapipe
        virtual void pull_time_sample(datapipe* pipe, const std::vector<unsigned int>& serial_numbers, std::vector<double>& sample,
                                      unsigned int worker) = 0;

        //! Pull a set of 2pf k-configuration serial numbers from a datapipe
        virtual void pull_twopf_kconfig_sample(datapipe* pipe, const std::vector<unsigned int>& serial_numbers, std::vector<twopf_configuration>& sample,
                                               unsigned int worker) = 0;

        //! Pull a set of 3pd k-configuration serial numbesr from a datapipe
        //! Simultaneously, populates three lists (k1, k2, k3) with serial numbers for the 2pf k-configurations
        //! corresponding to k1, k2, k3
        virtual void pull_threepf_kconfig_sample(datapipe* pipe, const std::vector<unsigned int>& serial_numbers, std::vector<threepf_configuration>& sample,
                                                 unsigned int worker) = 0;

        //! Pull a time sample of a background field from a datapipe
        virtual void pull_background_time_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& t_serials, std::vector<number>& sample,
                                                 unsigned int worker) = 0;

        //! Pull a time sample of a twopf component at fixed k-configuration from a datapipe
        virtual void pull_twopf_time_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& t_serials,
                                            unsigned int k_serial, std::vector<number>& sample, datapipe_twopf_type type,
                                            unsigned int worker) = 0;

        //! Pull a sample of a threepf at fixed k-configuration from a datapipe
        virtual void pull_threepf_time_sample(datapipe* pipe, unsigned int id, const std::vector<unsigned int>& t_serials,
                                              unsigned int kserial, std::vector<number>& sample,
                                              unsigned int worker) = 0;

        // INTERNAL DATA

      protected:

        //! Maximum memory available to each worker process
        unsigned int capacity;
      };


		// DATAPIPE METHODS



    template <typename number>
    data_manager<number>::datapipe::datapipe(unsigned int cap, const boost::filesystem::path& lp, const boost::filesystem::path& tp,
                                             unsigned int w, boost::timer::cpu_timer& tm,
                                             output_group_finder& fd,
                                             datapipe_attach_function& at, datapipe_detach_function& dt,
                                             datapipe_content_dispatch_function& df,
                                             datapipe_time_sample_function& tsf,
                                             datapipe_twopf_kconfig_sample_function& twopf_kcfg_sf,
                                             datapipe_threepf_kconfig_sample_function& threepf_kcfg_sf,
                                             datapipe_background_time_sample_function& btsf,
                                             datapipe_twopf_time_sample_function& twopf_tsf,
                                             datapipe_threepf_time_sample_function& threepf_tsf)
	    : logdir_path(lp), temporary_path(tp), worker_number(w), timer(tm),
        output_finder(fd),
	      attach_callback(at), detach_callback(dt),
        dispatch_callback(df),
        time_sample_callback(tsf),
	      background_time_sample_callback(btsf),
	      twopf_kconfig_sample_callback(twopf_kcfg_sf),
	      threepf_kconfig_sample_callback(threepf_kcfg_sf),
	      twopf_time_sample_callback(twopf_tsf),
	      threepf_time_sample_callback(threepf_tsf),
	      attached_group(nullptr),
	      time_config_cache_table(nullptr),
	      twopf_kconfig_cache_table(nullptr),
				threepf_kconfig_cache_table(nullptr),
				time_data_cache_table(nullptr),
        time_config_cache(__CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
        twopf_kconfig_cache(__CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
        threepf_kconfig_cache(__CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
        time_data_cache(__CPP_TRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE)
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
		    assert(this->database_timer.is_stopped());

		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "";
        BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "-- Closing datapipe. Usage statistics:";
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   time spent reading database        = " << format_time(this->database_timer.elapsed().wall);
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   time-configuration cache hits      = " << this->time_config_cache.get_hits();
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   twopf k-configuration cache hits   = " << this->twopf_kconfig_cache.get_hits();
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   threepf k-configuration cache hits = " << this->threepf_kconfig_cache.get_hits();
		    BOOST_LOG_SEV(this->log_source, data_manager<number>::normal) << "--   time-data cache hits:              = " << this->time_data_cache.get_hits();

        boost::shared_ptr<boost::log::core> core = boost::log::core::get();

        core->remove_sink(this->log_sink);

        // detach any attached output group, if necessary
        if(this->attached_group != nullptr) this->detach();
	    }


    template <typename number>
    void data_manager<number>::datapipe::attach(integration_task<number>* tk, const std::list<std::string>& tags)
	    {
        assert(tk != nullptr);
        if(tk == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NULL_TASK);

        assert(this->attached_group == nullptr
	               &&this->time_config_cache_table == nullptr
	               && this->twopf_kconfig_cache_table == nullptr
	               && this->threepf_kconfig_cache_table == nullptr
	               && this->time_data_cache_table == nullptr);
        if(this->attached_group != nullptr ||
	        this->time_config_cache_table != nullptr ||
	        this->twopf_kconfig_cache_table != nullptr ||
	        this->threepf_kconfig_cache_table != nullptr ||
	        this->time_data_cache_table != nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_ATTACH_PIPE_ALREADY_ATTACHED);

        this->attached_group = new typename repository<number>::template output_group< typename repository<number>::integration_payload >(this->attach_callback(this, this->output_finder, tk, tags));

        typename repository<number>::integration_payload& payload = this->attached_group->get_payload();

        BOOST_LOG_SEV(this->get_log(), data_manager<number>::normal) << "** DATAPIPE ATTACH output group " << boost::posix_time::to_simple_string(this->attached_group->get_creation_time())
          << " (generated using integration backend '" << payload.get_backend() << "')";

		     // attach new cache tables
		    this->time_config_cache_table = &(this->time_config_cache.get_table_handle(payload.get_container_path().string()));
		    this->twopf_kconfig_cache_table = &(this->twopf_kconfig_cache.get_table_handle(payload.get_container_path().string()));
		    this->threepf_kconfig_cache_table = &(this->threepf_kconfig_cache.get_table_handle(payload.get_container_path().string()));
		    this->time_data_cache_table = &(this->time_data_cache.get_table_handle(payload.get_container_path().string()));
	    }


		template <typename number>
    void data_manager<number>::datapipe::detach(void)
	    {
		    assert(this->attached_group != nullptr
		           &&this->time_config_cache_table != nullptr
		           && this->twopf_kconfig_cache_table != nullptr
		           && this->threepf_kconfig_cache_table != nullptr
		           && this->time_data_cache_table != nullptr);
		    if(this->attached_group == nullptr ||
		       this->time_config_cache_table == nullptr ||
		       this->twopf_kconfig_cache_table == nullptr ||
		       this->threepf_kconfig_cache_table == nullptr ||
		       this->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_DETACH_PIPE_NOT_ATTACHED);

		    BOOST_LOG_SEV(this->get_log(), data_manager<number>::normal) << "** DATAPIPE DETACH output group " << boost::posix_time::to_simple_string(this->attached_group->get_creation_time());

				this->detach_callback(this);

		    delete this->attached_group;

		    this->attached_group = nullptr;
		    this->time_config_cache_table = nullptr;
		    this->twopf_kconfig_cache_table = nullptr;
		    this->threepf_kconfig_cache_table = nullptr;
		    this->time_data_cache_table = nullptr;
	    }


    template <typename number>
    typename repository<number>::template output_group< typename repository<number>::integration_payload >* data_manager<number>::datapipe::get_attached_output_group(void) const
	    {
        assert(this->attached_group != nullptr
	               &&this->time_config_cache_table != nullptr
	               && this->twopf_kconfig_cache_table != nullptr
	               && this->threepf_kconfig_cache_table != nullptr
	               && this->time_data_cache_table != nullptr);
        if(this->attached_group == nullptr ||
	        this->time_config_cache_table == nullptr ||
	        this->twopf_kconfig_cache_table == nullptr ||
	        this->threepf_kconfig_cache_table == nullptr ||
	        this->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

		    return(this->attached_group);
	    }


		template <typename number>
		typename data_manager<number>::datapipe::time_config_handle& data_manager<number>::datapipe::new_time_config_handle(const std::vector<unsigned int>& sns) const
			{
		    assert(this->attached_group != nullptr
			           &&this->time_config_cache_table != nullptr
			           && this->twopf_kconfig_cache_table != nullptr
			           && this->threepf_kconfig_cache_table != nullptr
			           && this->time_data_cache_table != nullptr);
		    if(this->attached_group == nullptr ||
			    this->time_config_cache_table == nullptr ||
			    this->twopf_kconfig_cache_table == nullptr ||
			    this->threepf_kconfig_cache_table == nullptr ||
			    this->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

				return(this->time_config_cache_table->get_serial_handle(sns, time_config_group));
			}


    template <typename number>
    typename data_manager<number>::datapipe::twopf_kconfig_handle& data_manager<number>::datapipe::new_twopf_kconfig_handle(const std::vector<unsigned int>& sns) const
	    {
        assert(this->attached_group != nullptr
	               &&this->time_config_cache_table != nullptr
	               && this->twopf_kconfig_cache_table != nullptr
	               && this->threepf_kconfig_cache_table != nullptr
	               && this->time_data_cache_table != nullptr);
        if(this->attached_group == nullptr ||
	        this->time_config_cache_table == nullptr ||
	        this->twopf_kconfig_cache_table == nullptr ||
	        this->threepf_kconfig_cache_table == nullptr ||
	        this->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->twopf_kconfig_cache_table->get_serial_handle(sns, twopf_kconfig_group));
	    }


    template <typename number>
    typename data_manager<number>::datapipe::threepf_kconfig_handle& data_manager<number>::datapipe::new_threepf_kconfig_handle(const std::vector<unsigned int>& sns) const
	    {
        assert(this->attached_group != nullptr
	               &&this->time_config_cache_table != nullptr
	               && this->twopf_kconfig_cache_table != nullptr
	               && this->threepf_kconfig_cache_table != nullptr
	               && this->time_data_cache_table != nullptr);
        if(this->attached_group == nullptr ||
	        this->time_config_cache_table == nullptr ||
	        this->twopf_kconfig_cache_table == nullptr ||
	        this->threepf_kconfig_cache_table == nullptr ||
	        this->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->threepf_kconfig_cache_table->get_serial_handle(sns, threepf_kconfig_group));
	    }


    template <typename number>
    typename data_manager<number>::datapipe::time_data_handle& data_manager<number>::datapipe::new_time_data_handle(const std::vector<unsigned int>& sns) const
	    {
        assert(this->attached_group != nullptr
	               &&this->time_config_cache_table != nullptr
	               && this->twopf_kconfig_cache_table != nullptr
	               && this->threepf_kconfig_cache_table != nullptr
	               && this->time_data_cache_table != nullptr);
        if(this->attached_group == nullptr ||
	        this->time_config_cache_table == nullptr ||
	        this->twopf_kconfig_cache_table == nullptr ||
	        this->threepf_kconfig_cache_table == nullptr ||
	        this->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->time_data_cache_table->get_serial_handle(sns, time_serial_group));
	    }


		template <typename number>
		typename data_manager<number>::datapipe::time_config_tag data_manager<number>::datapipe::new_time_config_tag()
			{
				time_config_tag tag(this);
				return(tag);
			}


    template <typename number>
    typename data_manager<number>::datapipe::twopf_kconfig_tag data_manager<number>::datapipe::new_twopf_kconfig_tag()
	    {
		    twopf_kconfig_tag tag(this);
        return(tag);
	    }


    template <typename number>
    typename data_manager<number>::datapipe::threepf_kconfig_tag data_manager<number>::datapipe::new_threepf_kconfig_tag()
	    {
		    threepf_kconfig_tag tag(this);
        return(tag);
	    }


		template <typename number>
		typename data_manager<number>::datapipe::background_time_data_tag data_manager<number>::datapipe::new_background_time_data_tag(unsigned int id)
			{
				background_time_data_tag tag(this, id);
				return(tag);
			}


		template <typename number>
		typename data_manager<number>::datapipe::cf_time_data_tag data_manager<number>::datapipe::new_cf_time_data_tag(cf_time_data_type type, unsigned int id, unsigned int kserial)
			{
				cf_time_data_tag tag(this, type, id, kserial);
				return(tag);
			}


	  template <typename number>
		bool data_manager<number>::datapipe::validate(void)
		  {
			  return(this->attached_group != nullptr);
		  }


		template <typename number>
		void data_manager<number>::datapipe::time_config_tag::pull(const std::vector<unsigned int>& sns, std::vector<double>& data)
			{
		    assert(this->pipe->attached_group != nullptr
		           &&this->pipe->time_config_cache_table != nullptr
		           && this->pipe->twopf_kconfig_cache_table != nullptr
		           && this->pipe->threepf_kconfig_cache_table != nullptr
		           && this->pipe->time_data_cache_table != nullptr);
		    if(this->pipe->attached_group == nullptr ||
		       this->pipe->time_config_cache_table == nullptr ||
		       this->pipe->twopf_kconfig_cache_table == nullptr ||
		       this->pipe->threepf_kconfig_cache_table == nullptr ||
		       this->pipe->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << "** DATAPIPE pull time sample request";

				this->pipe->database_timer.resume();
		    this->pipe->time_sample_callback(this->pipe, sns, data, this->pipe->worker_number);
		    this->pipe->database_timer.stop();
			}


		template <typename number>
		void data_manager<number>::datapipe::twopf_kconfig_tag::pull(const std::vector<unsigned int>& sns, std::vector<twopf_configuration>& data)
			{
		    assert(this->pipe->attached_group != nullptr
		           &&this->pipe->time_config_cache_table != nullptr
		           && this->pipe->twopf_kconfig_cache_table != nullptr
		           && this->pipe->threepf_kconfig_cache_table != nullptr
		           && this->pipe->time_data_cache_table != nullptr);
		    if(this->pipe->attached_group == nullptr ||
		       this->pipe->time_config_cache_table == nullptr ||
		       this->pipe->twopf_kconfig_cache_table == nullptr ||
		       this->pipe->threepf_kconfig_cache_table == nullptr ||
		       this->pipe->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << "** DATAPIPE pull 2pf k-configuration sample request";

				this->pipe->database_timer.resume();
		    this->pipe->twopf_kconfig_sample_callback(this->pipe, sns, data, this->pipe->worker_number);
		    this->pipe->database_timer.stop();
			}


		template <typename number>
		void data_manager<number>::datapipe::threepf_kconfig_tag::pull(const std::vector<unsigned int>& sns, std::vector<threepf_configuration>& data)
			{
		    assert(this->pipe->attached_group != nullptr
		           &&this->pipe->time_config_cache_table != nullptr
		           && this->pipe->twopf_kconfig_cache_table != nullptr
		           && this->pipe->threepf_kconfig_cache_table != nullptr
		           && this->pipe->time_data_cache_table != nullptr);
		    if(this->pipe->attached_group == nullptr ||
		       this->pipe->time_config_cache_table == nullptr ||
		       this->pipe->twopf_kconfig_cache_table == nullptr ||
		       this->pipe->threepf_kconfig_cache_table == nullptr ||
		       this->pipe->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << "** DATAPIPE pull 3pf k-configuration sample request";

				this->pipe->database_timer.resume();
		    this->pipe->threepf_kconfig_sample_callback(this->pipe, sns, data, this->pipe->worker_number);
		    this->pipe->database_timer.stop();
			}


		template <typename number>
		void data_manager<number>::datapipe::background_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
			{
		    assert(this->pipe->attached_group != nullptr
		           &&this->pipe->time_config_cache_table != nullptr
		           && this->pipe->twopf_kconfig_cache_table != nullptr
		           && this->pipe->threepf_kconfig_cache_table != nullptr
		           && this->pipe->time_data_cache_table != nullptr);
		    if(this->pipe->attached_group == nullptr ||
		       this->pipe->time_config_cache_table == nullptr ||
		       this->pipe->twopf_kconfig_cache_table == nullptr ||
		       this->pipe->threepf_kconfig_cache_table == nullptr ||
		       this->pipe->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

		    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << "** DATAPIPE pull background time sample request for element " << this->id;

				this->pipe->database_timer.resume();
		    this->pipe->background_time_sample_callback(this->pipe, this->id, sns, sample, this->pipe->worker_number);
		    this->pipe->database_timer.stop();
			}


		template <typename number>
		void data_manager<number>::datapipe::cf_time_data_tag::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
			{
		    assert(this->pipe->attached_group != nullptr
			           &&this->pipe->time_config_cache_table != nullptr
			           && this->pipe->twopf_kconfig_cache_table != nullptr
			           && this->pipe->threepf_kconfig_cache_table != nullptr
			           && this->pipe->time_data_cache_table != nullptr);
		    if(this->pipe->attached_group == nullptr ||
			    this->pipe->time_config_cache_table == nullptr ||
			    this->pipe->twopf_kconfig_cache_table == nullptr ||
			    this->pipe->threepf_kconfig_cache_table == nullptr ||
			    this->pipe->time_data_cache_table == nullptr) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

				if(this->type == cf_twopf_re)
					{
				    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << "** DATAPIPE pull twopf time sample request, type = real, for element " << this->id << ", k-configuration " << this->kserial;

						this->pipe->database_timer.resume();
				    this->pipe->twopf_time_sample_callback(this->pipe, this->id, sns, this->kserial, sample, twopf_real, this->pipe->worker_number);
						this->pipe->database_timer.stop();
					}
				else if(this->type == cf_twopf_im)
					{
				    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << "** DATAPIPE pull twopf time sample request, type = imaginary, for element " << this->id << ", k-configuration " << this->kserial;

						this->pipe->database_timer.resume();
				    this->pipe->twopf_time_sample_callback(this->pipe, this->id, sns, this->kserial, sample, twopf_imag, this->pipe->worker_number);
						this->pipe->database_timer.stop();
					}
				else if (this->type == cf_threepf)
					{
				    BOOST_LOG_SEV(this->pipe->get_log(), data_manager<number>::normal) << "** DATAPIPE pull threepf time sample request for element " << this->id << ", k-configuration " << this->kserial;

						this->pipe->database_timer.resume();
				    this->pipe->threepf_time_sample_callback(this->pipe, this->id, sns, this->kserial, sample, this->pipe->worker_number);
						this->pipe->database_timer.stop();
					}
				else
					{
						assert(false);
						throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_UNKNOWN_CF_TYPE);
					}
			}


		template <typename number>
		bool data_manager<number>::datapipe::background_time_data_tag::operator==(const time_data_tag& obj) const
			{
				const background_time_data_tag* bg_tag = dynamic_cast<const background_time_data_tag*>(&obj);

				if(bg_tag == nullptr) return(false);
				return(this->id == bg_tag->id);
			}


		template <typename number>
		bool data_manager<number>::datapipe::cf_time_data_tag::operator==(const time_data_tag& obj) const
			{
				const cf_time_data_tag* cf_tag = dynamic_cast<const cf_time_data_tag*>(&obj);

				if(cf_tag == nullptr) return(false);
				return(this->id == cf_tag->id && this->kserial == cf_tag->kserial);
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

	    }   // namespace linecache -- specializaitons


  }   // namespace transport



#endif //__data_manager_H_
