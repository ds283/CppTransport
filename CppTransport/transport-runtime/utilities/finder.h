//
// Created by David Seery on 14/08/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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
#include "boost/algorithm/string.hpp"


namespace transport
  {

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
        finder& add(const Container& plist);
        
        //! add the current working directory to the list of search paths
        finder& add_cwd();
        
        //! add a :-separated list from a system environment variable, optionally offset
        //! by a fixed relative path
        finder& add_environment_variable(const std::string var, const boost::filesystem::path tail = boost::filesystem::path{});
        
        
        // INTERFACE -- FIND FULLY-QUALIFIED PATHS
      
      public:
        
        // find fully-qualified path name corresponding to a given leafname;
        // if present, returns the FQPN, otherwise returns boost::none
        boost::optional< boost::filesystem::path >
        find(const boost::filesystem::path& leaf);
        
        
        // INTERNAL DATA
      
      private:
        
        //! list of paths to search
        std::list<boost::filesystem::path> paths;
        
      };
    
    
    template <typename Container>
    finder& finder::add(const Container& plist)
      {
        for(const auto& t : plist)
          {
            this->add(t);
          }
        
        return *this;
      }
    
    
    inline finder& finder::add(boost::filesystem::path p)
      {
        // add path to list, if it isn't already present
        auto t = std::find(this->paths.begin(), this->paths.end(), p);
        
        if(t == this->paths.end()) this->paths.emplace_back(std::move(p));
        
        return *this;
      }
    
    
    inline finder& finder::add_environment_variable(const std::string var, const boost::filesystem::path leaf)
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
            
            // add 'templates' directory to each root
            boost::filesystem::path rpath(path);
            
            // if path is not absolute, make relative to current working directory
            if(!rpath.is_absolute()) rpath = boost::filesystem::absolute(rpath);
            
            if(!leaf.empty()) this->add(rpath / leaf);
            else this->add(rpath);
          }
        
        return *this;
      }
    
    
    inline finder& finder::add_cwd()
      {
        boost::filesystem::path cwd{ boost::filesystem::initial_path<boost::filesystem::path>() };
        
        return this->add(cwd);
      }
    
    
    inline boost::optional< boost::filesystem::path >
    finder::find(const boost::filesystem::path& leaf)
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
    
  }   // namespace transport


#endif //CPPTRANSPORT_FINDER_H
