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

  }   // namespace transport


#endif //CPPTRANSPORT_TASK_TYPES_H
