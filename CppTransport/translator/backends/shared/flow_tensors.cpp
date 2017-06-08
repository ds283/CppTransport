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

#include "flow_tensors.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, f, cw, lm, prn))


namespace macro_packages
  {

    flow_tensors::flow_tensors(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, lm, p, prn)
      {
        pre_package.emplace_back(BIND(replace_V, "POTENTIAL"));
        pre_package.emplace_back(BIND(replace_Hsq, "HUBBLE_SQ"));
        pre_package.emplace_back(BIND(replace_eps, "EPSILON"));

        index_package.emplace_back(BIND(replace_parameter, "PARAMETER"));
        index_package.emplace_back(BIND(replace_field, "FIELD"));
        index_package.emplace_back(BIND(replace_coordinate, "COORDINATE"));
        index_package.emplace_back(BIND(replace_SR_velocity, "SR_VELOCITY"));
        index_package.emplace_back(BIND(replace_dV, "DV"));
        index_package.emplace_back(BIND(replace_ddV, "DDV"));
        index_package.emplace_back(BIND(replace_dddV, "DDDV"));
      }


    // *******************************************************************


    std::string replace_V::evaluate(const macro_argument_list& args)
      {
        GiNaC::ex V = this->Hubble_obj->compute_V();



        // pass to CSE module for evalaution
        this->cse_worker.parse(V);

        // emit
        return this->cse_worker.get_symbol_with_use_count(V);
      }


    std::string replace_Hsq::evaluate(const macro_argument_list& args)
      {
        GiNaC::ex Hsq = this->Hubble_obj->compute_Hsq();

        // pass to CSE module for evaluation
        this->cse_worker.parse(Hsq);

        // emit
        return this->cse_worker.get_symbol_with_use_count(Hsq);
      }


    std::string replace_eps::evaluate(const macro_argument_list& args)
      {
        GiNaC::ex eps = this->Hubble_obj->compute_eps();

        // pass to CSE module for evaluation
        this->cse_worker.parse(eps);

        // emit
        return this->cse_worker.get_symbol_with_use_count(eps);
      }


    // ******************************************************************


    std::string replace_parameter::unroll(const macro_argument_list& args, const assignment_list& indices)
      {
        std::unique_ptr<symbol_list> parameters = this->shared.generate_parameters(this->printer);
        return this->printer.ginac((*parameters)[indices[0].get_numeric_value()]);
      }


    std::string replace_field::unroll(const macro_argument_list& args, const assignment_list& indices)
      {
        std::unique_ptr<symbol_list> fields = this->shared.generate_fields(this->printer);
        return this->printer.ginac((*fields)[indices[0].get_numeric_value()]);
      }


    std::string replace_coordinate::unroll(const macro_argument_list& args, const assignment_list& indices)
      {
        std::unique_ptr<symbol_list> fields = this->shared.generate_fields(this->printer);
        std::unique_ptr<symbol_list> derivs = this->shared.generate_derivs(this->printer);

        std::string rval;

        if(indices[0].is_field())
          {
            rval = this->printer.ginac((*fields)[indices[0].species()]);
          }
        else if(indices[0].is_momentum())
          {
            rval = this->printer.ginac((*derivs)[indices[0].species()]);
          }
        else
          {
            assert(false);
          }

        return(rval);
      }


    // ******************************************************************


    void replace_SR_velocity::pre_hook(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->SR_velocity_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_dV::pre_hook(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->dV_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_ddV::pre_hook(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->ddV_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    void replace_dddV::pre_hook(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->dddV_tensor->compute();
        this->map = std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    // ******************************************************************


    std::string replace_parameter::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        GiNaC::ex sym = this->shared.generate_parameters(*indices[0], this->printer);
        return this->printer.ginac(sym);
      }


    std::string replace_field::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        GiNaC::ex sym = this->shared.generate_fields(*indices[0], this->printer);
        return this->printer.ginac(sym);
      }


    std::string replace_coordinate::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        // safe to re-use generate_fields since the symbol is the same as for the field list
        GiNaC::ex sym = this->shared.generate_fields(*indices[0], this->printer);
        return this->printer.ginac(sym);
      }


    std::string replace_SR_velocity::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::unique_ptr<atomic_lambda> lambda = this->SR_velocity_tensor->compute_lambda(*indices[0]);
        return this->lambda_mgr.cache(std::move(lambda));
      }


    std::string replace_dV::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::unique_ptr<atomic_lambda> lambda = this->dV_tensor->compute_lambda(*indices[0]);

        // assume that the result will always be just a single symbol, so can be safely inlined
        return this->printer.ginac(**lambda);
      }


    std::string replace_ddV::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::unique_ptr<atomic_lambda> lambda = this->ddV_tensor->compute_lambda(*indices[0], *indices[1]);

        // assume that the result will always be just a single symbol, so can be safely inlined
        return this->printer.ginac(**lambda);
      }


    std::string replace_dddV::roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::unique_ptr<atomic_lambda> lambda = this->dddV_tensor->compute_lambda(*indices[0], *indices[1], *indices[2]);

        // assume that the result will always be just a single symbol, so can be safely inlined
        return this->printer.ginac(**lambda);
      }

  } // namespace macro_packages
