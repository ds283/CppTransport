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

#include "metric_resource.h"
#include "concepts/tensor_exception.h"


namespace nontrivial_metric
  {
    
    void metric_resource::clear()
      {
        this->G.reset(nullptr);
        this->Ginv.reset(nullptr);
      }
    
    
    GiNaC::ex metric_resource::operator()(const field_index& i, const field_index& j)
      {
        // if mixed indices, then return a Kronecker delta
        // if mixed indices, then return a Kronecker delta
        if(i.get_variance() != j.get_variance())
          {
            auto idx_i = this->share.generate_index<GiNaC::varidx>(i);
            auto idx_j = this->share.generate_index<GiNaC::varidx>(j);
        
            return GiNaC::delta_tensor(idx_i, idx_j);
          }
        
        // if covariant then return a metric component
        if(i.get_variance() == variance::covariant)
          {
            if(!this->G) this->G = this->res.metric_resource(this->printer);
            return (*this->G)[this->fl.flatten(i,j)];
          }
        
        // otherwise, return inverse metric component
        if(!this->Ginv) this->Ginv = this->res.metric_inverse_resource(this->printer);
        return (*this->Ginv)[this->fl.flatten(i,j)];
      }
    
    
    GiNaC::ex metric_resource::operator()(const index_literal& i, const index_literal& j)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception(ERROR_METRIC_INDICES_ARE_FIELDS);
        if(j.get_class() != index_class::field_only) throw tensor_exception(ERROR_METRIC_INDICES_ARE_FIELDS);
        
        // if mixed indices, then return a Kronecker delta
        if(i.get_variance() != j.get_variance())
          {
            auto idx_i = this->share.generate_index<GiNaC::varidx>(i);
            auto idx_j = this->share.generate_index<GiNaC::varidx>(j);
    
            return GiNaC::delta_tensor(idx_i, idx_j);
          }
        
        // if covariant then return a metric component
        if(i.get_variance() == variance::covariant)
          {
            return this->res.metric_resource(i, j, this->printer);
          }
        
        // otherwise, return inverse metric component
        return this->res.metric_inverse_resource(i, j, this->printer);
      }
    
  }   // namespace nontrivial_metric
