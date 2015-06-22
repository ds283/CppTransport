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


        // TERMINAL PROPERTIES

      public:

        bool get_terminal_colour_support() const { return(this->colour_output); }


        // INTERNAL DATA

      protected:

        // LOCATION OF EXECUTABLES

        //! Python executable
        boost::filesystem::path python_location;


        // TERMINAL PROPERTIES

        //! terminal supports colour output?
        bool colour_output;

      };


    local_environment::local_environment()
      {
        // set up python path
        python_location = find_python();

        // determine if terminal supports colour output
        char* term_type_cstr = std::getenv("TERM");

        if(term_type_cstr == nullptr)
          {
            colour_output = false;
            return;
          }

        std::string term_type(term_type_cstr);

        colour_output = term_type == "xterm"
          || term_type == "xterm-color"
          || term_type == "xterm-256color"
          || term_type == "screen"
          || term_type == "linux"
          || term_type == "cygwin";
      }


  }   // namespace transport

#endif //__environment_H_
