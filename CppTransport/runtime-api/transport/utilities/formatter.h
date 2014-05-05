//
// Created by David Seery on 21/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __formatter_H_
#define __formatter_H_


#include <iomanip>
#include <sstream>

#include "transport/messages.h"

#include <boost/timer/timer.hpp>


inline std::string format_memory(unsigned int size, unsigned int precision=2)
  {
    std::ostringstream out;

    constexpr unsigned int gigabyte = 1024*1024*1024;
    constexpr unsigned int megabyte = 1024*1024;
    constexpr unsigned int kilobyte = 1024;

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


inline std::string format_time(boost::timer::nanosecond_type time, unsigned int precision=3)
  {
    std::ostringstream out;

    constexpr boost::timer::nanosecond_type mu_sec = 1000;
    constexpr boost::timer::nanosecond_type m_sec  = 1000*mu_sec;
    constexpr boost::timer::nanosecond_type sec    = 1000*m_sec;
    constexpr boost::timer::nanosecond_type minute = 60*sec;
    constexpr boost::timer::nanosecond_type hour   = 60*minute;

    if(time > hour)
      {
        out << time/hour << __CPP_TRANSPORT_HOUR << " ";
        time = time % hour;
      }
    if(time > minute)
      {
        out << time/minute << __CPP_TRANSPORT_MINUTE << " ";
        time = time % minute;
      }
    out << std::setprecision(precision) << static_cast<double>(time) / sec << __CPP_TRANSPORT_SECOND;

    return(out.str());
  }


#endif //__formatter_H_
