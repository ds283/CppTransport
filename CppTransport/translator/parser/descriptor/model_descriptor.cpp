//
// Created by David Seery on 18/06/2013.
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

#include <sstream>
#include <assert.h>
#include <algorithm>

#include "lexical.h"
#include "model_descriptor.h"


// ******************************************************************


model_descriptor::model_descriptor(symbol_factory& s, error_context err_ctx)
  : err_count(0),
    model(err_count, s, std::move(err_ctx)),
    meta(err_count),
    templates(err_count),
    misc(err_count)
  {
  }
