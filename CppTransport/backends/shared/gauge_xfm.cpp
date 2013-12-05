//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "gauge_xfm.h"

#include "cse.h"
#include "cse_map.h"


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
          { &this->pre_zeta_xfm_1,        &this->pre_zeta_xfm_2
          };

        const std::vector<replacement_rule_post> posts =
          { &this->generic_post_hook,     &this->generic_post_hook
          };

        const std::vector<replacement_rule_index> rules =
          { &this->replace_1index_tensor, &this->replace_2index_tensor
          };

        const std::vector<unsigned int> args =
          { 0,                            0
          };

        const std::vector<unsigned int> indices =
          { 1,                            2
          };

        const std::vector<unsigned int> ranges =
          { 2,                            2
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


    void* gauge_xfm::pre_zeta_xfm_1(const std::vector<std::string>& args)
      {
        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_zeta_xfm_1(*container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return(map);
      }


    void* gauge_xfm::pre_zeta_xfm_2(const std::vector<std::string>& args)
      {
        std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
        this->data.u_factory->compute_zeta_xfm_2(*container, this->data.fl);

        cse_map* map = new cse_map(container, this->data, this->printer);

        return(map);
      }

  } // namespace macro_packages
