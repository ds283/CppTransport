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

    // CONSTRUCTOR, DESTRUCTOR

  public:

    language_printer()
      : debug(false)
      {
      }

    virtual ~language_printer() = default;


    // INTERFACE

  public:

    virtual std::string ginac  (const GiNaC::ex& expr) = 0;
    virtual std::string comment(const std::string tag) = 0;

    void set_debug_output(bool g) { this->debug = g; }


    // INTERNAL DATA

  protected:

    bool debug;

  };


#endif //__language_printer_H_
