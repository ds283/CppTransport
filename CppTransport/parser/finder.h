//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __finder_H_
#define __finder_H_

#include <iostream>
#include <string>
#include <vector>

class finder
  {
    public:
      finder();
      finder(std::string path);
      ~finder();

      // add a path to the search list
      void add(std::string path);

      // find fully-qualified path name corresponding to a given leafname
      bool fqpn(std::string leaf, std::string& fqpn);

    private:
      std::vector<std::string> paths;
  };


#endif //__finder_H_
