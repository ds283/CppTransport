//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_OUTPUT_GROUP_METADATA_DECL_H
#define CPPTRANSPORT_OUTPUT_GROUP_METADATA_DECL_H


namespace transport
  {

    // OUTPUT METADATA

    //! Timing metadata for an integration
    class integration_metadata : public serializable
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! null constructor - set all fields to zero
        integration_metadata(void)
          : total_wallclock_time(0),
            total_aggregation_time(0),
            total_integration_time(0),
            min_mean_integration_time(0),
            max_mean_integration_time(0),
            global_min_integration_time(0),
            global_max_integration_time(0),
            total_batching_time(0),
            min_mean_batching_time(0),
            max_mean_batching_time(0),
            global_min_batching_time(0),
            global_max_batching_time(0),
            total_configurations(0),
            total_failures(0),
            total_refinements(0)
          {
          }

        //! value constructor - ensure all fields get set at once
        integration_metadata(boost::timer::nanosecond_type wc, boost::timer::nanosecond_type ag,
                             boost::timer::nanosecond_type it, boost::timer::nanosecond_type min_m_it, boost::timer::nanosecond_type max_m_it,
                             boost::timer::nanosecond_type min_it, boost::timer::nanosecond_type max_it,
                             boost::timer::nanosecond_type bt, boost::timer::nanosecond_type min_m_bt, boost::timer::nanosecond_type max_m_bt,
                             boost::timer::nanosecond_type min_bt, boost::timer::nanosecond_type max_bt,
                             unsigned int num_processed, unsigned int num_failed, unsigned int num_refined,
                             const std::list<unsigned int>& failures, bool f)
          : total_wallclock_time(wc),
            total_aggregation_time(ag),
            total_integration_time(it),
            min_mean_integration_time(min_m_it),
            max_mean_integration_time(max_m_it),
            global_min_integration_time(min_it),
            global_max_integration_time(max_it),
            total_batching_time(bt),
            min_mean_batching_time(min_m_bt),
            max_mean_batching_time(max_m_bt),
            global_min_batching_time(min_bt),
            global_max_batching_time(max_bt),
            total_configurations(num_processed),
            total_failures(num_failed),
            total_refinements(num_refined)
          {
          }

        //! deserialization constructor
        integration_metadata(Json::Value& reader);


        // SERIALIZE -- implements a 'serializable' interface

      public:

        //! serialize this object
        void serialize(Json::Value& writer) const override;


        // DATA

      public:

        //! total wallclock time (measured on master process)
        boost::timer::nanosecond_type total_wallclock_time;

        //! total aggregation time (measured on master process)
        boost::timer::nanosecond_type total_aggregation_time;

        //! total integration time (measured by aggregating times reported by workers)
        boost::timer::nanosecond_type total_integration_time;

        //! minimum mean integration time (taken over all workers)
        boost::timer::nanosecond_type min_mean_integration_time;

        //! maxmimum mean integration time (taken over all workers)
        boost::timer::nanosecond_type max_mean_integration_time;

        //! global minimum integration time (per configuration)
        boost::timer::nanosecond_type global_min_integration_time;

        // global maximum integration time (per configuration)
        boost::timer::nanosecond_type global_max_integration_time;

        // total batching time (measured by aggregating times reported by workers)
        boost::timer::nanosecond_type total_batching_time;

        //! minimum mean batching time (taken over all workers)
        boost::timer::nanosecond_type min_mean_batching_time;

        // maximum mean batching time (taken over all workers)
        boost::timer::nanosecond_type max_mean_batching_time;

        // global minimum batching time (per configuration)
        boost::timer::nanosecond_type global_min_batching_time;

        // global maximum batching time (per configuration)
        boost::timer::nanosecond_type global_max_batching_time;

        // total number of configurations processed
        unsigned int total_configurations;

        //! total number of failures reported
        unsigned int total_failures;

        //! total number of mesh refinements reported
        unsigned int total_refinements;

      };


    //! Timing metadata for an output task
    class output_metadata : public serializable
      {
      public:

        //! null constructor - set all fields to zero
        output_metadata(void)
          : work_time(0),
            db_time(0),
            aggregation_time(0),
            time_config_hits(0),
            time_config_unloads(0),
            twopf_kconfig_hits(0),
            twopf_kconfig_unloads(0),
            threepf_kconfig_hits(0),
            threepf_kconfig_unloads(0),
            stats_hits(0),
            stats_unloads(0),
            data_hits(0),
            data_unloads(0),
            time_config_evictions(0),
            twopf_kconfig_evictions(0),
            threepf_kconfig_evictions(0),
            stats_evictions(0),
            data_evictions(0)
          {
          }

        //! value constructor - ensure all fields get set at once
        output_metadata(boost::timer::nanosecond_type wt, boost::timer::nanosecond_type dt, boost::timer::nanosecond_type ag,
                        unsigned int tc_h, unsigned int tc_u,
                        unsigned int tw_k_h, unsigned int tw_k_u,
                        unsigned int th_k_h, unsigned int th_k_u,
                        unsigned int sc_h, unsigned int sc_u,
                        unsigned int dc_h, unsigned int dc_u,
                        boost::timer::nanosecond_type tc_e, boost::timer::nanosecond_type tw_e,
                        boost::timer::nanosecond_type th_e, boost::timer::nanosecond_type s_e,
                        boost::timer::nanosecond_type d_e)
          : work_time(wt),
            db_time(dt),
            aggregation_time(ag),
            time_config_hits(tc_h),
            time_config_unloads(tc_u),
            twopf_kconfig_hits(tw_k_h),
            twopf_kconfig_unloads(tw_k_u),
            threepf_kconfig_hits(th_k_h),
            threepf_kconfig_unloads(th_k_u),
            stats_hits(sc_h),
            stats_unloads(sc_u),
            data_hits(dc_h),
            data_unloads(dc_u),
            time_config_evictions(tc_e),
            twopf_kconfig_evictions(tw_e),
            threepf_kconfig_evictions(th_e),
            stats_evictions(s_e),
            data_evictions(d_e)
          {
          }

        //! deserialization constructor
        output_metadata(Json::Value& reader);


        // SERIALIZE -- implements a 'serializable' interface

      public:

        //! serialize this object
        void serialize(Json::Value& writer) const override;


        // DATA

      public:

        //! total time spent working
        boost::timer::nanosecond_type work_time;

        //! total time spent querying the database
        boost::timer::nanosecond_type db_time;

        //! total time spend aggregating
        boost::timer::nanosecond_type aggregation_time;

        //! total number of time-configuration cache hits
        unsigned int time_config_hits;

        //! total number of twopf k-configuration cache hits
        unsigned int twopf_kconfig_hits;

        //! total number of threepf k-configuration cache hits
        unsigned int threepf_kconfig_hits;

        //! total number of stats cache hits
        unsigned int stats_hits;

        //! total number of data cache hits
        unsigned int data_hits;

        //! total number of time-configuration cache unloads
        unsigned int time_config_unloads;

        //! total number of twopf k-configuration cache unloads
        unsigned int twopf_kconfig_unloads;

        //! total number of threepf k-configuration cache unloads
        unsigned int threepf_kconfig_unloads;

        // total number of stats cache unloads
        unsigned int stats_unloads;

        // total number of data cache unloads
        unsigned int data_unloads;

        // total time spent doing time-config cache evictions
        boost::timer::nanosecond_type time_config_evictions;

        //! total time spent doing twopf k-config cache evictions
        boost::timer::nanosecond_type twopf_kconfig_evictions;

        //! total time spent doing threepf k-config cache evictions
        boost::timer::nanosecond_type threepf_kconfig_evictions;

        //! total time spent doing stats cache evictions
        boost::timer::nanosecond_type stats_evictions;

        //! total time spent doing data cache evictions
        boost::timer::nanosecond_type data_evictions;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_OUTPUT_GROUP_METADATA_DECL_H
