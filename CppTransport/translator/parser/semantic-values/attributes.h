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


		// INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy attribute blocks
    
    //! contexted value representing LaTeX name
    std::shared_ptr< contexted_value<std::string> > latex;

	};


#endif //CPPTRANSPORT_ATTRIBUTES_H
