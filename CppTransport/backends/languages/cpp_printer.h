//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __cpp_printer_H_
#define __cpp_printer_H_


#include <string>

#include "ginac/ginac.h"


namespace cpp
  {

    std::string print(const GiNaC::ex& expr);

  } // namespace cpp


#endif //__cpp_printer_H_
