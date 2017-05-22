//
// Created by David Seery on 17/06/2013.
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



#ifndef CPPTRANSPORT_Y_DRIVER_H
#define CPPTRANSPORT_Y_DRIVER_H


#include <iostream>
#include <string>

#include "semantic_data.h"
#include "model_descriptor.h"

#include "symbol_factory.h"
#include "argument_cache.h"
#include "local_environment.h"

#include "y_common.h"
#include "expression_tree.h"
#include "lagrangian.h"
#include "metadata.h"
#include "templates.h"
#include "utilities.h"


namespace y
	{

    //! y_driver contains the main methods called by the Bison parser
    class y_driver
	    {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        //! error_context is passed down from parent translation_unit.
        //! It is only used to pass to our script object 'root' and is used to construct
        //! fake error contexts for default reserved symbols such as M_Planck
        y_driver(symbol_factory& s, argument_cache& c, local_environment& e, error_context err_ctx);

        //! destructor is default
        ~y_driver() = default;


		    // INTERFACE -- FAILURE AND ERROR CONDITIONS

      public:

        //! detect error conditions reported during processing
        bool failed() const { return(this->root.failed()); }

        //! raise error, bypassing normal context-reporting system (eg. if no context can be found)
        void error(std::string msg);
        
        
        // INTERFACE -- GET MODEL DESCRIPTION CONTAINER
        
      public:

        const model_descriptor& get_descriptor();
        

		    // INTERNAL DATA

      private:

        // MODEL DESCRIPTOR
        
        //! container for model descriptor information
        model_descriptor root;


        // REFERENCES TO DELEGATE OBJECTS

        //! reference to symbol factory
		    symbol_factory& sym_factory;

        //! reference to argument cache
        argument_cache& cache;

        //! reference to local environment
        local_environment& env;
    
    
        // COMPONENT FUNCTIONALITY OBJECTS
  
      public:
    
        //! expression trees
        expression_tree etree;
    
        //! lagrangian data
        lagrangian lag;
    
        //! metadata
        metadata meta;
    
        //! template data
        templates templ;
    
        //! utility methods
        utilities utils;

	    };

	}


#endif // CPPTRANSPORT_Y_DRIVER_H
