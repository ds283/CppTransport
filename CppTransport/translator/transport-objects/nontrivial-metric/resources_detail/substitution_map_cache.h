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

#ifndef CPPTRANSPORT_SUBSTITUTION_MAP_CACHE_H
#define CPPTRANSPORT_SUBSTITUTION_MAP_CACHE_H


#include "nontrivial-metric/resources.h"
#include "common.h"


namespace nontrivial_metric
  {

    class SubstitutionMapCache
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor captures resource manager
        SubstitutionMapCache(const resources& r, const language_printer& p)
          : res(r),
            printer(p)
          {
          }
        
        //! destructor is default
        ~SubstitutionMapCache() = default;
        
        
        // INTERFACE
      
      public:
        
        GiNaC::exmap& get();
        
        
        // INTERNAL DATA
      
      private:
        
        //! resource manager
        const resources& res;
        
        //! language printer
        const language_printer& printer;
        
        
        // CACHE
        
        //! substitition map
        boost::optional< GiNaC::exmap > map;
        
      };
    
  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_SUBSTITUTION_MAP_CACHE_H
