//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <functional>

#include "flow_tensors.h"
#include "cse.h"


#define BIND1(X) std::bind(&flow_tensors::X, this, std::placeholders::_1)
#define BIND3(X) std::bind(&flow_tensors::X, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)


namespace macro_packages
  {
    const std::vector<simple_rule> flow_tensors::get_pre_rules()
      {
        std::vector<simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
          { BIND1(replace_V), BIND1(replace_Hsq), BIND1(replace_eps)
          };

        const std::vector<std::string> names =
          { "POTENTIAL",      "HUBBLE_SQ",        "EPSILON"
          };

        const std::vector<unsigned int> args =
          { 0,                0,                  0
          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); i++)
          {
            simple_rule rule;

            rule.rule = rules[i];
            rule.args = args[i];
            rule.name = names[i];

            package.push_back(rule);
          }

        return(package);
      }


    const std::vector<simple_rule> flow_tensors::get_post_rules()
      {
        std::vector<simple_rule> package;

        return(package);
      }


    const std::vector<index_rule> flow_tensors::get_index_rules()
      {
        std::vector<index_rule> package;

        const std::vector<replacement_rule_pre> pres =
          { nullptr,                           nullptr,              nullptr,
            BIND1(pre_sr_velocity)
          };

        const std::vector<replacement_rule_post> posts =
          { nullptr,                           nullptr,              nullptr,
            BIND1(generic_post_hook)
          };

        const std::vector<replacement_rule_index> rules =
          { BIND3(replace_parameter),          BIND3(replace_field), BIND3(replace_coordinate),
            BIND3(replace_1index_field_tensor)
          };

        const std::vector<std::string> names =
          { "PARAMETER",                       "FIELD",              "COORDINATE",
            "SR_VELOCITY"
          };

        const std::vector<unsigned int> args =
          { 0,                                 0,                    0,
            0
          };

        const std::vector<unsigned int> indices =
          { 1,                                 1,                    1,
            1
          };

        const std::vector<unsigned int> ranges =
          { INDEX_RANGE_PARAMETER,             1,                    2,
            1
          };

        assert(pres.size() == posts.size());
        assert(pres.size() == rules.size());
        assert(pres.size() == names.size());
        assert(pres.size() == args.size());
        assert(pres.size() == ranges.size());

        for(int i = 0; i < pres.size(); i++)
          {
            index_rule rule;

            rule.rule    = rules[i];
            rule.name    = names[i];
            rule.pre     = pres[i];
            rule.post    = posts[i];
            rule.args    = args[i];
            rule.indices = indices[i];
            rule.range   = ranges[i];

            package.push_back(rule);
          }

        return(package);
      }


    // *******************************************************************


    std::string flow_tensors::replace_V(const std::vector<std::string> &args)
      {
        GiNaC::ex potential = this->data.parse_tree->get_potential();

        return(this->printer.ginac(potential));
      }


    std::string flow_tensors::replace_Hsq(const std::vector<std::string> &args)
      {
        GiNaC::ex Hsq = this->u_factory->compute_Hsq();

        return(this->printer.ginac(Hsq));
      }


    std::string flow_tensors::replace_eps(const std::vector<std::string> &args)
      {
        GiNaC::ex eps = this->u_factory->compute_eps();

        return(this->printer.ginac(eps));
      }


    // ******************************************************************


    std::string flow_tensors::replace_parameter(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state)
      {
        assert(indices.size() == 1);
        assert(indices[0].trait == index_parameter);
        assert(indices[0].species < this->data.parse_tree->get_number_params());

        std::vector<GiNaC::symbol> parameters = this->data.parse_tree->get_param_symbols();
        return(this->printer.ginac(parameters[indices[0].species]));
      }


    std::string flow_tensors::replace_field(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state)
      {
        assert(indices.size() == 1);
        assert(indices[0].trait == index_field);
        assert(indices[0].species < this->data.parse_tree->get_number_fields());

        std::vector<GiNaC::symbol> fields = this->data.parse_tree->get_field_symbols();
        return(this->printer.ginac(fields[indices[0].species]));
      }


    std::string flow_tensors::replace_coordinate(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state)
      {
        assert(indices.size() == 1);
        assert(indices[0].species < this->data.parse_tree->get_number_fields());

        std::vector<GiNaC::symbol> fields  = this->data.parse_tree->get_field_symbols();
        std::vector<GiNaC::symbol> momenta = this->data.parse_tree->get_deriv_symbols();

        std::string rval;

        switch(indices[0].trait)
          {
            case index_field:
              rval = this->printer.ginac(fields[indices[0].species]);
              break;

            case index_momentum:
              rval = this->printer.ginac(momenta[indices[0].species]);
              break;

            case index_parameter:
            default:
              assert(false);
          }

        return(rval);
      }


// ******************************************************************


    void* flow_tensors::pre_sr_velocity(const std::vector<std::string>& args)
      {
        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->u_factory->compute_sr_u(*container, this->fl);

        cse_map* map = this->cse_worker->map_factory(container);

        return(map);
      }


  } // namespace macro_packages
