//
// Created by David Seery on 19/12/2015.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
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

#include "dV.h"


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> dV::compute(const index_literal_list& indices)
      {
        if(indices.size() != DV_TENSOR_INDICES) throw tensor_exception("dV indices");

        return(this->res.dV_resource(this->printer));
      }
    
    
    unroll_behaviour dV::get_unroll()
      {
        if(this->res.can_roll_dV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }
    
    
    std::unique_ptr<atomic_lambda> dV::compute_lambda(const index_literal& i)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("dV");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);

        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(use_dddV, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));

        GiNaC::ex result = this->res.dV_resource(i, this->printer);

        return std::make_unique<atomic_lambda>(i, result, expression_item_types::dV_lambda, *args, this->shared.generate_working_type());
      }
    
    
    dV::dV(language_printer& p, cse& cw, resources& r, shared_resources& s, index_flatten& f)
      : ::dV(),
        printer(p),
        cse_worker(cw),
        res(r),
        shared(s),
        fl(f)
      {
      }
    
    
    void dV::pre_explicit(const index_literal_list& indices)
      {
      }
    
    
    void dV::post()
      {
      }

  }   // namespace nontrivial_metric
