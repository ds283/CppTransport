//
// Created by David Seery on 28/06/2017.
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

#ifndef CPPTRANSPORT_METRIC_RESOURCE_H
#define CPPTRANSPORT_METRIC_RESOURCE_H


#include "resources.h"


namespace nontrivial_metric
  {
    
    class metric_resource
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        metric_resource(resources& r, shared_resources& s, index_flatten& f, const language_printer& p)
          : res(r),
            share(s),
            fl(f),
            printer(p)
          {
          }
        
        //! destructor is default
        ~metric_resource() = default;
        
        
        // INTERFACE
      
      public:
        
        //! clear data
        void clear();
        
        
        // GENERATE METRIC COMPONENT
      
      public:
        
        //! build concrete metric component
        GiNaC::ex operator()(const field_index& i, const field_index& j);
        
        //! build abstract metric component
        GiNaC::ex operator()(const index_literal& i, const index_literal& j);
        
        
        // INTERNAL DATA
      
      private:
        
        // AGENTS
        
        //! resource manager
        resources& res;
        
        //! shared resource manager
        shared_resources& share;
        
        //! index flattener
        index_flatten& fl;
        
        //! language printer
        const language_printer& printer;
        
        
        // CACHES
        
        //! metric, if we currently hold a copy
        std::unique_ptr<flattened_tensor> G;
        
        //! inverse metric, if we currently hold a copy
        std::unique_ptr<flattened_tensor> Ginv;
        
      };
    
  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_METRIC_RESOURCE_H
