//
// Created by David Seery on 26/05/2017.
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

#ifndef CPPTRANSPORT_ATTRIBUTES_H
#define CPPTRANSPORT_ATTRIBUTES_H


#include <iostream>
#include <string>
#include <memory>

#include "filestack.h"
#include "lexeme.h"
#include "contexted_value.h"
#include "y_common.h"
#include "msg_en.h"

#include "disable_warnings.h"
#include "ginac/ginac.h"


class attributes
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    attributes() = default;

    //! destructor
    ~attributes() = default;


		// INTERFACE

  public:

    //! get LaTeX name
    // TODO: move to boost::optional<>
    const std::string get_latex() const { if(this->latex) return *this->latex; else return std::string(); }

    //! set LaTeX name; returns true if value was set, otherwise reports an error
    //! if an existing value would be overwritten
    bool set_latex(const std::string& ltx, const y::lexeme_type& l);

    // Functions used for the sampling section of the CppT translator

    //! get attribute value
    const std::string get_value() const { if(this->value) return *this->value; else return std::string(); }

    //! set attribute value
    bool set_value(const std::string& val, const y::lexeme_type& l);

    //! get attribute's derivative value
    const std::string get_derivvalue() const { if(this->deriv_value) return *this->deriv_value; else return std::string(); }

    //! set attribute's derivative value
    bool set_derivvalue(const std::string& deriv_val, const y::lexeme_type& l);

    //! get attribute prior
    const std::string get_prior() const { if(this->prior) return *this->prior; else return std::string(); }

    //! set attribute prior
    bool set_prior(const std::string& pri, const y::lexeme_type& l);

    //! get attribute's derivative prior
    const std::string get_derivprior() const { if(this->deriv_prior) return *this->deriv_prior; else return std::string(); }

    //! set attribute's derivative prior
    bool set_derivprior(const std::string& deriv_pri, const y::lexeme_type& l);


		// INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy attribute blocks
    
    //! contexted value representing LaTeX name
    std::shared_ptr< contexted_value<std::string> > latex;

    // Internal values for the values & priors of the fields & parameters for writing to CosmoSIS .ini files

    //! contexted value representing attribute value
    std::shared_ptr< contexted_value<std::string> > value;

    //! contexted value representing attribute's initial value
    std::shared_ptr< contexted_value<std::string> > deriv_value;

    //! contexted value representing attribute prior
    std::shared_ptr< contexted_value<std::string> > prior;

    //! contexted value representing attribute's initial prior
    std::shared_ptr< contexted_value<std::string> > deriv_prior;

	};


#endif //CPPTRANSPORT_ATTRIBUTES_H
