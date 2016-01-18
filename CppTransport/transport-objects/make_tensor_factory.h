//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MAKE_FACTORY_H
#define CPPTRANSPORT_MAKE_FACTORY_H


#include "backend_data.h"

#include "canonical/tensor_factory.h"

// generate an appropriate tensor_factory instance
inline std::unique_ptr<tensor_factory> make_tensor_factory(backend_data& backend, translator_data& p, expression_cache& cache)
  {
    // at the moment, nothing to do - only canonical models implemented
    std::unique_ptr<canonical::tensor_factory> obj = std::make_unique<canonical::tensor_factory>(p, cache);
    return std::move(obj);      // std::move() required because no implicit conversion for upcast
  }



#endif //CPPTRANSPORT_MAKE_FACTORY_H
