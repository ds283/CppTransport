//
// Created by David Seery on 04/07/2017.
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

#include "potential_cache.h"


namespace nontrivial_metric
  {
    
    const GiNaC::ex& PotentialResourceCache::get_V()
      {
        // V is provided with all necessary substitutions applied, so there is no need
        // to do further substitutions here
        if(!this->subs_V) this->subs_V = this->res.raw_V_resource(printer);
   
        return *this->subs_V;
      }
    
    
    const symbol_list& PotentialResourceCache::get_symbol_list()
      {
        if(!this->f_list) this->f_list = this->share.generate_field_symbols(printer);
    
        return *this->f_list;
      }

  }   // namespace nontrivial_metric
