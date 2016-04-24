//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>
#include <sstream>

#include "utensors.h"


#define BIND_SYMBOL(X, N) std::move(std::make_unique<X>(N, f, cw, lm, p.get_symbol_factory(), prn))


namespace macro_packages
  {

    utensors::utensors(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, lm, p, prn)
      {
        index_package.emplace_back(BIND_SYMBOL(replace_U1, "U1_PREDEF"));
        index_package.emplace_back(BIND_SYMBOL(replace_U2, "U2_PREDEF"));
        index_package.emplace_back(BIND_SYMBOL(replace_U3, "U3_PREDEF"));
      }


    // *******************************************************************


    void replace_U1::pre_hook(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->u1_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_U2::pre_hook(const macro_argument_list& args)
      {
        GiNaC::symbol k = sym_factory.get_symbol(args[U2_PREDEF_K_ARGUMENT]);
        GiNaC::symbol a = sym_factory.get_symbol(args[U2_PREDEF_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->u2_tensor->compute(k, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_U3::pre_hook(const macro_argument_list& args)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[U3_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[U3_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[U3_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[U3_PREDEF_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->u3_tensor->compute(k1, k2, k3, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    // *******************************************************************


    std::string replace_U1::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        std::unique_ptr<map_lambda> lambda = this->u1_tensor->compute_lambda(indices[0]);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_U2::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        GiNaC::symbol k = sym_factory.get_symbol(args[U2_PREDEF_K_ARGUMENT]);
        GiNaC::symbol a = sym_factory.get_symbol(args[U2_PREDEF_A_ARGUMENT]);

        std::unique_ptr<map_lambda> lambda = this->u2_tensor->compute_lambda(indices[0], indices[1], k, a);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_U3::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[U3_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[U3_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[U3_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[U3_PREDEF_A_ARGUMENT]);

        std::unique_ptr<map_lambda> lambda = this->u3_tensor->compute_lambda(indices[0], indices[1], indices[2], k1, k2, k3, a);
        return this->lambda_mgr.cache(std::move(lambda));
      }

  } // namespace macro_packages
