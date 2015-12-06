//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>

#include "gauge_xfm.h"
#include "cse.h"
#include "u_tensor_factory.h"


#define BIND1(X) std::bind(&gauge_xfm::X, this, std::placeholders::_1)
#define BIND3(X) std::bind(&gauge_xfm::X, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)


namespace macro_packages
  {

    const std::vector<simple_rule> gauge_xfm::get_pre_rules()
      {
        std::vector<simple_rule> package;

        return(package);
      }


    const std::vector<simple_rule> gauge_xfm::get_post_rules()
      {
        std::vector<simple_rule> package;

        return(package);
      }


    const std::vector<index_rule> gauge_xfm::get_index_rules()
      {
        std::vector<index_rule> package;

        const std::vector<replacement_rule_pre> pres =
          { BIND1(pre_zeta_xfm_1),        BIND1(pre_zeta_xfm_2),
            BIND1(pre_deltaN_xfm_1),      BIND1(pre_deltaN_xfm_2)
          };

        const std::vector<replacement_rule_post> posts =
          { BIND1(generic_post_hook),     BIND1(generic_post_hook),
            BIND1(generic_post_hook),     BIND1(generic_post_hook),
          };

        const std::vector<replacement_rule_index> rules =
          { BIND3(replace_1index_tensor), BIND3(replace_2index_tensor),
            BIND3(replace_1index_tensor), BIND3(replace_2index_tensor)
          };

        const std::vector<std::string> names =
          { "ZETA_XFM_1",                 "ZETA_XFM_2",
            "DELTAN_XFM_1",               "DELTAN_XFM_2"
          };

        const std::vector<unsigned int> args =
          { 2,                            6,
            0,                            0
          };

        const std::vector<unsigned int> indices =
          { 1,                            2,
            1,                            2
          };

        const std::vector<unsigned int> ranges =
          { 2,                            2,
            2,                            2
          };

        assert(pres.size() == posts.size());
        assert(pres.size() == rules.size());
        assert(pres.size() == names.size());
        assert(pres.size() == args.size());
        assert(pres.size() == ranges.size());

        for(int i = 0; i < pres.size(); ++i)
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


    void* gauge_xfm::pre_zeta_xfm_1(const std::vector<std::string>& args)
      {
        assert(args.size() == 2);

        symbol_factory& sym_factory = this->data_payload.get_symbol_factory();

        GiNaC::symbol Hsq_symbol = sym_factory.get_symbol(args.size() >= 1 ? args[0] : this->default_Hsq);
        GiNaC::symbol eps_symbol = sym_factory.get_symbol(args.size() >= 2 ? args[1] : this->default_eps);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->u_factory->compute_zeta_xfm_1(Hsq, eps, *container, *this->fl);

        cse_map* map = this->cse_worker->map_factory(container);

        return(map);
      }


    void* gauge_xfm::pre_zeta_xfm_2(const std::vector<std::string>& args)
      {
        assert(args.size() == 6);

        symbol_factory& sym_factory = this->data_payload.get_symbol_factory();

        GiNaC::symbol  k = sym_factory.get_symbol(args.size() >= 1 ? args[0] : this->default_k);
        GiNaC::symbol k1 = sym_factory.get_symbol(args.size() >= 2 ? args[1] : this->default_k1);
        GiNaC::symbol k2 = sym_factory.get_symbol(args.size() >= 3 ? args[2] : this->default_k2);
        GiNaC::symbol  a = sym_factory.get_symbol(args.size() >= 4 ? args[3] : this->default_a);

        GiNaC::symbol Hsq_symbol = sym_factory.get_symbol(args.size() >= 5 ? args[4] : this->default_Hsq);
        GiNaC::symbol eps_symbol = sym_factory.get_symbol(args.size() >= 6 ? args[5] : this->default_eps);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->u_factory->compute_zeta_xfm_2(k, k1, k2, a, Hsq, eps, *container, *this->fl);

        cse_map* map = this->cse_worker->map_factory(container);

        return(map);
      }


    void* gauge_xfm::pre_deltaN_xfm_1(const std::vector<std::string>& args)
	    {
        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->u_factory->compute_deltaN_xfm_1(*container, *this->fl);

        cse_map* map = this->cse_worker->map_factory(container);

        return(map);
	    }


    void* gauge_xfm::pre_deltaN_xfm_2(const std::vector<std::string>& args)
	    {
        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->u_factory->compute_deltaN_xfm_2(*container, *this->fl);

        cse_map* map = this->cse_worker->map_factory(container);

        return(map);
	    }


  } // namespace macro_packages
