//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_STEPPER_H
#define CPPTRANSPORT_STEPPER_H


#include <string>


struct stepper
  {
    double      abserr;
    double      relerr;
    double      stepsize;
    std::string name;
  };


#endif //CPPTRANSPORT_STEPPER_H
