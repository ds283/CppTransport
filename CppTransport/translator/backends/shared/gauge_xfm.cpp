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

#include "gauge_xfm.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, f, cw, lm, prn))
#define BIND_SYMBOL(X, N) std::move(std::make_unique<X>(N, f, cw, lm, p.get_symbol_factory(), prn))


namespace macro_packages
  {

    gauge_xfm::gauge_xfm(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, lm, p, prn)
      {
        index_package.emplace_back(BIND(replace_zeta1, "ZETA_XFM_1"));
        index_package.emplace_back(BIND_SYMBOL(replace_zeta2, "ZETA_XFM_2"));
        index_package.emplace_back(BIND(replace_dN1, "DELTAN_XFM_1"));
        index_package.emplace_back(BIND(replace_dN2, "DELTAN_XFM_2"));
      }


    // *******************************************************************


    std::string replace_zeta1::roll(const macro_argument_list& args, const abstract_index_database& indices)
      {
        std::unique_ptr<map_lambda> lambda = this->zeta1_tensor->compute_lambda(indices[0]);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_zeta2::roll(const macro_argument_list& args, const abstract_index_database& indices)
      {
        GiNaC::symbol  k = sym_factory.get_symbol(args[ZETA_XFM_2_K_ARGUMENT]);
        GiNaC::symbol k1 = sym_factory.get_symbol(args[ZETA_XFM_2_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[ZETA_XFM_2_K2_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[ZETA_XFM_2_A_ARGUMENT]);

        std::unique_ptr<map_lambda> lambda = this->zeta2_tensor->compute_lambda(indices[0], indices[1], k, k1, k2, a);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_dN1::roll(const macro_argument_list& args, const abstract_index_database& indices)
      {
        throw rule_apply_fail(ERROR_DN_DOES_NOT_ROLL);
      }


    std::string replace_dN2::roll(const macro_argument_list& args, const abstract_index_database& indices)
      {
        throw rule_apply_fail(ERROR_DN_DOES_NOT_ROLL);
      }


    // *******************************************************************


    void replace_zeta1::pre_hook(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->zeta1_tensor->compute();
        this->map =  std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_zeta2::pre_hook(const macro_argument_list& args)
      {
        GiNaC::symbol  k = sym_factory.get_symbol(args[ZETA_XFM_2_K_ARGUMENT]);
        GiNaC::symbol k1 = sym_factory.get_symbol(args[ZETA_XFM_2_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[ZETA_XFM_2_K2_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[ZETA_XFM_2_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->zeta2_tensor->compute(k, k1, k2, a);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_dN1::pre_hook(const macro_argument_list& args)
	    {
        std::unique_ptr<flattened_tensor> container = this->dN1_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
	    }


    void replace_dN2::pre_hook(const macro_argument_list& args)
	    {
        std::unique_ptr<flattened_tensor> container = this->dN2_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
	    }


  } // namespace macro_packages
