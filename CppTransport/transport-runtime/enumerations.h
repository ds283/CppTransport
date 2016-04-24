//
// Created by David Seery on 20/08/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_ENUMERATIONS_H
#define CPPTRANSPORT_ENUMERATIONS_H


namespace transport
  {

    enum class twopf_type
      {
        real, imag
      };

    enum class threepf_type
      {
        momentum, Nderiv
      };

    enum class worker_type
      {
        cpu, gpu
      };

  }


#endif //CPPTRANSPORT_ENUMERATIONS_H
