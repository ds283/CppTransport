//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_METADATA_IMPL_H
#define CPPTRANSPORT_METADATA_IMPL_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_NODE_METADATA_GROUP = "metadata";
    constexpr auto CPPTRANSPORT_NODE_METADATA_CREATED = "created";
    constexpr auto CPPTRANSPORT_NODE_METADATA_EDITED = "edited";
    constexpr auto CPPTRANSPORT_NODE_METADATA_RUNTIME_API = "api";

    // REPOSITORY METADATA METHODS


    record_metadata::record_metadata()
      : creation_time(boost::posix_time::second_clock::universal_time()),
        last_edit_time(boost::posix_time::second_clock::universal_time()),    // don't initialize from creation_time; order of initialization depends on order of *declaration* in class, and that might change
        runtime_api(CPPTRANSPORT_RUNTIME_API_VERSION)
      {
      }


    record_metadata::record_metadata(Json::Value& reader)
      {
        std::string ctime_str = reader[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_CREATED].asString();
        this->creation_time = boost::posix_time::from_iso_string(ctime_str);

        std::string etime_str = reader[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_EDITED].asString();
        this->last_edit_time = boost::posix_time::from_iso_string(etime_str);

        this->runtime_api = reader[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_RUNTIME_API].asUInt();
      }


    void record_metadata::serialize(Json::Value& writer) const
      {
        Json::Value metadata(Json::objectValue);

        writer[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_CREATED] = boost::posix_time::to_iso_string(this->creation_time);
        writer[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_EDITED] = boost::posix_time::to_iso_string(this->last_edit_time);
        writer[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_RUNTIME_API] = this->runtime_api;
      }

  }


#endif //CPPTRANSPORT_METADATA_IMPL_H
