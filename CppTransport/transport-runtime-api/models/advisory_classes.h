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

  }


#endif //__advisory_classes_H_
