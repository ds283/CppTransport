// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __opencl_printer_H_
#define __opencl_printer_H_


#include "C_style_printer.h"


constexpr auto         OPENCL_COMMENT_SEPARATOR = "//";
constexpr auto         OPENCL_OPEN_BRACE        = "{";
constexpr auto         OPENCL_CLOSE_BRACE       = "}";
constexpr unsigned int OPENCL_BRACE_INDENT      = 1;
constexpr unsigned int OPENCL_BLOCK_INDENT      = 3;


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
