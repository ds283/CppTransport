//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __stepper_H_
#define __stepper_H_


#include <string>


struct stepper
  {
    double      abserr;
    double      relerr;
    double      stepsize;
    std::string name;
  };


#endif //__stepper_H_
