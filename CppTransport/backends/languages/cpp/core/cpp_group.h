//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __core_group_H_
#define __core_group_H_


#include "package_group.h"
#include "ginac_cache.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "cpp_steppers.h"
#include "cpp_printer.h"


class cpp_group : public package_group
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    cpp_group(translator_data& p, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache);

    //! destructor is default
    virtual ~cpp_group() = default;

  };


#endif //__core_group_H_
