//
// Created by David Seery on 19/04/15.
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


#ifndef CPPTRANSPORT_ADVISORY_CLASSES_H
#define CPPTRANSPORT_ADVISORY_CLASSES_H


#include <string>
#include <exception>

#include "transport-runtime/messages.h"


namespace transport
  {

    //! advisory class thrown if backend cannot find the end of inflation
    class end_of_inflation_not_found: public std::exception
      {

      public:

        virtual const char* what() const noexcept override { return(this->message.c_str()); }

      private:

        std::string message{CPPTRANSPORT_ADVISORY_EMPTY};

      };


    //! generic advisory class associated with an integration event
    class advisory_event: public std::exception
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures e-folding number associated with event
        template <typename StateValues, typename StateNames>
        advisory_event(double N, double Hsq, double eps, double V, const StateValues& vevs, const StateNames& names)
          : efolds(N)
          {
            std::ostringstream str;

            str << CPPTRANSPORT_ADVISORY_DETECTED_AT << efolds << " " << CPPTRANSPORT_ADVISORY_EFOLDS << " | "
                << " H^2 = " << Hsq << ", eps = " << eps << ", V = " << V << '\n';

            auto u = vevs.begin();
            auto v = names.begin();
            while(u != vevs.end() && v != names.end())
              {
                str << "  * " << *v << " = " << *u << '\n';
                ++u;
                ++v;
              }


            message = str.str();
          }


        //! std::exception interface

      public:

        //! return message
        virtual const char* what() const noexcept override { return(this->message.c_str()); }


        // INTERNAL DATA

      private:

        //! capture e-folding number of event
        double efolds;

        //! message returned by what()
        std::string message;

      };


    //! advisory class thrown when Hsq is negative
    class Hsq_is_negative: public advisory_event
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename StateValues, typename StateNames>
		    Hsq_is_negative(double N, double Hsq, double eps, double V, const StateValues& vevs, const StateNames& names)
			    : advisory_event(N, Hsq, eps, V, vevs, names)
			    {
			    }

      };


    //! advisory class thrown if backend produces a NAN during integration
    class integration_produced_nan: public advisory_event
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename StateValues, typename StateNames>
		    integration_produced_nan(double N, double Hsq, double eps, double V, const StateValues& vevs, const StateNames& names)
			    : advisory_event(N, Hsq, eps, V, vevs, names)
			    {
			    }

      };


    //! advisory class thrown if epsilon becomes negative during integration
    class eps_is_negative: public advisory_event
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename StateValues, typename StateNames>
        eps_is_negative(double N, double Hsq, double eps, double V, const StateValues& vevs, const StateNames& names)
          : advisory_event(N, Hsq, eps, V, vevs, names)
          {
          }

      };


    //! advisory class thrown if epsilon becomes too large (ie. exceeds 3) during intgeration
    class eps_too_large: public advisory_event
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename StateValues, typename StateNames>
        eps_too_large(double N, double Hsq, double eps, double V, const StateValues& vevs, const StateNames& names)
          : advisory_event(N, Hsq, eps, V, vevs, names)
          {
          }

      };


    //! advisory class thrown if potential evaluates to a negative number during integration
    class V_is_negative: public advisory_event
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename StateValues, typename StateNames>
        V_is_negative(double N, double Hsq, double eps, double V, const StateValues& vevs, const StateNames& names)
          : advisory_event(N, Hsq, eps, V, vevs, names)
          {
          }

      };


    //! advisory class thrown if backend can't compute horizon exit times for all k-modes
    class failed_to_compute_horizon_exit: public std::exception
      {

      public:

        failed_to_compute_horizon_exit(double sb, double se, bool fe, size_t N, double lN, double laH, double lk)
          : search_begin(sb),
            search_end(se),
            found_end(fe),
            N_samples(N),
            last_N(lN),
            last_log_aH(laH),
            largest_k(lk)
          {
            std::ostringstream str;

            str << CPPTRANSPORT_ADVISORY_SEARCHED_FROM << search_begin << " " << CPPTRANSPORT_ADVISORY_SEARCHED_TO << search_end << " " << CPPTRANSPORT_ADVISORY_EFOLDS;

            if(found_end) str << " " << CPPTRANSPORT_ADVISORY_DETECT_END;
            else          str << " " << CPPTRANSPORT_ADVISORY_NO_DETECT_END;

            str << ", " << CPPTRANSPORT_ADVISORY_RECORDED << " " << N_samples << " " << CPPTRANSPORT_ADVISORY_SAMPLES;
            str << ", " << CPPTRANSPORT_ADVISORY_LAST_RECORDED << " " << last_log_aH << " " << CPPTRANSPORT_ADVISORY_AT_N << last_N;
            str << ", " << CPPTRANSPORT_ADVISORY_LARGEST_K << " " << std::log(largest_k);

            message = str.str();
          }

        double get_search_begin()   const { return(this->search_begin); }

        double get_search_end()     const { return(this->search_end); }

        bool get_found_end()        const { return(this->found_end); }

        size_t get_N_samples()      const { return(this->N_samples); }

        double get_last_N()         const { return(this->last_N); }

        double get_last_log_aH()    const { return(this->last_log_aH); }

        double get_largest_k()      const { return(this->largest_k); }

        const char* what()          const noexcept override { return(this->message.c_str()); }

      private:

        //! time at which search was started
        double search_begin;

        //! time up to which the search was performed
        double search_end;

        //! did we find an end for inflation, or did we just search up to a fixed number of e-folds?
        bool found_end;

        //! number of samples of aH recorded
        size_t N_samples;

        //! last N-value recorded by the search
        double last_N;

        //! last log(aH)-value recorded by the search
        double last_log_aH;

        //! largest k-mode being searched for
        double largest_k;

        //! exception message
        std::string message;

      };


    //! advisory class thrown if backend can't find a safe initial time for some k-modes
    class no_massless_time: public std::exception
      {

      public:

        no_massless_time(double kM_i, double kM_e, double t_e, double k_conv)
          : kM_ratio_init(kM_i),
            kM_ratio_exit(kM_e),
            t_exit(t_e),
            k_conventional(k_conv)
          {
            std::ostringstream str;

            str << CPPTRANSPORT_ADVISORY_FOR_K_MODE << " " << k_conv << " "
                << CPPTRANSPORT_ADVISORY_WITH_TEXIT << " " << t_e << ": ";

            str << CPPTRANSPORT_ADVISORY_AT_TINIT << " " << CPPTRANSPORT_ADVISORY_K_A2M << " = " << kM_i << ", ";
            str << CPPTRANSPORT_ADVISORY_AT_TEXIT << " " << CPPTRANSPORT_ADVISORY_K_A2M << " = " << kM_e;

            message = str.str();
          }

        const char* what()      const noexcept override { return(this->message.c_str()); }

      private:

        //! (k/a)^2 / M ratio at initial time
        double kM_ratio_init;

        //! (k/a)^2 / M ratio at horizon exit time
        double kM_ratio_exit;

        //! horizon exit time for this k-mode
        double t_exit;

        //! conventionally-normalized wavenumber for this mode
        double k_conventional;

        //! exception message
        std::string message;

      };

    //! advisory class thrown if adaptive initial conditions needs to integrate before N_initial
    class adaptive_ics_before_Ninit: public std::exception
      {
      public:
        adaptive_ics_before_Ninit(std::string task_name, double adaptive_efolds, double earliest_required, double N_initial)
          : t_name(task_name),
            a_efolds(adaptive_efolds),
            e_req_time(earliest_required),
            N_i(N_initial)
          {
            std::ostringstream str;
            str << "'" << task_name << "': " << CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_A << adaptive_efolds << " "
              << CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_B << earliest_required << " "
              << CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_C << N_initial;

            message = str.str();
          }

        const char* what()      const noexcept override { return(this->message.c_str()); }

      private:
        //! task name we're trying to perform e.g. model.twopf, model.threepf, etc.
        std::string t_name;

        //! number of adaptive e-folds assigned in this task
        double a_efolds;

        //! earliest required time given from the background solution
        double e_req_time;

        //! initial time set by the user
        double N_i;

        //! exception message
        std::string message;
      };

  }


#endif //CPPTRANSPORT_ADVISORY_CLASSES_H
