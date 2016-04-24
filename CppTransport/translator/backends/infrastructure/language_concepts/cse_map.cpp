//
// Created by David Seery on 14/12/2015.
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

#include "cse_map.h"
#include "msg_en.h"


cse_map::cse_map(std::unique_ptr< std::vector<GiNaC::ex> > l, cse& c)
  : list(std::move(l)),
    cse_worker(c)
  {
    // parse the whole vector of expressions;
    // if CSE is disabled, will have no effect
    for(const GiNaC::ex& expr: *list)
      {
        cse_worker.parse(expr);
      }
  }


std::string cse_map::operator[](unsigned int index)
  {
    std::string rval = "";

    // check whether subscripting is within bounds;
    // if so, return appropriate symbol
    // if CSE is disabled this will be just the raw expression
    if(index < this->list->size()) return this->cse_worker.get_symbol_with_use_count((*this->list)[index]);

    throw std::out_of_range(ERROR_OUT_OF_BOUNDS_CSE_MAP);
  }
