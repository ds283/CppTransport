//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>

#include "flow_tensors.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, f, cw, prn))


namespace macro_packages
  {

    flow_tensors::flow_tensors(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, p, prn)
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


    std::string replace_parameter::evaluate(const macro_argument_list& args, const assignment_list& indices)
      {
        std::unique_ptr<symbol_list> parameters = this->shared.generate_parameters(this->printer);
        return this->printer.ginac((*parameters)[indices[0].get_numeric_value()]);
      }


    std::string replace_field::evaluate(const macro_argument_list& args, const assignment_list& indices)
      {
        std::unique_ptr<symbol_list> fields = this->shared.generate_fields(this->printer);
        return this->printer.ginac((*fields)[indices[0].get_numeric_value()]);
      }


    std::string replace_coordinate::evaluate(const macro_argument_list& args, const assignment_list& indices)
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


  } // namespace macro_packages
