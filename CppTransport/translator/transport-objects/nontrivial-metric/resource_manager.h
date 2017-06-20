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

#ifndef CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCE_MANAGER_H
#define CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCE_MANAGER_H


#include "canonical/resource_manager.h"


namespace nontrivial_metric
  {
    
    //! resource manager inherits all functionality of a canonical resource manager,
    //! but adds new features to deal with curvature tensors
    class nontrivial_metric_resource_manager: public canonical::canonical_resource_manager
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor is default
        nontrivial_metric_resource_manager() = default;
        
        //! destructor is default
        ~nontrivial_metric_resource_manager() = default;
        
      };
  
  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCE_MANAGER_H
