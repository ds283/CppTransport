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
    
    PotentialResourceCache::value_type PotentialResourceCache::get()
      {
        // acquire resources if we do not already have them;
        // these resources are provided with all necessary substitutions applied, so there is no need
        // to do further substitutions in the resource manager
    
        if(!this->subs_V) this->subs_V = this->res.raw_V_resource(printer);
        if(!this->f_list) this->f_list = this->share.generate_field_symbols(printer);
    
        // return references
        return std::make_pair(std::cref(*subs_V), std::cref(*f_list));
      }

  }   // namespace nontrivial_metric
