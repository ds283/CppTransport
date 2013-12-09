//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "cpp_printer.h"


namespace cpp
  {

    std::string cpp_printer::ginac(const GiNaC::ex& expr)
      {
        return this->C_style_printer::ginac(expr);
      }

    std::string cpp_printer::comment(const std::string tag)
      {
        return this->C_style_printer::comment(tag);
      }

  } // namespace cpp
