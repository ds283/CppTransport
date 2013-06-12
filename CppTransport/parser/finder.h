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
#include <list>

class finder
  {
    public:
      finder(std::string path);
      ~finder();

    private:
      std::list<std::string> paths;
  };


#endif //__finder_H_
