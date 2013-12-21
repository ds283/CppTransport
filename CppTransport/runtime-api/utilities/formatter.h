//
// Created by David Seery on 21/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __formatter_H_
#define __formatter_H_


#include <iomanip>
#include <sstream>

#include "transport/messages_en.h"

constexpr unsigned int gigabyte = 1024*1024*1024;
constexpr unsigned int megabyte = 1024*1024;
constexpr unsigned int kilobyte = 1024;


inline std::string format_memory(unsigned int size, unsigned int precision=2)
  {
    std::ostringstream out;

    if(size > gigabyte)
      {
        out << std::setprecision(precision) << static_cast<double>(size) / gigabyte << " " << __CPP_TRANSPORT_GIGABYTE;
      }
    else if(size > megabyte)
      {
        out << std::setprecision(precision) << static_cast<double>(size) / megabyte << " " << __CPP_TRANSPORT_MEGABYTE;
      }
    else if(size > kilobyte)
      {
        out << std::setprecision(precision) << static_cast<double>(size) / kilobyte << " " << __CPP_TRANSPORT_KILOBYTE;
      }
    else
      {
        out << size << " " << __CPP_TRANSPORT_BYTE;
      }

    return(out.str());
  }


#endif //__formatter_H_
