//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __cpp_printer_H_
#define __cpp_printer_H_


#include "C_style_printer.h"


constexpr auto         CPP_COMMENT_SEPARATOR = "//";
constexpr auto         CPP_OPEN_BRACE        = "{";
constexpr auto         CPP_CLOSE_BRACE       = "}";
constexpr unsigned int CPP_BRACE_INDENT      = 1;
constexpr unsigned int CPP_BLOCK_INDENT      = 3;


namespace cpp
  {

    class cpp_printer: public C_style_printer
      {

      public:

        cpp_printer() = default;

        virtual ~cpp_printer() = default;


        // no need to override ginac() and comment() methods, which can
        // pass directly through to those inherited from C_style_printer

      };

  } // namespace cpp


#endif //__cpp_printer_H_
