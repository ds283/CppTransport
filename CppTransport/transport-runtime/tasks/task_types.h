//
// Created by David Seery on 19/03/2016.
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

#ifndef CPPTRANSPORT_TASK_TYPES_H
#define CPPTRANSPORT_TASK_TYPES_H


#include <stdexcept>

namespace transport
  {

    enum class task_type { integration, postintegration, output };

    enum class integration_task_type { twopf, threepf };

    enum class postintegration_task_type { twopf, threepf, fNL };


    template <typename number, integration_task_type Type> struct integration_task_traits;

    template <typename number, postintegration_task_type Type> struct postintegration_task_traits;


    inline std::string task_type_to_string(task_type type)
      {
        switch(type)
          {
            case task_type::integration:
              return std::string(CPPTRANSPORT_TASK_TYPE_INTEGRATION);

            case task_type::postintegration:
              return std::string(CPPTRANSPORT_TASK_TYPE_POSTINTEGRATION);

            case task_type::output:
              return std::string(CPPTRANSPORT_TASK_TYPE_OUTPUT);

            default:
              throw std::invalid_argument("Invalid task_type");
          }
      }


    inline std::string task_type_to_string(integration_task_type type)
      {
        switch(type)
          {
            case integration_task_type::twopf:
              return std::string(CPPTRANSPORT_TASK_TYPE_INTEGRATION_TWOPF);

            case integration_task_type::threepf:
              return std::string(CPPTRANSPORT_TASK_TYPE_INTEGRATION_THREEPF);

            default:
              throw std::invalid_argument("Invalid integration_task_type");
          }
      }


    inline std::string task_type_to_string(postintegration_task_type type)
      {
        switch(type)
          {
            case postintegration_task_type::twopf:
              return std::string(CPPTRANSPORT_TASK_TYPE_POSTINTEGRATION_TWOPF);

            case postintegration_task_type::threepf:
              return std::string(CPPTRANSPORT_TASK_TYPE_POSTINTEGRATION_THREEPF);

            case postintegration_task_type::fNL:
              return std::string(CPPTRANSPORT_TASK_TYPE_POSTINTEGRATION_FNL);

            default:
              throw std::invalid_argument("Invalid postintegration_task_type");
          }
      }


  }   // namespace transport


#endif //CPPTRANSPORT_TASK_TYPES_H
