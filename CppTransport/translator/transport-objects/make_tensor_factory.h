//
// Created by David Seery on 19/12/2015.
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

#ifndef CPPTRANSPORT_MAKE_FACTORY_H
#define CPPTRANSPORT_MAKE_FACTORY_H


#include <stdexcept>

#include "backend_data.h"

#include "canonical/tensor_factory.h"
#include "nontrivial-metric/tensor_factory.h"


// generate an appropriate tensor_factory instance
inline std::unique_ptr<tensor_factory> make_tensor_factory(translator_data& p, expression_cache& cache)
  {
    switch(p.model.get_lagrangian_type())
      {
        case model_type::canonical:
          {
            return std::make_unique<canonical::tensor_factory>(p, cache);
          }

        case model_type::nontrivial_metric:
          {
            return std::make_unique<nontrivial_metric::tensor_factory>(p, cache);
          }

        default:
          throw std::invalid_argument("Invalid lagrangian type");
      }

    // at the moment, nothing to do - only canonical models implemented
  }



#endif //CPPTRANSPORT_MAKE_FACTORY_H
