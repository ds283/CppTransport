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

#ifndef CPPTRANSPORT_CANONICAL_RESOURCE_MANAGER_H
#define CPPTRANSPORT_CANONICAL_RESOURCE_MANAGER_H


#include "concepts/resource_manager.h"


namespace canonical
  {
    
    class canonical_resource_manager: public resource_manager
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor is default
        canonical_resource_manager() = default;
        
        //! destructor is default
        ~canonical_resource_manager() = default;
    
    
        // INTERFACE - GLOBAL RELEASE OF RESOURCE LABELS
  
      public:
    
        //! release all resources, but not flatteners (this is the most common use case;
        //! we wish to release resource labels at the end of a function, but not the flattener labels)
        void release();
    
        //! release flatteners
        void release_flatteners();
    
      };
    
  }   // namespace canonical



#endif //CPPTRANSPORT_CANONICAL_RESOURCE_MANAGER_H
