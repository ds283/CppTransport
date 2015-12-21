//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>

#include "gauge_xfm.h"


#define BIND1(X) std::bind(&gauge_xfm::X, this, std::placeholders::_1)
#define BIND3(X) std::bind(&gauge_xfm::X, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)


namespace macro_packages
  {

    constexpr unsigned int ZETA_XFM_1_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int ZETA_XFM_1_TOTAL_INDICES = 1;

    constexpr unsigned int ZETA_XFM_2_K_ARGUMENT = 0;
    constexpr unsigned int ZETA_XFM_2_K1_ARGUMENT = 1;
    constexpr unsigned int ZETA_XFM_2_K2_ARGUMENT = 2;
    constexpr unsigned int ZETA_XFM_2_A_ARGUMENT = 3;
    constexpr unsigned int ZETA_XFM_2_TOTAL_ARGUMENTS = 4;
    constexpr unsigned int ZETA_XFM_2_TOTAL_INDICES = 2;

    constexpr unsigned int DELTAN_XFM_1_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DELTAN_XFM_1_TOTAL_INDICES = 1;

    constexpr unsigned int DELTAN_XFM_2_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DELTAN_XFM_2_TOTAL_INDICES = 2;


    gauge_xfm::gauge_xfm(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, p, prn)
      {
        zeta1_tensor = f.make_zeta1(prn);
        zeta2_tensor = f.make_zeta2(prn);
        dN1_tensor = f.make_dN1(prn);
        dN2_tensor = f.make_dN2(prn);
      }


    const std::vector<index_rule> gauge_xfm::get_index_rules()
      {
        std::vector<index_rule> package;

        const std::vector<replacement_pre_unroll> pres =
          { BIND1(pre_zeta_xfm_1),        BIND1(pre_zeta_xfm_2),
            BIND1(pre_deltaN_xfm_1),      BIND1(pre_deltaN_xfm_2)
          };

        const std::vector<replacement_post_unroll> posts =
          { nullptr,                      nullptr,
            nullptr,                      nullptr,
          };

        const std::vector<replacement_rule_unroll> rules =
          { BIND3(replace_1index_tensor), BIND3(replace_2index_tensor),
            BIND3(replace_1index_tensor), BIND3(replace_2index_tensor)
          };

        const std::vector<std::string> names =
          { "ZETA_XFM_1",                 "ZETA_XFM_2",
            "DELTAN_XFM_1",               "DELTAN_XFM_2"
          };

        const std::vector<unsigned int> args =
          { ZETA_XFM_1_TOTAL_ARGUMENTS,   ZETA_XFM_2_TOTAL_ARGUMENTS,
            DELTAN_XFM_1_TOTAL_ARGUMENTS, DELTAN_XFM_2_TOTAL_ARGUMENTS
          };

        const std::vector<unsigned int> indices =
          { ZETA_XFM_1_TOTAL_INDICES,     ZETA_XFM_2_TOTAL_INDICES,
            DELTAN_XFM_1_TOTAL_INDICES,   DELTAN_XFM_2_TOTAL_INDICES
          };

        const std::vector<enum index_class> ranges =
          { index_class::full,            index_class::full,
            index_class::full,            index_class::full
          };

        const std::vector<enum unroll_behaviour> unroll =
          { unroll_behaviour::allow,      unroll_behaviour::allow,
            unroll_behaviour::force,      unroll_behaviour::force         // delta-N macros have to be unrolled; they involve computing derivatives of H
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


    std::unique_ptr<cse_map> gauge_xfm::pre_zeta_xfm_1(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->zeta1_tensor->compute();
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> gauge_xfm::pre_zeta_xfm_2(const macro_argument_list& args)
      {
        GiNaC::symbol  k = sym_factory.get_symbol(args[ZETA_XFM_2_K_ARGUMENT]);
        GiNaC::symbol k1 = sym_factory.get_symbol(args[ZETA_XFM_2_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[ZETA_XFM_2_K2_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[ZETA_XFM_2_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->zeta2_tensor->compute(k, k1, k2, a);
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> gauge_xfm::pre_deltaN_xfm_1(const macro_argument_list& args)
	    {
        std::unique_ptr<flattened_tensor> container = this->dN1_tensor->compute();
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
	    }


    std::unique_ptr<cse_map> gauge_xfm::pre_deltaN_xfm_2(const macro_argument_list& args)
	    {
        std::unique_ptr<flattened_tensor> container = this->dN2_tensor->compute();
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
	    }


  } // namespace macro_packages
