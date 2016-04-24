//
// Created by David Seery on 25/01/2016.
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

#ifndef CPPTRANSPORT_MESSAGE_HANDLERS_H
#define CPPTRANSPORT_MESSAGE_HANDLERS_H


#include <string>

#include "transport-runtime/manager/argument_cache.h"
#include "transport-runtime/manager/environment.h"

#include "transport-runtime/ansi_colour_codes.h"
#include "transport-runtime/messages.h"


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
