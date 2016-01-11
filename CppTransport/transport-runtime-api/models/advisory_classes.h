//
// Created by David Seery on 19/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __advisory_classes_H_
#define __advisory_classes_H_


#include <string>
#include <exception>

#include "transport-runtime-api/messages.h"


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


    //! advisory class thrown when Hsq is negative
    class Hsq_is_negative: public std::exception
      {

      public:

		    Hsq_is_negative(double N)
			    : efolds(N)
			    {
		        std::ostringstream str;

		        str << CPPTRANSPORT_ADVISORY_DETECTED_AT << efolds << " " << CPPTRANSPORT_ADVISORY_EFOLDS;
		        message = str.str();
			    }

        virtual const char* what() const noexcept override { return(this->message.c_str()); }

      private:

        double efolds;

		    std::string message;

      };


    //! advisory class thrown if backend produces a NAN during integration
    class integration_produced_nan: public std::exception
      {

      public:

		    integration_produced_nan(double N)
			    : efolds(N)
			    {
		        std::ostringstream str;

            str << CPPTRANSPORT_ADVISORY_DETECTED_AT << efolds << " " << CPPTRANSPORT_ADVISORY_EFOLDS;
		        message = str.str();
			    }

        virtual const char* what() const noexcept override { return(this->message.c_str()); }

      private:

        double efolds;

		    std::string message;

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

        virtual const char* what()  const noexcept override { return(this->message.c_str()); }

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


  }


#endif //__advisory_classes_H_
