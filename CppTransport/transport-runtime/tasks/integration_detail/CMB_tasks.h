//
// Created by Alessandro Maraio on 21/03/2020.
// --@@
// Copyright (c) 2020 University of Sussex. All rights reserved.
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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_CMB_TASKS_H
#define CPPTRANSPORT_CMB_TASKS_H


#include <string>
#include <iostream>
#include <fstream>
#include "transport-runtime/tasks/integration_detail/twopf_task.h"
#include "transport-runtime/tasks/integration_detail/threepf_task.h"
#include "transport-runtime/defaults.h"
#include "transport-runtime/tasks/sampling_detail/Nexit_phys_k.cpp"
#include "transport-runtime/tasks/sampling_detail/Logspace.cpp"


namespace transport
  {

    // For the cubic three-pf task, we require that *all* valid configurations are computed, not just the usual
    // k1 > k2 > k3 region. Hence, we need to make our own StoragePolicy and TrianglePolicy objects
    // which are used in setting up the tk3 task below.

    struct CMB_StoragePolicy
    {
      storage_outcome operator()(const threepf_kconfig& data)
        {
          return storage_outcome::accept;  // Don't do any filtering here, accept all valid triangles
        }
    };


    struct CMB_TrianglePolicy
    {
      bool operator()(unsigned i, unsigned j, unsigned k, double k1, double k2, double k3)
        {
          // Convert (k1, k2, k3) into (kt, alpha, beta) values which make imposing 'squeezing' limits much easier
          // in (alpha, beta) space than in the original k space

          const double kt = k1 + k2 + k3;
          const double alpha = 2 * (k1 - k2) / kt;
          const double beta = 1 - 2 * k3 / kt;

          // beta should lie between 0 and 1
          if(beta < 0.0) return false;
          if(beta > 1.0) return false;

          // alpha should lie between 1-beta and beta-1
          if(beta - 1.0 - alpha > 1E-6) return false;
          if(alpha - (1.0 - beta) > 1E-6) return false;

          // demand that squeezing should not be too small
          const double squeeze = 0.0075; // Adjustable parameter which adjusts how squeezed the triangles formed are
          if(std::abs(1.0 - beta) < squeeze) return false;
          if(1 - std::abs(alpha) < squeeze) return false;
          if(std::abs(1.0 + alpha + beta) < squeeze) return false;
          if(std::abs(1.0 - alpha + beta) < squeeze) return false;

          // If we're here, then it's passed all checks, so return true as is valid configuration
          return true;
        }
    };


    template <typename number=default_number_type>
    void CMB_task(const std::string& name, const parameters<number> params, const initial_conditions<number>& ics,
                  model<number>* mdl, repository<number>& repo, const int num_k)
    {
      // Construct a dummy task to find the end of inflation
      basic_range<double> dummy_times{0.0, CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH, 2, spacing::linear};
      background_task<number> bkg{ics, dummy_times};

      double N_end = mdl->compute_end_of_inflation(&bkg, CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH);
      std::cout << "End of inflation: " << N_end << std::endl;

      // If we have less than 60 e-folds, then raise an exception and break out.
      if(N_end < 60) throw runtime_exception(exception_type::RUNTIME_ERROR, "Less than 60 e-folds!");

      // Now we need to construct a new initial conditions block that reflect our use of using the correct N_pre
      const double N_pre = N_end - 55;
      initial_conditions<number> init(name + "_ics", params, ics.get_vector(), 0.0, N_pre);

      // Now construct another dummy task to use with compute_H
      basic_range<double> times{0.0, N_end, 1000, spacing::linear};
      background_task<number> bkg2{init, times};

      // Set up vectors to store output in, and then call compute_H
      std::vector<double> N_H;
      std::vector<number> log_H;
      mdl->compute_H(&bkg2, N_H, log_H, boost::none);

      // Sean's implementation of the matching equation:
      // Set-up the different parameters needed for the matching equation
      double Hend = 0.5 * log_H.back(); // value of H at the end of inflation
      const double norm_const   = std::log(243.5363 * pow(3.0, 0.25)); // dimnless matching eq has const = (3^(1/4)*M_P)/1E16 GeV
      const double k_pivot      = std::log(0.05); // pivot scale defined as 0.05 Mpc^-1 in the matching eq (DO NOT CHANGE!)
      const double e_fold_const = 55.75; // constant defined in the matching eq.
      double constants = e_fold_const + k_pivot + norm_const - Hend; // wrap up constants in a single term

      // Find the matching equation solutions across the inflation time range.
      std::vector<double> log_physical_k(N_H.size());
      for (std::size_t i = 0; i != N_H.size(); ++i)
      {
        log_physical_k[i] = log_H[i] - (N_end - N_H[i]) + constants;
      }

      // Set-up a spline to use with the bisection method defined later.
      spline1d<double> spline_match_eq(N_H, log_physical_k);

      // Construct the wave-numbers using a linearity relation.
      // Build CppT normalised wave-numbers by using the linear relation k_phys = gamma * k_cppt and k_cppt[Npre] == 1
      double gamma = spline_match_eq(N_pre);

      // We want to build a k-grid from 10^-6 to about 50, in units of Mpc^-1, which is the right range
      // for a CMB task
      std::vector<double> phys_wavenum = pyLogspace<double> (-6, 1.7, num_k, 10);
      std::vector<double> k_conventional(phys_wavenum.size());
      // Store these in a aggregate_range, which is what we then use in making tasks
      aggregate_range<double> ks;

      // Create a table where we store both CppT and physical wavenumbers, which can then be read in by my own
      // python code afterwards to convert back to physical k there.
      const std::string filename = repo.get_name() + "/k_table.dat";
      std::ofstream k_range_out(filename, std::ios::out | std::ios::trunc);
      k_range_out << "k_cpptransport" << "\t" << "k_physical" << "\n";

      // Iterate through the wave-number list, calculating the corresponding CppT value and then saving them
      for (std::size_t i = 0; i != k_conventional.size(); ++i)
      {
        double k_cppt = phys_wavenum[i] / exp(gamma);
        k_conventional[i] = k_cppt;

        basic_range<double> k_temp{k_cppt, k_cppt, 0};
        ks += k_temp;
        k_range_out << std::setprecision(std::numeric_limits<double>::digits10 + 1)
                    << k_cppt  << "\t"
                    << phys_wavenum[i] << "\n";
      }
      k_range_out.close();

      // The pivot choice. At the moment hard-coded to 0.002 Mpc^-1, as used by Planck, but could be changed?
      const double k_pivot_choice = 0.002;
      // Construct a CppT normalised wave-number for the chosen pivot scale using the linearity constant
      double k_pivot_cppt = k_pivot_choice / std::exp(gamma);

      // Form a two-point function for our range of ks, from which we can get the CMB power spectrum
      twopf_task<number> tk2(name + "_twopf", init, times, ks);
      tk2.set_adaptive_ics_efolds(5.0);

      // Now convert the above two-point function into a zeta two-point function
      zeta_twopf_task<number> ztk2(name + "_twopf_zeta", tk2);
      ztk2.set_paired(true);

      // Commit the two-point functions to the provided repository
      repo.commit(ztk2);

      // Now build a cubic three-point function from the provided k_range
      threepf_cubic_task<number> tk3(name + "_threepf", init, times, ks, true, CMB_StoragePolicy(), CMB_TrianglePolicy());
      tk3.set_adaptive_ics_efolds(5.0);

      // Now link this to a zeta three-point function task
      zeta_threepf_task<number> ztk3(name + "_threepf_zeta", tk3);
      ztk3.set_paired(true);

      // Finally commit the zeta threepf function to the repository, and then we can exit this function
      repo.commit(ztk3);

    }

  } // namespace transport

#endif //CPPTRANSPORT_CMB_TASKS_H
