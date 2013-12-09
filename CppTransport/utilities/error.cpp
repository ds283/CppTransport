//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <iostream>
#include <sstream>

#include "core.h"
#include "basic_error.h"
#include "error.h"


// ******************************************************************


void warn(std::string const msg)
  {
    basic_warn(msg);
  }


void error(std::string const msg)
  {
    basic_error(msg);
  }


void warn(std::string const msg, unsigned int line, filestack* path)
  {
    warn(msg, line, path, WARN_PATH_LEVEL);
  }


void error(std::string const msg, unsigned int line, filestack* path)
  {
    error(msg, line, path, ERROR_PATH_LEVEL);
  }


void warn(std::string const msg, unsigned int line, filestack* path, unsigned int level)
  {
    std::ostringstream out;

    out << ERROR_MESSAGE_AT_LINE << " " << line << path->write(level) << std::endl;
    out << ERROR_MESSAGE_WRAP_PAD << msg << std::endl;

    basic_warn(out.str());
  }


void error(std::string const msg, unsigned int line, filestack* path, unsigned int level)
  {
    std::ostringstream out;

    out << ERROR_MESSAGE_AT_LINE << " " << line << path->write(level) << std::endl;
    out << ERROR_MESSAGE_WRAP_PAD << msg << std::endl;

    basic_error(out.str());
  }
