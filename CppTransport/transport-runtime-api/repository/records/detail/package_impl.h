//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_PACKAGE_IMPL_H
#define CPPTRANSPORT_PACKAGE_IMPL_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_NODE_INITIAL_CONDITIONS = "package-data";


    // PACKAGE RECORD


    template <typename number>
    package_record<number>::package_record(const initial_conditions<number>& i, repository_record::handler_package& pkg)
      : repository_record(i.get_name(), pkg),
        ics(i)
      {
      }


    template <typename number>
    package_record<number>::package_record(Json::Value& reader, model_finder<number>& f,
                                           repository_record::handler_package& pkg)
      : repository_record(reader, pkg),
        ics(this->name, reader[CPPTRANSPORT_NODE_INITIAL_CONDITIONS], f)        // name gets deserialized by repository_record, so is safe to use here
      {
      }


    template <typename number>
    void package_record<number>::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_RECORD_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_PACKAGE);

        Json::Value ics_package(Json::objectValue);
        this->ics.serialize(ics_package);
        writer[CPPTRANSPORT_NODE_INITIAL_CONDITIONS] = ics_package;

        this->repository_record::serialize(writer);
      }

  }   // namespace transport


#endif //CPPTRANSPORT_PACKAGE_IMPL_H
