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
    unsigned int index = this->fl.flatten(i);
    
    if(!cached) { this->populate_workspace(); this->cached = true; }
    
    GiNaC::ex result = (*this->param_symbols)[index];
    
    return result;
  }


std::unique_ptr<atomic_lambda> parameters::compute_lambda(const abstract_index& i)
  {
    if(i.get_class() != index_class::parameter) throw tensor_exception("parameters");
    
    auto idx_i = this->shared.generate_index<GiNaC::idx>(i);
    
    auto args = std::make_unique<cache_tags>();
    
    GiNaC::ex result = this->shared.generate_parameter_vector(i, this->printer);
    
    return std::make_unique<atomic_lambda>(i, result, expression_item_types::parameters_lambda, *args, this->shared.generate_working_type());
  }


unroll_behaviour parameters::get_unroll()
  {
    if(this->shared.can_roll_parameters()) return unroll_behaviour::allow;
    return unroll_behaviour::force;
  }


void parameters::populate_workspace()
  {
    param_symbols = this->shared.generate_parameter_symbols(this->printer);
  }
