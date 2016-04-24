//
// Created by David Seery on 12/06/2013.
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

#ifndef CPPTRANSPORT_FINDER_H
#define CPPTRANSPORT_FINDER_H


#include <iostream>
#include <string>
#include <list>

#include "boost/filesystem/operations.hpp"


class finder
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! default constructor; sets search path to be current working direcotry
    finder();

    //! value constructor; sets search path as specified, but does not include current working directory
    finder(boost::filesystem::path path);

    //! destructor is default
    ~finder() = default;


    // INTERFACE -- ADD PATHS

  public:

    //! add a path to the search list
    void add(boost::filesystem::path p);

    //! add a list of paths to the search list
    void add(const std::list<boost::filesystem::path>& plist);


    // INTERFACE-- FIND FULLY-QUALIFIED PATHS

  public:

    // find fully-qualified path name corresponding to a given leafname
    bool fqpn(const boost::filesystem::path& leaf, boost::filesystem::path& fqpn);


    // INTERNAL DATA

  private:

    //! list of paths to search
    std::list<boost::filesystem::path> paths;

  };


#endif //CPPTRANSPORT_FINDER_H
