//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <iostream>
#include <sstream>

#include "basic_error.h"
#include "core.h"


void basic_warn(std::string const msg)
  {
    std::ostringstream out;

    out << CPPTRANSPORT_NAME << ": " << WARNING_TOKEN << msg;
    std::cout << out.str() << std::endl;
  }

void basic_error(std::string const msg)
  {
    std::ostringstream out;

    out << CPPTRANSPORT_NAME << ": " << ERROR_TOKEN << msg;
    std::cout << out.str() << std::endl;
  }
