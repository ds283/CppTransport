//
// Created by David Seery on 21/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_FORMATTER_H
#define CPPTRANSPORT_FORMATTER_H


#include <iomanip>
#include <sstream>

#include "transport-runtime/messages.h"

#include <boost/timer/timer.hpp>


inline std::string format_memory(unsigned int size, unsigned int precision=2)
  {
    std::ostringstream out;

    constexpr unsigned int gigabyte = 1024*1024*1024;
    constexpr unsigned int megabyte = 1024*1024;
    constexpr unsigned int kilobyte = 1024;

    if(size > gigabyte)
      {
        out << std::setprecision(precision) << static_cast<double>(size) / gigabyte << " " << CPPTRANSPORT_GIGABYTE;
      }
    else if(size > megabyte)
      {
        out << std::setprecision(precision) << static_cast<double>(size) / megabyte << " " << CPPTRANSPORT_MEGABYTE;
      }
    else if(size > kilobyte)
      {
        out << std::setprecision(precision) << static_cast<double>(size) / kilobyte << " " << CPPTRANSPORT_KILOBYTE;
      }
    else
      {
        out << size << " " << CPPTRANSPORT_BYTE;
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
        out << time/hour << CPPTRANSPORT_HOUR << " ";
        time = time % hour;
      }
    if(time > minute)
      {
        out << time/minute << CPPTRANSPORT_MINUTE << " ";
        time = time % minute;
      }
    out << std::setprecision(precision) << static_cast<double>(time) / sec << CPPTRANSPORT_SECOND;

    return(out.str());
  }


inline std::string format_number(double number, unsigned int precision=3)
  {
    std::ostringstream out;

    if(std::abs(number) > 1E3 || std::abs(number) < 1E-3)
      {
        out << std::scientific;
      }

    out << std::setprecision(precision) << number;

    return(out.str());
  }


#endif //CPPTRANSPORT_FORMATTER_H
