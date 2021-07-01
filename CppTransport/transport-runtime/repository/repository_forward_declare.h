//
// Created by David Seery on 02/05/2016.
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

// no need to check whether CPPTRANSPORT_REPOSITORY_H is defined, as in other forward_declare type headers,
// because repository.h includes this file in order to get a correct declaration of the template default arguments
#ifndef CPPTRANSPORT_REPOSITORY_FORWARD_DECLARE_H
#define CPPTRANSPORT_REPOSITORY_FORWARD_DECLARE_H


#include "transport-runtime/defaults.h"


namespace transport
  {

    // forward declare default content group policy
    namespace repository_impl
      {

        template <typename ContentDatabaseType> class MostRecentContentPolicy;

      }   // namespace repository_impl

    // forward-declare repository class
    template <typename number=default_number_type> class repository;

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_FORWARD_DECLARE_H
