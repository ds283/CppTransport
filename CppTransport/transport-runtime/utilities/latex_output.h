//
// Created by David Seery on 04/08/2013.
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

#ifndef CPPTRANSPORT_LATEX_OUTPUT_H_
#define CPPTRANSPORT_LATEX_OUTPUT_H_

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

inline std::string output_latex_number(double number, unsigned int sf)
  {
    std::ostringstream test;

    test << std::setprecision(sf);

    test << number;

    std::string teststr = test.str();
    size_t pos = teststr.find("e");  // find position of exponent

    std::ostringstream output;

    if(pos == std::string::npos)        // if there was no 'e', then nothing needs to be done
      {
        output << teststr;
      }
    else
      {
        std::string mantissa = teststr.substr(0, pos);                          // extract mantissa
        std::string exponent = teststr.substr(pos+1, teststr.length()-pos-1);   // extract exponent

        std::istringstream exp_convert(exponent);
        int exp;

        if(exp_convert >> exp)  // was able to convert the exponent satisfactorily
          {
            output << mantissa << " \\times 10^{"
                   << exp      << "}";
          }
        else
          {
            output << mantissa << " \\times 10^{"
                   << exponent << "}";
          }
      }

    return(output.str());
  }

#endif // CPPTRANSPORT_LATEX_OUTPUT_H_
