//
// Created by David Seery on 04/12/2013.
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


#include <assert.h>
#include <functional>

#include "lagrangian_tensors.h"


#define BIND_SYMBOL(X, N) std::move(std::make_unique<X>(N, f, cw, lm, p.get_symbol_factory(), prn))


namespace macro_packages
  {

    lagrangian_tensors::lagrangian_tensors(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, lm, p, prn)
      {
        index_package.emplace_back(BIND_SYMBOL(replace_A, "A_TENSOR"));
        index_package.emplace_back(BIND_SYMBOL(replace_Atilde, "ATILDE_TENSOR"));
        index_package.emplace_back(BIND_SYMBOL(replace_B, "B_TENSOR"));
        index_package.emplace_back(BIND_SYMBOL(replace_C, "C_TENSOR"));
        index_package.emplace_back(BIND_SYMBOL(replace_M, "M_TENSOR"));
      }


    // *******************************************************************


    void replace_A::pre_hook(const macro_argument_list& args)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[A_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[A_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[A_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[A_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->A_tensor->compute(k1, k2, k3, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }
    
    
    void replace_Atilde::pre_hook(const macro_argument_list& args)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[ATILDE_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[ATILDE_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[ATILDE_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[ATILDE_A_ARGUMENT]);
        
        std::unique_ptr<flattened_tensor> container = this->Atilde_tensor->compute(k1, k2, k3, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_B::pre_hook(const macro_argument_list& args)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[B_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[B_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[B_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[B_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->B_tensor->compute(k1, k2, k3, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_C::pre_hook(const macro_argument_list& args)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[C_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[C_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[C_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[C_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->C_tensor->compute(k1, k2, k3, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_M::pre_hook(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->M_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    // *******************************************************************


    std::string replace_A::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[A_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[A_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[A_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[A_A_ARGUMENT]);

        std::unique_ptr<atomic_lambda> lambda = this->A_tensor->compute_lambda(indices[0], indices[1], indices[2], k1, k2, k3, a);
        return this->lambda_mgr.cache(std::move(lambda));
      }
    
    
    std::string replace_Atilde::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[ATILDE_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[ATILDE_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[ATILDE_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[ATILDE_A_ARGUMENT]);
        
        std::unique_ptr<atomic_lambda> lambda = this->Atilde_tensor->compute_lambda(indices[0], indices[1], indices[2], k1, k2, k3, a);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_B::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[A_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[A_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[A_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[A_A_ARGUMENT]);

        std::unique_ptr<atomic_lambda> lambda = this->B_tensor->compute_lambda(indices[0], indices[1], indices[2], k1, k2, k3, a);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_C::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[A_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[A_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[A_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[A_A_ARGUMENT]);

        std::unique_ptr<atomic_lambda> lambda = this->C_tensor->compute_lambda(indices[0], indices[1], indices[2], k1, k2, k3, a);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_M::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        std::unique_ptr<atomic_lambda> lambda = this->M_tensor->compute_lambda(indices[0], indices[1]);
        return this->lambda_mgr.cache(std::move(lambda));
      }

  } // namespace macro_packages
