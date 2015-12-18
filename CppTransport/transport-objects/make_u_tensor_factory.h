//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MAKE_U_TENSOR_FACTORY_H
#define CPPTRANSPORT_MAKE_U_TENSOR_FACTORY_H


#include "u_tensor_factory.h"
#include "canonical_u_tensor_factory.h"

#include "backend_data.h"


// factory function to manufacture a u_tensor_factory instance
inline std::unique_ptr<u_tensor_factory> make_u_tensor_factory(backend_data& backend, translator_data& p, expression_cache& cache)
  {
    // at the moment, nothing to do - only canonical models implemented
    return std::make_unique<canonical_u_tensor_factory>(p, cache);
  }


#endif //CPPTRANSPORT_MAKE_U_TENSOR_FACTORY_H
