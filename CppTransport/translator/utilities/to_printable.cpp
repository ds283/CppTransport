//
// Created by David Seery on 26/06/2013.
// based on http://stackoverflow.com/questions/2417588/escaping-a-c-string
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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#include "to_printable.h"


std::string to_printable(const std::string& input, bool quote, bool allow_newlines, bool init)
  {
    std::string result = "";

    std::back_insert_iterator<std::string> out = std::back_inserter(result);

    if(quote) *out++ = '"';

    for(const char c : input)
      {
        if(!quote or (' ' <= c and c <= '~' and c != '\\' and c != '"') or (c == '\n' and allow_newlines))
          {
            *out++ = c;
          }
        else
          {
            *out++ = '\\';
            switch(c)
              {
                case '"':
                  {
                    *out++ = '"';
                    break;
                  }

                case '\\':
                  {
                    *out++ = '\\';
                    break;
                  }
                case '\t':
                  {
                    *out++ = 't';
                    break;
                  }
 
                case '\r':
                  {
                    *out++ = 'r';
                    break;
                  }
 
                case '\n':
                  {
                    *out++ = 'n';
                    break;
                  }
 
                default:
                  {
                    char const *const hexdig = "0123456789ABCDEF";
                    *out++ = 'x';
                    *out++ = hexdig[c >> 4];
                    *out++ = hexdig[c & 0xF];
                  }
              }
          }
      }
 
    if(quote) *out++ = '"';
    if(init)
    {
      *out++ = '_';
      *out++ = 'I';
      *out++ = 'n';
      *out++ = 'i';
      *out++ = 't';
    }
    
    return (result);
  }
