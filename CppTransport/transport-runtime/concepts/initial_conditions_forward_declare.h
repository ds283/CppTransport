//
// Created by David Seery on 26/03/15.
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


// no need to check CPPTRANSPORT_INITIAL_CONDITIONS_H since initial_conditions.h includes this file
// in order to get correct declaration of default template parameter
#ifndef CPPTRANSPORT_INITIAL_CONDITIONS_FORWARD_DECLARE_H
#define CPPTRANSPORT_INITIAL_CONDITIONS_FORWARD_DECLARE_H

#include "transport-runtime/defaults.h"

namespace transport
	{

    template <typename number=default_number_type> class initial_conditions;

	}   // namespace transport


#endif //CPPTRANSPORT_INITIAL_CONDITIONS_FORWARD_DECLARE_H
