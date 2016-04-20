//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MESSAGE_HANDLERS_H
#define CPPTRANSPORT_MESSAGE_HANDLERS_H


#include <string>

#include "transport-runtime-api/manager/argument_cache.h"
#include "transport-runtime-api/manager/environment.h"

#include "transport-runtime-api/ansi_colour_codes.h"
#include "transport-runtime-api/messages.h"


namespace transport
  {

    class error_handler
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures local_environment and argument_cache objects
        error_handler(local_environment& e, argument_cache& a)
          : env(e),
            args(a)
          {
          }

        //! destructor is default
        ~error_handler() = default;


        // INTERFACE

      public:

        //! report an error
        void operator()(const std::string& msg) const;


        // INTERNAL DATA

      private:

        //! reference to local environment object
        local_environment& env;

        //! reference to argument cache object
        argument_cache& args;

      };


    class warning_handler
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures local_environment and argument_cache objects
        warning_handler(local_environment& e, argument_cache& a)
          : env(e),
            args(a)
          {
          }

        //! destructor is default
        ~warning_handler() = default;


        // INTERFACE

      public:

        //! report an error
        void operator()(const std::string& msg) const;


        // INTERNAL DATA

      private:

        //! reference to local environment object
        local_environment& env;

        //! reference to argument cache object
        argument_cache& args;

      };


    class message_handler
      {

        // ASSOCIATED TYPE

      public:

        enum class highlight
          {
            none,
            heading
          };

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures local_environment and argument_cache objects
        message_handler(local_environment& e, argument_cache& a)
          : env(e),
            args(a)
          {
          }

        //! destructor is default
        ~message_handler() = default;


        // INTERFACE

      public:

        //! report an error
        void operator()(const std::string& msg, highlight mode=highlight::none) const;


        // INTERNAL DATA

      private:

        //! reference to local environment object
        local_environment& env;

        //! reference to argument cache object
        argument_cache& args;

      };


    void error_handler::operator()(const std::string& msg) const
      {
        bool colour = this->env.has_colour_terminal_support() && this->args.get_colour_output();

        if(colour) std::cout << ColourCode(ANSI_colour::bold_red);
        std::cout << msg << '\n';
        if(colour) std::cout << ColourCode(ANSI_colour::normal);
      }


    void warning_handler::operator()(const std::string& msg) const
      {
        bool colour = this->env.has_colour_terminal_support() && this->args.get_colour_output();

        if(colour) std::cout << ColourCode(ANSI_colour::bold_magenta);
        std::cout << CPPTRANSPORT_TASK_MANAGER_WARNING_LABEL << " ";
        if(colour) std::cout << ColourCode(ANSI_colour::normal);
        std::cout << msg << '\n';
      }


    void message_handler::operator()(const std::string& msg, highlight mode) const
      {
        bool colour = this->env.has_colour_terminal_support() && this->args.get_colour_output();

        if(this->args.get_verbose())
          {
            if(mode == highlight::heading) if(colour) std::cout << ColourCode(ANSI_colour::green);
            std::cout << msg << '\n';
            if(mode != highlight::none) if(colour) std::cout << ColourCode(ANSI_colour::normal);
          }
      }


  }   // namespace transport


#endif //CPPTRANSPORT_MESSAGE_HANDLERS_H
