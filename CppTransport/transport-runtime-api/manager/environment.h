//
// Created by David Seery on 02/06/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __environment_H_
#define __environment_H_


#include <string>

#include "transport-runtime-api/utilities/python_finder.h"

#include "boost/optional.hpp"
#include "boost/filesystem/operations.hpp"


namespace transport
  {

    class local_environment
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor detects properties of environment
        local_environment();

        //! destructor is default
        ~local_environment() = default;


        // PYTHON SUPPORT

      public:

        //! get location of Python executable
        std::string get_python_location() const { return(this->python_location.string()); }

        //! execute a Python script;
        //! returns exit code provided by system
        int execute_python(const boost::filesystem::path& script) const;


        // TERMINAL PROPERTIES

      public:

        //! determine whether the terminal we are running in has support for ANSI colourized output
        bool has_colour_terminal_support() const { return(this->colour_output); }


        // ENVIRONMENT PATHS

      public:

        //! get path to config file, if it exists
        boost::optional< boost::filesystem::path > config_file_path() const;


        // INTERNAL DATA

      protected:


        // ENVIRONMENT PATHS

        //! user home directory
        boost::optional< boost::filesystem::path > home;


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

        // detect home directory
        char* home_cstr = std::getenv(CPPTRANSPORT_HOME_ENV);

        if(home_cstr != nullptr)
          {
            std::string home_path(home_cstr);
            this->home = boost::filesystem::path(home_path);
          }

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


    int local_environment::execute_python(const boost::filesystem::path& script) const
      {
        std::ostringstream command;

        // source user's .profile script if it exists
        const char* user_home = getenv("HOME");
        if(user_home != nullptr)
          {
            boost::filesystem::path user_profile = boost::filesystem::path(std::string(user_home)) / boost::filesystem::path(std::string(".profile"));
            if(boost::filesystem::exists(user_profile))
              {
                // . is the POSIX command for 'source'; 'source' is a csh command which has been imported to other shells
                command << ". " << user_profile.string() << "; ";
              }
          }

        command << this->python_location.string() << " \"" << script.string() << "\"";

        return std::system(command.str().c_str());
      }


    boost::optional<boost::filesystem::path> local_environment::config_file_path() const
      {
        if(!this->home) return boost::optional<boost::filesystem::path>();

        boost::filesystem::path config_path = *this->home;

        return config_path / CPPTRANSPORT_RUNTIME_CONFIG_FILE;
      }



  }   // namespace transport

#endif //__environment_H_
