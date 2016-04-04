//
// Created by David Seery on 19/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_TASK_TYPES_H
#define CPPTRANSPORT_TASK_TYPES_H

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
          }
      }


  }   // namespace transport


#endif //CPPTRANSPORT_TASK_TYPES_H
