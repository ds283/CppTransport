//
// Created by David Seery on 02/06/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_ENVIRONMENT_H
#define CPPTRANSPORT_ENVIRONMENT_H


#include <fstream>
#include <string>

#include <sys/ioctl.h>
#include <unistd.h>

#include "transport-runtime-api/defaults.h"

#include "boost/algorithm/string.hpp"
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

        //! determine whether a Python interpreter is available
        bool has_python() { if(!this->python_cached) this->detect_python(); return(this->python_available); }

        //! execute a Python script;
        //! returns exit code provided by system
        int execute_python(const boost::filesystem::path& script);

      protected:

        //! detect Python installation details
        void detect_python();

        //! get location of Python executable
        std::string get_python_location() { if(!this->python_cached) this->detect_python(); return(this->python_location.string()); }


        // MATPLOTLIB SUPPORT

      public:

        //! determine whether Matplotlib is available
        bool has_matplotlib() { if(!this->matplotlib_cached) this->detect_matplotlib(); return(this->matplotlib_available); }

        //! determine whether Matplotlib style sheets are available
        bool has_matplotlib_style_sheets() { if(!this->matplotlib_cached) this->detect_matplotlib(); return(this->matplotlib_style_sheets); }

        //! determine whether Seaborn is available
        bool has_seaborn() { if(!this->seaborn_cached) this->detect_seaborn(); return(this->seaborn_available); }

      protected:

        //! detect Matplotlib installation
        void detect_matplotlib();

        //! detect seaborn installation
        void detect_seaborn();


        // TERMINAL PROPERTIES

      public:

        //! determine whether the terminal we are running in has support for ANSI colourized output
        bool has_colour_terminal_support() const { return(this->colour_output); }

        //! determine current terminal width
        unsigned int detect_terminal_width() const;

      protected:

        //! detect terminal type
        void detect_terminal_type();


        // ENVIRONMENT PATHS

      public:

        //! get path to config file, if it exists
        boost::optional< boost::filesystem::path > config_file_path() const;

      protected:

        //! detect home direcotry
        void detect_home();


        // INTERNAL DATA

      protected:


        // ENVIRONMENT PATHS

        //! user home directory
        boost::optional< boost::filesystem::path > home;


        // PYTHON SUPPORT

        //! has the status of Python support been cached?
        bool python_cached;

        //! is Python available
        bool python_available;

        //! Python executable
        boost::filesystem::path python_location;


        // MATPLOTLIB SUPPORT

        //! has the status of Matplotlib availability been cached?
        bool matplotlib_cached;

        //! has Matplotlib available?
        bool matplotlib_available;

        //! Matplotlib has style sheet support?
        bool matplotlib_style_sheets;

        //! has the status of Seaborn availability been cached?
        bool seaborn_cached;

        //! is Seaborn available?
        bool seaborn_available;


        // TERMINAL PROPERTIES

        //! terminal supports colour output?
        bool colour_output;

      };


    local_environment::local_environment()
      : python_cached(false),
        python_available(false),
        matplotlib_cached(false),
        matplotlib_available(false),
        matplotlib_style_sheets(false),
        seaborn_cached(false),
        seaborn_available(false)
      {
        // detect home directory
        detect_home();

        // detect terminal colour support
        detect_terminal_type();

        // detection of Python support (Python interpreter, Matplotlib, style sheets, Seaborn, etc.) is
        // deferred until needed, since it slows down program initialization
      }


    void local_environment::detect_home()
      {
        // TODO: Platform introspection
        // detect home directory
        char* home_cstr = std::getenv(CPPTRANSPORT_HOME_ENV);

        if(home_cstr != nullptr)
          {
            std::string home_path(home_cstr);
            this->home = boost::filesystem::path(home_path);
          }
      }


    void local_environment::detect_terminal_type()
      {
        // TODO: Platform introspection
        // determine if terminal supports colour output
        char* term_type_cstr = std::getenv("TERM");

        if(term_type_cstr == nullptr)
          {
            this->colour_output = false;
            return;
          }

        std::string term_type(term_type_cstr);

        this->colour_output = term_type == "xterm"
          || term_type == "xterm-color"
          || term_type == "xterm-256color"
          || term_type == "screen"
          || term_type == "linux"
          || term_type == "cygwin";
      }


    void local_environment::detect_python()
      {
        // TODO: Platform introspection
        std::string path;
        FILE* f = popen("which python", "r");

        if(!f)
          {
            this->python_available = false;
            this->python_location = CPPTRANSPORT_DEFAULT_PYTHON_PATH;
          }
        else
          {
            char buffer[1024];
            char* line = fgets(buffer, sizeof(buffer), f);
            pclose(f);

            if(line != nullptr)
              {
                this->python_available = true;
                std::string temp = std::string(line);
                boost::algorithm::trim_right(temp);
                this->python_location = temp;
              }
            else
              {
                this->python_available = false;
                path = CPPTRANSPORT_DEFAULT_PYTHON_PATH;
              }
          }

        this->python_cached = true;
      }


    void local_environment::detect_matplotlib()
      {
        if(!this->python_available)
          {
            this->matplotlib_available = false;
            return;
          }

        // get name of temporary file
        boost::filesystem::path temp_mpl = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

        std::fstream outf(temp_mpl.string(), std::ios_base::out | std::ios_base::trunc);
        outf << "import matplotlib.pyplot as plt" << '\n';
        outf.close();

        this->matplotlib_available = this->execute_python(temp_mpl) == 0;

        if(!this->matplotlib_available) return;

        // get name of second temporary file
        boost::filesystem::path temp_sheets = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

        std::fstream outf2(temp_sheets.string(), std::ios_base::out | std::ios_base::trunc);
        outf2 << "import matplotlib.pyplot as plt" << '\n';
        outf2 << "plt.style.use('ggplot')" << '\n';
        outf2.close();

        this->matplotlib_style_sheets = this->execute_python(temp_sheets) == 0;

        this->matplotlib_cached = true;
      }


    void local_environment::detect_seaborn()
      {
        if(!this->python_available)
          {
            this->seaborn_available = false;
            return;
          }

        // get name of temporary file
        boost::filesystem::path temp_sns = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

        std::fstream outf(temp_sns.string(), std::ios_base::out | std::ios_base::trunc);
        outf << "import seaborn as sns" << '\n';
        outf.close();

        this->seaborn_available = this->execute_python(temp_sns) == 0;

        this->seaborn_cached = true;
      }


    int local_environment::execute_python(const boost::filesystem::path& script)
      {
        std::ostringstream command;

        // source user's .profile script if it exists
        // TODO: Platform introspection
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

        if(!this->python_cached) this->detect_python();

        if(this->python_available)
          {
            command << this->python_location.string() << " \"" << script.string() << "\" > /dev/null 2>&1";
            return std::system(command.str().c_str());
          }
        else
          {
            return EXIT_FAILURE;
          }
      }


    boost::optional<boost::filesystem::path> local_environment::config_file_path() const
      {
        if(!this->home) return boost::optional<boost::filesystem::path>();

        boost::filesystem::path config_path = *this->home;

        return config_path / CPPTRANSPORT_RUNTIME_CONFIG_FILE;
      }


    unsigned int local_environment::detect_terminal_width() const
      {
        // TODO: Platform introspection
        // Read terminal display width (assuming the output *is* a terminal)
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        return(w.ws_col > 0 ? w.ws_col : CPPTRANSPORT_DEFAULT_TERMINAL_WIDTH);
      }



  }   // namespace transport

#endif //CPPTRANSPORT_ENVIRONMENT_H
