//
// Created by David Seery on 05/06/2014.
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


#ifndef CPPTRANSPORT_TASK_HELPER_H
#define CPPTRANSPORT_TASK_HELPER_H

#include <string>
#include <memory>

#include "transport-runtime/serialization/serializable.h"

#include "transport-runtime/concepts/initial_conditions.h"

#include "transport-runtime/tasks/task.h"
#include "transport-runtime/tasks/integration_tasks.h"
#include "transport-runtime/tasks/output_tasks.h"
#include "transport-runtime/tasks/postintegration_tasks.h"

#include "transport-runtime/messages.h"

#include "sqlite3.h"


namespace transport
  {

    namespace integration_task_helper
      {

        template <typename number>
        std::unique_ptr< integration_task<number> > deserialize(const std::string& nm, Json::Value& reader, sqlite3* handle, package_finder<number>& f)
          {
            std::string type = reader[CPPTRANSPORT_NODE_TASK_TYPE].asString();

            // extract initial conditions
            std::string pkg_name = reader[CPPTRANSPORT_NODE_PACKAGE_NAME].asString();
            std::unique_ptr< package_record<number> > record = f(pkg_name);
            initial_conditions<number> ics = record->get_ics();

            if(type == CPPTRANSPORT_NODE_TASK_TYPE_TWOPF)              return std::make_unique< twopf_task<number> >(nm, reader, handle, ics);
            else if(type == CPPTRANSPORT_NODE_TASK_TYPE_THREEPF_CUBIC) return std::make_unique< threepf_cubic_task<number> >(nm, reader, handle, ics);
            else if(type == CPPTRANSPORT_NODE_TASK_TYPE_THREEPF_FLS)   return std::make_unique< threepf_fls_task<number> >(nm, reader, handle, ics);

            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
          }

      }   // namespace integration_task_helper

    namespace output_task_helper
      {

        template <typename number>
        std::unique_ptr< output_task<number> > deserialize(const std::string& nm, Json::Value& reader, derived_product_finder<number>& pfinder)
          {
            std::string type = reader[CPPTRANSPORT_NODE_TASK_TYPE].asString();

            if(type == CPPTRANSPORT_NODE_TASK_TYPE_OUTPUT) return std::make_unique< output_task<number> >(nm, reader, pfinder);

            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
          }

      }   // namespace output_task_helper

    namespace postintegration_task_helper
      {

        template <typename number>
        std::unique_ptr< postintegration_task<number> > deserialize(const std::string& nm, Json::Value& reader, task_finder<number>& f)
          {
            std::string type = reader[CPPTRANSPORT_NODE_TASK_TYPE].asString();

            if     (type == CPPTRANSPORT_NODE_TASK_TYPE_ZETA_TWOPF)   return std::make_unique< zeta_twopf_task<number> >(nm, reader, f);
            else if(type == CPPTRANSPORT_NODE_TASK_TYPE_ZETA_THREEPF) return std::make_unique< zeta_threepf_task<number> >(nm, reader, f);
            else if(type == CPPTRANSPORT_NODE_TASK_TYPE_FNL)          return std::make_unique< fNL_task<number> >(nm, reader, f);

            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
          }

      }   // namespace postintegration_task_helper

  }   // namespace transport


#endif //CPPTRANSPORT_TASK_HELPER_H
