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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_UTILITIES_DRIVER_H
#define CPPTRANSPORT_UTILITIES_DRIVER_H


#include "semantic_values.h"
#include "model_descriptor.h"
#include "symbol_factory.h"
#include "argument_cache.h"
#include "local_environment.h"


namespace y
  {
    
    class misc_driver
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        misc_driver(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le);
        
        //! destructor is default
        ~misc_driver() = default;
        
        
        // PLATFORM SETTINGS
        
      public:
        
        //! set minimum required CppTransport version
        void set_required_version(lexeme_type& lex);
    
        
        // MISCELLANEOUS SETTINGS
  
      public:
    
        //! set indexorder as left-most first
        void set_indexorder_left();
    
        //! set index order as right-most first
        void set_indexorder_right();
        
        
        // UTILITY FUNCTIONS
        
      public:
    
        //! add a LaTeX name to an attribute block
        void set_attribute_latex(attributes& a, lexeme_type& lex);

        //! add a value to an attribute block
        void set_attribute_value(attributes& a, lexeme_type& lex);

        //! add an inital value to an attribute block
        void set_attribute_derivvalue(attributes& a, lexeme_type& lex);

        //! add a prior to an attribute block
        void set_attribute_prior(attributes& a, lexeme_type& lex);

        //! add an inital prior to an attribute block
        void set_attribute_derivprior(attributes& a, lexeme_type& lex);

        //! add an inital prior to an attribute block
        void set_attribute_log_value(attributes& a, lexeme_type& lex);
    
        //! add a string to a string array
        void add_string(string_array& a, lexeme_type& lex);
    
    
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


#endif //CPPTRANSPORT_UTILITIES_DRIVER_H
