//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __opencl_printer_H_
#define __opencl_printer_H_


#include "C_style_printer.h"


namespace opencl
  {

    class opencl_printer: public C_style_printer
      {
      public:
        std::string ginac  (const GiNaC::ex& expr);
        std::string comment(const std::string tag);
      };

  } // namespace opencl


#endif //__opencl_printer_H_
