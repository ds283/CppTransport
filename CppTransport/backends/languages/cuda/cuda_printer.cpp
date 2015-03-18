//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "cuda_printer.h"


namespace cuda
  {

    std::string cuda_printer::ginac(const GiNaC::ex& expr)
      {
        return(this->C_style_printer::ginac(expr));
      }

    std::string cuda_printer::comment(const std::string tag)
      {
        return(this->C_style_printer::comment(tag));
      }

  } // namespace cuda
