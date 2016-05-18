//
// Created by David Seery on 31/12/2013.
// based on password generator code at
// http://www.boost.org/doc/libs/1_55_0/doc/html/boost_random/tutorial.html#boost_random.tutorial.generating_a_random_password
//


#ifndef CPPTRANSPORT_RANDOM_STRING_H
#define CPPTRANSPORT_RANDOM_STRING_H


#include <string>
#include <sstream>

#include "boost/random/random_device.hpp"
#include "boost/random/uniform_int_distribution.hpp"

inline std::string random_string(unsigned int length=10)
  {
    std::string chars(
      "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "1234567890"
        "!@#$%^&*()"
        "`~-_=+[{]{\\|;:'\",<.>/? ");

    boost::random::random_device              rng;
    boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);

    std::ostringstream str;

    for(int i = 0; i < length; ++i)
      {
        str << chars[index_dist(rng)];
      }

    return (str.str());
  }


#endif //CPPTRANSPORT_RANDOM_STRING_H
