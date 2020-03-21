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


#ifndef CPPTRANSPORT_NEXIT_PHYS_K_CPP
#define CPPTRANSPORT_NEXIT_PHYS_K_CPP


#include <cmath>
#include <string>
#include "transport-runtime/defaults.h"
#include "transport-runtime/utilities/spline1d.h"
#include "transport-runtime/tasks/integration_detail/twopf_db_task.h"


namespace transport
{

    // definition of tolerance for the bisection of physical k values
    template <typename number = default_number_type>
    struct ToleranceCondition
    {
        bool operator () (number min, number max)
        {
          return abs(min - max) <= 1E-12;
        }
    };

    // Set-up a bisection function using a spline to extract a value of N_exit from some desired value of
    // phys_k as no. of e-folds before end of inflation
    template <typename number = default_number_type>
    number compute_Nexit_for_physical_k (number Phys_k, spline1d<number>& matching_eq,
                                         ToleranceCondition<number> tol, number Nend)
    {
      matching_eq.set_offset(std::log(Phys_k));
      std::string task_name = "find N_exit of physical wave-number";
      std::string bracket_error = "extreme values of N didn't bracket the exit value";

      number Nexit;
      Nexit = task_impl::find_zero_of_spline(task_name, bracket_error, matching_eq, tol);

      matching_eq.set_offset(0.0);

      Nexit = Nend - Nexit;
      return Nexit;
    }


} // namespace transport


#endif //CPPTRANSPORT_NEXIT_PHYS_K_CPP
