//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __cpp_printer_H_
#define __cpp_printer_H_


#include "C_style_printer.h"


namespace cpp
  {

    class cpp_printer: public C_style_printer
      {
      public:
        cpp_printer()
          {
          }

        ~cpp_printer()
          {
          }

        std::string ginac  (const GiNaC::ex& expr);
        std::string comment(const std::string tag);
      };

  } // namespace cpp


#endif //__cpp_printer_H_
