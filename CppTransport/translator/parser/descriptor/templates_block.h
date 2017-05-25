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

#ifndef CPPTRANSPORT_TEMPLATES_BLOCK_H
#define CPPTRANSPORT_TEMPLATES_BLOCK_H


#include "contexted_value.h"
#include "model_settings.h"
#include "y_common.h"
#include "stepper.h"


class templates_block
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    templates_block(unsigned int& ec)
      : err_count(ec)
      {
      }

    //! destructor
    ~templates_block() = default;
    
    
    // SERVICES
  
  public:
    
    //! perform validation
    std::list<std::unique_ptr<std::string>> validate() const;


    // TEMPLATE SPECIFICATION

  public:

    //! set core template name
    bool set_core(const std::string& c, const y::lexeme_type& l);

    //! get core template name as contexted value
    boost::optional< contexted_value<std::string>& > get_core_template() const;


    //! set implementation template name
    bool set_implementation(const std::string& i, const y::lexeme_type& l);

    //! get implementation template name as contexted value
    boost::optional< contexted_value<std::string>& > get_implementation_template() const;


    //! set model tag (an identifier)
    bool set_model(const std::string& m, const y::lexeme_type& l);

    //! get model tag as a contexted value
    boost::optional< contexted_value<std::string>& > get_model() const;


    // IMPLEMENTATION DATA

  public:

    //! set background stepper name
    bool set_background_stepper(stepper& s, const y::lexeme_type& l);

    //! get background stepper name as contexted value
    boost::optional< contexted_value<stepper>& > get_background_stepper() const;


    //! set perturbations stepper name
    bool set_perturbations_stepper(stepper& s, const y::lexeme_type& l);

    //! get perturbations stepper name as contexted value
    boost::optional< contexted_value<stepper>& > get_perturbations_stepper() const;


    // INTERNAL DATA

  private:

    //! error count
    unsigned int& err_count;


    // TEMPLATE DATA

    //! 'model' is the C++ tag used for object names
    std::unique_ptr<contexted_value<std::string> > model;

    //! name of core template
    std::unique_ptr<contexted_value<std::string> > core;

    //! name of implementation template
    std::unique_ptr<contexted_value<std::string> > implementation;

    //! specification of background stepper
    std::unique_ptr<contexted_value<stepper> > background_stepper;

    //! specification of perturbations stepper
    std::unique_ptr<contexted_value<stepper> > perturbations_stepper;

  };


#endif //CPPTRANSPORT_TEMPLATES_BLOCK_H
