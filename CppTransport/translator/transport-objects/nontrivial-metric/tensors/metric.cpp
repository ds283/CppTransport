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


#include "metric.h"


namespace nontrivial_metric
  {
    
    metric::metric(language_printer& p, cse& cw, resources& r, shared_resources& s, index_flatten& f)
      : ::metric(),
        printer(p),
        cse_worker(cw),
        res(r),
        shared(s),
        fl(f)
      {
      }
    
    
    std::unique_ptr<flattened_tensor>
    metric::compute(const index_literal_list& indices)
      {
        if(indices.size() != METRIC_TENSOR_INDICES) throw tensor_exception("metric indices");
        if(indices[0]->get_class() != index_class::field_only) throw tensor_exception("metric");
        if(indices[1]->get_class() != index_class::field_only) throw tensor_exception("metric");
        if(indices[0]->get_variance() != variance::covariant) throw tensor_exception("metric");
        if(indices[1]->get_variance() != variance::covariant) throw tensor_exception("metric");

        return this->res.metric_resource(this->printer);
      }

    
    std::unique_ptr<atomic_lambda>
    metric::compute_lambda(const index_literal& i, const index_literal& j)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("metric");
        if(j.get_class() != index_class::field_only) throw tensor_exception("metric");
        if(i.get_variance() != variance::covariant) throw tensor_exception("metric");
        if(j.get_variance() != variance::covariant) throw tensor_exception("metric");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);

        auto args = this->res.generate_cache_arguments<index_literal>(0, {i,j}, this->printer);
        args += { idx_i, idx_j };

        GiNaC::ex result = this->res.metric_resource(i, j, this->printer);

        return std::make_unique<atomic_lambda>(i, j, result, expression_item_types::metric_lambda, args, this->shared.generate_working_type());
      }
    
    
    unroll_state metric::get_unroll(const index_literal_list& idx_list)
      {
        if(res.can_roll_metric()) return unroll_state::allow;
        
        return unroll_state::force;   // can't roll-up
      }
    
    
    void metric::pre_explicit(const index_literal_list& indices)
      {
      }


    void metric::post()
      {
      }
    
  }   // namespace nontrivial_metric
