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

#include "ddV.h"


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> ddV::compute(const index_literal_list& indices)
      {
        if(indices.size() != DDV_TENSOR_INDICES) throw tensor_exception("ddV indices");

        return(this->res.ddV_resource(this->printer));
      }
    
    
    unroll_behaviour ddV::get_unroll()
      {
        if(this->res.can_roll_ddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll
      }
    
    
    std::unique_ptr<atomic_lambda> ddV::compute_lambda(const index_literal& i, const index_literal& j)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("ddV");
        if(j.get_class() != index_class::field_only) throw tensor_exception("ddV");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);

        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(use_dddV, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_j.get_value()));

        GiNaC::ex result = this->res.ddV_resource(i, j, this->printer);

        return std::make_unique<atomic_lambda>(i, j, result, expression_item_types::ddV_lambda, *args, this->shared.generate_working_type());
      }
    
    
    ddV::ddV(language_printer& p, cse& cw, resources& r, shared_resources& s, index_flatten& f)
      : ::ddV(),
        printer(p),
        cse_worker(cw),
        res(r),
        shared(s),
        fl(f)
      {
      }
    
    
    void ddV::pre_explicit(const index_literal_list& indices)
      {
      }
    
    
    void ddV::post()
      {
      }

  }   // namespace nontrivial_metric
