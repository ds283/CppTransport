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

#include "symbol_factory.h"


GiNaC::symbol symbol_factory::get_symbol(const std::string& name)
	{
    std::string LaTeX;
		return(this->get_symbol(name, LaTeX));
	}


GiNaC::symbol symbol_factory::get_symbol(const std::string& name, const std::string& LaTeX)
	{
    std::unordered_map<std::string, GiNaC::symbol>::iterator t = this->cache.find(name);

		if(t != this->cache.end()) return(t->second);

    GiNaC::symbol rval(name, LaTeX);
		this->cache[name] = rval;

		return(rval);
	}
