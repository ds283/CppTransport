//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <fstream>
#include <sstream>

#include "finder.h"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

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

finder::~finder()
  {
  }

// ******************************************************************

void finder::add(std::string path)
  {
    int i;
    bool match = false;

    for(i = 0; i < this->paths.size() && match == false; i++)
      {
        if(this->paths[i] == path)
          {
            match = true;
          }
      }

    if(match == false)
      {
        paths.push_back(path);
      }
  }

bool finder::fqpn(std::string leaf, std::string& fqpn)
  {
    bool match = false;

    int i;
    for(i = 0; match == false && i < this->paths.size(); i++)
      {
        std::ostringstream path;

        path << this->paths[i] << "/" << leaf;

        std::ifstream stream;
        stream.open(path.str().c_str());  // when building with GCC LLVM 4.2, stream.open() doesn't accept std::string

        if(stream.is_open())
          {
            match = true;
            fqpn  = path.str();
          }
      }

    return(match);
  }
