//
// Created by David Seery on 29/11/2015.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#include <cstdlib>
#include <string>

#include "core.h"
#include "local_environment.h"

#include "boost/range.hpp"
#include "boost/algorithm/string.hpp"


local_environment::local_environment()
  : colour_terminal(false)
  {
    this->set_terminal_colour_support();
    this->set_home_directory();
  }


void local_environment::set_terminal_colour_support()
  {
    // determine if terminal supports colour output
    char* term_type_cstr = std::getenv("TERM");

    if(term_type_cstr == nullptr)
      {
        colour_terminal = false;
        return;
      }

    std::string term_type(term_type_cstr);

    colour_terminal = term_type == "xterm"
                      || term_type == "xterm-color"
                      || term_type == "xterm-256color"
                      || term_type == "screen"
                      || term_type == "linux"
                      || term_type == "cygwin";
  }


void local_environment::set_home_directory()
  {
    char* home_cstr = std::getenv(HOME_ENV);

    if(home_cstr == nullptr) return;

    std::string home_path(home_cstr);
    this->home = boost::filesystem::path(home_path);
  }


boost::optional<boost::filesystem::path> local_environment::config_file_path() const
  {
    if(!this->home) return boost::optional<boost::filesystem::path>();

    boost::filesystem::path config_path = *this->home;

    return config_path / CONFIG_FILE_LOCATION;
  }
