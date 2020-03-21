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


#ifndef CPPTRANSPORT_DISPERSION_H
#define CPPTRANSPORT_DISPERSION_H

#include <vector>

#include "transport-runtime/defaults.h"


namespace transport
  {

    template <typename RangeType=default_number_type, typename SamplesType=default_number_type>
    class Dispersion
      {

        // Constructors and destructors
        public:
          // Constructor for transport::basic_range k samples
          Dispersion(basic_range<RangeType>& k_samples, basic_range<RangeType>& time_samples,
                     std::vector<SamplesType>& spectrum_samples)
                    : k_size(k_samples.size()),
                      time_size(time_samples.size()),
                      samples(spectrum_samples)
          {
          };

          // Constructor for transport::aggregate_range k samples
          Dispersion(aggregate_range<RangeType>& k_samples, basic_range<RangeType>& time_samples,
                     std::vector<SamplesType>& spectrum_samples)
                    : k_size(k_samples.size()),
                      time_size(time_samples.size()),
                      samples(spectrum_samples)
          {
          };

          // move constructor
          Dispersion(Dispersion<RangeType, SamplesType>&&) = default;

          // destructor
          ~Dispersion() = default;

        // Dispersion calculation
        public:
          bool dispersion_check();


        // Internal data
        protected:
          size_t k_size;
          size_t time_size;
          std::vector<SamplesType>& samples;
      };

  } // namespace transport


#endif //CPPTRANSPORT_DISPERSION_H
