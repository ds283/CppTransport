//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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
        index_package.emplace_back(BIND_SYMBOL(replace_A, "A_PREDEF"));
        index_package.emplace_back(BIND_SYMBOL(replace_B, "B_PREDEF"));
        index_package.emplace_back(BIND_SYMBOL(replace_C, "C_PREDEF"));
        index_package.emplace_back(BIND_SYMBOL(replace_M, "M_PREDEF"));
      }


    // *******************************************************************


    void replace_A::pre_hook(const macro_argument_list& args)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[A_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[A_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[A_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[A_PREDEF_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->A_tensor->compute(k1, k2, k3, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_B::pre_hook(const macro_argument_list& args)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[B_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[B_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[B_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[B_PREDEF_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->B_tensor->compute(k1, k2, k3, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_C::pre_hook(const macro_argument_list& args)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[C_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[C_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[C_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[C_PREDEF_A_ARGUMENT]);

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
        GiNaC::symbol k1 = sym_factory.get_symbol(args[A_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[A_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[A_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[A_PREDEF_A_ARGUMENT]);

        std::unique_ptr<atomic_lambda> lambda = this->A_tensor->compute_lambda(indices[0], indices[1], indices[2], k1, k2, k3, a);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_B::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[A_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[A_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[A_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[A_PREDEF_A_ARGUMENT]);

        std::unique_ptr<atomic_lambda> lambda = this->B_tensor->compute_lambda(indices[0], indices[1], indices[2], k1, k2, k3, a);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_C::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[A_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[A_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[A_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[A_PREDEF_A_ARGUMENT]);

        std::unique_ptr<atomic_lambda> lambda = this->C_tensor->compute_lambda(indices[0], indices[1], indices[2], k1, k2, k3, a);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_M::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        std::unique_ptr<atomic_lambda> lambda = this->M_tensor->compute_lambda(indices[0], indices[1]);
        return this->lambda_mgr.cache(std::move(lambda));
      }

  } // namespace macro_packages
