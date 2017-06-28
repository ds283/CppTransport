//
// Created by David Seery on 16/06/2017.
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


#include "parameters.h"


std::unique_ptr<flattened_tensor> parameters::compute(const index_literal_list& indices)
  {
    if(indices.size() != PARAMETER_TENSOR_INDICES) throw tensor_exception("parameter indices");

    auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<param_index>(PARAMETER_TENSOR_INDICES));
    
    const param_index max_i = this->shared.get_max_param_index();
    
    this->cached = false;
    
    for(param_index i = param_index(0); i < max_i; ++i)
      {
        (*result)[this->fl.flatten(i)] = this->compute_component(i);
      }

    return result;
  }


GiNaC::ex parameters::compute_component(param_index i)
  {
    if(!this->cached) throw tensor_exception("parameters cache not ready");

    unsigned int index = this->fl.flatten(i);

    GiNaC::ex result = (*this->param_symbols)[index];
    
    return result;
  }


std::unique_ptr<atomic_lambda> parameters::compute_lambda(const index_literal& i)
  {
    if(i.get_class() != index_class::parameter) throw tensor_exception("parameters");
    
    auto idx_i = this->shared.generate_index<GiNaC::idx>(i);

    GiNaC::ex result = this->shared.generate_parameter_vector(i, this->printer);
    
    return std::make_unique<atomic_lambda>(i, result, expression_item_types::parameters_lambda, cache_tags(), this->shared.generate_working_type());
  }


unroll_state parameters::get_unroll(const index_literal_list& idx_list)
  {
    if(this->shared.can_roll_parameters()) return unroll_state::allow;
    return unroll_state::force;
  }


void parameters::pre_explicit(const index_literal_list& indices)
  {
    if(cached) throw tensor_exception("parameters already cached");

    param_symbols = this->shared.generate_parameter_symbols(this->printer);

    this->cached = true;
  }


void parameters::post()
  {
    if(!this->cached) throw tensor_exception("parameters not cached");

    // invalidate cache
    this->cached = false;
  }
