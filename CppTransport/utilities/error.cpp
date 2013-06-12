//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <iostream>
#include <sstream>

#include "core.h"
#include "error.h"

// ******************************************************************

void warn(std::string msg)
  {
    std::ostringstream out;

    out << CPPTRANSPORT_NAME << ": " << WARNING_TOKEN << msg;
    std::cerr << out.str() << "\n";
  }

void error(std::string msg)
  {
    std::ostringstream out;

    out << CPPTRANSPORT_NAME << ": " << ERROR_TOKEN << msg;
    std::cerr << out.str() << "\n";
  }

void warn(std::string msg, unsigned int line, std::list<std::string>& path)
  {
    warn(msg, line, path, WARN_PATH_LEVEL);
  }

void error(std::string msg, unsigned int line, std::list<std::list>& path)
  {
    error(msg, line, path, ERROR_PATH_LEVEL);
  }

void warn(std::string msg, unsigned int line, std::list<std::string>& path, unsigned int level)
  {
    if(path.size() < level)
      {
        level = path.size();
      }

    std::ostringstream out;

    out << CPPTRANSPORT_NAME << ": " << WARNING_TOKEN << msg << " at line " << line;
    if(level >= 1)
      {
        out << " of '" << path[0] << "'";
      }
    std::cerr << out.str() << "\n";

    int i;
    for(i = 1; i < level; i++)
      {
        std::cerr << "  included from file '" << path[i] << "'\n";
      }
  }

void error(std::string msg, unsigned int line, std::list<std::string>& path, unsigned int level)
  {
    if(path.size() < level)
      {
        level = path.size();
      }

    std::ostringstream out;

    out << CPPTRANSPORT_NAME << ": " << ERROR_TOKEN << msg << " at line " << line;
    if(level >= 1)
      {
        out << " of '" << path[0] << "'";
      }
    std::cerr << out.str() << "\n";

    int i;
    for(i = 1; i < level; i++)
      {
        std::cerr << "  included from file '" << path[i] << "'\n";
      }
  }
