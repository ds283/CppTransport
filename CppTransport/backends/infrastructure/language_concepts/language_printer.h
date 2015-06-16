//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __language_printer_H_
#define __language_printer_H_


#include <functional>
#include <string>

#include "ginac/ginac.h"


// define the concept of a 'language printer', an agent which knows
// how to format output for a particular language


class language_printer
  {
  public:
    virtual ~language_printer()
      {
      }

    virtual std::string ginac  (const GiNaC::ex& expr) = 0;
    virtual std::string comment(const std::string tag) = 0;
  };


#endif //__language_printer_H_
