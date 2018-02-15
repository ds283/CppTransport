//
// Created by David Seery on 26/05/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_POLICIES_H
#define CPPTRANSPORT_POLICIES_H


constexpr auto POLICY_DEFAULT_VERSION = 201603;

// versions of CppTransport prior to 201801 computed UUIDs incorrectly
constexpr auto POLICY_HEX_UUID = 1;
constexpr auto POLICY_HEX_UUID_VERSION = 201801;


#endif //CPPTRANSPORT_POLICIES_H
