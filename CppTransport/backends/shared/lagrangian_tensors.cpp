//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "lagrangian_tensors.h"

#include "cse_map.h"


namespace macro_packages
  {

    const std::vector<simple_rule> lagrangian_tensors::get_pre_rules()
      {
        std::vector<simple_rule> package;

        return(package);
      }


    const std::vector<simple_rule> lagrangian_tensors::get_post_rules()
      {
        std::vector<simple_rule> package;

        return(package);
      }


    const std::vector<index_rule> lagrangian_tensors::get_index_rules()
      {
        std::vector<index_rule> package;

        const std::vector<replacement_rule_pre> pres =
          { &this->pre_A_tensor,                &this->pre_B_tensor,                &this->pre_C_tensor,
            &this->pre_A_predef,                &this->pre_B_predef,                &this->pre_C_predef,
            &this->pre_M_tensor,                &this->pre_M_predef
          };

        const std::vector<replacement_rule_post> posts =
          { &this->generic_post_hook,           &this->generic_post_hook,           &this->generic_post_hook,
            &this->generic_post_hook,           &this->generic_post_hook,           &this->generic_post_hook,
            &this->generic_post_hook,           &this->generic_post_hook
          };

        const std::vector<replacement_rule_index> rules =
          { &this->replace_1index_field_tensor, &this->replace_2index_field_tensor, &this->replace_3index_field_tensor,
            &this->replace_1index_field_tensor, &this->replace_2index_field_tensor, &this->replace_3index_field_tensor,
            &this->replace_2index_field_tensor, &this->replace_2index_field_tensor
          };

        const std::vector<unsigned int> args =
          { 4,                                  4,                                  4,
            6,                                  6,                                  6,
            0,                                  2
          };

        const std::vector<unsigned int> indices =
          { 3,                                  3,                                  3,
            3,                                  3,                                  3,
            2,                                  2
          };

        const std::vector<unsigned int> ranges =
          { 1,                                  1,                                  1,
            1,                                  1,                                  1,
            1,                                  1
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


    void* lagrangian_tensors::pre_A_tensor(const std::vector<std::string>& args)
      {
        assert(args.size() == 4);

        GiNaC::symbol k1(args.size() >= 1 ? args[0] : this->default_k1);
        GiNaC::symbol k2(args.size() >= 2 ? args[1] : this->default_k2);
        GiNaC::symbol k3(args.size() >= 3 ? args[2] : this->default_k3);
        GiNaC::symbol  a(args.size() >= 4 ? args[3] : this->default_a);

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_A(k1, k2, k3, a, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return(map);
      }


    void* lagrangian_tensors::pre_A_predef(const std::vector<std::string>& args)
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
        this->data.u_factory->compute_A(k1, k2, k3, a, Hsq, eps, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return(map);
      }


      // ******************************************************************


    void* lagrangian_tensors::pre_B_tensor(const std::vector<std::string> &args)
      {
        assert(args.size() == 4);

        GiNaC::symbol k1(args.size() >= 1 ? args[0] : this->default_k1);
        GiNaC::symbol k2(args.size() >= 2 ? args[1] : this->default_k2);
        GiNaC::symbol k3(args.size() >= 3 ? args[2] : this->default_k3);
        GiNaC::symbol a(args.size() >= 4 ? args[3] : this->default_a);

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_B(k1, k2, k3, a, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return (map);
      }


    void* lagrangian_tensors::pre_B_predef(const std::vector<std::string>& args)
      {
        assert(args.size() == 6);

        GiNaC::symbol k1(args.size() >= 1 ? args[0] : this->default_k1);
        GiNaC::symbol k2(args.size() >= 2 ? args[1] : this->default_k2);
        GiNaC::symbol k3(args.size() >= 3 ? args[2] : this->default_k3);
        GiNaC::symbol a(args.size() >= 4 ? args[3] : this->default_a);

        GiNaC::symbol Hsq_symbol(args.size() >= 5 ? args[4] : this->default_Hsq);
        GiNaC::symbol eps_symbol(args.size() >= 6 ? args[5] : this->default_eps);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_B(k1, k2, k3, a, Hsq, eps, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return (map);
      }


    // ******************************************************************


    void* lagrangian_tensors::pre_C_tensor(const std::vector<std::string> &args)
      {
        assert(args.size() == 4);

        GiNaC::symbol k1(args.size() >= 1 ? args[0] : this->default_k1);
        GiNaC::symbol k2(args.size() >= 2 ? args[1] : this->default_k2);
        GiNaC::symbol k3(args.size() >= 3 ? args[2] : this->default_k3);
        GiNaC::symbol a(args.size() >= 4 ? args[3] : this->default_a);

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_C(k1, k2, k3, a, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return (map);
      }


    void* lagrangian_tensors::pre_C_predef(const std::vector<std::string> &args)
      {
        assert(args.size() == 6);

        GiNaC::symbol k1(args.size() >= 1 ? args[0] : this->default_k1);
        GiNaC::symbol k2(args.size() >= 2 ? args[1] : this->default_k2);
        GiNaC::symbol k3(args.size() >= 3 ? args[2] : this->default_k3);
        GiNaC::symbol a(args.size() >= 4 ? args[3] : this->default_a);

        GiNaC::symbol Hsq_symbol(args.size() >= 5 ? args[4] : this->default_Hsq);
        GiNaC::symbol eps_symbol(args.size() >= 6 ? args[5] : this->default_eps);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_C(k1, k2, k3, a, Hsq, eps, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return (map);
      }


// ******************************************************************


    void* lagrangian_tensors::pre_M_tensor(const std::vector<std::string> &args)
      {
        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_M(*container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return (map);
      }


    void* lagrangian_tensors::pre_M_predef(const std::vector<std::string> &args)
      {
        assert(args.size() == 2);

        GiNaC::symbol Hsq_symbol(args.size() >= 1 ? args[0] : this->default_Hsq);
        GiNaC::symbol eps_symbol(args.size() >= 2 ? args[1] : this->default_eps);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_M(Hsq, eps, *container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return (map);
      }

  } // namespace macro_packages
