//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <iostream>
#include <sstream>

#include <cstdlib>

#include "core.h"
#include "error.h"


#define ANSI_BOLD_RED     "\033[1;31m"
#define ANSI_BOLD_GREEN   "\033[1;32m"
#define ANSI_BOLD_MAGENTA "\033[1;35m"
#define ANSI_NORMAL       "\033[0m"

// ******************************************************************


static bool colour_output = true;


void set_up_error_environment(void)
  {
    char* term_type_cstr = std::getenv("TERM");

    if(term_type_cstr == nullptr)
      {
        colour_output = false;
        return;
      }

    std::string term_type(term_type_cstr);

    colour_output = term_type == "xterm"
      || term_type == "xterm-color"
      || term_type == "xterm-256color"
      || term_type == "screen"
      || term_type == "linux"
      || term_type == "cygwin";
  }


void disable_colour_errors(void)
  {
    colour_output = false;
  }


// ******************************************************************


void warn(std::string const msg)
  {
    std::ostringstream out;

    out << CPPTRANSPORT_NAME << ": " << (colour_output ? ANSI_BOLD_MAGENTA : "") << WARNING_TOKEN << (colour_output ? ANSI_NORMAL : "") << msg;
    std::cout << out.str() << std::endl;
  }


void error(std::string const msg)
  {
    std::ostringstream out;

    out << CPPTRANSPORT_NAME << ": " << (colour_output ? ANSI_BOLD_MAGENTA : "") << ERROR_TOKEN << (colour_output ? ANSI_NORMAL : "") << msg;
    std::cout << out.str() << std::endl;
  }


void warn(std::string const msg, std::shared_ptr<filestack> path)
  {
    warn(msg, path, WARN_PATH_LEVEL);
  }


void error(std::string const msg, std::shared_ptr<filestack> path)
  {
    error(msg, path, ERROR_PATH_LEVEL);
  }


void warn(std::string const msg, std::shared_ptr<filestack> path, unsigned int level)
  {
    std::ostringstream out;

    if(path) out << ERROR_MESSAGE_AT_LINE << " " << path->write(level) << std::endl << ERROR_MESSAGE_WRAP_PAD;
    out << msg;

    warn(out.str());
  }


void error(std::string const msg, std::shared_ptr<filestack> path, unsigned int level)
  {
    std::ostringstream out;

    if(path) out << ERROR_MESSAGE_AT_LINE << " " << path->write(level) << std::endl << ERROR_MESSAGE_WRAP_PAD;
    out << msg;

    error(out.str());
  }


void warn(std::string const msg, std::shared_ptr<filestack> path, std::shared_ptr<std::string> line, unsigned int char_pos)
  {
    warn(msg, path, line, char_pos, WARN_PATH_LEVEL);
  }


void error(std::string const msg, std::shared_ptr<filestack> path, std::shared_ptr<std::string> line, unsigned int char_pos)
  {
    error(msg, path, line, char_pos, ERROR_PATH_LEVEL);
  }


void warn(std::string const msg, std::shared_ptr<filestack> path, std::shared_ptr<std::string> line, unsigned int char_pos, unsigned int level)
  {
    std::ostringstream out;

    if(path) out << ERROR_MESSAGE_AT_LINE << " " << path->write(level) << std::endl;
    out << ERROR_MESSAGE_WRAP_PAD << (colour_output ? ANSI_BOLD_RED : "") << msg << (colour_output ? ANSI_NORMAL : "") << std::endl;

    out << ERROR_MESSAGE_WRAP_PAD << *line << std::endl;

    out << ERROR_MESSAGE_WRAP_PAD;
    for(unsigned int i = 0; i < char_pos; ++i)
      {
        out << " ";
      }
    out << (colour_output ? ANSI_BOLD_GREEN : "") << "^" << (colour_output ? ANSI_NORMAL : "");

    warn(out.str());
  }


void error(std::string const msg, std::shared_ptr<filestack> path, std::shared_ptr<std::string> line, unsigned int char_pos, unsigned int level)
  {
    std::ostringstream out;

    if(path) out << ERROR_MESSAGE_AT_LINE << " " << path->write(level) << std::endl;
    out << ERROR_MESSAGE_WRAP_PAD << (colour_output ? ANSI_BOLD_RED : "") << msg << (colour_output ? ANSI_NORMAL : "") << std::endl;
    out << ERROR_MESSAGE_WRAP_PAD << *line << std::endl;

    out << ERROR_MESSAGE_WRAP_PAD;
    for(unsigned int i = 0; i < char_pos; ++i)
      {
        out << " ";
      }
    out << (colour_output ? ANSI_BOLD_GREEN : "") << "^" << (colour_output ? ANSI_NORMAL : "");

    error(out.str());
  }
