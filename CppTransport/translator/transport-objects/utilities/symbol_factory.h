//
// Created by David Seery on 12/03/15.
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


#ifndef CPPTRANSPORT_SYMBOL_FACTORY_H
#define CPPTRANSPORT_SYMBOL_FACTORY_H


#include <unordered_map>

#include "disable_warnings.h"
#include "ginac/ginac.h"


class symbol_factory
	{

		// CONSTRUCTOR

  public:

		symbol_factory() = default;

		~symbol_factory() = default;


		// INTERFACE

  public:

		//! get a symbol
		GiNaC::symbol get_symbol(const std::string& name);

		//! get a symbol, specifying a LaTeX equivalent
		GiNaC::symbol get_symbol(const std::string& name, const std::string& LaTeX);


		// INTERNAL DATA

  private:

		std::unordered_map<std::string, GiNaC::symbol> cache;

	};


#endif //CPPTRANSPORT_SYMBOL_FACTORY_H
