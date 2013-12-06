//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "cpp_printer.h"


namespace cpp
  {

    std::string print(const GiNaC::ex& expr)
      {
        std::ostringstream out;

        out << GiNaC::csrc << expr;

        return(out.str());
      }

  } // namespace cpp
