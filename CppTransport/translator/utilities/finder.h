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

#include "boost/optional.hpp"
#include "boost/filesystem/operations.hpp"


class finder
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! default constructor
    finder() = default;
    
    //! destructor is default
    ~finder() = default;


    // INTERFACE -- ADD PATHS

  public:

    //! add a path to the search list
    finder& add(boost::filesystem::path p);

    //! add a container of paths to the search list
    template <typename Container>
    finder& add(const Container& plist, const boost::filesystem::path tail = boost::filesystem::path{});
    
    //! add the current working directory to the list of search paths
    finder& add_cwd();
    
    //! add a :-separated list from a system environment variable, optionally offset
    //! by a fixed relative path
    finder& add_environment_variable(const std::string var, const boost::filesystem::path tail = boost::filesystem::path{});


    // INTERFACE -- FIND FULLY-QUALIFIED PATHS

  public:

    // find fully-qualified path name corresponding to a given leafname;
    // if present, returns the FQPN, otherwise returns boost::none
    boost::optional< boost::filesystem::path > find(const boost::filesystem::path& leaf) const;


    // INTERNAL DATA

  private:

    //! list of paths to search
    std::list<boost::filesystem::path> paths;

  };


template <typename Container>
finder& finder::add(const Container& plist, const boost::filesystem::path tail)
  {
    for(const auto& t : plist)
      {
        if(!tail.empty()) this->add(t / tail);
        else this->add(t);
      }
    
    return *this;
  }


#endif //CPPTRANSPORT_FINDER_H
