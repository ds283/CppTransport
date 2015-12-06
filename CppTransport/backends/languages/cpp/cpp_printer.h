//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __cpp_printer_H_
#define __cpp_printer_H_


#include "C_style_printer.h"

#define CPP_COMMENT_SEPARATOR "//"


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
