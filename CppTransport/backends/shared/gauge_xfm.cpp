//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>

#include "gauge_xfm.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, f, cw, prn))
#define BIND_SYMBOL(X, N) std::move(std::make_unique<X>(N, f, cw, p.get_symbol_factory(), prn))


namespace macro_packages
  {

    gauge_xfm::gauge_xfm(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, p, prn)
      {
        index_package.emplace_back(BIND(replace_zeta1, "ZETA_XFM_1"));
        index_package.emplace_back(BIND_SYMBOL(replace_zeta2, "ZETA_XFM_2"));
        index_package.emplace_back(BIND(replace_dN1, "DELTAN_XFM_1"));
        index_package.emplace_back(BIND(replace_dN2, "DELTAN_XFM_2"));
      }


    // *******************************************************************


    void replace_zeta1::pre_evaluate(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->zeta1_tensor->compute();
        this->map =  std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_zeta2::pre_evaluate(const macro_argument_list& args)
      {
        GiNaC::symbol  k = sym_factory.get_symbol(args[ZETA_XFM_2_K_ARGUMENT]);
        GiNaC::symbol k1 = sym_factory.get_symbol(args[ZETA_XFM_2_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[ZETA_XFM_2_K2_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[ZETA_XFM_2_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->zeta2_tensor->compute(k, k1, k2, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_dN1::pre_evaluate(const macro_argument_list& args)
	    {
        std::unique_ptr<flattened_tensor> container = this->dN1_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
	    }


    void replace_dN2::pre_evaluate(const macro_argument_list& args)
	    {
        std::unique_ptr<flattened_tensor> container = this->dN2_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
	    }


  } // namespace macro_packages
