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

#include "Dispersion.h"

namespace transport {

    template<typename RangeType, typename SamplesType>
    bool Dispersion<RangeType, SamplesType>::dispersion_check() {
      // find the mean of the power spectrum amplitudes
      std::vector<SamplesType> mean(k_size), std_dev(k_size);

      for (std::size_t i = 0; i != k_size; i++)
      {
        SamplesType sum = 0;
        for (std::size_t j = 0; j != time_size; j++)
        {
          sum += samples[(time_size * i) + j];
        }
        mean[i] = sum / time_size;
      }

      // find sum_square values for standard deviation
      for (std::size_t i = 0; i != k_size; i++)
      {
        SamplesType sum_sq = 0;
        for (std::size_t j = 0; j != time_size; j++)
        {
          sum_sq += pow(samples[(time_size * i) + j] - mean[i], 2);
        }
        std_dev[i] = sqrt(sum_sq / (time_size - 1)); // divide by time_size-1 for N-1 samples
      }

      // find a measure of the dispersion of power spectrum values -> std-dev/mean
      std::vector<SamplesType> DispersionVec(k_size);
      for (std::size_t i = 0; i != mean.size(); ++i)
      {
        DispersionVec[i] = (1 + 1.0 / (4.0 * time_size)) * std_dev[i] / mean[i]; // unbiased estimator (1 + 1/4n)
      }

      // return true if the dispersion is >1% for any of the k samples
      for (auto i: DispersionVec)
      {
        if (i > 0.05) return true;
      }

      return false;

    }

} // namespace transport
