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


#include "Riemann_A3.h"


namespace nontrivial_metric
  {
    
    Riemann_A3::Riemann_A3(language_printer& p, cse& cw, resources& r, shared_resources& s, index_flatten& f)
      : ::Riemann_A3(),
        printer(p),
        cse_worker(cw),
        res(r),
        shared(s),
        fl(f)
      {
      }
    
    
    std::unique_ptr<flattened_tensor>
    Riemann_A3::compute(const index_literal_list& indices)
      {
        if(indices.size() != RIEMANN_A3_TENSOR_INDICES) throw tensor_exception("Riemann_A3 indices");
        if(indices[0]->get_class() != index_class::field_only) throw tensor_exception("Riemann_A3");
        if(indices[1]->get_class() != index_class::field_only) throw tensor_exception("Riemann_A3");
        if(indices[2]->get_class() != index_class::field_only) throw tensor_exception("Riemann_A3");
    
        return this->res.Riemann_A3_resource(indices[0]->get_variance(), indices[1]->get_variance(), indices[2]->get_variance(), printer);
      }
    
    
    std::unique_ptr<atomic_lambda>
    Riemann_A3::compute_lambda(const index_literal& i, const index_literal& j, const index_literal& k)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("Riemann_A3");
        if(j.get_class() != index_class::field_only) throw tensor_exception("Riemann_A3");
        if(k.get_class() != index_class::field_only) throw tensor_exception("Riemann_A3");
    
        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);
        auto idx_k = this->shared.generate_index<GiNaC::varidx>(k);
    
        auto args = this->res.generate_cache_arguments<index_literal>(0, {i,j,k}, this->printer);
        args += { idx_i, idx_j, idx_k };
    
        GiNaC::ex result = this->res.Riemann_A3_resource(i, j, k, this->printer);
    
        return std::make_unique<atomic_lambda>(i, j, k, result, expression_item_types::Riemann_A3_lambda, args, this->shared.generate_working_type());
      }
    
    
    unroll_state Riemann_A3::get_unroll(const index_literal_list& idx_list)
      {
        const std::array< variance, RESOURCE_INDICES::RIEMANN_A3_INDICES > ijk = { idx_list[0]->get_variance(), idx_list[1]->get_variance(), idx_list[2]->get_variance() };

        if(res.can_roll_Riemann_A3(ijk)) return unroll_state::allow;
        
        return unroll_state::force;   // can't roll-up
      }
    
    
    void Riemann_A3::pre_explicit(const index_literal_list& indices)
      {
      }
    
    
    void Riemann_A3::post()
      {
      }
    
  }   // namespace nontrivial_metric
