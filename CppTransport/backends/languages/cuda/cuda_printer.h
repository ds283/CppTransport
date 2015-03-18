//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __cuda_printer_H_
#define __cuda_printer_H_


#include "C_style_printer.h"

#define CUDA_COMMENT_SEPARATOR "//"

namespace cuda
  {

    class cuda_printer: public C_style_printer
      {
      public:
        std::string ginac  (const GiNaC::ex& expr);
        std::string comment(const std::string tag);
      };

  } // namespace cuda


#endif //__cuda_printer_H_
