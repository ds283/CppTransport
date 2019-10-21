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
// --@@
//


#include <string>
#include <sstream>

#include "y_driver.h"


namespace y
	{

    y_driver::y_driver(model_descriptor& r, symbol_factory& s, argument_cache& c, local_environment& e)
	    : root(r),
        sym_factory(s),
        cache(c),
        env(e),
        expr(root, s, c, e),
        model(root, s, c, e),
        meta(root, s, c, e),
        templates(root, s, c, e),
        misc(root, s, c, e),
        cosmo(root, s, c, e)
	    {
	    }


    void y_driver::error(std::string msg)
	    {
        ::error(msg, this->cache, this->env);
	    }
    
    
  }
