//
// Created by David Seery on 21/06/2017.
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


#include "resource_manager.h"


namespace canonical
  {
    
    void canonical_resource_manager::release()
      {
        this->parameters_cache.reset();
        this->coordinates_cache.reset();
        this->dV_cache.reset();
        this->ddV_cache.reset();
        this->dddV_cache.reset();
      }
    
    
    void canonical_resource_manager::release_flatteners()
      {
        this->phase_flatten_cache.reset();
        this->field_flatten_cache.reset();
      }
    
  }   // namespace canonical
