//
// Created by David Seery on 03/04/2016.
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
