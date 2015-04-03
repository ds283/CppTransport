//
// Created by David Seery on 05/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __task_helper_H_
#define __task_helper_H_

#include <string>
#include <memory>

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/concepts/initial_conditions.h"

#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"
#include "transport-runtime-api/tasks/postintegration_tasks.h"

#include "transport-runtime-api/messages.h"


namespace transport
  {

    namespace integration_task_helper
      {

        template <typename number>
        integration_task<number>* deserialize(const std::string& nm, Json::Value& reader, typename repository_finder<number>::package_finder& f)
          {
            integration_task<number>* rval = nullptr;

            std::string type = reader[__CPP_TRANSPORT_NODE_TASK_TYPE].asString();

            // extract initial conditions
            std::string pkg_name = reader[__CPP_TRANSPORT_NODE_PACKAGE_NAME].asString();
            std::unique_ptr< package_record<number> > record(f(pkg_name));
            initial_conditions<number> ics = record->get_ics();

            if(type == __CPP_TRANSPORT_NODE_TASK_TYPE_TWOPF)              rval = new twopf_task<number>(nm, reader, ics);
            else if(type == __CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF_CUBIC) rval = new threepf_cubic_task<number>(nm, reader, ics);
            else if(type == __CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF_FLS)   rval = new threepf_fls_task<number>(nm, reader, ics);
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
                throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
              }

            return(rval);
          }

      }   // namespace integration_task_helper

    namespace output_task_helper
      {

        template <typename number>
        output_task<number>* deserialize(const std::string& nm, Json::Value& reader, typename repository_finder<number>::derived_product_finder& pfinder)
          {
            output_task<number>* rval = nullptr;

            std::string type = reader[__CPP_TRANSPORT_NODE_TASK_TYPE].asString();

            if(type == __CPP_TRANSPORT_NODE_TASK_TYPE_OUTPUT) rval = new output_task<number>(nm, reader, pfinder);
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
                throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
              }

            return(rval);
          }

      }   // namespace output_task_helper

    namespace postintegration_task_helper
      {

        template <typename number>
        postintegration_task<number>* deserialize(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& f)
          {
            postintegration_task<number>* rval = nullptr;

            std::string type = reader[__CPP_TRANSPORT_NODE_TASK_TYPE].asString();

            if     (type == __CPP_TRANSPORT_NODE_TASK_TYPE_ZETA_TWOPF)   rval = new zeta_twopf_task<number>(nm, reader, f);
            else if(type == __CPP_TRANSPORT_NODE_TASK_TYPE_ZETA_THREEPF) rval = new zeta_threepf_task<number>(nm, reader, f);
            else if(type == __CPP_TRANSPORT_NODE_TASK_TYPE_FNL)          rval = new fNL_task<number>(nm, reader, f);
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
                throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
              }

            return(rval);
          }

      }   // namespace postintegration_task_helper

  }   // namespace transport


#endif //__task_helper_H_
