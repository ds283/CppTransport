//
// Created by David Seery on 29/10/2019.
// --@@
// Copyright (c) 2019 University of Sussex. All rights reserved.
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


#include <string>

#include "boost/algorithm/string.hpp"

#include "string_to_bool.h"

bool string_to_bool(const std::string& in)
  {
    auto in_lower = boost::algorithm::to_lower_copy(in);

    return in_lower == 'true' || in_lower == 'yes' || in_lower == '1'
           || in_lower == 't' || in_lower == 'y'
  }
