//
// Created by David Seery on 12/06/2013.
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

#ifndef CPPTRANSPORT_ERROR_H
#define CPPTRANSPORT_ERROR_H

#include <string>
#include <deque>
#include <memory>

#include "core.h"
#include "error_context.h"
#include "argument_cache.h"
#include "local_environment.h"


constexpr unsigned int ERROR_PATH_LEVEL = 5;
constexpr unsigned int WARN_PATH_LEVEL  = 5;


// PLAIN ERROR MESSAGES, NO CONTEXT INFORMATION

void warn (const std::string& msg, const argument_cache& cache, const local_environment& env);
void error(const std::string& msg, const argument_cache& cache, const local_environment& env);


// CONTEXT BASED ERROR MESSAGES, WITH INPUT LINE AND CHARACTER POSITION IF AVAILABLE


void warn (const std::string& msg, const argument_cache& cache, const local_environment& env, const error_context& context);
void error(const std::string& msg, const argument_cache& cache, const local_environment& env, const error_context& context);

void warn (const std::string& msg, const argument_cache& cache, const local_environment& env, const error_context& context, unsigned int level);
void error(const std::string& msg, const argument_cache& cache, const local_environment& env, const error_context& context, unsigned int level);


#endif //CPPTRANSPORT_ERROR_H
