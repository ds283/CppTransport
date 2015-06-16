//
// Created by David Seery on 02/06/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __environment_H_
#define __environment_H_


#include <string>

#include "transport-runtime-api/utilities/python_finder.h"

#include "boost/filesystem/operations.hpp"


namespace transport
  {

    class local_environment
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        local_environment();

        ~local_environment() = default;


        // LOCATION OF EXECUTABLES

      public:

        std::string get_python_location() const { return(this->python_location.string()); }


        // INTERNAL DATA

      protected:

        // LOCATION OF EXECUTABLES

        //! Python executable
        boost::filesystem::path python_location;

      };


    local_environment::local_environment()
      {
        python_location = find_python();
      }


  }   // namespace transport

#endif //__environment_H_
