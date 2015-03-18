//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "opencl_printer.h"


namespace opencl
  {

    std::string opencl_printer::ginac(const GiNaC::ex& expr)
      {
        return(this->C_style_printer::ginac(expr));
      }

    std::string opencl_printer::comment(const std::string tag)
      {
        return(this->C_style_printer::comment(tag));
      }

  } // namespace opencl
