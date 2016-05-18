//
// Created by David Seery on 15/04/15.
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


#ifndef CPPTRANSPORT_RANGE_COMMON_H
#define CPPTRANSPORT_RANGE_COMMON_H


#include <vector>
#include <stdexcept>
#include <algorithm>

#include <assert.h>

#include "math.h"

#include "transport-runtime/serialization/serializable.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"


namespace aggregation_range_impl
	{

    template <typename value>
    class DuplicateRemovalPredicate
	    {
      public:
        DuplicateRemovalPredicate(value t)
	        : tol(t)
	        {
	        }

        bool operator()(value& a, value& b)
	        {
            return(std::abs((a-b)/a) < tol);
	        }

      private:
        value tol;
	    };

	}


#define CPPTRANSPORT_NODE_RANGE_TYPE          "range-type"
#define CPPTRANSPORT_NODE_RANGE_STEPPING      "stepping-range"
#define CPPTRANSPORT_NODE_RANGE_AGGREGATE     "aggregation-type"


#endif //#define CPPTRANSPORT_RANGE_COMMON_H

