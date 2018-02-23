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
#include <core.h>

#include "finder.h"

#include "boost/filesystem/operations.hpp"
#include "boost/algorithm/string.hpp"


// ******************************************************************


finder& finder::add(boost::filesystem::path p)
  {
    // add path to list, if it isn't already present
    auto t = std::find(this->paths.begin(), this->paths.end(), p);

    if(t == this->paths.end()) this->paths.emplace_back(std::move(p));
    
    return *this;
  }


finder& finder::add_environment_variable(const std::string var, const boost::filesystem::path tail)
  {
    // query value of environment variable
    char* resource_path_cstr = std::getenv(var.c_str());
    
    // return if environment variable did not exist
    if(resource_path_cstr == nullptr) return *this;
    
    std::string template_paths{resource_path_cstr};
    
    // parse environment variable into a : separated list of directories
    for(auto t = boost::algorithm::make_split_iterator(template_paths,
                                                       boost::algorithm::first_finder(":", boost::algorithm::is_equal()));
        t != boost::algorithm::split_iterator<std::string::iterator>(); ++t)
      {
        std::string path = boost::copy_range<std::string>(*t);
        
        boost::filesystem::path rpath(path);
        
        // if path is not absolute, make relative to current working directory
        if(!rpath.is_absolute()) rpath = boost::filesystem::absolute(rpath);
        
        if(!tail.empty()) this->add(rpath / tail);
        else this->add(rpath);
      }
    
    return *this;
  }


finder& finder::add_cwd()
  {
    boost::filesystem::path cwd{ boost::filesystem::initial_path<boost::filesystem::path>() };

    return this->add(cwd);
  }


boost::optional< boost::filesystem::path >
finder::find(const boost::filesystem::path& leaf) const
  {
    if(leaf.is_absolute())
      {
        auto fqpn = boost::filesystem::canonical(leaf);
        
        if(boost::filesystem::exists(leaf)) return fqpn;
        return boost::none;
      }
    
    for(const auto& path : this->paths)
      {
        auto file = path / leaf;
        
        if(boost::filesystem::exists(file)) return boost::filesystem::canonical(file);
      }
    
    return boost::none;
  }
