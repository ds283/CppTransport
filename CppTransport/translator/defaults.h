//
// Created by David Seery on 13/04/2016.
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

#ifndef CPPTRANSPORT_DEFAULTS_H
#define CPPTRANSPORT_DEFAULTS_H


constexpr double DEFAULT_ABS_ERR   = 1E-6;
constexpr double DEFAULT_REL_ERR   = 1E-6;
constexpr double DEFAULT_STEP_SIZE = 1E-12;
constexpr auto   DEFAULT_STEPPER   = "runge_kutta_dopri5";

constexpr unsigned int DEFAULT_MAX_ERROR_COUNT = 20;


#endif //CPPTRANSPORT_DEFAULTS_H
