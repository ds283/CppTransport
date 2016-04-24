//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//



#ifndef CPPTRANSPORT_CPP_PRINTER_H
#define CPPTRANSPORT_CPP_PRINTER_H


#include "C_style_printer.h"


namespace cpp
  {

    class cpp_printer: public C_style_printer
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        cpp_printer() = default;

        //! destructor is default
        virtual ~cpp_printer() = default;


        // NO METHODS OVERRIDDEN

      };

  } // namespace cpp


#endif //CPPTRANSPORT_CPP_PRINTER_H
