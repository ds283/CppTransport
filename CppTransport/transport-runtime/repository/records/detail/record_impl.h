//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_RECORD_IMPL_H
#define CPPTRANSPORT_RECORD_IMPL_H


#include "transport-runtime/messages.h"
#include "transport-runtime/defaults.h"
#include "transport-runtime/exceptions.h"


namespace transport
  {

    constexpr auto CPPTRANSPORT_NODE_RECORD_NAME                  = "name";

    constexpr auto CPPTRANSPORT_NODE_RECORD_TYPE                  = "record-type";
    constexpr auto CPPTRANSPORT_NODE_RECORD_PACKAGE               = "package";
    constexpr auto CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK      = "integration-task";
    constexpr auto CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK  = "postintegration-task";
    constexpr auto CPPTRANSPORT_NODE_RECORD_OUTPUT_TASK           = "output-task";
    constexpr auto CPPTRANSPORT_NODE_RECORD_DERIVED_PRODUCT       = "derived-product";
    constexpr auto CPPTRANSPORT_NODE_RECORD_CONTENT               = "content-group";


    // GENERIC REPOSITORY RECORD


    repository_record::repository_record(repository_record::handler_package pkg)
      : metadata(pkg.env.get_userid()),
        handlers(std::move(pkg))
      {
        name = boost::posix_time::to_iso_string(metadata.get_creation_time());
      }


    repository_record::repository_record(const std::string& nm, repository_record::handler_package pkg)
      : name(nm),
        metadata(pkg.env.get_userid()),
        handlers(std::move(pkg))
      {
      }


    repository_record::repository_record(Json::Value& reader, repository_record::handler_package pkg)
      : metadata(reader),
        handlers(std::move(pkg))
      {
        name = reader[CPPTRANSPORT_NODE_RECORD_NAME].asString();
      }


    void repository_record::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_RECORD_NAME] = this->name;
        this->metadata.serialize(writer);
      }


    void repository_record::commit()
      {
        if(this->handlers.mgr)
          {
            this->handlers.commit(*this, *this->handlers.mgr);
          }
        else // no authorization to commit this record; throw an exception
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_COMMIT_READONLY << " '" << this->name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
      }

  }   // namespace transport


#endif //CPPTRANSPORT_RECORD_IMPL_H
