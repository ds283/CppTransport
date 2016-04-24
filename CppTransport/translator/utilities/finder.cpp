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


#include <fstream>
#include <sstream>
#include <algorithm>

#include "finder.h"

#include "boost/filesystem/operations.hpp"


// ******************************************************************


finder::finder()
  {
    boost::filesystem::path cwd(boost::filesystem::initial_path<boost::filesystem::path>());

    paths.push_back(cwd);
  }


finder::finder(boost::filesystem::path path)
  {
    paths.emplace_back(std::move(path));
  }


// ******************************************************************


void finder::add(boost::filesystem::path p)
  {
    // add path to list, if it isn't already present
    std::list<boost::filesystem::path>::iterator t = std::find(this->paths.begin(), this->paths.end(), p);

    if(t == this->paths.end()) this->paths.emplace_back(std::move(p));
  }


void finder::add(const std::list<boost::filesystem::path>& plist)
  {
    for(const boost::filesystem::path& t : plist)
      {
        this->add(t);
      }
  }


bool finder::fqpn(const boost::filesystem::path& leaf, boost::filesystem::path& fqpn)
  {
    if(leaf.is_absolute())
      {
        fqpn = boost::filesystem::canonical(leaf);
        return(boost::filesystem::exists(leaf));
      }

    bool match = false;

    for(const boost::filesystem::path& path : this->paths)
      {
        boost::filesystem::path file = path / leaf;

        if(boost::filesystem::exists(file))
          {
            match = true;
            fqpn  = boost::filesystem::canonical(file);
            break;
          }
      }

    return(match);
  }
