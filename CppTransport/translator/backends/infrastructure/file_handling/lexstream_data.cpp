//
// Created by David Seery on 01/12/2015.
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

#include "lexstream_data.h"


lexstream_data::lexstream_data(const boost::filesystem::path& name, error_context::error_handler e,
                               error_context::warning_handler w, finder& s, argument_cache& c,
                               boost::optional<unsigned int> ml)
  : filename(name),
    max_lines(ml),
    err(e),
    wrn(w),
    cache(c),
    search_paths(s)
  {
  }
