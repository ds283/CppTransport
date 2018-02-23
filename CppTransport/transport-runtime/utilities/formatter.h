//
// Created by David Seery on 21/12/2013.
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


#ifndef CPPTRANSPORT_FORMATTER_H
#define CPPTRANSPORT_FORMATTER_H


#include <iomanip>
#include <sstream>
#include <type_traits>

#include "transport-runtime/messages.h"

#include "boost/timer/timer.hpp"
#include "boost/lexical_cast.hpp"


template <typename IntegerType>
std::string format_memory(IntegerType size, unsigned int precision=3)
  {
    static_assert(std::is_integral<IntegerType>::value, "format_memory requires an integral value type");

    std::ostringstream out;

    constexpr IntegerType gigabyte = 1024*1024*1024;
    constexpr IntegerType megabyte = 1024*1024;
    constexpr IntegerType kilobyte = 1024;

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
    constexpr boost::timer::nanosecond_type day    = 24*hour;
    constexpr boost::timer::nanosecond_type week   = 7*day;

    if(time > week)
      {
        out << time/week << CPPTRANSPORT_WEEK << " ";
        time = time % week;
      }
    if(time > day)
      {
        out << time/day << CPPTRANSPORT_DAY << " ";
        time = time % day;
      }
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

    double abs_number = std::abs(number);

    // use scientific notation for large or small numbers, unless
    // the number is exactly zero
    if(abs_number != 0.0 && (std::abs(number) > 1E3 || std::abs(number) < 1E-3))
      {
        out << std::scientific;
        if(precision > 0) --precision;    // in scientific format, precision means number of decimal digits
      }

    out << std::setprecision(precision) << number;

    return(out.str());
  }


inline std::string format_version(unsigned int version)
  {
    unsigned int major = version / 100;
    unsigned int minor = version % 100;

    std::string major_string = boost::lexical_cast<std::string>(major);
    std::string minor_string = boost::lexical_cast<std::string>(minor);

    return(major_string + "." + minor_string);
  }


#endif //CPPTRANSPORT_FORMATTER_H
