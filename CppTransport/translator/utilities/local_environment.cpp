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
    this->set_template_search_paths();
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


void local_environment::set_template_search_paths()
  {
    char* resource_path_cstr = std::getenv(CPPTRANSPORT_PATH_ENV);

    if(resource_path_cstr == nullptr) return;

    std::string template_paths(resource_path_cstr);

    // parse environment variable into a : separated list of directories
    for(boost::algorithm::split_iterator<std::string::iterator> t = boost::algorithm::make_split_iterator(template_paths, boost::algorithm::first_finder(":", boost::algorithm::is_equal()));
        t != boost::algorithm::split_iterator<std::string::iterator>(); ++t)
      {
        std::string path = boost::copy_range<std::string>(*t);

        // add 'templates' directory to each root
        boost::filesystem::path rpath(path);

        // if path is not absolute, make relative to current working directory
        if(!rpath.is_absolute()) rpath = boost::filesystem::absolute(rpath);

        this->template_search.emplace_back(rpath / CPPTRANSPORT_TEMPLATE_PATH);
      }
  }


boost::optional<boost::filesystem::path> local_environment::config_file_path() const
  {
    if(!this->home) return boost::optional<boost::filesystem::path>();

    boost::filesystem::path config_path = *this->home;

    return config_path / CONFIG_FILE_LOCATION;
  }
