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

#ifndef CPPTRANSPORT_NONTRIVIAL_METRIC_DERIVATIVE_SYMBOLS_H
#define CPPTRANSPORT_NONTRIVIAL_METRIC_DERIVATIVE_SYMBOLS_H


#include "nontrivial-metric/resources.h"
#include "common.h"


namespace nontrivial_metric
  {

    class DerivativeSymbolsCache
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        DerivativeSymbolsCache(const resources& r, const shared_resources& s, const language_printer& p)
          : res(r),
            share(s),
            printer(p)
          {
          }
        
        //! destructor is default
        ~DerivativeSymbolsCache() = default;
        
        
        // INTERFACE
      
      public:
        
        //! get contravariant components of derivative vector
        const symbol_list& get();
        
        
        // INTERNAL DATA
      
      private:
        
        //! resource manager
        const resources& res;
        
        //! shared resource manager
        const shared_resources& share;
        
        //! language printer
        const language_printer& printer;
        
        
        // CACHE
        
        //! derivative vector
        std::unique_ptr<symbol_list> derivs;
        
      };
    
  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_NONTRIVIAL_METRIC_DERIVATIVE_SYMBOLS_H
