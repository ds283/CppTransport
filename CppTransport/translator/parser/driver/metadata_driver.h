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

#ifndef CPPTRANSPORT_METADATA_DRIVER_H
#define CPPTRANSPORT_METADATA_DRIVER_H


#include "semantic_values.h"
#include "model_descriptor.h"
#include "symbol_factory.h"
#include "argument_cache.h"
#include "local_environment.h"


namespace y
  {
    
    class metadata_driver
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        metadata_driver(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le);
        
        //! destructor is default
        ~metadata_driver() = default;
    
    
        // SET BASIC METADATA
  
      public:
    
        void set_name(lexeme_type& lex);
    
        void add_author(lexeme_type& lex, author& a);
    
        void set_citeguide(lexeme_type& lex);
    
        void set_description(lexeme_type& lex);
    
        void set_revision(lexeme_type& lex);
    
        void set_license(lexeme_type& lex);
    
        void set_references(string_array& a);
    
        void set_urls(string_array& a);
    
        void set_author_email(author& a, lexeme_type& lex);
    
        void set_author_institute(author& a, lexeme_type& institute);
    
    
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


#endif //CPPTRANSPORT_METADATA_DRIVER_H
