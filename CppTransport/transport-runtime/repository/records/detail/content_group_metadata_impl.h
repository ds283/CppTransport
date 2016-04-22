//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CONTENT_GROUP_METADATA_IMPL_H
#define CPPTRANSPORT_CONTENT_GROUP_METADATA_IMPL_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_GROUP = "integration-metadata";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_WALLCLOCK_TIME = "total-wallclock-time";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_AGG_TIME = "total-aggregation-time";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_INT_TIME = "total-integration-time";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_INT_TIME = "global-min-integration-time";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_INT_TIME = "global-max-integration-time";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_BATCH_TIME = "total-batching-time";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_BATCH_TIME = "global-min-batching-time";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_BATCH_TIME = "global-max-batching-time";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_NUM_CONFIGURATIONS = "configurations-processed";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_NUM_FAILURES = "configurations-failed";
    constexpr auto CPPTRANSPORT_NODE_TIMINGDATA_NUM_REFINED = "configurations-refined";

    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_GROUP = "output-metadata";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_TOTAL_WALLCLOCK_TIME = "total-wallclock-time";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_TOTAL_DB_TIME = "total-db-time";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_TOTAL_AGG_TIME = "total-aggregation-time";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_HITS = "time-cache-hits";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_UNLOADS = "time-cache-unloads";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_HITS = "twopf-cache-hits";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_UNLOADS = "twopf-cache-unloads";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_HITS = "threepf-cache-hits";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_UNLOADS = "threepf-cache-unloads";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_HITS = "data-cache-hits";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_UNLOADS = "data-cache-unloads";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_STATS_CACHE_HITS = "stats-cache-hits";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_STATS_CACHE_UNLOADS = "stats-cache-unloads";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_EVICTIONS = "time-cache-evictions";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_EVICTIONS = "twopf-cache-evictions";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_EVICTIONS = "threepf-cache-evictions";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_EVICTIONS = "data-cache-evictions";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTDATA_STATS_CACHE_EVICTIONS = "stats-cache-evictions";


    // OUTPUT METADATA


    output_metadata::output_metadata(Json::Value& reader)
      {
        work_time                 = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TOTAL_WALLCLOCK_TIME].asLargestInt();
        db_time                   = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TOTAL_DB_TIME].asLargestInt();
        aggregation_time          = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TOTAL_AGG_TIME].asLargestInt();
        time_config_hits          = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_HITS].asUInt();
        time_config_unloads       = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_UNLOADS].asUInt();
        twopf_kconfig_hits        = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_HITS].asUInt();
        twopf_kconfig_unloads     = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_UNLOADS].asUInt();
        threepf_kconfig_hits      = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_HITS].asUInt();
        threepf_kconfig_unloads   = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_UNLOADS].asUInt();
        data_hits                 = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_HITS].asUInt();
        data_unloads              = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_UNLOADS].asUInt();
        stats_hits                = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_STATS_CACHE_HITS].asUInt();
        stats_unloads             = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_STATS_CACHE_UNLOADS].asUInt();
        time_config_evictions     = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_EVICTIONS].asLargestInt();
        twopf_kconfig_evictions   = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_EVICTIONS].asLargestInt();
        threepf_kconfig_evictions = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_EVICTIONS].asLargestInt();
        data_evictions            = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_EVICTIONS].asLargestInt();
        stats_evictions           = reader[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_STATS_CACHE_EVICTIONS].asLargestInt();
      }


    void output_metadata::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TOTAL_WALLCLOCK_TIME]    = static_cast<Json::LargestInt>(this->work_time);
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TOTAL_DB_TIME]           = static_cast<Json::LargestInt>(this->db_time);
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TOTAL_AGG_TIME]          = static_cast<Json::LargestInt>(aggregation_time);
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_HITS]         = this->time_config_hits;
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_UNLOADS]      = this->time_config_unloads;
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_HITS]        = this->twopf_kconfig_hits;
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_UNLOADS]     = this->twopf_kconfig_unloads;
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_HITS]      = this->threepf_kconfig_hits;
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_UNLOADS]   = this->threepf_kconfig_unloads;
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_STATS_CACHE_HITS]        = this->stats_hits;
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_STATS_CACHE_UNLOADS]     = this->stats_unloads;
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_HITS]         = this->data_hits;
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_UNLOADS]      = this->data_unloads;
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_EVICTIONS]    = static_cast<Json::LargestInt>(this->time_config_evictions);
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_EVICTIONS]   = static_cast<Json::LargestInt>(this->twopf_kconfig_evictions);
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_EVICTIONS] = static_cast<Json::LargestInt>(this->threepf_kconfig_evictions);
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_STATS_CACHE_EVICTIONS]   = static_cast<Json::LargestInt>(this->stats_evictions);
        writer[CPPTRANSPORT_NODE_OUTPUTDATA_GROUP][CPPTRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_EVICTIONS]    = static_cast<Json::LargestInt>(this->data_evictions);
      }


    // INTEGRATION METADATA


    integration_metadata::integration_metadata(Json::Value& reader)
      {
        total_wallclock_time        = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_WALLCLOCK_TIME].asLargestInt();
        total_aggregation_time      = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_AGG_TIME].asLargestInt();
        total_integration_time      = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_INT_TIME].asLargestInt();
        global_min_integration_time = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_INT_TIME].asLargestInt();
        global_max_integration_time = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_INT_TIME].asLargestInt();
        total_batching_time         = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_BATCH_TIME].asLargestInt();
        global_min_batching_time    = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_BATCH_TIME].asLargestInt();
        global_max_batching_time    = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_BATCH_TIME].asLargestInt();
        total_configurations        = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_NUM_CONFIGURATIONS].asUInt();
        total_failures              = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_NUM_FAILURES].asUInt();
        total_refinements           = reader[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_NUM_REFINED].asUInt();
      }


    void integration_metadata::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_WALLCLOCK_TIME]  = static_cast<Json::LargestInt>(this->total_wallclock_time);
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_AGG_TIME]        = static_cast<Json::LargestInt>(this->total_aggregation_time);
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_INT_TIME]        = static_cast<Json::LargestInt>(this->total_integration_time);
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_INT_TIME]   = static_cast<Json::LargestInt>(this->global_min_integration_time);
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_INT_TIME]   = static_cast<Json::LargestInt>(this->global_max_integration_time);
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_TOTAL_BATCH_TIME]      = static_cast<Json::LargestInt>(this->total_batching_time);
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_BATCH_TIME] = static_cast<Json::LargestInt>(this->global_min_batching_time);
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_BATCH_TIME] = static_cast<Json::LargestInt>(this->global_max_batching_time);
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_NUM_CONFIGURATIONS]    = this->total_configurations;
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_NUM_FAILURES]          = this->total_failures;
        writer[CPPTRANSPORT_NODE_TIMINGDATA_GROUP][CPPTRANSPORT_NODE_TIMINGDATA_NUM_REFINED]           = this->total_refinements;
      }

  }   // namespace transport


#endif //CPPTRANSPORT_CONTENT_GROUP_METADATA_IMPL_H
