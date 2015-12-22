//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>

#include "flow_tensors.h"


#define BIND1(X, N) std::move(std::make_unique<X>(N, this->fctry, this->cse_worker, this->printer))
#define BIND3(X) std::bind(&flow_tensors::X, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define BIND1OLD(X) std::bind(&flow_tensors::X, this, std::placeholders::_1)


namespace macro_packages
  {

    flow_tensors::flow_tensors(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, p, prn),
        shared(f.get_shared_resources())
      {
        dV_tensor = f.make_dV(prn);
        ddV_tensor = f.make_ddV(prn);
        dddV_tensor = f.make_dddV(prn);
        SR_velocity_tensor = f.make_SR_velocity(prn);

        pre_package.emplace_back(BIND1(replace_V, "POTENTIAL"));
        pre_package.emplace_back(BIND1(replace_Hsq, "HUBBLE_SQ"));
        pre_package.emplace_back(BIND1(replace_eps, "EPSILON"));
      }


    const std::vector<index_rule> flow_tensors::get_index_rules()
      {
        std::vector<index_rule> package;

        const std::vector<replacement_pre_unroll> pres =
          { nullptr,                            nullptr,                            nullptr,
            BIND1OLD(pre_sr_velocity),             BIND1OLD(pre_dV),                      BIND1OLD(pre_ddV),
            BIND1OLD(pre_dddV)
          };

        const std::vector<replacement_post_unroll> posts =
          { nullptr,                            nullptr,                            nullptr,
            nullptr,                            nullptr,                            nullptr,
            nullptr
          };

        const std::vector<replacement_rule_unroll> rules =
          { BIND3(replace_parameter),           BIND3(replace_field),               BIND3(replace_coordinate),
            BIND3(replace_1index_field_tensor), BIND3(replace_1index_field_tensor), BIND3(replace_2index_field_tensor),
            BIND3(replace_3index_field_tensor)
          };

        const std::vector<std::string> names =
          { "PARAMETER",                        "FIELD",                            "COORDINATE",
            "SR_VELOCITY",                      "DV",                               "DDV",
            "DDDV"
          };

        const std::vector<unsigned int> args =
          { PARAMETER_TOTAL_ARGUMENTS,          FIELD_TOTAL_ARGUMENTS,              COORDINATE_TOTAL_ARGUMENTS,
            SR_VELOCITY_TOTAL_ARGUMENTS,        DV_TOTAL_ARGUMENTS,                 DDV_TOTAL_ARGUMENTS,
            DDDV_TOTAL_ARGUMENTS
          };

        const std::vector<unsigned int> indices =
          { PARAMETER_TOTAL_INDICES,            FIELD_TOTAL_INDICES,                COORDINATE_TOTAL_INDICES,
            SR_VELOCITY_TOTAL_INDICES,          DV_TOTAL_INDICES,                   DDV_TOTAL_INDICES,
            DDDV_TOTAL_INDICES
          };

        const std::vector<enum index_class> ranges =
          { index_class::parameter,             index_class::field_only,            index_class::full,
            index_class::field_only,            index_class::field_only,            index_class::field_only,
            index_class::field_only
          };

        const std::vector<enum unroll_behaviour> unroll =
          { unroll_behaviour::force,            unroll_behaviour::force,            unroll_behaviour::force,
            unroll_behaviour::force,            unroll_behaviour::force,            unroll_behaviour::force,
            unroll_behaviour::force
          };

        assert(pres.size() == posts.size());
        assert(pres.size() == rules.size());
        assert(pres.size() == names.size());
        assert(pres.size() == args.size());
        assert(pres.size() == ranges.size());
        assert(pres.size() == unroll.size());

        for(int i = 0; i < pres.size(); ++i)
          {
            package.emplace_back(names[i], rules[i], pres[i], posts[i], args[i], indices[i], ranges[i], unroll[i]);
          }

        return(package);
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


    std::string flow_tensors::replace_parameter(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        std::unique_ptr<symbol_list> parameters = this->shared.generate_parameters(this->printer);
        return this->printer.ginac((*parameters)[indices[0].get_numeric_value()]);
      }


    std::string flow_tensors::replace_field(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        std::unique_ptr<symbol_list> fields = this->shared.generate_fields(this->printer);
        return this->printer.ginac((*fields)[indices[0].get_numeric_value()]);
      }


    std::string flow_tensors::replace_coordinate(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
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


    std::unique_ptr<cse_map> flow_tensors::pre_sr_velocity(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->SR_velocity_tensor->compute();
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> flow_tensors::pre_dV(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->dV_tensor->compute();
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> flow_tensors::pre_ddV(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->ddV_tensor->compute();
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> flow_tensors::pre_dddV(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->dddV_tensor->compute();
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


  } // namespace macro_packages
