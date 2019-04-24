//
// Created by David Seery on 02/06/15.
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


#ifndef CPPTRANSPORT_ENVIRONMENT_H
#define CPPTRANSPORT_ENVIRONMENT_H


#include <fstream>
#include <string>
#include <cstdlib>

#include <sys/ioctl.h>
#include <pwd.h>

#include "transport-runtime/defaults.h"
#include "transport-runtime/utilities/finder.h"
#include "transport-runtime/utilities/to_printable.h"

#include "boost/algorithm/string.hpp"
#include "boost/optional.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/lexical_cast.hpp"


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


        // USERS AND AUTHENTICATION

      public:

        //! get userid of owner
        std::string get_userid() const { return(this->userid); }

      protected:

        //! detect userid
        void detect_userid();


        // GRAPHVIZ SUPPORT

      public:

        //! set name of dot executable
        void set_dot_executable(std::string e);

        //! determine whether dot is available
        bool has_dot() { if(!this->graphviz_cached) this->detect_graphviz(); return this->dot_available; }

        //! execute a dot script
        //! returns exit code provided by system()
        int execute_dot(const boost::filesystem::path& script, const boost::filesystem::path& output, const std::string& format);

      protected:

        //! detect Graphviz installation details
        void detect_graphviz();


        // PYTHON SUPPORT

      public:

        //! set name of python executable
        void set_python_executable(std::string e);

        //! determine whether a Python interpreter is available
        bool has_python() { if(!this->python_cached) this->detect_python(); return(this->python_available); }

        //! execute a Python script;
        //! returns exit code provided by system()
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

        //! determine whether the tick_label kwarg is available
        bool has_matplotlib_tick_label() { if(!this->matplotlib_cached) this->detect_matplotlib(); return(this->matplotlib_tick_label); }

        //! determine whether Seaborn is available
        bool has_seaborn() { if(!this->seaborn_cached) this->detect_seaborn(); return(this->seaborn_available); }

      protected:

        //! detect Matplotlib installation
        void detect_matplotlib();

        //! detect whether base Matplotlib import works
        void detect_matplotlib_base();

        //! detect whether we need to specify a backend
        void detect_matplotlib_needs_backend();

        //! detect whether style sheets are available
        void detect_matplotlib_stylesheets();

        //! detect whether axes tick labels can be set
        void detect_matplotlib_has_tick_labels();

        //! detect seaborn installation
        void detect_seaborn();
        
        
        // EMAIL SEND SCRIPT SUPPORT
        
      public:

        //! set name of sendmail executable (usually a script)
        void set_sendmail_executable(std::string e);
        
        //! determine whether send email script is available
        bool has_sendmail() { if(!this->sendmail_cached) this->detect_sendmail(); return sendmail_available; }
        
        //! execute sendmail script
        int execute_sendmail(const std::string& body, const std::string& to);
        
      protected:
        
        //! detect send email script
        void detect_sendmail();


        // TERMINAL PROPERTIES

      public:

        //! determine whether the terminal we are running in has support for ANSI colourized output
        bool has_colour_terminal_support() const { return this->colour_output; }

        //! determine current terminal width
        unsigned int detect_terminal_width(unsigned int default_width) const;

      protected:

        //! detect terminal type
        void detect_terminal_type();


        // ENVIRONMENT PATHS

      public:

        //! get path to config file, if it exists
        boost::optional< boost::filesystem::path > config_file_path() const;
        
        //! manufacture finder object used to search for resources/assets,
        //! with optional tail relative to $CPPTRANSPORT_PATH
        std::unique_ptr<finder> make_resource_finder(boost::filesystem::path tail = boost::filesystem::path{}) const;
  
      protected:

        //! detect home directory
        void detect_home();


        // INTERNAL DATA

      protected:
        
        // DELGATES
        
        //! filesystem object finder
        finder find;

        
        // USERS AND AUTHENTICATION

        //! userid
        std::string userid;

        
        // ENVIRONMENT PATHS
        
        //! user home directory, if detected
        boost::optional< boost::filesystem::path > home;
    
    
        // GRAPHVIZ SUPPORT

        //! has the status of dot support been cached
        bool graphviz_cached{false};

        //! is dot available?
        bool dot_available{false};

        //! name of dot executable
        std::string dot_executable{CPPTRANSPORT_DEFAULT_DOT_EXECUTABLE};

        //! dot executable
        boost::filesystem::path dot_location;


        // PYTHON SUPPORT

        //! has the status of Python support been cached?
        bool python_cached{false};

        //! is Python available?
        bool python_available{false};

        //! name of Python executable
        std::string python_executable{CPPTRANSPORT_DEFAULT_PYTHON_EXECUTABLE};

        //! Python executable
        boost::filesystem::path python_location;


        // MATPLOTLIB SUPPORT

        //! has the status of Matplotlib availability been cached?
        bool matplotlib_cached{false};

        //! has Matplotlib available?
        bool matplotlib_available{false};

        //! Matplotlib has style sheet support?
        bool matplotlib_style_sheets{false};

        //! Matplotlib has the tick_label kwarg?
        bool matplotlib_tick_label{false};

        //! has the status of Seaborn availability been cached?
        bool seaborn_cached{false};

        //! is Seaborn available?
        bool seaborn_available{false};
        
        
        // SEND EMAIL SUPPORT

        //! has the status of sendmail support been cached
        bool sendmail_cached{false};
        
        //! is the sendmail script available?
        bool sendmail_available{false};

        //! name of sendmail executable
        std::string sendmail_executable{CPPTRANSPORT_DEFAULT_SENDMAIL_EXECUTABLE};
        
        //! location of sendmail script
        boost::filesystem::path sendmail_location;


        // TERMINAL PROPERTIES

        //! terminal supports colour output?
        bool colour_output{false};

      };


    local_environment::local_environment()
      {
        // add $PATH to object finder
        find.add_environment_variable(CPPTRANSPORT_SHELL_PATH_ENV);
        
        // detect user id
        this->detect_userid();

        // detect home directory
        this->detect_home();

        // detect terminal colour support
        this->detect_terminal_type();

        // detection of Python support (Python interpreter, Matplotlib, style sheets, Seaborn, etc.) is
        // deferred until needed, since it slows down program initialization

        // detection of Graphviz and sendmail support is also deferred until needed
      }


    void local_environment::detect_userid()
      {
        struct passwd* pw;
        uid_t uid;

        uid = geteuid();
        pw = getpwuid(uid);

        if(pw != nullptr) this->userid = std::string(pw->pw_name);
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
        char* term_type_cstr = std::getenv(CPPTRANSPORT_TERM_ENV);

        if(term_type_cstr == nullptr)
          {
            this->colour_output = false;
            return;
          }

        std::string term_type(term_type_cstr);

        this->colour_output =
             term_type == "xterm"
          || term_type == "xterm-color"
          || term_type == "xterm-256color"
          || term_type == "screen"
          || term_type == "linux"
          || term_type == "cygwin";
      }


    void local_environment::set_python_executable(std::string e)
      {
        this->python_executable = std::move(e);

        this->python_cached = false;
        this->python_available = false;
        this->python_location.clear();
      }


    void local_environment::set_dot_executable(std::string e)
      {
        this->dot_executable = std::move(e);

        this->dot_available = false;
        this->graphviz_cached = false;
        this->dot_location.clear();
      }


    void local_environment::set_sendmail_executable(std::string e)
      {
        this->sendmail_executable = std::move(e);

        this->sendmail_available = false;
        this->sendmail_cached = false;
        this->sendmail_location.clear();
      }


    void local_environment::detect_graphviz()
      {
        // TODO: platform introspection
        auto dot = this->find.find(this->dot_executable);

        this->graphviz_cached = true;

        if(!dot)
          {
            this->dot_available = false;
            this->dot_location = boost::filesystem::path{CPPTRANSPORT_DEFAULT_DOT_LOCATION} / this->dot_executable;
            return;
          }

        this->dot_available = true;
        this->dot_location = *dot;
      }


    void local_environment::detect_sendmail()
      {
        // TODO: platform introspection
        auto sendmail = this->find.find(this->sendmail_executable);

        this->sendmail_cached = true;

        if(!sendmail)
          {
            this->sendmail_available = false;
            this->sendmail_location =
              boost::filesystem::path{CPPTRANSPORT_DEFAULT_SENDMAIL_LOCATION} / this->sendmail_executable;
            return;
          }
        
        this->sendmail_available = true;
        this->sendmail_location = *sendmail;
      }


    void local_environment::detect_python()
      {
        // TODO: Platform introspection
        auto python = this->find.find(this->python_executable);

        this->python_cached = true;

        if(!python)
          {
            this->python_available = false;
            this->python_location =
              boost::filesystem::path{CPPTRANSPORT_DEFAULT_PYTHON_LOCATION} / this->python_executable;
            return;
          }
        
        this->python_available = true;
        this->python_location = *python;
      }


    void local_environment::detect_matplotlib()
      {
        // python detection is lazy; we don't look for it until we need it
        if(!this->python_cached) this->detect_python();

        if(!this->python_available)
          {
            this->matplotlib_available = false;
            return;
          }

        this->detect_matplotlib_base();
        this->matplotlib_cached = true;
        if(!this->matplotlib_available) return;

        this->detect_matplotlib_needs_backend();
        this->detect_matplotlib_stylesheets();
        this->detect_matplotlib_has_tick_labels();
      }


    void local_environment::detect_matplotlib_base()
      {
        // get name of temporary file
        boost::filesystem::path temp_mpl = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

        std::ofstream outf(temp_mpl.string(), std::ios_base::out | std::ios_base::trunc);
        outf << "import matplotlib.pyplot as plt" << '\n';
        outf.close();

        this->matplotlib_available = this->execute_python(temp_mpl) == 0;
      }


    void local_environment::detect_matplotlib_needs_backend()
      {
        // get name of temporary file
        boost::filesystem::path temp_mpl = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

        std::ofstream outf(temp_mpl.string(), std::ios_base::out | std::ios_base::trunc);
        outf << "import matplotlib.pyplot as plt" << '\n';
        outf << "plt.figure()" << '\n';
        outf.close();

        if(this->execute_python(temp_mpl) == 0) return;
      }


    void local_environment::detect_matplotlib_stylesheets()
      {
        // get name of second temporary file
        boost::filesystem::path temp_sheets = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

        std::ofstream outf(temp_sheets.string(), std::ios_base::out | std::ios_base::trunc);
        outf << "import matplotlib.pyplot as plt" << '\n';
        outf << "plt.style.use('ggplot')" << '\n';
        outf.close();

        this->matplotlib_style_sheets = this->execute_python(temp_sheets) == 0;
      }


    void local_environment::detect_matplotlib_has_tick_labels()
      {
        // get name of third temporary file
        boost::filesystem::path temp_ticks = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

        std::ofstream outf(temp_ticks.string(), std::ios_base::out | std::ios_base::trunc);
        outf << "import matplotlib.pyplot as plt" << '\n';
        outf << "plt.figure()" << '\n';
        outf << "left = [ 0, 1, 2 ]" << '\n';
        outf << "height = [ 1, 2, 3 ]" << '\n';
        outf << "label = [ '1', '2', '3' ]" << '\n';
        outf << "plt.bar(left, height, tick_label=label)" << '\n';
        outf.close();

        this->matplotlib_tick_label = this->execute_python(temp_ticks) == 0;
      }


    void local_environment::detect_seaborn()
      {
        // python detection is lazy; we don't look for it until we need it
        if(!this->python_cached) this->detect_python();

        if(!this->python_available)
          {
            this->seaborn_available = false;
            return;
          }

        // get name of temporary file
        boost::filesystem::path temp_sns = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

        std::ofstream outf(temp_sns.string(), std::ios_base::out | std::ios_base::trunc);
        outf << "import seaborn as sns" << '\n';
        outf << "sns.set()" << '\n';
        outf.close();

        this->seaborn_available = this->execute_python(temp_sns) == 0;

        this->seaborn_cached = true;
      }
    
    
    inline boost::optional<std::string> source_profile()
      {
        // TODO: Platform introspection
        const char* user_home = std::getenv(CPPTRANSPORT_HOME_ENV);
        if(user_home == nullptr) return boost::none;

        auto user_profile = boost::filesystem::path{std::string{user_home}} /
          boost::filesystem::path{std::string{CPPTRANSPORT_PROFILE_CONFIG_FILE}};

        if(!boost::filesystem::exists(user_profile)) return {};

        std::ostringstream command;

        // . is the POSIX command for 'source'; 'source' is a csh command which has been imported to other shells
        command << ". " << user_profile.string() << "; ";

        return command.str();
      }


    int local_environment::execute_python(const boost::filesystem::path& script)
      {
        // python detection is lazy; we don't look for it until we need it
        if(!this->python_cached) this->detect_python();
        
        if(!this->python_available) return EXIT_FAILURE;
        
        std::ostringstream command;

        // source user's .profile script if it exists
        auto src_cmd = source_profile();
        if(src_cmd) command << *src_cmd;
        
        command << this->python_location.string() << " \"" << script.string() << "\" > /dev/null 2>&1";
        return std::system(command.str().c_str());
      }


    int local_environment::execute_dot(const boost::filesystem::path& script, const boost::filesystem::path& output, const std::string& format)
      {
        // detection is lazy
        if(!this->graphviz_cached) this->detect_graphviz();

        if(!this->dot_available) return EXIT_FAILURE;

        std::ostringstream command;
    
        // source user's .profile script if it exists
        auto src_cmd = source_profile();
        if(src_cmd) command << *src_cmd;

        command << this->dot_location.string() << " -T" << format << " \"" << script.string() << "\" -o \"" << output.string() << "\" > /dev/null 2>&1";
        return std::system(command.str().c_str());
      }
    
    
    int local_environment::execute_sendmail(const std::string& body, const std::string& to)
      {
        // detection is lazy
        if(!this->sendmail_cached) this->detect_sendmail();

        if(!this->sendmail_available) return EXIT_FAILURE;

        std::ostringstream command;
    
        // write message body to a temporary file
        boost::filesystem::path body_file = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
        std::ofstream outf(body_file.string(), std::ios_base::out | std::ios_base::trunc);
        outf << body;
        outf.close();
    
        // source user's .profile script if it exists
        auto src_cmd = source_profile();
        if(src_cmd) command << *src_cmd;

        command << this->sendmail_location << " --body " << body_file.string() << " --rcpt " << to_printable(to) << " > /dev/null 2>&1";
        return std::system(command.str().c_str());
      }


    boost::optional<boost::filesystem::path> local_environment::config_file_path() const
      {
        if(!this->home) return boost::optional<boost::filesystem::path>();

        boost::filesystem::path config_path = *this->home;

        return config_path / CPPTRANSPORT_RUNTIME_CONFIG_FILE;
      }
    
    
    std::unique_ptr<finder> local_environment::make_resource_finder(boost::filesystem::path tail) const
      {
        auto f = std::make_unique<finder>();
        
        f->add_environment_variable(CPPTRANSPORT_PATH_ENV, std::move(tail));
        
        return f;
      }


    unsigned int local_environment::detect_terminal_width(unsigned int default_width) const
      {
        // TODO: Platform introspection
        
        // Attempt to read terminal display width (assuming the output *is* a terminal)
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        if(w.ws_col > 0) return w.ws_col;
        
        // fall back on trying to read $COLUMNS
        const char* columns = std::getenv(CPPTRANSPORT_COLUMNS_ENV);
        if(columns != nullptr)
          {
            std::string col_str{columns};
            unsigned int n_col = 0;
            try
              {
                n_col = boost::lexical_cast<unsigned int>(col_str);
              }
            catch(boost::bad_lexical_cast& xe)
              {
              }

            if(n_col > 0) return n_col;
          }
        
        return default_width;
      }


  }   // namespace transport

#endif //CPPTRANSPORT_ENVIRONMENT_H
