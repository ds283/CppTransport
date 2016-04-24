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

#ifndef CPPTRANSPORT_CONTAINER_DISPATCH_FUNCTION_H
#define CPPTRANSPORT_CONTAINER_DISPATCH_FUNCTION_H


namespace transport
  {

    // forward-declare generic batcher
    class generic_batcher;

    class container_dispatch_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        container_dispatch_function() = default;

        //! destructor is default
        virtual ~container_dispatch_function() = default;


        // INTERFACE

      public:

        //! dispatch container to master node
        virtual void operator()(generic_batcher& batcher) = 0;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_CONTAINER_DISPATCH_FUNCTION_H
