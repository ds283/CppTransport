//
// Created by David Seery on 04/08/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
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
