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


#include "templates_block.h"
#include "generics_descriptor.h"


// TEMPLATE SPECIFICATION


bool templates_block::set_core(const std::string& c, const y::lexeme_type& l)
  {
    return SetContextedValue(this->core, c, l, ERROR_CORE_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > templates_block::get_core_template() const
  {
    if(this->core) return(*this->core); else return(boost::none);
  }


bool templates_block::set_implementation(const std::string& i, const y::lexeme_type& l)
  {
    return SetContextedValue(this->implementation, i, l, ERROR_IMPLEMENTATION_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > templates_block::get_implementation_template() const
  {
    if(this->implementation) return(*this->implementation); else return(boost::none);
  }


bool templates_block::set_model(const std::string& m, const y::lexeme_type& l)
  {
    return SetContextedValue(this->model, m, l, ERROR_MODEL_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > templates_block::get_model() const
  {
    if(this->model) return(*this->model); else return(boost::none);
  }


bool templates_block::set_background_stepper(stepper& s, const y::lexeme_type& l)
{
return SetContextedValue(this->background_stepper, s, l, ERROR_BACKGROUND_REDECLARATION);
}


bool templates_block::set_perturbations_stepper(stepper& s, const y::lexeme_type& l)
{
return SetContextedValue(this->perturbations_stepper, s, l, ERROR_PERTURBATIONS_REDECLARATION);
}


boost::optional< contexted_value<stepper>& > templates_block::get_background_stepper() const
  {
    if(this->background_stepper) return *this->background_stepper; else return boost::none;
  }


boost::optional< contexted_value<stepper>& > templates_block::get_perturbations_stepper() const
  {
    if(this->perturbations_stepper) return *this->perturbations_stepper; else return boost::none;
  }


validation_exceptions templates_block::validate() const
  {
    validation_exceptions list;
    
    if(!this->model) list.push_back(std::make_unique<validation_message>(true, ERROR_NO_MODEL_BLOCK));

    if(!this->background_stepper) list.push_back(std::make_unique<validation_message>(true, ERROR_NO_BACKGROUND_STEPPER_BLOCK));
    
    if(!this->perturbations_stepper) list.push_back(std::make_unique<validation_message>(true, ERROR_NO_PERTURBATIONS_STEPPER_BLOCK));
    
    return list;
  }
