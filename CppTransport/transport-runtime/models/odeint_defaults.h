//
// Created by David Seery on 25/07/2017.
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

#ifndef CPPTRANSPORT_ODEINT_DEFAULTS_H
#define CPPTRANSPORT_ODEINT_DEFAULTS_H


// supply defaults for the algebra name and operations name used by odeint-v2
// if no user-supplied versions have already been defined

#ifndef CPPTRANSPORT_ALGEBRA_NAME
#define CPPTRANSPORT_ALGEBRA_NAME(state) typename boost::numeric::odeint::algebra_dispatcher<state>::algebra_type
#endif

#ifndef CPPTRANSPORT_OPERATIONS_NAME
#define CPPTRANSPORT_OPERATIONS_NAME(state) typename boost::numeric::odeint::operations_dispatcher<state>::operations_type
#endif


#endif //CPPTRANSPORT_ODEINT_DEFAULTS_H
