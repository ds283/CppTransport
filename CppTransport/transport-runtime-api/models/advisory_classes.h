//
// Created by David Seery on 19/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __advisory_classes_H_
#define __advisory_classes_H_


#include <string>
#include <exception>


namespace transport
  {

    //! advisory class thrown if backend cannot find the end of inflation
    class end_of_inflation_not_found: public std::exception
      {

      public:

        virtual const char* what() const noexcept override { return(this->message.c_str()); }

      private:

        std::string message{"EMPTY"};

      };


    //! advisory class thrown when Hsq is negative
    class Hsq_is_negative: public std::exception
      {

      public:

		    Hsq_is_negative(double N)
			    : efolds(N)
			    {
		        std::ostringstream str;

		        str << "detected at N=" << this->efolds << " e-folds";
		        this->message = str.str();
			    }

        virtual const char* what() const noexcept override { return(this->message.c_str()); }

      private:

        double efolds;

		    std::string message;

      };


    //! advisory class thrown is backend produces a NAN during integration
    class integration_produced_nan: public std::exception
      {

      public:

		    integration_produced_nan(double N)
			    : efolds(N)
			    {
		        std::ostringstream str;

		        str << "detected at N=" << this->efolds << " e-folds";
		        this->message = str.str();
			    }

        virtual const char* what() const noexcept override { return(this->message.c_str()); }

      private:

        double efolds;

		    std::string message;

      };


  }


#endif //__advisory_classes_H_
