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


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_dV::compute()
      {
        return(this->res.dV_resource(this->printer));
      }


    unroll_behaviour canonical_dV::get_unroll()
      {
        if(this->res.roll_dV()) return unroll_behaviour::allow;
        return unroll_behaviour::force;   // can't roll-up
      }


    std::unique_ptr<atomic_lambda> canonical_dV::compute_lambda(const abstract_index& i)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("dV");

        GiNaC::idx idx_i = this->shared.generate_index(i);

        std::unique_ptr<ginac_cache_tags> args = this->res.generate_arguments(use_dddV_argument, this->printer);
        args->push_back(GiNaC::ex_to<GiNaC::symbol>(idx_i.get_value()));

        GiNaC::ex result = this->res.dV_resource(i, this->printer);

        return std::make_unique<atomic_lambda>(i, result, expression_item_types::dV_lambda, *args, this->shared.generate_working_type());
      }

  }   // namespace canonical
