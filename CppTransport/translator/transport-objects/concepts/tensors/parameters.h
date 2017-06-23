//
// Created by David Seery on 16/06/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_PARAMETERS_H
#define CPPTRANSPORT_PARAMETERS_H


#include <memory>

#include "shared/shared_resources.h"
#include "canonical/resources.h"

#include "concepts/transport_tensor.h"
#include "concepts/flattened_tensor.h"
#include "indices.h"

#include "lambdas.h"
#include "index_literal.h"
#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"


constexpr auto PARAMETER_TENSOR_INDICES = 1;


class parameters: public transport_tensor
  {
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    parameters(language_printer& p, cse& cw, shared_resources& s, index_flatten& f, index_traits& t)
      : printer(p),
        cse_worker(cw),
        shared(s),
        fl(f),
        traits(t)
      {
      }
    
    //! destructor is default
    virtual ~parameters() = default;
    
    
    // INTERFACE
    
  public:
    
    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor>
    compute(const index_literal_list& indices);
    
    //! evaluate component of tensor
    virtual GiNaC::ex
    compute_component(param_index i);
    
    //! evaluate lambda for tensor
    virtual std::unique_ptr<atomic_lambda>
    compute_lambda(const index_literal& i);
    
    
    // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT
  
  public:
    
    //! determine whether this tensor can be unrolled with the current resources
    virtual unroll_behaviour get_unroll(const index_literal_list& idx_list) override;


    // JANITORIAL FUNCTIONS -- API USED BY JANITOR OBJECT

  protected:

    //! cache resources required for evaluation on an explicit index assignment (here, nothing to do)
    void pre_explicit(const index_literal_list& indices) override { return; };

    //! release resources (here, nothing to do)
    void post() override { return; };


    // INTERNAL API
    
  protected:
    
    //! populate workspace
    void populate_workspace();
    
    
    // INTERNAL DATA
  
  private:
    
    // CACHES
    
    //! reference to supplied language printer
    language_printer& printer;
    
    //! reference to supplied CSE worker
    cse& cse_worker;
    
    //! reference to shared resource object
    shared_resources& shared;
    
    
    // AGENTS
    
    //! index flattener
    index_flatten& fl;
    
    //! index introspection
    index_traits& traits;
    
    
    // WORKSPACE AND CACHE
    
    //! list of symbols representing parameters
    std::unique_ptr<symbol_list> param_symbols;
    
    //! cache status
    bool cached;
    
  };


#endif //CPPTRANSPORT_PARAMETERS_H
