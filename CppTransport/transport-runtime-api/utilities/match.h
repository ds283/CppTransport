//
// Created by David Seery on 03/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#include <string>

#include "boost/regex.hpp"

#ifndef CPPTRANSPORT_MATCH_H
#define CPPTRANSPORT_MATCH_H


bool check_match(std::string s, std::string e, bool exact=false)
  {
    // check for regular expression syntax
    if(!e.empty() && e.front() == '{' && e.back() == '}')
      {
        std::string expr = e.substr(1, e.length()-2);
        boost::regex regex(expr);

        return boost::regex_match(s, regex);
      }
    else    // treat as standard string
      {
        if(exact)
          {
            return s == e;
          }
        else
          {
            // we return a match if the search string starts with the requested expression
            return s.find(e) == 0;
          }
      }
  }


#endif //CPPTRANSPORT_MATCH_H
