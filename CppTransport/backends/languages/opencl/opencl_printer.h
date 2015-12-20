// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_OPENCL_PRINTER_H
#define CPPTRANSPORT_OPENCL_PRINTER_H


#include "C_style_printer.h"


namespace opencl
  {

    class opencl_printer: public C_style_printer
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        opencl_printer() = default;

        //! destructor is default
        virtual ~opencl_printer() = default;


        // NO METHODS OVERRIDDEN

      };

  } // namespace opencl


#endif //CPPTRANSPORT_OPENCL_PRINTER_H
