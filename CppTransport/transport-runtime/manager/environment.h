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

#include <sys/ioctl.h>
#include <unistd.h>
#include <pwd.h>

#include "transport-runtime/defaults.h"
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

        //! determine whether dot is available
        bool has_dot() const { return this->dot_available; }

        //! execute a dot script
        //! returns exit code provided by system()
        int execute_dot(const boost::filesystem::path& script, const boost::filesystem::path& output, const std::string& format);

      protected:

        //! detect Graphviz installation details
        void detect_graphviz();


        // PYTHON SUPPORT

      public:

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
        
        //! determine whether send email script is available
        bool has_sendmail() const { return sendmail_available; }
        
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

        //! list of paths to search for resources/assets
        //! may be empty if not paths have been specified
        const std::list<boost::filesystem::path>& get_resource_paths() const;

      protected:

        //! detect home direcotry
        void detect_home();


        // INTERNAL DATA

      protected:

        // USERS AND AUTHENTICATION

        //! userid
        std::string userid;

        // ENVIRONMENT PATHS

        //! user home directory
        boost::optional< boost::filesystem::path > home;

        //! CppTransport resource installation paths
        std::list< boost::filesystem::path > resources;


        // GRAPHVIZ SUPPORT

        //! is dot available?
        bool dot_available;

        //! dot executable
        boost::filesystem::path dot_location;


        // PYTHON SUPPORT

        //! has the status of Python support been cached?
        bool python_cached;

        //! is Python available?
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

        //! Matplotlib has the tick_label kwarg?
        bool matplotlib_tick_label;

        //! has the status of Seaborn availability been cached?
        bool seaborn_cached;

        //! is Seaborn available?
        bool seaborn_available;
        
        
        // SEND EMAIL SUPPORT
        
        //! is the sendmail script available?
        bool sendmail_available;
        
        //! location of sendmail script
        boost::filesystem::path sendmail_location;


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
        matplotlib_tick_label(false),
        seaborn_cached(false),
        seaborn_available(false),
        dot_available(false),
        sendmail_available(false)
      {
        // detect user id
        this->detect_userid();

        // detect home directory
        this->detect_home();

        // detect terminal colour support
        this->detect_terminal_type();

        // detect Graphviz installation
        this->detect_graphviz();
        
        // deteect sendmail
        this->detect_sendmail();

        // detection of Python support (Python interpreter, Matplotlib, style sheets, Seaborn, etc.) is
        // deferred until needed, since it slows down program initialization
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

        char* path_cstr = std::getenv(CPPTRANSPORT_PATH_ENV);

        if(path_cstr != nullptr)
          {
            std::string resource_paths(path_cstr);

            // parse environment variable into a : separated list of directories
            for(boost::algorithm::split_iterator<std::string::iterator> t = boost::algorithm::make_split_iterator(resource_paths, boost::algorithm::first_finder(":", boost::algorithm::is_equal()));
                t != boost::algorithm::split_iterator<std::string::iterator>(); ++t)
              {
                std::string path = boost::copy_range<std::string>(*t);

                boost::filesystem::path p = path;

                // if path is not absolute, make relative to current working directory
                if(!p.is_absolute())
                  {
                    p = boost::filesystem::absolute(p);
                  }

                this->resources.emplace_back(p);
              }
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


    void local_environment::detect_graphviz()
      {
        // TODO: platform introspection
        FILE* f = popen("which dot", "r");

        if(!f)
          {
            this->dot_available = false;
            this->dot_location = CPPTRANSPORT_DEFAULT_DOT_PATH;
          }
        else
          {
            char buffer[1024];
            char* line = fgets(buffer, sizeof(buffer), f);
            pclose(f);

            if(line != nullptr)
              {
                this->dot_available = true;
                std::string temp = std::string(line);
                boost::algorithm::trim_right(temp);
                this->dot_location = temp;
              }
            else
              {
                this->dot_available = false;
                this->dot_location = CPPTRANSPORT_DEFAULT_DOT_PATH;
              }
          }
      }
    
    
    void local_environment::detect_sendmail()
      {
        // TODO: platform introspection
        FILE* f = popen("which CppTransport-sendmail", "r");
        
        if(!f)
          {
            this->sendmail_available = false;
            this->sendmail_location = CPPTRANSPORT_DEFAULT_SENDMAIL_PATH;
          }
        else
          {
            char buffer[1024];
            char* line = fgets(buffer, sizeof(buffer), f);
            pclose(f);
    
            if(line != nullptr)
              {
                this->sendmail_available = true;
                std::string temp = std::string(line);
                boost::algorithm::trim_right(temp);
                this->sendmail_location = temp;
              }
            else
              {
                this->sendmail_available = false;
                this->sendmail_location = CPPTRANSPORT_DEFAULT_SENDMAIL_PATH;
              }
          }
      }


    void local_environment::detect_python()
      {
        // TODO: Platform introspection
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
                this->python_location = CPPTRANSPORT_DEFAULT_PYTHON_PATH;
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


    int local_environment::execute_python(const boost::filesystem::path& script)
      {
        if(!this->python_cached) this->detect_python();
        
        if(!this->python_available) return EXIT_FAILURE;
        
        std::ostringstream command;

        // source user's .profile script if it exists
        // TODO: Platform introspection
        const char* user_home = getenv("HOME");
        if(user_home != nullptr)
          {
            boost::filesystem::path user_profile = boost::filesystem::path{std::string{user_home}} / boost::filesystem::path{std::string{".profile"}};
            if(boost::filesystem::exists(user_profile))
              {
                // . is the POSIX command for 'source'; 'source' is a csh command which has been imported to other shells
                command << ". " << user_profile.string() << "; ";
              }
          }
        
        command << this->python_location.string() << " \"" << script.string() << "\" > /dev/null 2>&1";
        return std::system(command.str().c_str());
      }


    int local_environment::execute_dot(const boost::filesystem::path& script, const boost::filesystem::path& output, const std::string& format)
      {
        if(!this->dot_available) return EXIT_FAILURE;

        std::ostringstream command;

        // source user's .profile script if it exists
        // TODO: Platform introspection
        const char* user_home = getenv("HOME");
        if(user_home != nullptr)
          {
            boost::filesystem::path user_profile = boost::filesystem::path{std::string{user_home}} / boost::filesystem::path{std::string{".profile"}};
            if(boost::filesystem::exists(user_profile))
              {
                // . is the POSIX command for 'source'; 'source' is a csh command which has been imported to other shells
                command << ". " << user_profile.string() << "; ";
              }
          }

        command << this->dot_location.string() << " -T" << format << " \"" << script.string() << "\" -o \"" << output.string() << "\" > /dev/null 2>&1";
        return std::system(command.str().c_str());
      }
    
    
    int local_environment::execute_sendmail(const std::string& body, const std::string& to)
      {
        if(!this->sendmail_available) return EXIT_FAILURE;

        std::ostringstream command;
    
        // write message body to a temporary file
        boost::filesystem::path body_file = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
        std::ofstream outf(body_file.string(), std::ios_base::out | std::ios_base::trunc);
        outf << body;
        outf.close();
        
        // source user's .profile script if it exists
        // TODO: Platform introspection
        const char* user_home = getenv("HOME");
        if(user_home != nullptr)
          {
            boost::filesystem::path user_profile = boost::filesystem::path{std::string{user_home}} / boost::filesystem::path{std::string{".profile"}};
            if(boost::filesystem::exists(user_profile))
              {
                // . is the POSIX command for 'source'; 'source' is a csh command which has been imported to other shells
                command << ". " << user_profile.string() << "; ";
              }
          }

        command << this->sendmail_location << " --body " << body_file.string() << " --rcpt " << to_printable(to) << " > /dev/null 2>&1";
        return std::system(command.str().c_str());
      }


    boost::optional<boost::filesystem::path> local_environment::config_file_path() const
      {
        if(!this->home) return boost::optional<boost::filesystem::path>();

        boost::filesystem::path config_path = *this->home;

        return config_path / CPPTRANSPORT_RUNTIME_CONFIG_FILE;
      }


    const std::list<boost::filesystem::path>& local_environment::get_resource_paths() const
      {
        return this->resources;
      }


    unsigned int local_environment::detect_terminal_width(unsigned int default_width) const
      {
        // TODO: Platform introspection
        
        // Attempt to read terminal display width (assuming the output *is* a terminal)
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        if(w.ws_col > 0) return w.ws_col;
        
        // fall back on trying to read $COLUMNS
        const char* columns = std::getenv("COLUMNS");
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
