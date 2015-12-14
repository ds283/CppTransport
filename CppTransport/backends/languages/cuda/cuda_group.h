//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __cuda_group_H_
#define __cuda_group_H_


#include "package_group.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "kernel_argument_macros.h"
#include "cuda_printer.h"


class cuda_group : public package_group
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    cuda_group(translator_data& p, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache);

    //! destructor is default
    ~cuda_group() = default;


    // IMPLEMENTATION -- FOR LOOP

  public:

    //! plant a 'for' loop
    virtual std::string plant_for_loop(const std::string& loop_variable, unsigned int min, unsigned int max) const override;


    // INTERNAL API

  protected:

    cuda::cuda_printer                  printer;

  };


#endif //__cuda_group_H_
