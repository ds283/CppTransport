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

#ifndef CPPTRANSPORT_SUBEXPR_H
#define CPPTRANSPORT_SUBEXPR_H


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


class subexpr
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
		subexpr() = default;

    //! destructor
		~subexpr() = default;


		// INTERFACE

  public:

    //! get LaTeX name
    // TODO: move to boost::optional<>
		const std::string get_latex() const { if(this->latex) return *this->latex; else return std::string(); }
    
    //! set LaTeX name; returns true if value was set, otherwise reports an error
    //! if an existing value would be overwritten
    bool set_latex(const std::string& ltx, const y::lexeme_type& l);

    //! get symbolic value
    // TODO: move to boost::optional<>
		GiNaC::ex get_value() const { if(this->value) return *this->value; else return GiNaC::ex(0); }

    //! set symbolic value; returns true if the value was set, otherwise reports an error
    //! if an existing value would be overwritten
    bool set_value(GiNaC::ex v, const y::lexeme_type& l);


		// INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy a subexpr block

    //! contexted value representing LaTeX name
    std::shared_ptr< contexted_value<std::string> > latex;

    //! contexted value representing symbolic value
    std::shared_ptr< contexted_value<GiNaC::ex> > value;

	};


#endif //CPPTRANSPORT_SUBEXPR_H
