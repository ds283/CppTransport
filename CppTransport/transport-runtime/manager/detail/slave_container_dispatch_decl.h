//
// Created by David Seery on 26/01/2016.
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

#ifndef CPPTRANSPORT_SLAVE_CONTAINER_DISPATCH_DECL_H
#define CPPTRANSPORT_SLAVE_CONTAINER_DISPATCH_DECL_H


#include <string>

#include "transport-runtime/data/batchers/container_dispatch_function.h"


namespace transport
  {

    // forward declare slave controller
    template <typename number> class slave_controller;

    template <typename number>
    class slave_container_dispatch: public container_dispatch_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures slave controller, MPI message, and logging tag
        slave_container_dispatch(slave_controller<number>& c, unsigned int m, std::string t)
          : container_dispatch_function(),
            controller(c),
            message(m),
            tag(std::move(t))
          {
          }

        //! destructor is default
        ~slave_container_dispatch() = default;


        // INTERFACE

      public:

        //! dispatch container
        void operator()(generic_batcher& batcher) override;


        // INTERNAL DATA

      private:

        //! reference to controller
        slave_controller<number>& controller;

        //! MPI message
        unsigned int message;

        //! logging tag
        std::string tag;

      };

  }


#endif //CPPTRANSPORT_SLAVE_CONTAINER_DISPATCH_DECL_H
