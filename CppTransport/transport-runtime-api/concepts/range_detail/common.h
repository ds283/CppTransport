//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __range_common_H_
#define __range_common_H_


#include <vector>
#include <stdexcept>
#include <algorithm>

#include <assert.h>

#include "math.h"

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


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


#endif //__range_common_H_
