//
// Created by David Seery on 22/05/2017.
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

#ifndef CPPTRANSPORT_UTILITIES_H
#define CPPTRANSPORT_UTILITIES_H


#include "semantic_data.h"
#include "model_descriptor.h"
#include "symbol_factory.h"
#include "argument_cache.h"
#include "local_environment.h"


namespace y
  {
    
    class utilities
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        utilities(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le);
        
        //! destructor is default
        ~utilities() = default;
    
        
        // INTERFACE -- UTILITY METHODS TO EMPLACE ATTRIBUTE DATA
  
      public:
    
        void set_indexorder_left();
    
        void set_indexorder_right();
    
        void add_latex_attribute(attributes* a, lexeme_type* lex);
    
        void add_string(string_array* a, lexeme_type* lex);
    
    
        // INTERNAL DATA
      
      protected:
        
        //! model description container
        model_descriptor& root;
        
        //! delegated symbol factory
        symbol_factory& sym_factory;
        
        //! delegated argument cache
        argument_cache& cache;
        
        //! delegated local environment
        local_environment& env;
        
      };
    
  }


#endif //CPPTRANSPORT_UTILITIES_H
