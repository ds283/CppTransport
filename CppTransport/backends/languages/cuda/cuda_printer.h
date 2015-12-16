//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __cuda_printer_H_
#define __cuda_printer_H_


#include "C_style_printer.h"


namespace cuda
  {

    class cuda_printer: public C_style_printer
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        cuda_printer() = default;

        //! destructor is default
        virtual ~cuda_printer() = default;


        // NO METHODS OVERRIDDEN

      };

  } // namespace cuda


#endif //__cuda_printer_H_
