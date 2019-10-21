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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//



#ifndef CPPTRANSPORT_Y_DRIVER_H
#define CPPTRANSPORT_Y_DRIVER_H


#include <iostream>
#include <string>

#include "semantic_values.h"
#include "model_descriptor.h"

#include "symbol_factory.h"
#include "argument_cache.h"
#include "local_environment.h"

#include "y_common.h"

#include "expression_tree_driver.h"
#include "lagrangian_driver.h"
#include "metadata_driver.h"
#include "templates_driver.h"
#include "misc_driver.h"
#include "cosmology_driver.h"

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
        y_driver(model_descriptor& r, symbol_factory& s, argument_cache& c, local_environment& e);

        //! destructor is default
        ~y_driver() = default;


		    // INTERFACE -- FAILURE AND ERROR CONDITIONS

      public:
    
        //! raise error, bypassing normal context-reporting system (eg. if no context can be found)
        void error(std::string msg);
    
    
        // INTERNAL DATA

      private:

        // MODEL DESCRIPTOR
        
        //! reference to model descriptor supplied by parent translation_unit;
        //! this encapsulates all data about the model and is populated
        //! as parsing progresses
        model_descriptor& root;


        // REFERENCES TO DELEGATE OBJECTS

        //! reference to symbol factory
		    symbol_factory& sym_factory;

        //! reference to argument cache
        argument_cache& cache;

        //! reference to local environment
        local_environment& env;
    
    
        // FUNCTIONALITY BLOCKS
        // (must come after construction of root field)
  
      public:
    
        //! expression trees
        expression_tree_driver expr;
    
        //! lagrangian data
        lagrangian_driver model;
    
        //! metadata
        metadata_driver meta;
    
        //! template data
        templates_driver templates;
    
        //! miscellaneous settings and methods
        misc_driver misc;

        // cosmology_block data
        cosmology_driver cosmo;

	    };

	}


#endif // CPPTRANSPORT_Y_DRIVER_H
