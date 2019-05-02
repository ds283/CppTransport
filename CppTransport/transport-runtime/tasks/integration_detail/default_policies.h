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

    enum class storage_outcome { accept, reject_retain, reject_remove };

    namespace task_policy_impl
      {

        // default storage policy stores all triangles that satisfy k1 > k2 > k3
        class DefaultStoragePolicy
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor is default
            DefaultStoragePolicy(double tol = CPPTRANSPORT_DEFAULT_KCONFIG_TOLERANCE)
              : tolerance(std::abs(tol))
              {
              }

            //! destructor is default
            ~DefaultStoragePolicy() = default;


            // INTERFACE

          public:

            enum storage_outcome operator()(const threepf_kconfig& data)
              {
                // impose k1 > k2 > k3
                if(data.k2_conventional - data.k1_conventional > this->tolerance)
                  return storage_outcome::reject_remove;

                if(data.k3_conventional - data.k2_conventional > this->tolerance)
                  return storage_outcome::reject_remove;

                return(storage_outcome::accept);
              }


            // INTERNAL DATA

          private:

            //! tolerance
            double tolerance;

          };


        // default cubic triangle policy requires ordering k1 > k2 > k3
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

            bool operator()(unsigned int i, unsigned int j, unsigned int k, double k1, double k2, double k3)
              {
                // impose triangle
                double max = std::max(std::max(k1, k2), k3);
                return(k1 + k2 + k3 - 2.0*max >= -this->tolerance);
              }


            // INTERNAL DATA

          private:

            //! tolerance
            double tolerance;

          };


        // default alpha-beta triangle policy required ordering k1 > k2 > k3
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
