//
// Created by David Seery on 26/06/2017.
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


#include "curvature_tensors.h"


#define BIND_SYMBOL(X, N) std::move(std::make_unique<X>(N, f, cw, lm, p.get_symbol_factory(), prn))
#define EMPLACE(pkg, obj) try { emplace_rule(pkg, obj); } catch(std::exception& xe) { }


namespace macro_packages
  {

    curvature_tensors::curvature_tensors(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, lm, p, prn)
      {
        EMPLACE(index_package, BIND_SYMBOL(replace_metric, "METRIC"));
        EMPLACE(index_package, BIND_SYMBOL(replace_connexion, "CONNECTION"));
        EMPLACE(index_package, BIND_SYMBOL(replace_Riemann_A2, "RIEMANN_A2"));
        EMPLACE(index_package, BIND_SYMBOL(replace_Riemann_A3, "RIEMANN_A3"));
        EMPLACE(index_package, BIND_SYMBOL(replace_Riemann_B3, "RIEMANN_B3"));
      }


    // *******************************************************************


    unroll_state replace_metric::get_unroll(const index_literal_list& idx_list) const
      {
        if(idx_list[0]->get_variance() != idx_list[1]->get_variance()) throw rule_apply_fail(ERROR_METRIC_RULE_MIXED_INDICES);

        if(idx_list[0]->get_variance() == variance::covariant)
          {
            return this->G->get_unroll(idx_list);
          }

        return this->Ginv->get_unroll(idx_list);
      }


    void replace_metric::pre_hook(const macro_argument_list& args, const index_literal_list& indices)
      {
        if(indices[0]->get_variance() != indices[1]->get_variance()) throw rule_apply_fail(ERROR_METRIC_RULE_MIXED_INDICES);

        std::unique_ptr<flattened_tensor> container;

        if(indices[0]->get_variance() == variance::covariant)
          {
            container = this->G->compute(indices);
          }
        else
          {
            container = this->Ginv->compute(indices);
          }

        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::string replace_metric::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        if(indices[0]->get_variance() != indices[1]->get_variance()) throw rule_apply_fail(ERROR_METRIC_RULE_MIXED_INDICES);

        std::unique_ptr<atomic_lambda> lambda;
        if(indices[0]->get_variance() == variance::covariant)
          {
            lambda = this->G->compute_lambda(*indices[0], *indices[1]);
          }
        else
          {
            lambda = this->Ginv->compute_lambda(*indices[0], *indices[1]);
          }

        return this->lambda_mgr.cache(std::move(lambda));
      }


    unroll_state replace_connexion::get_unroll(const index_literal_list& idx_list) const
      {
        if(idx_list[0]->get_variance() != variance::contravariant) throw rule_apply_fail(ERROR_CONNEXION_INDICES);
        if(idx_list[1]->get_variance() != variance::covariant) throw rule_apply_fail(ERROR_CONNEXION_INDICES);
        if(idx_list[2]->get_variance() != variance::covariant) throw rule_apply_fail(ERROR_CONNEXION_INDICES);

        return this->Gamma->get_unroll(idx_list);
      }


    void replace_connexion::pre_hook(const macro_argument_list& args, const index_literal_list& indices)
      {
        if(indices[0]->get_variance() != variance::contravariant) throw rule_apply_fail(ERROR_CONNEXION_INDICES);
        if(indices[1]->get_variance() != variance::covariant) throw rule_apply_fail(ERROR_CONNEXION_INDICES);
        if(indices[2]->get_variance() != variance::covariant) throw rule_apply_fail(ERROR_CONNEXION_INDICES);

        std::unique_ptr<flattened_tensor> container = this->Gamma->compute(indices);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::string replace_connexion::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        if(indices[0]->get_variance() != variance::contravariant) throw rule_apply_fail(ERROR_CONNEXION_INDICES);
        if(indices[1]->get_variance() != variance::covariant) throw rule_apply_fail(ERROR_CONNEXION_INDICES);
        if(indices[2]->get_variance() != variance::covariant) throw rule_apply_fail(ERROR_CONNEXION_INDICES);

        std::unique_ptr<atomic_lambda> lambda = this->Gamma->compute_lambda(*indices[0], *indices[1], *indices[2]);
        return this->lambda_mgr.cache(std::move(lambda));
      }
    
    
    void replace_Riemann_A2::pre_hook(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::unique_ptr<flattened_tensor> container = this->A2->compute(indices);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }
    
    
    std::string replace_Riemann_A2::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::unique_ptr<atomic_lambda> lambda = this->A2->compute_lambda(*indices[0], *indices[1]);
        return this->lambda_mgr.cache(std::move(lambda));
      }
    
    
    void replace_Riemann_A3::pre_hook(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::unique_ptr<flattened_tensor> container = this->A3->compute(indices);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }
    
    
    std::string replace_Riemann_A3::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::unique_ptr<atomic_lambda> lambda = this->A3->compute_lambda(*indices[0], *indices[1], *indices[2]);
        return this->lambda_mgr.cache(std::move(lambda));
      }
    
    
    void replace_Riemann_B3::pre_hook(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::unique_ptr<flattened_tensor> container = this->B3->compute(indices);
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }
    
    
    std::string replace_Riemann_B3::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::unique_ptr<atomic_lambda> lambda = this->B3->compute_lambda(*indices[0], *indices[1], *indices[2]);
        return this->lambda_mgr.cache(std::move(lambda));
      }

  }   // namespace macro_packages
