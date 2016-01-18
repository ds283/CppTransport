//
// Created by David Seery on 29/11/2015.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
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
    char* template_paths_cstr = std::getenv(TEMPLATE_PATHS_ENV);

    if(template_paths_cstr == nullptr) return;

    std::string template_paths(template_paths_cstr);

    for(boost::algorithm::split_iterator<std::string::iterator> t = boost::algorithm::make_split_iterator(template_paths, boost::algorithm::first_finder(":", boost::algorithm::is_equal()));
        t != boost::algorithm::split_iterator<std::string::iterator>(); ++t)
      {
        std::string path = boost::copy_range<std::string>(*t);
        this->template_search.emplace_back(path);
      }
  }


boost::optional<boost::filesystem::path> local_environment::config_file_path() const
  {
    if(!this->home) return boost::optional<boost::filesystem::path>();

    boost::filesystem::path config_path = *this->home;

    return config_path / CONFIG_FILE_LOCATION;
  }
