//
// Created by David Seery on 07/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DEFAULT_POLICIES_H
#define CPPTRANSPORT_DEFAULT_POLICIES_H


#include "transport-runtime-api/tasks/task_configurations.h"


namespace transport
  {

    namespace task_policy_impl
      {

        // default storage policy stores all triangles
        class DefaultStoragePolicy
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor is default
            DefaultStoragePolicy() = default;

            //! destructor is default
            ~DefaultStoragePolicy() = default;


            // INTERFACE

          public:

            bool operator()(const threepf_kconfig& data)
              {
                return(true);
              }

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
                // impose ordering k1 > k2 > k3
                if(i < j) return false;
                if(j < k) return false;

                // impose triangle
                double max = std::max(std::max(k1, k2), k3);
                return(k1 + k2 + k3 - 2.0*max >= -tolerance);
              }


            // INTERNAL DATA

          private:

            //! tolerance
            double tolerance;

          };


        // default FLS triangle policy required ordering k1 > k2 > k3
        class DefaultFLSTrianglePolicy
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            DefaultFLSTrianglePolicy(double tol = CPPTRANSPORT_DEFAULT_KCONFIG_TOLERANCE, double sq = CPPTRANSPORT_DEFAULT_SMALLEST_SQUEEZING)
              : tolerance(std::abs(tol)),
                smallest_squeezing(std::abs(sq))
              {
              }

            //! destructor
            ~DefaultFLSTrianglePolicy() = default;


            // INTERFACE

          public:

            bool operator()(double alpha, double beta)
              {
                // beta should lie between 0 and 1
                if(beta < 0.0) return false;
                if(beta > 1.0) return false;

                // alpha should lie between 1-beta and beta-1
                if(beta - 1.0 - alpha > tolerance) return false;
                if(alpha - (1.0 - beta) > tolerance) return false;

                // impose k1 > k2 > k3
                if(alpha < 0) return false;
                if(beta - (1.0 + alpha)/3.0 < -tolerance) return false;

                // demand that squeezing should not be too small
                if(std::abs(1.0 - beta) < smallest_squeezing) return false;
                if(std::abs(1.0 + alpha + beta) < smallest_squeezing) return false;
                if(std::abs(1.0 - alpha + beta) < smallest_squeezing) return false;

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
