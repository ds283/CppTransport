// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __opencl_printer_H_
#define __opencl_printer_H_


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


#endif //__opencl_printer_H_
