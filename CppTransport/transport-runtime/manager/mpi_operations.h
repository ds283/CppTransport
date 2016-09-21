//
// Created by David Seery on 22/02/2014.
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


#ifndef CPPTRANSPORT_MPI_OPERATIONS_H
#define CPPTRANSPORT_MPI_OPERATIONS_H

#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/list.hpp"
#include "boost/serialization/set.hpp"
#include "boost/date_time/posix_time/time_serialize.hpp"
#include "boost/timer/timer.hpp"

#include "transport-runtime/manager/argument_cache.h"

#include "transport-runtime/enumerations.h"


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

            const unsigned int WORKER_SETUP               = 100;
		        const unsigned int WORKER_IDENTIFICATION      = 101;
		        const unsigned int NEW_WORK_ASSIGNMENT        = 102;
		        const unsigned int NEW_WORK_ACKNOWLEDGMENT    = 103;
            const unsigned int REPORT_ERROR               = 104;
            const unsigned int SET_LOCAL_CHECKPOINT       = 105;
            const unsigned int UNSET_LOCAL_CHECKPOINT     = 106;
            const unsigned int QUERY_PERFORMANCE_DATA     = 107;
            const unsigned int REPORT_PERFORMANCE_DATA    = 108;

		        const unsigned int END_OF_WORK                = 900;
            const unsigned int WORKER_CLOSE_DOWN          = 901;

            const unsigned int TERMINATE                  = 999;

            // MPI ranks
            const unsigned int RANK_MASTER = 0;


            // MPI message payloads

            class error_report
              {

              public:

                //! Default constructor (used for receiving messages)
                error_report() = default;

                //! Value constructor (used for constructing messages to send)
                error_report(std::string m)
                  : msg(std::move(m))
                  {
                  }

                //! Get message
                const std::string& get_message() const { return(this->msg); }

              private:

                //! message
                std::string msg;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & msg;
                  }

              };

            class slave_setup_payload
              {

              public:

                //! Default constructor (used for receiving messages)
                slave_setup_payload() = default;

                //! Value constructor (used for constructing messages to send)
                slave_setup_payload(const boost::filesystem::path& rp, argument_cache& ac)
                  : repository(rp.string()),
                    arg_cache(ac)
                  {
                  }

		            //! Get path to repository
                boost::filesystem::path get_repository_path() const { return(boost::filesystem::path(this->repository)); }

                //! Get argument cache
                const argument_cache&   get_argument_cache()  const { return(this->arg_cache); }

              private:

                //! Pathname to repository
                std::string repository;

                //! Argument cache
                argument_cache arg_cache;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & repository;
		                ar & arg_cache;
                  }

              };


		        class slave_information_payload
			        {

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
                new_integration_payload(const std::string& tk, const std::string& nm,
                                        const boost::filesystem::path& tmp_d,
                                        const boost::filesystem::path& log_d,
                                        unsigned int wg)
                : task(tk),
                  group_name(nm),
                  tempdir(tmp_d.string()),
                  logdir(log_d.string()),
                  workgroup_number(wg)
                  {
                  }

		            //! Get task name
                const std::string&      get_task_name()        const { return(this->task); }
                
                //! Get output group name
                const std::string&      get_group_name()       const { return(this->group_name); }

		            //! Get path to temporary directory
                boost::filesystem::path get_tempdir_path()     const { return(boost::filesystem::path(this->tempdir)); }

		            //! Get path to log directory
                boost::filesystem::path get_logdir_path()      const { return(boost::filesystem::path(this->logdir)); }

                //! Get workgroup numbers
                unsigned int            get_workgroup_number() const { return(this->workgroup_number); }

              private:

                //! Name of task, to be looked up in repository database
                std::string task;
                
                //! Name of output group
                std::string group_name;

                //! Pathname to directory for temporary files
                std::string tempdir;

                //! Pathname to directory for log files
                std::string logdir;

                //! Workgroup number
                unsigned int workgroup_number;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & task;
                    ar & group_name;
                    ar & tempdir;
                    ar & logdir;
                    ar & workgroup_number;
                  }

              };


            class data_ready_payload
              {
              public:

                //! Default constructor (used for receiving messages)
                data_ready_payload() = default;

                //! Value constructor (used for sending messages)
                data_ready_payload(const boost::filesystem::path& p)
                : container_path(p.string()),
                  timestamp(boost::posix_time::second_clock::universal_time())
                  {
                  }

                //! Get container path
                const boost::filesystem::path  get_container_path() const { return(this->container_path); }

                //! Get timestamp
                boost::posix_time::ptime       get_timestamp()      const { return(this->timestamp); }

              private:

                //! Path to container; note serialized as a string because boost::filesystem::path serialization
                //! isn't provided out-of-the-box
                std::string container_path;

                //! Timestamp
                boost::posix_time::ptime timestamp;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & container_path;
                    ar & timestamp;
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
                                             const unsigned int& n, const unsigned int& nr, const unsigned int& nf,
                                             const std::set<unsigned int>& fs, const double ld)
                  : integration_time(i),
                    max_integration_time(max_i),
                    min_integration_time(min_i),
                    batching_time(b),
                    max_batching_time(max_b),
                    min_batching_time(min_b),
                    wallclock_time(w),
                    num_integrations(n),
                    num_refinements(nr),
                    num_failures(nf),
                    failed_serials(fs),
                    load_average(ld),
                    timestamp(boost::posix_time::second_clock::universal_time())
                  {
                  }

                //! Get total integration time
                boost::timer::nanosecond_type   get_integration_time()     const { return(this->integration_time); }

                //! Get longest integration time
                boost::timer::nanosecond_type   get_max_integration_time() const { return(this->max_integration_time); }

                //! Get shortest integration time
                boost::timer::nanosecond_type   get_min_integration_time() const { return(this->min_integration_time); }

                //! Get total batching time
                boost::timer::nanosecond_type   get_batching_time()        const { return(this->batching_time); }

                //! Get longest batching time
                boost::timer::nanosecond_type   get_max_batching_time()    const { return(this->max_batching_time); }

                //! Get shortest batching time
                boost::timer::nanosecond_type   get_min_batching_time()    const { return(this->min_batching_time); }

                //! Get total wallclock time
                boost::timer::nanosecond_type   get_wallclock_time()       const { return(this->wallclock_time); }

                //! Get total number of reported integrations
                unsigned int                    get_num_integrations()     const { return(this->num_integrations); }

                //! Get total number of integrations which required mesh refinement
                unsigned int                    get_num_refinements()      const { return(this->num_refinements); }

                //! Get total number of failures (this counts failure reports, not necessarily individual k-configurations
                unsigned int                    get_num_failures()         const { return(this->num_failures); }

                //! Get list of failed serial numbers (if supported by the backend)
                const std::set<unsigned int>&   get_failed_serials()       const { return(this->failed_serials); }

                //! Get current load average
                double                          get_load_average()         const { return(this->load_average); }
                
		            //! Get timestamp
		            boost::posix_time::ptime        get_timestamp()            const { return(this->timestamp); }

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

                //! Total number of reported failures (counts failure reports, not necessarily individual k-configurations)
                unsigned int num_failures;

                //! Total number of reported integrations requiring mesh refinement
                unsigned int num_refinements;

                //! Total elapsed wallclock time
                boost::timer::nanosecond_type wallclock_time;
                
                //! Current load average
                double load_average;

                //! List of failed serial numbers (not all backends may support collection of this data)
                std::set<unsigned int> failed_serials;

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
                    ar & num_failures;
                    ar & num_refinements;
                    ar & failed_serials;
                    ar & load_average;
		                ar & timestamp;
                  }

              };


            class new_derived_content_payload
	            {

              public:

                //! Default constructor (used for receiving messages)
                new_derived_content_payload() = default;

                //! Value constructor (used for constructing messages to send)
                new_derived_content_payload(const std::string& tk, const std::string& nm,
                                            const boost::filesystem::path& tmp_d,
                                            const boost::filesystem::path& log_d,
                                            const std::list<std::string>& tg)
	                : task(tk),
                    group_name(nm),
                    tempdir(tmp_d.string()),
                    logdir(log_d.string()),
                    tags(tg)
	                {
	                }

		            //! Get task name
                const std::string&            get_task_name()     const { return(this->task); }
    
                //! Get output group name
                const std::string&            get_group_name()    const { return(this->group_name); }

		            //! Get path to the temporary directory
                boost::filesystem::path       get_tempdir_path()  const { return(boost::filesystem::path(this->tempdir)); }

		            //! Get path to the log directory
                boost::filesystem::path       get_logdir_path()   const { return(boost::filesystem::path(this->logdir)); }

		            //! Get tags specified on the command line, used to narrow-down the list of content groups
		            const std::list<std::string>& get_tags()          const { return(this->tags); }

              private:

                //! Name of task, to be looked up in repository database
                std::string task;
    
                //! Name of output group
                std::string group_name;

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
                    ar & group_name;
                    ar & tempdir;
                    ar & logdir;
		                ar & tags;
	                }

	            };


            class content_ready_payload
	            {

              public:

                enum class payload_type { twopf_payload, threepf_payload };

                //! Default constructor (used for receiving messages)
                content_ready_payload() = default;

                //! Value constructor (used for sending messages)
                content_ready_payload(const std::string& dp, const std::list<std::string>& g)
	                : product(dp),
                    content_groups(g),
                    timestamp(boost::posix_time::second_clock::universal_time())
	                {
	                }

                //! Get derived product name
                const std::string&            get_product_name()   const { return(this->product); }

                //! Get content groups used
                const std::list<std::string>& get_content_groups() const { return(this->content_groups); }

                //! Get timestamp
                boost::posix_time::ptime      get_timestamp()      const { return(this->timestamp); }


              private:

		            //! Name of derived product
		            std::string product;

                //! Content groups used to create it
                std::list<std::string> content_groups;

                //! Timestamp
                boost::posix_time::ptime timestamp;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
	                {
                    ar & product;
                    ar & content_groups;
		                ar & timestamp;
	                }

	            };


		        class finished_derived_payload
			        {

		          public:

		            //! Default constructor (used for receiving messages)
		            finished_derived_payload() = default;

		            //! Value constructor (used for sending messages)
		            finished_derived_payload(const std::list<std::string>& cg, const boost::timer::nanosecond_type db, const boost::timer::nanosecond_type cpu,
		                                     const unsigned int ip, const boost::timer::nanosecond_type tp,
		                                     const boost::timer::nanosecond_type max_tp, const boost::timer::nanosecond_type min_tp,
		                                     const unsigned int tc, const unsigned int tc_u,
		                                     const unsigned int twopf_k, const unsigned int twopf_k_u,
		                                     const unsigned int threepf_k, const unsigned int threepf_k_u,
                                         const unsigned int ts, const unsigned int ts_u,
		                                     const unsigned int td, const unsigned int td_u,
		                                     const boost::timer::nanosecond_type tce, const boost::timer::nanosecond_type twopf_e,
		                                     const boost::timer::nanosecond_type threepf_e, const boost::timer::nanosecond_type s_e,
                                         const boost::timer::nanosecond_type de, const double ld)
			            : content_groups(cg),
                    database_time(db),
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
                    stats_hits(ts),
                    stats_unloads(ts_u),
			              data_hits(td),
			              data_unloads(td_u),
		                time_config_evictions(tce),
			              twopf_kconfig_evictions(twopf_e),
		                threepf_kconfig_evictions(threepf_e),
                    stats_evictions(s_e),
			              data_evictions(de),
                    load_average(ld),
		                timestamp(boost::posix_time::second_clock::universal_time())
			            {
			            }

                //! Get content gorups
                const std::list<std::string>   get_content_groups()            const { return(this->content_groups); }

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

                //! Get stats hits
                unsigned int                   get_stats_hits()                const { return(this->stats_hits); }

				        //! Get data hits
				        unsigned int                   get_data_hits()                 const { return(this->data_hits); }

		            //! Get time config unloads
		            unsigned int                   get_time_config_unloads()       const { return(this->time_config_unloads); }

		            //! Get twopf kconfig unloads
		            unsigned int                   get_twopf_kconfig_unloads()     const { return(this->twopf_kconfig_unloads); }

		            //! Get threepf kconfig unloads
		            unsigned int                   get_threepf_kconfig_unloads()   const { return(this->threepf_kconfig_unloads); }

                //! Get stats unloads
                unsigned int                   get_stats_unloads()             const { return(this->stats_unloads); }

		            //! Get data unloads
		            unsigned int                   get_data_unloads()              const { return(this->data_unloads); }

				        //! Get time-config cache evictions
				        boost::timer::nanosecond_type  get_time_config_evictions()     const { return(this->time_config_evictions); }

				        //! Get twopf k-config cache evictions
				        boost::timer::nanosecond_type  get_twopf_kconfig_evictions()   const { return(this->twopf_kconfig_evictions); }

				        //! Get threepf k-config cache evictions
				        boost::timer::nanosecond_type  get_threepf_kconfig_evictions() const { return(this->threepf_kconfig_evictions); }

                //! Get stats cache evictions
                boost::timer::nanosecond_type  get_stats_evictions()           const { return(this->stats_evictions); }

				        //! Get data cache evictions
				        boost::timer::nanosecond_type  get_data_evictions()            const { return(this->data_evictions); }

                //! Get current load average
                double                         get_load_average()              const { return(this->load_average); }
                
				        //! Get timestamp
				        boost::posix_time::ptime       get_timestamp()                 const { return(this->timestamp); }


		          private:

                //! Content groups used to produce this derived content
                std::list<std::string> content_groups;

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

                //! Number of stats cache hits
                unsigned int stats_hits;

                //! Number of stats cache unloads
                unsigned int stats_unloads;

				        //! Number of data cache hits
				        unsigned int data_hits;

				        //! Number of data cache unloads
				        unsigned int data_unloads;

				        //! Time spent doing time-config cache evictions
				        boost::timer::nanosecond_type time_config_evictions;

				        //! Time spent doing twopf k-config cache evictions
				        boost::timer::nanosecond_type twopf_kconfig_evictions;

				        //! Time spent doing threepf k-config cache evictions
				        boost::timer::nanosecond_type threepf_kconfig_evictions;

                //! Time spent doing stats cache evictions
                boost::timer::nanosecond_type stats_evictions;

				        //! Time spent doing data cache evictions
				        boost::timer::nanosecond_type data_evictions;

                //! Current load average
                double load_average;
                
		            //! Timestamp
		            boost::posix_time::ptime timestamp;

		            // enable boost::serialization support, and hence automated packing for transmission over MPI
		            friend class boost::serialization::access;

		            template <typename Archive>
		            void serialize(Archive& ar, unsigned int version)
			            {
                    ar & content_groups;
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
                    ar & stats_hits;
                    ar & stats_unloads;
				            ar & data_hits;
				            ar & data_unloads;
				            ar & time_config_evictions;
				            ar & twopf_kconfig_evictions;
				            ar & threepf_kconfig_evictions;
                    ar & stats_evictions;
				            ar & data_evictions;
                    ar & load_average;
				            ar & timestamp;
			            }

			        };


            class new_postintegration_payload
              {

              public:

                //! Default constructor (used for receiving messages)
                new_postintegration_payload() = default;

                //! Value constructor for standard postintegrations (used for constructing messages to send)
                new_postintegration_payload(const std::string& tk, const std::string& nm,
                                            const boost::filesystem::path& tmp_d,
                                            const boost::filesystem::path& log_d,
                                            const std::list<std::string>& tg)
                  : task(tk),
                    group_name(nm),
                    tempdir(tmp_d.string()),
                    logdir(log_d.string()),
                    tags(tg),
                    workgroup_number(0)
                  {
                  }

                //! Value constructor for paired integrations (used for constructing messages to send)
                new_postintegration_payload(const std::string& tk, const std::string& nm,
                                            const boost::filesystem::path& p_tmp_d,
                                            const boost::filesystem::path& p_log_d,
                                            const std::list<std::string>& tg,
                                            const boost::filesystem::path& i_tmp_d,
                                            const boost::filesystem::path& i_log_d,
                                            unsigned int wg)
                  : task(tk),
                    group_name(nm),
                    tempdir(p_tmp_d.string()),
                    logdir(p_log_d.string()),
                    tags(tg),
                    paired_tempdir(i_tmp_d.string()),
                    paired_logdir(i_log_d.string()),
                    workgroup_number(wg)
                  {
                  }

                //! Get task name
                const std::string&            get_task_name()               const { return(this->task); }
    
                //! Get output group name
                const std::string&            get_group_name()              const { return(this->group_name); }

                //! Get path to the temporary directory
                boost::filesystem::path       get_tempdir_path()            const { return(boost::filesystem::path(this->tempdir)); }

                //! Get path to the log directory
                boost::filesystem::path       get_logdir_path()             const { return(boost::filesystem::path(this->logdir)); }

                //! Get path to paired temporary directory
                boost::filesystem::path       get_paired_tempdir_path()     const { return(boost::filesystem::path(this->paired_tempdir)); }

                //! Get path to paired log directory
                boost::filesystem::path       get_paired_logdir_path()      const { return(boost::filesystem::path(this->paired_logdir)); }

                //! Get workgroup number for paired integration
                unsigned int                  get_paired_workgroup_number() const { return(this->workgroup_number); }

                //! Get tags specified on the command line, used to narrow-down the list of content groups
                const std::list<std::string>& get_tags()                    const { return(this->tags); }

              private:

                //! Name of task, to be looked up in repository database
                std::string task;
    
                //! Name of output group
                std::string group_name;

                //! Pathname to directory for temporary files
                std::string tempdir;

                //! Pathname to directory for log files
                std::string logdir;

                //! Search tags specified on the command line
                std::list<std::string> tags;

                //! Pathname to paired directory for temporary files (if using)
                std::string paired_tempdir;

                //! Pathname to paired directory for log files (if using)
                std::string paired_logdir;

                //! Workgroup number for paired integration (if using)
                unsigned int workgroup_number;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & task;
                    ar & group_name;
                    ar & tempdir;
                    ar & logdir;
                    ar & tags;
                    ar & paired_tempdir;
                    ar & paired_logdir;
                    ar & workgroup_number;
                  }

              };


            class finished_postintegration_payload
              {

              public:

                //! Default constructor (used for receiving messages)
                finished_postintegration_payload() = default;

                //! Value constructor (used for sending messages)
                finished_postintegration_payload(const std::string& g, const boost::timer::nanosecond_type db, const boost::timer::nanosecond_type cpu,
                                                 const unsigned int ip, const boost::timer::nanosecond_type tp,
                                                 const boost::timer::nanosecond_type max_tp, const boost::timer::nanosecond_type min_tp,
                                                 const unsigned int tc, const unsigned int tc_u,
                                                 const unsigned int twopf_k, const unsigned int twopf_k_u,
                                                 const unsigned int threepf_k, const unsigned int threepf_k_u,
                                                 const unsigned int stats, const unsigned int stats_u,
                                                 const unsigned int td, const unsigned int td_u,
                                                 const boost::timer::nanosecond_type tce, const boost::timer::nanosecond_type twopf_e,
                                                 const boost::timer::nanosecond_type threepf_e, const boost::timer::nanosecond_type stats_e,
                                                 const boost::timer::nanosecond_type de, double ld)
                  : content_groups(std::list<std::string>{g}),
                    database_time(db),
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
                    stats_hits(stats),
                    stats_unloads(stats_u),
                    data_hits(td), data_unloads(td_u),
                    time_config_evictions(tce),
                    twopf_kconfig_evictions(twopf_e),
                    threepf_kconfig_evictions(threepf_e),
                    stats_evictions(stats_e),
                    data_evictions(de),
                    load_average(ld),
                    timestamp(boost::posix_time::second_clock::universal_time())
                  {
                  }

                //! Get parent group
                const std::list<std::string>&  get_content_groups()            const { return(this->content_groups); }

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

                //! Get stats hits
                unsigned int                   get_stats_hits()                 const { return(this->stats_hits); }

                //! Get data hits
                unsigned int                   get_data_hits()                 const { return(this->data_hits); }

                //! Get time config unloads
                unsigned int                   get_time_config_unloads()       const { return(this->time_config_unloads); }

                //! Get twopf kconfig unloads
                unsigned int                   get_twopf_kconfig_unloads()     const { return(this->twopf_kconfig_unloads); }

                //! Get threepf kconfig unloads
                unsigned int                   get_threepf_kconfig_unloads()   const { return(this->threepf_kconfig_unloads); }

                //! Get stats unloads
                unsigned int                   get_stats_unloads()             const { return(this->stats_unloads); }

                //! Get data unloads
                unsigned int                   get_data_unloads()              const { return(this->data_unloads); }

                //! Get time-config cache evictions
                boost::timer::nanosecond_type  get_time_config_evictions()     const { return(this->time_config_evictions); }

                //! Get twopf k-config cache evictions
                boost::timer::nanosecond_type  get_twopf_kconfig_evictions()   const { return(this->twopf_kconfig_evictions); }

                //! Get threepf k-config cache evictions
                boost::timer::nanosecond_type  get_threepf_kconfig_evictions() const { return(this->threepf_kconfig_evictions); }

                //! Get stats cache evictions
                boost::timer::nanosecond_type  get_stats_evictions()           const { return(this->stats_evictions); }

                //! Get data cache evictions
                boost::timer::nanosecond_type  get_data_evictions()            const { return(this->data_evictions); }
                
                //! Get current load average
                double                         get_load_average()              const { return(this->load_average); }

		            //! Get timestamp
		            boost::posix_time::ptime       get_timestamp()                 const { return(this->timestamp); }


              private:

                //! Parent content group
                std::list<std::string> content_groups;

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

                //! Number of stats cache hits
                unsigned int stats_hits;

                //! Number of stats cache unloads
                unsigned int stats_unloads;

                //! Number of data cache hits
                unsigned int data_hits;

                //! Number of data cache unloads
                unsigned int data_unloads;

                //! Time spent doing time-config cache evictions
                boost::timer::nanosecond_type time_config_evictions;

                //! Time spent doing twopf k-config cache evictions
                boost::timer::nanosecond_type twopf_kconfig_evictions;

                //! Time spent doing threepf k-config cache evictions
                boost::timer::nanosecond_type threepf_kconfig_evictions;

                //! Time spent doing stats cache evictions
                boost::timer::nanosecond_type stats_evictions;

                //! Time spent doing data cache evictions
                boost::timer::nanosecond_type data_evictions;
                
                //! Current load average
                double load_average;

		            //! Timestamp
		            boost::posix_time::ptime timestamp;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & content_groups;
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
                    ar & stats_hits;
                    ar & stats_unloads;
                    ar & data_hits;
                    ar & data_unloads;
                    ar & time_config_evictions;
                    ar & twopf_kconfig_evictions;
                    ar & threepf_kconfig_evictions;
                    ar & stats_evictions;
                    ar & data_evictions;
                    ar & load_average;
		                ar & timestamp;
                  }

              };
            
            
            class performance_data_payload
              {
                
              public:
                
                //! Default constructor (used for receiving messages)
                performance_data_payload() = default;
                
                //! Value constructor (used for sending messages)
                performance_data_payload(const double ld)
                  : load_average(ld)
                  {
                  }
                
                
                //! Get load average
                double get_load_average() const { return this->load_average; }
                
                
              private:
                
                //! Final value of load average
                double load_average;
    
                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;
    
                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & load_average;
                  }
                
              };


          }   // unnamed namespace

      }   // namespace MPI

  } // namespace transport


#endif //CPPTRANSPORT_MPI_OPERATIONS_H
