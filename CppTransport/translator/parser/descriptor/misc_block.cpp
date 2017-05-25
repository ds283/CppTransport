//
// Created by David Seery on 24/05/2017.
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


#include "misc_block.h"
#include "generics_descriptor.h"


// MISCELLANEOUS SETTINGS


void misc_block::set_indexorder(index_order o)
  {
    this->order = o;
  }


index_order misc_block::get_indexorder() const
  {
    return(this->order);
  }


bool misc_block::set_required_version(unsigned v, const y::lexeme_type& l)
  {
    return SetContextedValue(this->min_version, v, l, ERROR_REQUIRED_VERSION_REDECLARATION);
  }


validation_exceptions misc_block::validate() const
  {
    validation_exceptions list;
    
    if(!this->min_version) list.push_back(std::make_unique<validation_message>(false, WARNING_NO_REQUIRED_VERSION));
    if(this->min_version)
      {
        if(*this->min_version > CPPTRANSPORT_NUMERIC_VERSION)
          {
            std::ostringstream msg;
            msg << ERROR_REQUIRED_VERSION_TOO_HIGH_A
                << " " << (*this->min_version / 100) << "." << (*this->min_version) % 100
                << ERROR_REQUIRED_VERSION_TOO_HIGH_B
                << " " << CPPTRANSPORT_VERSION
                << ERROR_REQUIRED_VERSION_TOO_HIGH_C;
            list.push_back(std::make_unique<validation_message>(true, msg.str()));
          }
      }
    
    return list;
  }
