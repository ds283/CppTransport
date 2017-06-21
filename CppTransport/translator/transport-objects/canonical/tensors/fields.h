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

#ifndef CPPTRANSPORT_CANONICAL_FIELDS_H
#define CPPTRANSPORT_CANONICAL_FIELDS_H


#include <memory>

#include "concepts/tensors/fields.h"
#include "utilities/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"


namespace canonical
  {
    
    class fields : public ::fields
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        fields(language_printer& p, cse& cw, resources& r, shared_resources& s,
               index_flatten& f, index_traits& t);
        
        //! destructor is default
        virtual ~fields() = default;
        
        
        // INTERFACE -- IMPLEMENTS A 'COORDINATES' TENSOR CONCEPT
      
      public:

        //! evaluate full tensor, returning a flattened list
        std::unique_ptr<flattened_tensor>
        compute(const index_literal_list& indices) override;
        
        //! evaluate component of tensor
        GiNaC::ex
        compute_component(field_index i) override;
        
        //! evaluate lambda for tensor
        std::unique_ptr<atomic_lambda>
        compute_lambda(const index_literal& i) override;
    
    
        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT
  
      public:
    
        //! determine whether this tensor can be unrolled with the current resources
        unroll_behaviour get_unroll() override;


        // INTERFACE -- JANITORIAL API

        //! cache resources required for evaluation
        void pre_explicit(const index_literal_list& indices) override;

        //! cache resources required for evaluation on a lambda
        void pre_lambda();

        //! release resources
        void post() override;


        // INTERNAL API


        // INTERNAL DATA
      
      private:
        
        // CACHES
        
        //! reference to supplied language printer
        language_printer& printer;
        
        //! reference to supplied CSE worker
        cse& cse_worker;
        
        //! reference to resource object
        resources& res;
        
        //! reference to shared resource object
        shared_resources& shared;
        
        
        // AGENTS
        
        //! index flattener
        index_flatten& fl;
        
        //! index introspection
        index_traits& traits;
        
        
        // WORKSPACE AND CACHE
        
        //! field symbols
        std::unique_ptr<flattened_tensor> f;
        
        //! cache status
        bool cached;
        
      };
    
  }   // namespace canonical


#endif //CPPTRANSPORT_CANONICAL_FIELDS_H
