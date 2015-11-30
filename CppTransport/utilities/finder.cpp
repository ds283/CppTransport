//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <fstream>
#include <sstream>
#include <algorithm>

#include "finder.h"

#include "boost/filesystem/operations.hpp"


// ******************************************************************


finder::finder()
  {
    using namespace boost::filesystem;

    path cwd(initial_path<path>());

    paths.push_back(cwd.string());
  }


finder::finder(std::string path)
  {
    paths.push_back(path);
  }


// ******************************************************************


void finder::add(const std::string& p)
  {
    // add path to list, if it isn't already present
    std::list<std::string>::iterator t = std::find(this->paths.begin(), this->paths.end(), p);

    if(t == this->paths.end()) this->paths.emplace_back(p);
  }


void finder::add(const std::list<std::string>& plist)
  {
    for(const std::string& t : plist)
      {
        this->add(t);
      }
  }


bool finder::fqpn(const std::string leaf, std::string& fqpn)
  {
    bool match = false;

    for(const std::string& t : this->paths)
      {
        boost::filesystem::path file = boost::filesystem::path(t) / boost::filesystem::path(leaf);

        if(boost::filesystem::exists(file))
          {
            match = true;
            fqpn  = boost::filesystem::canonical(file).string();
            break;
          }
      }

    return(match);
  }
