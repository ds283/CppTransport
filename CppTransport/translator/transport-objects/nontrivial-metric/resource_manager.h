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
    
    
        // INTERFACE - QUERY FOR RESOURCE LABELS
        
      public:
    
        //! get connexion label
        boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::CONNEXION_INDICES>, contexted_value<std::string> > >
        connexion(std::array<variance, RESOURCE_INDICES::CONNEXION_INDICES> v, bool exact=true)
          { return this->connexion_cache.find(v, exact); }
    
        //! get Riemann A2 label
        boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES>, contexted_value<std::string> > >
        Riemann_A2(std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES> v, bool exact=true)
          { return this->Riemann_A2_cache.find(v, exact); }
    
        //! get Riemann A3 label
        boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES>, contexted_value<std::string> > >
        Riemann_A3(std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES> v, bool exact=true)
          { return this->Riemann_A3_cache.find(v, exact); }
    
        //! get Riemann B3 label
        boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES>, contexted_value<std::string> > >
        Riemann_B3(std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES> v, bool exact=true)
          { return this->Riemann_B3_cache.find(v, exact); }
    
    
        // INTERFACE - ASSIGN RESOURCE LABELS
    
      public:
    
        //! assign connexion resource label
        void assign_connexion(const contexted_value<std::string>& c,
                              std::array<variance, RESOURCE_INDICES::CONNEXION_INDICES> v)
          { this->connexion_cache.assign(c, v); }
    
        //! assign Riemann A2 resource label
        void assign_Riemann_A2(const contexted_value<std::string>& R,
                               std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES> v)
          { this->Riemann_A2_cache.assign(R, v); }
    
        //! assign Riemann A3 resource label
        void assign_Riemann_A3(const contexted_value<std::string>& R,
                               std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES> v)
          { this->Riemann_A3_cache.assign(R, v); }
    
        //! assign Riemann B3 resource label
        void assign_Riemann_B3(const contexted_value<std::string>& R,
                               std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES> v)
          { this->Riemann_B3_cache.assign(R, v); }
    
    
        // INTERFACE - RELEASE INDIVIDUAL RESOURCE LABELS

      public:
    
        //! release connexion resource
        void release_connexion()
          { this->connexion_cache.reset(); }
    
        //! release Riemann A2 resource
        void release_Riemann_A2()
          { this->Riemann_A2_cache.reset(); }
    
        //! release Riemann A3 resource
        void release_Riemann_A3()
          { this->Riemann_A3_cache.reset(); }
    
        //! release Riemann B3 resource
        void release_Riemann_B3()
          { this->Riemann_B3_cache.reset(); }
    
    
        // INTERFACE - GLOBAL RELEASE OF RESOURCE LABELS
  
      public:
    
        //! release all resources, but not flatteners (this is the most common use case;
        //! we wish to release resource labels at the end of a function, but not the flattener labels)
        void release();
    
        //! release flatteners
        void release_flatteners();
    
    
        // INTERNAL DATA
        
      protected:
    
        //! cache connexion resource labels
        indexed_resource<RESOURCE_INDICES::CONNEXION_INDICES, std::string> connexion_cache;
    
        //! cache Riemann A2 resource labels
        indexed_resource<RESOURCE_INDICES::RIEMANN_A2_INDICES, std::string> Riemann_A2_cache;
    
        //! cache Riemann A3 resource labels
        indexed_resource<RESOURCE_INDICES::RIEMANN_A3_INDICES, std::string> Riemann_A3_cache;
    
        //! cache Riemann B3 resource labels
        indexed_resource<RESOURCE_INDICES::RIEMANN_B3_INDICES, std::string> Riemann_B3_cache;
        
      };
  
  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCE_MANAGER_H
