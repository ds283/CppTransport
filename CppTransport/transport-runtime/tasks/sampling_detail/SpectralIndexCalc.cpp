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


#ifndef CPPTRANSPORT_SPECTRALINDEXCALC_CPP
#define CPPTRANSPORT_SPECTRALINDEXCALC_CPP


#include <vector>
#include <cmath>

#include "transport-runtime/defaults.h"
#include "transport-runtime/tasks/sampling_detail/Logspace.cpp"
#include "transport-runtime/tasks/sampling_detail/Polyfit.cpp"


namespace transport
{

  template <typename number = default_number_type>
  number spec_index_deriv(double k_value, std::vector<double>& k, std::vector<number>& A,
                          bool scalar, int nDegree = 2)
  {
    std::vector<number> coeffs = polyfit<number>(vector_logger<double>(k), vector_logger<number>(A), nDegree);
    number spectral_index = 0.0;

    for (std::size_t i = 1; i != coeffs.size(); i++)
    {
      number power = i - 1.0;
      spectral_index += i * std::pow(std::log(k_value), power) * coeffs[i];
    }

    if(scalar) spectral_index++;

    return spectral_index;
  }

}  // namespace transport


#endif //CPPTRANSPORT_SPECTRALINDEXCALC_CPP
