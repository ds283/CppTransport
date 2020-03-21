//
// Created by Alessandro Maraio on 15/10/2019.
//
// Copyright (c) 2019 University of Sussex. All rights reserved.
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
// @contributor: Alessandro Maraio  <am963@sussex.ac.uk>
// @contributor: Sean Butchers      <smlb20@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_LOGSPACE_CPP
#define CPPTRANSPORT_LOGSPACE_CPP


#include <vector>
#include <cmath>
#include <algorithm>
#include "transport-runtime/defaults.h"


namespace transport{

  template <typename number = default_number_type>
  class Logspace
  {

    public:
      Logspace(number first, number base) : curValue(first), base(base) {}

      number operator()()
      {
        number retval = curValue;
        curValue *= base;
        return retval;
      }

    private:
        number curValue, base;
  };

  template <typename number = default_number_type>
  std::vector<number> pyLogspace (number start, number stop, int num, number base = 10)
  {
    number logStart = pow(base, start);
    number logBase = pow(base, (stop-start)/num);

    std::vector<number> log_vector;
    log_vector.reserve(num+1);
    std::generate_n(std::back_inserter(log_vector), num+1, Logspace<number>(logStart, logBase));
    return log_vector;
  }

  // Set-up a function that applies ln to every element of the vector (useful for dlnA/dlnk derivatives)
  template <typename number = default_number_type>
  std::vector<number> vector_logger( std::vector<number>& no_log_vec)
  {
    size_t vecSize = no_log_vec.size();
    std::vector<number> logvec(vecSize);
    for (size_t i = 0; i < vecSize; i++)
    {
      logvec[i] = std::log(no_log_vec[i]);
    }

    return logvec;
  }

} // namespace transport


#endif //CPPTRANSPORT_LOGSPACE_CPP
