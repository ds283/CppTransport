//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "utensors.h"

#include "cse_map.h"


namespace macro_packages
  {

    const std::vector<simple_rule> utensors::get_pre_rules()
      {
        std::vector<simple_rule> package;

        return(package);
      }


    const std::vector<simple_rule> utensors::get_post_rules()
      {
        std::vector<simple_rule> package;

        return(package);
      }


    const std::vector<index_rule> utensors::get_index_rules()
      {
        std::vector<index_rule> package;

        const std::vector<replacement_rule_pre> pres =
          { &this->pre_u1_tensor,         &this->pre_u2_tensor,         &this->pre_u3_tensor,
            &this->pre_u1_predef,         &this->pre_u2_predef,         &this->pre_u3_predef,
            nullptr,                      nullptr
          };

        const std::vector<replacement_rule_post> posts =
          { &this->generic_post_hook,     &this->generic_post_hook,     &this->generic_post_hook,
            &this->generic_post_hook,     &this->generic_post_hook,     &this->generic_post_hook,
            nullptr,                      nullptr
          };

        const std::vector<replacement_rule_index> rules =
          { &this->replace_1index_tensor, &this->replace_2index_tensor, &this->replace_3index_tensor,
            &this->replace_1index_tensor, &this->replace_2index_tensor, &this->replace_3index_tensor,
            &this->replace_u2_name,       &this->replace_u3_name
          };

        const std::vector<unsigned int> args =
          { 0,                            2,                            4,
            2,                            4,                            6,
            1,                            1
          };

        const std::vector<unsigned int> indices =
          { 1,                            2,                            3,
            1,                            2,                            3,
            2,                            3
          };

        const std::vector<unsigned int> ranges =
          { 2,                            2,                            2,
            2,                            2,                            2,
            2,                            2
          };

        assert(pres.size() == posts.size());
        assert(pres.size() == rules.size());
        assert(pres.size() == args.size());
        assert(pres.size() == ranges.size());

        for(int i = 0; i < pres.size(); i++)
          {
            index_rule rule;

            rule.rule    = rules[i];
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


    void* utensors::pre_u1_tensor(const std::vector<std::string>& args)
      {
        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_u1(*container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return(map);
      }


    void* utensors::pre_u1_predef(const std::vector<std::string>& args)
      {
        assert(args.size() == 2);

        GiNaC::symbol Hsq_symbol(args.size() >= 1 ? args[0] : this->default_Hsq);
        GiNaC::symbol eps_symbol(args.size() >= 2 ? args[1] : this->default_eps);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_u1(Hsq, eps, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return(map);
      }


    // ******************************************************************


    void* utensors::pre_u2_tensor(const std::vector<std::string>& args)
      {
        assert(args.size() == 2);

        GiNaC::symbol k(args.size() >= 1 ? args[0] : this->default_k);
        GiNaC::symbol a(args.size() >= 2 ? args[1] : this->default_a);

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_u2(k, a, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return(map);
      }


    void* utensors::pre_u2_predef(const std::vector<std::string>& args)
      {
        assert(args.size() == 4);

        GiNaC::symbol k(args.size() >= 1 ? args[0] : this->default_k);
        GiNaC::symbol a(args.size() >= 2 ? args[1] : this->default_a);

        GiNaC::symbol Hsq_symbol(args.size() >= 3 ? args[2] : this->default_Hsq);
        GiNaC::symbol eps_symbol(args.size() >= 4 ? args[3] : this->default_eps);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_u2(k, a, Hsq, eps, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return(map);
      }


    // ******************************************************************


    void* utensors::pre_u3_tensor(const std::vector<std::string>& args)
      {
        assert(args.size() == 4);

        GiNaC::symbol k1(args.size() >= 1 ? args[0] : this->default_k1);
        GiNaC::symbol k2(args.size() >= 2 ? args[1] : this->default_k2);
        GiNaC::symbol k3(args.size() >= 3 ? args[2] : this->default_k3);
        GiNaC::symbol  a(args.size() >= 4 ? args[3] : this->default_a);

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_u3(k1, k2, k3, a, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return(map);
      }


    void* utensors::pre_u3_predef(const std::vector<std::string>& args)
      {
        assert(args.size() == 6);

        GiNaC::symbol k1(args.size() >= 1 ? args[0] : this->default_k1);
        GiNaC::symbol k2(args.size() >= 2 ? args[1] : this->default_k2);
        GiNaC::symbol k3(args.size() >= 3 ? args[2] : this->default_k3);
        GiNaC::symbol  a(args.size() >= 4 ? args[3] : this->default_a);

        GiNaC::symbol Hsq_symbol(args.size() >= 5 ? args[4] : this->default_Hsq);
        GiNaC::symbol eps_symbol(args.size() >= 6 ? args[5] : this->default_eps);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_u3(k1, k2, k3, a, Hsq, eps, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return(map);
      }


    // ******************************************************************


    std::string utensors::replace_u2_name(const std::vector<std::string>& args,
                                                 std::vector<struct index_assignment> indices, void* state)
      {
        std::ostringstream out;

        assert(indices.size() == 2);
        assert(indices[0].species < this->data.source->get_number_fields());
        assert(indices[1].species < this->data.source->get_number_fields());

        assert(args.size() == 1);

        out << (args.size() >= 1 ? args[0] : this->default_u2) << this->index_pad;

        for(int i = 0; i < indices.size(); i++)
          {
            out << (i > 0 ? this->index_pad : "") << index_numeric(indices[i]);
          }

        return(out.str());
      }


    std::string utensors::replace_u3_name(const std::vector<std::string>& args,
                                          std::vector<struct index_assignment> indices, void* state)
      {
        std::ostringstream out;

        assert(indices.size() == 3);
        assert(indices[0].species < this->data.source->get_number_fields());
        assert(indices[1].species < this->data.source->get_number_fields());
        assert(indices[2].species < this->data.source->get_number_fields());

        assert(args.size() == 1);

        out << (args.size() >= 1 ? args[0] : this->default_u3) << this->index_pad;

        for(int i = 0; i < indices.size(); i++)
          {
            out << (i > 0 ? this->index_pad : "") << index_numeric(indices[i]);
          }

        return(out.str());
      }

  }
