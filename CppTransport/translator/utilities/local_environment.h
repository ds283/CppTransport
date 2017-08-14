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

#ifndef CPPTRANSPORT_LOCAL_ENVIRONMENT_H
#define CPPTRANSPORT_LOCAL_ENVIRONMENT_H


#include <list>


#include "boost/filesystem/operations.hpp"
#include "boost/optional.hpp"


class local_environment
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    local_environment();

    //! destructor is default
    ~local_environment() = default;


    // TERMINAL PROPERTIES

  public:

    //! does the terminal support colourized output?
    bool has_colour_terminal_support() const { return(this->colour_terminal); }


    // ENVIRONMENT PATHS

  public:
    
    //! get path to config file, if it exists
    boost::optional< boost::filesystem::path > config_file_path() const;


    // INTERNAL API

  private:

    //! detect terminal colour support
    void set_terminal_colour_support();

    //! detect home directory
    void set_home_directory();
    
    
    // INTERNAL DATA

  private:

    // TERMINAL PROPERTIES

    //! does terminal support colour output?
    bool colour_terminal;


    // ENVIRONMENT PATHS

    //! user home directory
    boost::optional< boost::filesystem::path > home;
    
  };


#endif //CPPTRANSPORT_LOCAL_ENVIRONMENT_H
