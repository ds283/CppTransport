//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_CUDA_PRINTER_H
#define CPPTRANSPORT_CUDA_PRINTER_H


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


#endif //CPPTRANSPORT_CUDA_PRINTER_H
