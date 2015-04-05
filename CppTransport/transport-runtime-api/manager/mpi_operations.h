//
// Created by David Seery on 22/02/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __mpi_operations_H_
#define __mpi_operations_H_

#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/list.hpp"
#include "boost/date_time/posix_time/time_serialize.hpp"
#include "boost/timer/timer.hpp"


namespace transport
  {

    namespace MPI
      {

        // MPI messages
        namespace
          {

            // MPI message tags
            const unsigned int NEW_INTEGRATION            = 0;
            const unsigned int FINISHED_INTEGRATION       = 1;
            const unsigned int INTEGRATION_DATA_READY     = 2;
		        const unsigned int INTEGRATION_FAIL           = 9;

		        const unsigned int NEW_DERIVED_CONTENT        = 10;
		        const unsigned int DERIVED_CONTENT_READY      = 11;
		        const unsigned int FINISHED_DERIVED_CONTENT   = 12;
		        const unsigned int DERIVED_CONTENT_FAIL       = 19;

            const unsigned int NEW_POSTINTEGRATION        = 20;
            const unsigned int FINISHED_POSTINTEGRATION   = 21;
            const unsigned int POSTINTEGRATION_DATA_READY = 22;
            const unsigned int POSTINTEGRATION_FAIL       = 29;

            const unsigned int INFORMATION_REQUEST        = 90;
		        const unsigned int INFORMATION_RESPONSE       = 91;
		        const unsigned int NEW_WORK_ASSIGNMENT        = 92;
		        const unsigned int NEW_WORK_ACKNOWLEDGMENT    = 93;
		        const unsigned int END_OF_WORK                = 97;
		        const unsigned int WORKER_CLOSE_DOWN          = 98;
            const unsigned int TERMINATE                  = 99;

            // MPI ranks
            const unsigned int RANK_MASTER = 0;

            // MPI message payloads

            class slave_setup_payload
              {

              public:

                //! Default constructor (used for receiving messages)
                slave_setup_payload() = default;

                //! Value constructor (used for constructing messages to send)
                slave_setup_payload(const boost::filesystem::path& rp, unsigned int bcp, unsigned int pcp, unsigned int zcp)
                  : repository(rp.string()),
                    batcher_capacity(bcp),
                    data_capacity(pcp),
                    zeta_capacity(zcp)
                  {
                  }

		            //! Get path to repository
                boost::filesystem::path get_repository_path() const { return(boost::filesystem::path(this->repository)); }

		            //! Get batcher capacity
		            unsigned int get_batcher_capacity() const { return(this->batcher_capacity); }

		            //! Get datapipe main cache capacity
		            unsigned int get_data_capacity()    const { return(this->data_capacity); }

		            //! Get datapipe zeta cache capacity
		            unsigned int get_zeta_capacity()    const { return(this->zeta_capacity); }

              private:

                //! Pathname to repository
                std::string repository;

		            //! Batcher capacity
		            unsigned int batcher_capacity;

		            //! Datapipe main cache capacity
		            unsigned int data_capacity;

		            //! Datapipe zeta cache capacity
		            unsigned int zeta_capacity;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & repository;
		                ar & batcher_capacity;
		                ar & data_capacity;
		                ar & zeta_capacity;
                  }

              };


		        class slave_information_payload
			        {

		          public:

			          typedef enum { cpu, gpu } worker_type;

		          public:

				        //! Default constructor (used for receiving messages)
				        slave_information_payload() = default;

				        //! Value constructor (used for constructing messages to send)
				        slave_information_payload(worker_type t, unsigned int c, unsigned int p)
				          : type(t),
		                capacity(c),
		                priority(p)
					        {
					        }

				        //! Get worker type
				        worker_type get_type() const { return(this->type); }

				        //! Get worker capacity
				        unsigned int get_capacity() const { return(this->capacity); }

				        //! Get worker priority
				        unsigned int get_priority() const { return(this->priority); }

		          private:

				        //! Worker type
				        worker_type type;

				        //! Worker memory capacity (integrations only)
				        unsigned int capacity;

				        //! Worker priority
				        unsigned int priority;

		            // enable boost::serialization support, and hence automated packing for transmission over MPI
		            friend class boost::serialization::access;

		            template <typename Archive>
		            void serialize(Archive& ar, unsigned int version)
			            {
		                ar & type;
		                ar & capacity;
		                ar & priority;
			            }

			        };


		        class work_assignment_payload
			        {
		          public:
				        //! Default constructor (used for receiving messages)
				        work_assignment_payload() = default;

				        //! Value constructor (used for constructing messages to send)
				        work_assignment_payload(const std::list<unsigned int>& i)
		              : items(i)
					        {
					        }

				        //! Get items
				        const std::list<unsigned int>& get_items() const { return(this->items); }

		          private:

				        //! work items
				        std::list<unsigned int> items;

		            // enable boost::serialization support, and hence automated packing for transmission over MPI
		            friend class boost::serialization::access;

		            template <typename Archive>
		            void serialize(Archive& ar, unsigned int version)
			            {
		                ar & items;
			            }

			        };


		        class work_acknowledgment_payload
			        {
		          public:
				        //! Default constructor (used for receiving messages)
				        work_acknowledgment_payload() = default;

				        //! get timestamp
				        boost::posix_time::ptime get_timestamp() const { return(this->timestamp); }

				        //! set timestamp
				        void set_timestamp() { this->timestamp = boost::posix_time::second_clock::universal_time(); }

		          private:

				        //! timestamp
				        boost::posix_time::ptime timestamp;

				        // enable boost::serialization support, and hence automated packing for transmission over MPI
				        friend class boost::serialization::access;

				        template <typename Archive>
				        void serialize(Archive& ar, unsigned int version)
					        {
						        ar & timestamp;
					        }

			        };


            class new_integration_payload
              {
              public:
                //! Default constructor (used for receiving messages)
                new_integration_payload() = default;

                //! Value constructor (used for constructing messages to send)
                new_integration_payload(const std::string& tk,
                                        const boost::filesystem::path& tmp_d,
                                        const boost::filesystem::path& log_d)
                : task(tk), tempdir(tmp_d.string()), logdir(log_d.string())
                  {
                  }

		            //! Get task name
                const std::string&      get_task_name()     const { return(this->task); }

		            //! Get path to temporary directory
                boost::filesystem::path get_tempdir_path()  const { return(boost::filesystem::path(this->tempdir)); }

		            //! Get path to log directory
                boost::filesystem::path get_logdir_path()   const { return(boost::filesystem::path(this->logdir)); }

              private:
                //! Name of task, to be looked up in repository database
                std::string task;

                //! Pathname to directory for temporary files
                std::string tempdir;

                //! Pathname to directory for log files
                std::string logdir;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & task;
                    ar & tempdir;
                    ar & logdir;
                  }

              };


            class data_ready_payload
              {
              public:

                //! Default constructor (used for receiving messages)
                data_ready_payload() = default;

                //! Value constructor (used for sending messages)
                data_ready_payload(const std::string& p)
                : container_path(p)
                  {
                  }

                //! Get container path
                const std::string& get_container_path() const { return(this->container_path); }

              private:
                //! Path to container
                std::string container_path;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & container_path;
                  }
              };


            class finished_integration_payload
              {
              public:

                //! Default constructor (used for receiving messages)
                finished_integration_payload() = default;

                //! Value constructor (used for constructing messages to send)
                finished_integration_payload(const boost::timer::nanosecond_type& i,
                                             const boost::timer::nanosecond_type& max_i, const boost::timer::nanosecond_type& min_i,
                                             const boost::timer::nanosecond_type& b,
                                             const boost::timer::nanosecond_type& max_b, const boost::timer::nanosecond_type& min_b,
                                             const boost::timer::nanosecond_type& w,
                                             const unsigned int& n)
                  : integration_time(i),
                    max_integration_time(max_i),
                    min_integration_time(min_i),
                    batching_time(b),
                    max_batching_time(max_b),
                    min_batching_time(min_b),
                    wallclock_time(w),
                    num_integrations(n),
                    timestamp(boost::posix_time::second_clock::universal_time())
                  {
                  }

                //! Get total integration time
                boost::timer::nanosecond_type get_integration_time()     const { return(this->integration_time); }

                //! Get longest integration time
                boost::timer::nanosecond_type get_max_integration_time() const { return(this->max_integration_time); }

                //! Get shortest integration time
                boost::timer::nanosecond_type get_min_integration_time() const { return(this->min_integration_time); }

                //! Get total batching time
                boost::timer::nanosecond_type get_batching_time()        const { return(this->batching_time); }

                //! Get longest batching time
                boost::timer::nanosecond_type get_max_batching_time()    const { return(this->max_batching_time); }

                //! Get shortest batching time
                boost::timer::nanosecond_type get_min_batching_time()    const { return(this->min_batching_time); }

                //! Get total wallclock time
                boost::timer::nanosecond_type get_wallclock_time()       const { return(this->wallclock_time); }

                //! Get total number of reported integrations
                unsigned int                  get_num_integrations()     const { return(this->num_integrations); }

		            //! Get timestamp
		            boost::posix_time::ptime      get_timestamp()            const { return(this->timestamp); }

              private:

                //! Total integration time
                boost::timer::nanosecond_type integration_time;

                //! Longest integration time
                boost::timer::nanosecond_type max_integration_time;

                //! Shortest integration time
                boost::timer::nanosecond_type min_integration_time;

                //! Total batching time
                boost::timer::nanosecond_type batching_time;

                //! Longest integration time
                boost::timer::nanosecond_type max_batching_time;

                //! Shortest integration time
                boost::timer::nanosecond_type min_batching_time;

                //! Total number of reported integrations
                unsigned int num_integrations;

                //! Total elapsed wallclock time
                boost::timer::nanosecond_type wallclock_time;

		            //! Timestamp
		            boost::posix_time::ptime timestamp;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & integration_time;
                    ar & max_integration_time;
                    ar & min_integration_time;
                    ar & batching_time;
                    ar & max_batching_time;
                    ar & min_batching_time;
                    ar & wallclock_time;
                    ar & num_integrations;
		                ar & timestamp;
                  }

              };


            class new_derived_content_payload
	            {

              public:

                //! Default constructor (used for receiving messages)
                new_derived_content_payload() = default;

                //! Value constructor (used for constructing messages to send)
                new_derived_content_payload(const std::string& tk,
                                            const boost::filesystem::path& tmp_d,
                                            const boost::filesystem::path& log_d,
                                            const std::list<std::string>& tg)
	                : task(tk), tempdir(tmp_d.string()), logdir(log_d.string()), tags(tg)
	                {
	                }

		            //! Get task name
                const std::string&            get_task_name()     const { return(this->task); }

		            //! Get path to the temporary directory
                boost::filesystem::path       get_tempdir_path()  const { return(boost::filesystem::path(this->tempdir)); }

		            //! Get path to the log directory
                boost::filesystem::path       get_logdir_path()   const { return(boost::filesystem::path(this->logdir)); }

		            //! Get tags specified on the command line, used to narrow-down the list of output groups
		            const std::list<std::string>& get_tags()          const { return(this->tags); }

              private:

                //! Name of task, to be looked up in repository database
                std::string task;

                //! Pathname to directory for temporary files
                std::string tempdir;

                //! Pathname to directory for log files
                std::string logdir;

 		            //! Search tags specified on the command line
		            std::list<std::string> tags;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
	                {
                    ar & task;
                    ar & tempdir;
                    ar & logdir;
		                ar & tags;
	                }

	            };


            class content_ready_payload
	            {

              public:

                typedef enum { twopf_payload, threepf_payload } payload_type;

                //! Default constructor (used for receiving messages)
                content_ready_payload() = default;

                //! Value constructor (used for sending messages)
                content_ready_payload(const std::string& dp)
	                : product(dp)
	                {
	                }

                //! Get derived product name
                const std::string& get_product_name() const { return(this->product); }

              private:

		            //! Name of derived product
		            std::string product;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
	                {
                    ar & product;
	                }

	            };


		        class finished_derived_payload
			        {

		          public:

		            //! Default constructor (used for receiving messages)
		            finished_derived_payload() = default;

		            //! Value constructor (used for sending messages)
		            finished_derived_payload(const boost::timer::nanosecond_type db, const boost::timer::nanosecond_type cpu,
		                                     const unsigned int ip, const boost::timer::nanosecond_type tp,
		                                     const boost::timer::nanosecond_type max_tp, const boost::timer::nanosecond_type min_tp,
		                                     const unsigned int tc, const unsigned int tc_u,
		                                     const unsigned int twopf_k, const unsigned int twopf_k_u,
		                                     const unsigned int threepf_k, const unsigned int threepf_k_u,
		                                     const unsigned int td, const unsigned int td_u,
                                         const unsigned int tz, const unsigned int tz_u,
		                                     const boost::timer::nanosecond_type tce, const boost::timer::nanosecond_type twopf_e,
		                                     const boost::timer::nanosecond_type threepf_e, const boost::timer::nanosecond_type de,
                                         const boost::timer::nanosecond_type zeta_e)
			            : database_time(db),
			              cpu_time(cpu),
			              items_processed(ip),
			              processing_time(tp),
			              max_processing_time(max_tp),
			              min_processing_time(min_tp),
			              time_config_hits(tc),
			              time_config_unloads(tc_u),
			              twopf_kconfig_hits(twopf_k),
			              twopf_kconfig_unloads(twopf_k_u),
			              threepf_kconfig_hits(threepf_k),
			              threepf_kconfig_unloads(threepf_k_u),
			              data_hits(td),
			              data_unloads(td_u),
                    zeta_hits(tz),
			              zeta_unloads(tz_u),
		                time_config_evictions(tce),
			              twopf_kconfig_evictions(twopf_e),
		                threepf_kconfig_evictions(threepf_e),
			              data_evictions(de),
                    zeta_evictions(zeta_e),
		                timestamp(boost::posix_time::second_clock::universal_time())
			            {
			            }

		            //! Get database time
		            boost::timer::nanosecond_type  get_database_time()             const { return(this->database_time); }

				        //! Get total CPU time
				        boost::timer::nanosecond_type  get_cpu_time()                  const { return(this->cpu_time); }

		            //! Get total number of items processed
		            unsigned int                   get_items_processed()           const { return(this->items_processed); }

		            //! Get total processing time
		            boost::timer::nanosecond_type  get_processing_time()           const { return(this->processing_time); }

		            //! Get max individual processing time
		            boost::timer::nanosecond_type  get_max_processing_time()       const { return(this->max_processing_time); }

		            //! Get min individual processing time
		            boost::timer::nanosecond_type  get_min_processing_time()       const { return(this->min_processing_time); }

				        //! Get time config hits
				        unsigned int                   get_time_config_hits()          const { return(this->time_config_hits); }

				        //! Get twopf kconfig hits
				        unsigned int                   get_twopf_kconfig_hits()        const { return(this->twopf_kconfig_hits); }

				        //! Get threepf kconfig hits
				        unsigned int                   get_threepf_kconfig_hits()      const { return(this->threepf_kconfig_hits); }

				        //! Get data hits
				        unsigned int                   get_data_hits()                 const { return(this->data_hits); }

                //! Get zeta hits
                unsigned int                   get_zeta_hits()                 const { return(this->zeta_hits); }

		            //! Get time config unloads
		            unsigned int                   get_time_config_unloads()       const { return(this->time_config_unloads); }

		            //! Get twopf kconfig unloads
		            unsigned int                   get_twopf_kconfig_unloads()     const { return(this->twopf_kconfig_unloads); }

		            //! Get threepf kconfig unloads
		            unsigned int                   get_threepf_kconfig_unloads()   const { return(this->threepf_kconfig_unloads); }

		            //! Get data unloads
		            unsigned int                   get_data_unloads()              const { return(this->data_unloads); }

                //! Get zeta unloads
                unsigned int                   get_zeta_unloads()              const { return(this->zeta_unloads); }

				        //! Get time-config cache evictions
				        boost::timer::nanosecond_type  get_time_config_evictions()     const { return(this->time_config_evictions); }

				        //! Get twopf k-config cache evictions
				        boost::timer::nanosecond_type  get_twopf_kconfig_evictions()   const { return(this->twopf_kconfig_evictions); }

				        //! Get threepf k-config cache evictions
				        boost::timer::nanosecond_type  get_threepf_kconfig_evictions() const { return(this->threepf_kconfig_evictions); }

				        //! Get data cache evictions
				        boost::timer::nanosecond_type  get_data_evictions()            const { return(this->data_evictions); }

                //! Get zeta cache evictions
                boost::timer::nanosecond_type  get_zeta_evictions()            const { return(this->zeta_evictions); }

				        //! Get timestamp
				        boost::posix_time::ptime       get_timestamp()                 const { return(this->timestamp); }


		          private:

		            //! Time spent reading database
		            boost::timer::nanosecond_type database_time;

				        //! Total CPU time
				        boost::timer::nanosecond_type cpu_time;

		            //! Total number of items processed
		            unsigned int items_processed;

		            //! Total processing time
		            boost::timer::nanosecond_type processing_time;

		            //! Max individual processing time
		            boost::timer::nanosecond_type max_processing_time;

		            //! Min individual processing time
		            boost::timer::nanosecond_type min_processing_time;

		            //! Number of time-config cache hits
		            unsigned int time_config_hits;

				        //! Number of time-config cache unloads
				        unsigned int time_config_unloads;

		            //! Number of twopf-kconfig cache hits
		            unsigned int twopf_kconfig_hits;

				        //! Number of twopf-kconfig cache unloads
				        unsigned int twopf_kconfig_unloads;

				        //! Number of threepf-kconfig cache hits
				        unsigned int threepf_kconfig_hits;

				        //! Number of threepf-kconfig cache unloads
				        unsigned int threepf_kconfig_unloads;

				        //! Number of data cache hits
				        unsigned int data_hits;

				        //! Number of data cache unloads
				        unsigned int data_unloads;

                //! Number of zeta cache hits
                unsigned int zeta_hits;

                //! Number of zeta cache unloads
                unsigned int zeta_unloads;

				        //! Time spent doing time-config cache evictions
				        boost::timer::nanosecond_type time_config_evictions;

				        //! Time spent doing twopf k-config cache evictions
				        boost::timer::nanosecond_type twopf_kconfig_evictions;

				        //! Time spent doing threepf k-config cache evictions
				        boost::timer::nanosecond_type threepf_kconfig_evictions;

				        //! Time spent doing data cache evictions
				        boost::timer::nanosecond_type data_evictions;

                //! Time spent doing zeta cache evictions
                boost::timer::nanosecond_type zeta_evictions;

		            //! Timestamp
		            boost::posix_time::ptime timestamp;

		            // enable boost::serialization support, and hence automated packing for transmission over MPI
		            friend class boost::serialization::access;

		            template <typename Archive>
		            void serialize(Archive& ar, unsigned int version)
			            {
		                ar & database_time;
				            ar & cpu_time;
		                ar & items_processed;
		                ar & processing_time;
		                ar & max_processing_time;
		                ar & min_processing_time;
				            ar & time_config_hits;
				            ar & time_config_unloads;
				            ar & twopf_kconfig_hits;
				            ar & twopf_kconfig_unloads;
				            ar & threepf_kconfig_hits;
				            ar & threepf_kconfig_unloads;
				            ar & data_hits;
				            ar & data_unloads;
                    ar & zeta_hits;
                    ar & zeta_unloads;
				            ar & time_config_evictions;
				            ar & twopf_kconfig_evictions;
				            ar & threepf_kconfig_evictions;
				            ar & data_evictions;
                    ar & zeta_evictions;
				            ar & timestamp;
			            }

			        };


            class new_postintegration_payload
              {

              public:

                //! Default constructor (used for receiving messages)
                new_postintegration_payload() = default;

                //! Value constructor (used for constructing messages to send)
                new_postintegration_payload(const std::string& tk,
                                            const boost::filesystem::path& tmp_d,
                                            const boost::filesystem::path& log_d,
                                            const std::list<std::string>& tg)
                  : task(tk), tempdir(tmp_d.string()), logdir(log_d.string()), tags(tg)
                  {
                  }

                //! Get task name
                const std::string&            get_task_name()     const { return(this->task); }

                //! Get path to the temporary directory
                boost::filesystem::path       get_tempdir_path()  const { return(boost::filesystem::path(this->tempdir)); }

                //! Get path to the log directory
                boost::filesystem::path       get_logdir_path()   const { return(boost::filesystem::path(this->logdir)); }

                //! Get tags specified on the command line, used to narrow-down the list of output groups
                const std::list<std::string>& get_tags()          const { return(this->tags); }

              private:

                //! Name of task, to be looked up in repository database
                std::string task;

                //! Pathname to directory for temporary files
                std::string tempdir;

                //! Pathname to directory for log files
                std::string logdir;

                //! Search tags specified on the command line
                std::list<std::string> tags;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & task;
                    ar & tempdir;
                    ar & logdir;
                    ar & tags;
                  }

              };


            class finished_postintegration_payload
              {

              public:

                //! Default constructor (used for receiving messages)
                finished_postintegration_payload() = default;

                //! Value constructor (used for sending messages)
                finished_postintegration_payload(const boost::timer::nanosecond_type db, const boost::timer::nanosecond_type cpu,
                                                 const unsigned int ip, const boost::timer::nanosecond_type tp,
                                                 const boost::timer::nanosecond_type max_tp, const boost::timer::nanosecond_type min_tp,
                                                 const unsigned int tc, const unsigned int tc_u,
                                                 const unsigned int twopf_k, const unsigned int twopf_k_u,
                                                 const unsigned int threepf_k, const unsigned int threepf_k_u,
                                                 const unsigned int td, const unsigned int td_u,
                                                 const unsigned int tz, const unsigned int tz_u,
                                                 const boost::timer::nanosecond_type tce, const boost::timer::nanosecond_type twopf_e,
                                                 const boost::timer::nanosecond_type threepf_e, const boost::timer::nanosecond_type de,
                                                 const boost::timer::nanosecond_type zeta_e)
                  : database_time(db),
                    cpu_time(cpu),
                    items_processed(ip),
                    processing_time(tp),
                    max_processing_time(max_tp),
                    min_processing_time(min_tp),
                    time_config_hits(tc),
                    time_config_unloads(tc_u),
                    twopf_kconfig_hits(twopf_k),
                    twopf_kconfig_unloads(twopf_k_u),
                    threepf_kconfig_hits(threepf_k),
                    threepf_kconfig_unloads(threepf_k_u),
                    data_hits(td), data_unloads(td_u),
                    zeta_hits(tz), zeta_unloads(tz_u),
                    time_config_evictions(tce),
                    twopf_kconfig_evictions(twopf_e),
                    threepf_kconfig_evictions(threepf_e),
                    data_evictions(de),
                    zeta_evictions(zeta_e),
                    timestamp(boost::posix_time::second_clock::universal_time())
                  {
                  }

                //! Get database time
                boost::timer::nanosecond_type  get_database_time()             const { return(this->database_time); }

                //! Get total CPU time
                boost::timer::nanosecond_type  get_cpu_time()                  const { return(this->cpu_time); }

		            //! Get total number of items processed
		            unsigned int                   get_items_processed()           const { return(this->items_processed); }

		            //! Get total processing time
		            boost::timer::nanosecond_type  get_processing_time()           const { return(this->processing_time); }

		            //! Get max individual processing time
		            boost::timer::nanosecond_type  get_max_processing_time()       const { return(this->max_processing_time); }

		            //! Get min individual processing time
		            boost::timer::nanosecond_type  get_min_processing_time()       const { return(this->min_processing_time); }

                //! Get time config hits
                unsigned int                   get_time_config_hits()          const { return(this->time_config_hits); }

                //! Get twopf kconfig hits
                unsigned int                   get_twopf_kconfig_hits()        const { return(this->twopf_kconfig_hits); }

                //! Get threepf kconfig hits
                unsigned int                   get_threepf_kconfig_hits()      const { return(this->threepf_kconfig_hits); }

                //! Get data hits
                unsigned int                   get_data_hits()                 const { return(this->data_hits); }

                //! Get zeta hits
                unsigned int                   get_zeta_hits()                 const { return(this->zeta_hits); }

                //! Get time config unloads
                unsigned int                   get_time_config_unloads()       const { return(this->time_config_unloads); }

                //! Get twopf kconfig unloads
                unsigned int                   get_twopf_kconfig_unloads()     const { return(this->twopf_kconfig_unloads); }

                //! Get threepf kconfig unloads
                unsigned int                   get_threepf_kconfig_unloads()   const { return(this->threepf_kconfig_unloads); }

                //! Get data unloads
                unsigned int                   get_data_unloads()              const { return(this->data_unloads); }

                //! Get zeta unloads
                unsigned int                   get_zeta_unloads()              const { return(this->zeta_unloads); }

                //! Get time-config cache evictions
                boost::timer::nanosecond_type  get_time_config_evictions()     const { return(this->time_config_evictions); }

                //! Get twopf k-config cache evictions
                boost::timer::nanosecond_type  get_twopf_kconfig_evictions()   const { return(this->twopf_kconfig_evictions); }

                //! Get threepf k-config cache evictions
                boost::timer::nanosecond_type  get_threepf_kconfig_evictions() const { return(this->threepf_kconfig_evictions); }

                //! Get data cache evictions
                boost::timer::nanosecond_type  get_data_evictions()            const { return(this->data_evictions); }

                //! Get zeta cache evictions
                boost::timer::nanosecond_type  get_zeta_evictions()            const { return(this->zeta_evictions); }

		            //! Get timestamp
		            boost::posix_time::ptime       get_timestamp()                 const { return(this->timestamp); }


              private:

                //! Time spent reading database
                boost::timer::nanosecond_type database_time;

                //! Total CPU time
                boost::timer::nanosecond_type cpu_time;

		            //! Total number of items processed
		            unsigned int items_processed;

		            //! Total processing time
		            boost::timer::nanosecond_type processing_time;

		            //! Max individual processing time
		            boost::timer::nanosecond_type max_processing_time;

		            //! Min individual processing time
		            boost::timer::nanosecond_type min_processing_time;

                //! Number of time-config cache hits
                unsigned int time_config_hits;

                //! Number of time-config cache unloads
                unsigned int time_config_unloads;

                //! Number of twopf-kconfig cache hits
                unsigned int twopf_kconfig_hits;

                //! Number of twopf-kconfig cache unloads
                unsigned int twopf_kconfig_unloads;

                //! Number of threepf-kconfig cache hits
                unsigned int threepf_kconfig_hits;

                //! Number of threepf-kconfig cache unloads
                unsigned int threepf_kconfig_unloads;

                //! Number of data cache hits
                unsigned int data_hits;

                //! Number of data cache unloads
                unsigned int data_unloads;

                //! Number of zeta cache hits
                unsigned int zeta_hits;

                //! Number of zeta cache unloads
                unsigned int zeta_unloads;

                //! Time spent doing time-config cache evictions
                boost::timer::nanosecond_type time_config_evictions;

                //! Time spent doing twopf k-config cache evictions
                boost::timer::nanosecond_type twopf_kconfig_evictions;

                //! Time spent doing threepf k-config cache evictions
                boost::timer::nanosecond_type threepf_kconfig_evictions;

                //! Time spent doing data cache evictions
                boost::timer::nanosecond_type data_evictions;

                //! Time spent doing zeta cache evictions
                boost::timer::nanosecond_type zeta_evictions;

		            //! Timestamp
		            boost::posix_time::ptime timestamp;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & database_time;
                    ar & cpu_time;
		                ar & items_processed;
		                ar & processing_time;
		                ar & max_processing_time;
		                ar & min_processing_time;
                    ar & time_config_hits;
                    ar & time_config_unloads;
                    ar & twopf_kconfig_hits;
                    ar & twopf_kconfig_unloads;
                    ar & threepf_kconfig_hits;
                    ar & threepf_kconfig_unloads;
                    ar & data_hits;
                    ar & data_unloads;
                    ar & zeta_hits;
                    ar & zeta_unloads;
                    ar & time_config_evictions;
                    ar & twopf_kconfig_evictions;
                    ar & threepf_kconfig_evictions;
                    ar & data_evictions;
                    ar & zeta_evictions;
		                ar & timestamp;
                  }

              };


          }   // unnamed namespace

      }   // namespace MPI

  } // namespace transport


#endif //__mpi_operations_H_
