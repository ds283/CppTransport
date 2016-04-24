//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
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
