//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! default constructor; sets search path to be current working direcotry
    finder();

    //! value constructor; sets search path as specified, but does not include current working directory
    finder(std::string path);

    //! destructor is default
    ~finder() = default;


    // INTERFACE -- ADD PATHS

  public:

    //! add a path to the search list
    void add(const std::string& p);

    //! add a list of paths to the search list
    void add(const std::list<std::string>& plist);


    // INTERFACE-- FIND FULLY-QUALIFIED PATHS

  public:

    // find fully-qualified path name corresponding to a given leafname
    bool fqpn(const std::string leaf, std::string& fqpn);


    // INTERNAL DATA

  private:

    //! list of paths to search
    std::list<std::string> paths;

  };


#endif //__finder_H_
