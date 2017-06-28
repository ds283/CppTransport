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


#include "connexion.h"


namespace nontrivial_metric
  {
    
    connexion::connexion(language_printer& p, cse& cw, resources& r, shared_resources& s, index_flatten& f)
      : ::connexion(),
        printer(p),
        cse_worker(cw),
        res(r),
        shared(s),
        fl(f)
      {
      }


    std::unique_ptr<flattened_tensor>
    connexion::compute(const index_literal_list& indices)
      {
        if(indices.size() != CONNEXION_TENSOR_INDICES) throw tensor_exception("connexion indices");
        if(indices[0]->get_class() != index_class::field_only) throw tensor_exception("connexion");
        if(indices[1]->get_class() != index_class::field_only) throw tensor_exception("connexion");
        if(indices[2]->get_class() != index_class::field_only) throw tensor_exception("connexion");
        if(indices[0]->get_variance() != variance::contravariant) throw tensor_exception("connexion");
        if(indices[1]->get_variance() != variance::covariant) throw tensor_exception("connexion");
        if(indices[2]->get_variance() != variance::covariant) throw tensor_exception("connexion");

        return this->res.connexion_resource(this->printer);
      }


    std::unique_ptr<atomic_lambda>
    connexion::compute_lambda(const index_literal& i, const index_literal& j, const index_literal& k)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("metric_inverse");
        if(j.get_class() != index_class::field_only) throw tensor_exception("metric_inverse");
        if(k.get_class() != index_class::field_only) throw tensor_exception("metric_inverse");
        if(i.get_variance() != variance::contravariant) throw tensor_exception("metric_inverse");
        if(j.get_variance() != variance::covariant) throw tensor_exception("metric_inverse");
        if(k.get_variance() != variance::covariant) throw tensor_exception("metric_inverse");

        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::varidx>(j);
        auto idx_k = this->shared.generate_index<GiNaC::varidx>(k);

        auto args = this->res.generate_cache_arguments<index_literal>(use_Gamma, {i,j,k}, this->printer);
        args += { idx_i, idx_j, idx_k };

        GiNaC::ex result = this->res.connexion_resource(i, j, k, this->printer);

        return std::make_unique<atomic_lambda>(i, j, result, expression_item_types::connexion_lambda, args, this->shared.generate_working_type());
      }
    
    
    unroll_state connexion::get_unroll(const index_literal_list& idx_list)
      {
        if(res.can_roll_connexion()) return unroll_state::allow;
        
        return unroll_state::force;   // can't roll-up
      }
    
    
    void connexion::pre_explicit(const index_literal_list& indices)
      {
      }


    void connexion::post()
      {
      }
    
  }   // namespace nontrivial_metric
