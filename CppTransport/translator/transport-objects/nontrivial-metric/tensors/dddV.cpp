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

#include "dddV.h"


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> dddV::compute(const index_literal_list& indices)
      {
        if(indices.size() != DDDV_TENSOR_INDICES) throw tensor_exception("dddV indices");

        return(this->res.dddV_resource(this->printer));
      }
    
    
    unroll_behaviour dddV::get_unroll()
      {
        if(this->res.can_roll_dddV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll
      }


    std::unique_ptr<atomic_lambda>
    dddV::compute_lambda(const index_literal& i, const index_literal& j, const index_literal& k)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("dddV");
        if(j.get_class() != index_class::field_only) throw tensor_exception("dddV");
        if(k.get_class() != index_class::field_only) throw tensor_exception("dddV");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);
        auto idx_k = this->shared.generate_index<GiNaC::varidx>(k);

        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(use_dddV_argument, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_j.get_value()));
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_k.get_value()));

        GiNaC::ex result = this->res.dddV_resource(i, j, k, this->printer);

        return std::make_unique<atomic_lambda>(i, j, k, result, expression_item_types::dddV_lambda, *args, this->shared.generate_working_type());
      }
    
    
    dddV::dddV(language_printer& p, cse& cw, resources& r, shared_resources& s, index_flatten& f)
      : ::dddV(),
        printer(p),
        cse_worker(cw),
        res(r),
        shared(s),
        fl(f)
      {
      }
    
    
    void dddV::pre_explicit(const index_literal_list& indices)
      {
      }
    
    
    void dddV::post()
      {
      }

  }
