//
// Created by David Seery on 07/01/2014.
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


#ifndef CPPTRANSPORT_VERSION_H
#define CPPTRANSPORT_VERSION_H


namespace transport
  {

    constexpr auto         CPPTRANSPORT_BUILT_USING         = "built using CppTransport";
    constexpr auto         CPPTRANSPORT_NAME                = "CppTransport";
    constexpr auto         CPPTRANSPORT_VERSION             = "2021.1 beta";
    constexpr auto         CPPTRANSPORT_COPYRIGHT           = "(c) University of Sussex 2016-2021";
    
    constexpr auto         CPPTRANSPORT_RUNTIME_API         = "runtime version 2021.1 beta";
    constexpr unsigned int CPPTRANSPORT_RUNTIME_API_VERSION = 202101;
    
  }   // namespace transport


#endif //CPPTRANSPORT_VERSION_H
