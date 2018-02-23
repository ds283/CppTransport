//
// Created by David Seery on 18/06/2013.
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


#ifndef CPPTRANSPORT_MODEL_DESCRIPTOR_H
#define CPPTRANSPORT_MODEL_DESCRIPTOR_H


#include "symbol_factory.h"
#include "version_policy.h"
#include "error_context.h"

#include "lagrangian_block.h"
#include "metadata_block.h"
#include "templates_block.h"
#include "misc_block.h"


class model_descriptor
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor;
    //! symbol_factory is inherited from parent translation_unit
    //! error_context is passed down from parent translation_unit and is used to construct
    //! fake error contexts for default reserved symbols such as M_Planck
    model_descriptor(symbol_factory& s, version_policy& vp, error_context err_ctx);

    //! destructor is default
    ~model_descriptor() = default;


    // INTERFACE

  public:

    //! detect error condition
    bool failed() const { return this->err_count > 0; }
    
    //! report error condition
    void report_error() { ++this->err_count; }
    
    //! perform validation
    validation_exceptions validate() const;
    
    
    // FUNCTIONALITY BLOCKS
    
  public:
    
    //! Lagrangian management
    lagrangian_block model;
    
    //! Metadata management
    metadata_block meta;
    
    //! Template management
    templates_block templates;
    
    //! Miscellaneous settings
    misc_block misc;


		// INTERNAL DATA

  private:
    
    //! count of errors encountered
    unsigned err_count;
    
    //! reference to version policy registry
    version_policy& policy;

	};


#endif //CPPTRANSPORT_MODEL_DESCRIPTOR_H
