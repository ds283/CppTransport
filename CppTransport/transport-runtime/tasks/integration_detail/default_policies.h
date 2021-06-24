//
// Created by David Seery on 07/01/2016.
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

#ifndef CPPTRANSPORT_DEFAULT_POLICIES_H
#define CPPTRANSPORT_DEFAULT_POLICIES_H


#include "transport-runtime/tasks/task_configurations.h"


namespace transport
  {

    enum class storage_outcome
      {
        accept,          // 3pf configuration is stored
        reject_retain,   // 3pf configuration is not stored, but the 2pf configurations it generated are kept
        reject_remove,   // 3pf configuration is not stored, and the 2pf configurations it generated are removed
        reject_symmetry  // same as reject remove, but the rejection is for symmetry reasons (such as
                         // imposing the condition k1 > k2 > k3) and should not flag the grid as not simple
      };

    namespace task_policy_impl
      {

        // default storage policy optionally stores only triangles that satisfy k1 > k2 > k3
        // this is usually enabled by default
        class DefaultStoragePolicy
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor is default
            DefaultStoragePolicy(bool ord=CPPTRANSPORT_DEFAULT_ORDER_KS, double tol=CPPTRANSPORT_DEFAULT_KCONFIG_TOLERANCE)
              : order_ks{ord},
                tolerance{std::abs(tol)}
              {
              }

            //! destructor is default
            ~DefaultStoragePolicy() = default;


            // INTERFACE

          public:

            storage_outcome operator()(const threepf_kconfig& data)
              {
                if(!order_ks)
                  return storage_outcome::accept;

                // impose k1 > k2 > k3
                if(data.k2_conventional - data.k1_conventional > this->tolerance)
                  return storage_outcome::reject_symmetry;

                if(data.k3_conventional - data.k2_conventional > this->tolerance)
                  return storage_outcome::reject_symmetry;

                return storage_outcome::accept;
              }


            // INTERNAL DATA

          private:

            //! order k-modes?
            bool order_ks;

            //! tolerance
            double tolerance;

          };


        class DefaultCubicTrianglePolicy
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            DefaultCubicTrianglePolicy(double tol = CPPTRANSPORT_DEFAULT_KCONFIG_TOLERANCE)
              : tolerance(std::abs(tol))
              {
              }

            //! destructor is default
            ~DefaultCubicTrianglePolicy() = default;


            // INTERFACE

          public:

            //! operator returns true if this configuration represents a valid momentum triangle, otherwise false.
            //! If false, the configuraiton is rejected. Triangular configurations are not automatically
            //! accepted: this is determined by a StoragePolicy object
            bool operator()(unsigned int i, unsigned int j, unsigned int k, double k1, double k2, double k3)
              {
                // impose triangle condition
                double max = std::max(std::max(k1, k2), k3);
                return(k1 + k2 + k3 - 2.0*max >= -this->tolerance);
              }


            // INTERNAL DATA

          private:

            //! tolerance
            double tolerance;

          };


        class DefaultAlphaBetaTrianglePolicy
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            DefaultAlphaBetaTrianglePolicy(double tol = CPPTRANSPORT_DEFAULT_KCONFIG_TOLERANCE, double sq = CPPTRANSPORT_DEFAULT_SMALLEST_SQUEEZING)
              : tolerance(std::abs(tol)),
                smallest_squeezing(std::abs(sq))
              {
              }

            //! destructor
            ~DefaultAlphaBetaTrianglePolicy() = default;


            // INTERFACE

          public:

            //! operator returns true if this configuration represents a valid momentum triangle, otherwise false.
            //! If false, the configuraiton is rejected. Triangular configurations are not automatically
            //! accepted: this is determined by a StoragePolicy object
            bool operator()(double alpha, double beta)
              {
                // beta should lie between 0 and 1
                if(beta < 0.0) return false;
                if(beta > 1.0) return false;

                // alpha should lie between 1-beta and beta-1
                if(beta - 1.0 - alpha > this->tolerance) return false;
                if(alpha - (1.0 - beta) > this->tolerance) return false;

                // demand that squeezing should not be too small
                if(std::abs(1.0 - beta) < this->smallest_squeezing) return false;
                if(std::abs(1.0 + alpha + beta) < this->smallest_squeezing) return false;
                if(std::abs(1.0 - alpha + beta) < this->smallest_squeezing) return false;

                return true;
              }


            // INTERNAL DATA

          private:

            //! tolerance
            double tolerance;

            //! smallest acceptable squeezing
            double smallest_squeezing;

          };

      }   // namespace task_policy_impl

  }   // namespace transport


#endif //CPPTRANSPORT_DEFAULT_POLICIES_H
