//
// Created by David Seery on 28/09/2016.
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

#ifndef CPPTRANSPORT_TO_PRINTABLE_H
#define CPPTRANSPORT_TO_PRINTABLE_H


namespace transport
  {
    
    inline std::string to_printable(const std::string& input, bool quote = true, bool allow_newlines = false)
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
        
        return (result);
      }
    
  }   // namespace transport
//endregion


#endif //CPPTRANSPORT_TO_PRINTABLE_H
