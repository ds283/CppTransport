//
// Created by David Seery on 28/07/2017.
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

#ifndef CPPTRANSPORT_CANONICAL_POTENTIAL_CACHE_H
#define CPPTRANSPORT_CANONICAL_POTENTIAL_CACHE_H


#include "canonical/resources.h"


namespace canonical
  {

    class PotentialResourceCache
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor captures resource manager and shared resource manager
        PotentialResourceCache(const resources& r, const shared_resources& s, const language_printer& p)
          : res(r),
            share(s),
            printer(p)
          {
          }
        
        //! destructor is default
        ~PotentialResourceCache() = default;
        
        
        // INTERFACE
      
      public:
        
        //! get potential
        const GiNaC::ex& get_V();
        
        //! get symbol list
        const symbol_list& get_symbol_list();
        
        
        // INTERNAL DATA
      
      private:
        
        //! resource manager
        const resources& res;
        
        //! shared resources
        const shared_resources& share;
        
        //! language printer
        const language_printer& printer;
        
        
        // CACHE
        
        //! raw V expressions, after substitution with the current resource labels
        boost::optional< GiNaC::ex > subs_V;
        
        //! symbol list
        std::unique_ptr<symbol_list> f_list;
        
      };
    
  }   // namespace nontrivial metric


#endif //CPPTRANSPORT_POTENTIAL_CACHE_H
