//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>
#include <sstream>

#include "utensors.h"


#define BIND_SYMBOL(X, N) std::move(std::make_unique<X>(N, f, cw, p.get_symbol_factory(), prn))


namespace macro_packages
  {

    utensors::utensors(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, p, prn)
      {
        index_package.emplace_back(BIND_SYMBOL(replace_U1, "U1_PREDEF"));
        index_package.emplace_back(BIND_SYMBOL(replace_U2, "U2_PREDEF"));
        index_package.emplace_back(BIND_SYMBOL(replace_U3, "U3_PREDEF"));
      }


    // *******************************************************************


    void replace_U1::pre_evaluate(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->u1_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_U2::pre_evaluate(const macro_argument_list& args)
      {
        GiNaC::symbol k = sym_factory.get_symbol(args[U2_PREDEF_K_ARGUMENT]);
        GiNaC::symbol a = sym_factory.get_symbol(args[U2_PREDEF_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->u2_tensor->compute(k, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_U3::pre_evaluate(const macro_argument_list& args)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[U3_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[U3_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[U3_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[U3_PREDEF_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->u3_tensor->compute(k1, k2, k3, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }

  } // namespace macro_packages
