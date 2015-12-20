//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>
#include <sstream>

#include "utensors.h"


#define BIND1(X) std::bind(&utensors::X, this, std::placeholders::_1)
#define BIND3(X) std::bind(&utensors::X, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)


namespace macro_packages
  {

    constexpr unsigned int U1_PREDEF_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int U1_PREDEF_TOTAL_INDICES = 1;

    constexpr unsigned int U2_PREDEF_K_ARGUMENT = 0;
    constexpr unsigned int U2_PREDEF_A_ARGUMENT = 1;
    constexpr unsigned int U2_PREDEF_TOTAL_ARGUMENTS = 2;
    constexpr unsigned int U2_PREDEF_TOTAL_INDICES = 2;

    constexpr unsigned int U3_PREDEF_K1_ARGUMENT = 0;
    constexpr unsigned int U3_PREDEF_K2_ARGUMENT = 1;
    constexpr unsigned int U3_PREDEF_K3_ARGUMENT = 2;
    constexpr unsigned int U3_PREDEF_A_ARGUMENT = 3;
    constexpr unsigned int U3_PREDEF_TOTAL_ARGUMENTS = 4;
    constexpr unsigned int U3_PREDEF_TOTAL_INDICES = 3;


    utensors::utensors(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, p, prn)
      {
        u1_tensor = f.make_u1(prn);
        u2_tensor = f.make_u2(prn);
        u3_tensor = f.make_u3(prn);
      }


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

        const std::vector<replacement_pre_unroll> pres =
          {  BIND1(pre_u1_predef),         BIND1(pre_u2_predef),         BIND1(pre_u3_predef)
          };

        const std::vector<replacement_post_unroll> posts =
          { nullptr,                      nullptr,                      nullptr
          };

        const std::vector<replacement_rule_unroll> rules =
          { BIND3(replace_1index_tensor), BIND3(replace_2index_tensor), BIND3(replace_3index_tensor)
          };

        const std::vector<std::string> names =
          { "U1_PREDEF",                  "U2_PREDEF",                  "U3_PREDEF"
          };

        const std::vector<unsigned int> args =
          { U1_PREDEF_TOTAL_ARGUMENTS,    U2_PREDEF_TOTAL_ARGUMENTS,    U3_PREDEF_TOTAL_ARGUMENTS
          };

        const std::vector<unsigned int> indices =
          { U1_PREDEF_TOTAL_INDICES,      U2_PREDEF_TOTAL_INDICES,      U3_PREDEF_TOTAL_INDICES
          };

        const std::vector<enum index_class> ranges =
          { index_class::full,            index_class::full,            index_class::full
          };

        const std::vector<enum unroll_behaviour> unroll =
          { unroll_behaviour::allow,      unroll_behaviour::allow,      unroll_behaviour::allow
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


    std::unique_ptr<cse_map> utensors::pre_u1_predef(const macro_argument_list& args)
      {
        std::unique_ptr<flattened_tensor> container = this->u1_tensor->compute();
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> utensors::pre_u2_predef(const macro_argument_list& args)
      {
        GiNaC::symbol k = sym_factory.get_symbol(args[U2_PREDEF_K_ARGUMENT]);
        GiNaC::symbol a = sym_factory.get_symbol(args[U2_PREDEF_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->u2_tensor->compute(k, a);
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> utensors::pre_u3_predef(const macro_argument_list& args)
      {
        GiNaC::symbol k1 = sym_factory.get_symbol(args[U3_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[U3_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[U3_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[U3_PREDEF_A_ARGUMENT]);

        std::unique_ptr<flattened_tensor> container = this->u3_tensor->compute(k1, k2, k3, a);
        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }

  } // namespace macro_packages
