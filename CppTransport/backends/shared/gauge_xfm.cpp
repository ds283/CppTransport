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

    constexpr unsigned int ZETA_XFM_1_PARAM_KERNEL_ARGUMENT = 0;
    constexpr unsigned int ZETA_XFM_1_COORD_KERNEL_ARGUMENT = 1;
    constexpr unsigned int ZETA_XFM_1_DV_KERNEL_ARGUMENT = 2;
    constexpr unsigned int ZETA_XFM_1_FLATTEN_ARGUMENT = 3;
    constexpr unsigned int ZETA_XFM_1_FIELD_FLATTEN_ARGUMENT = 4;
    constexpr unsigned int ZETA_XFM_1_TOTAL_ARGUMENTS = 5;
    constexpr unsigned int ZETA_XFM_1_TOTAL_INDICES = 1;

    constexpr unsigned int ZETA_XFM_2_K_ARGUMENT = 0;
    constexpr unsigned int ZETA_XFM_2_K1_ARGUMENT = 1;
    constexpr unsigned int ZETA_XFM_2_K2_ARGUMENT = 2;
    constexpr unsigned int ZETA_XFM_2_A_ARGUMENT = 3;
    constexpr unsigned int ZETA_XFM_2_PARAM_KERNEL_ARGUMENT = 4;
    constexpr unsigned int ZETA_XFM_2_COORD_KERNEL_ARGUMENT = 5;
    constexpr unsigned int ZETA_XFM_2_DV_KERNEL_ARGUMENT = 6;
    constexpr unsigned int ZETA_XFM_2_FLATTEN_ARGUMENT = 7;
    constexpr unsigned int ZETA_XFM_2_FIELD_FLATTEN_ARGUMENT = 8;
    constexpr unsigned int ZETA_XFM_2_TOTAL_ARGUMENTS = 9;
    constexpr unsigned int ZETA_XFM_2_TOTAL_INDICES = 2;

    constexpr unsigned int DELTAN_XFM_1_PARAM_KERNEL_ARGUMENT = 0;
    constexpr unsigned int DELTAN_XFM_1_COORD_KERNEL_ARGUMENT = 1;
    constexpr unsigned int DELTAN_XFM_1_FLATTEN_ARGUMENT = 2;
    constexpr unsigned int DELTAN_XFM_1_TOTAL_ARGUMENTS = 3;
    constexpr unsigned int DELTAN_XFM_1_TOTAL_INDICES = 1;

    constexpr unsigned int DELTAN_XFM_2_PARAM_KERNEL_ARGUMENT = 0;
    constexpr unsigned int DELTAN_XFM_2_COORD_KERNEL_ARGUMENT = 1;
    constexpr unsigned int DELTAN_XFM_2_DV_KERNEL_ARGUMENT = 2;
    constexpr unsigned int DELTAN_XFM_2_FLATTEN_ARGUMENT = 3;
    constexpr unsigned int DELTAN_XFM_2_FIELD_FLATTEN_ARGUMENT = 4;
    constexpr unsigned int DELTAN_XFM_2_TOTAL_ARGUMENTS = 5;
    constexpr unsigned int DELTAN_XFM_2_TOTAL_INDICES = 2;


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
          { nullptr,                      nullptr,
            nullptr,                      nullptr,
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
        std::string param_kernel = args[ZETA_XFM_1_PARAM_KERNEL_ARGUMENT];
        std::string coord_kernel = args[ZETA_XFM_1_COORD_KERNEL_ARGUMENT];
        std::string dV_kernel    = args[ZETA_XFM_1_DV_KERNEL_ARGUMENT];
        std::string flattener    = args[ZETA_XFM_1_FLATTEN_ARGUMENT];
        std::string f_flattener  = args[ZETA_XFM_1_FIELD_FLATTEN_ARGUMENT];

        std::unique_ptr< std::vector<GiNaC::symbol> > params = this->parameter_list(param_kernel);
        std::unique_ptr< std::vector<GiNaC::symbol> > fields = this->field_list(coord_kernel, flattener);
        std::unique_ptr< std::vector<GiNaC::symbol> > derivs = this->deriv_list(coord_kernel, flattener);
        std::unique_ptr< std::vector<GiNaC::symbol> > dV     = this->index1_field_list(dV_kernel, f_flattener);

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_zeta_xfm_1(*params, *fields, *derivs, *dV, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> gauge_xfm::pre_zeta_xfm_2(const macro_argument_list& args)
      {
        std::string param_kernel = args[ZETA_XFM_2_PARAM_KERNEL_ARGUMENT];
        std::string coord_kernel = args[ZETA_XFM_2_COORD_KERNEL_ARGUMENT];
        std::string dV_kernel    = args[ZETA_XFM_2_DV_KERNEL_ARGUMENT];
        std::string flattener    = args[ZETA_XFM_2_FLATTEN_ARGUMENT];
        std::string f_flattener  = args[ZETA_XFM_2_FIELD_FLATTEN_ARGUMENT];

        std::unique_ptr< std::vector<GiNaC::symbol> > params = this->parameter_list(param_kernel);
        std::unique_ptr< std::vector<GiNaC::symbol> > fields = this->field_list(coord_kernel, flattener);
        std::unique_ptr< std::vector<GiNaC::symbol> > derivs = this->deriv_list(coord_kernel, flattener);
        std::unique_ptr< std::vector<GiNaC::symbol> > dV     = this->index1_field_list(dV_kernel, f_flattener);

        GiNaC::symbol  k = sym_factory.get_symbol(args[ZETA_XFM_2_K_ARGUMENT]);
        GiNaC::symbol k1 = sym_factory.get_symbol(args[ZETA_XFM_2_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[ZETA_XFM_2_K2_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[ZETA_XFM_2_A_ARGUMENT]);

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_zeta_xfm_2(k, k1, k2, a, *params, *fields, *derivs, *dV, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> gauge_xfm::pre_deltaN_xfm_1(const macro_argument_list& args)
	    {
        std::string param_kernel = args[DELTAN_XFM_1_PARAM_KERNEL_ARGUMENT];
        std::string coord_kernel = args[DELTAN_XFM_1_COORD_KERNEL_ARGUMENT];
        std::string flattener    = args[DELTAN_XFM_1_FLATTEN_ARGUMENT];

        std::unique_ptr< std::vector<GiNaC::symbol> > params = this->parameter_list(param_kernel);
        std::unique_ptr< std::vector<GiNaC::symbol> > fields = this->field_list(coord_kernel, flattener);
        std::unique_ptr< std::vector<GiNaC::symbol> > derivs = this->deriv_list(coord_kernel, flattener);

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_deltaN_xfm_1(*params, *fields, *derivs, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
	    }


    std::unique_ptr<cse_map> gauge_xfm::pre_deltaN_xfm_2(const macro_argument_list& args)
	    {
        std::string param_kernel = args[DELTAN_XFM_2_PARAM_KERNEL_ARGUMENT];
        std::string coord_kernel = args[DELTAN_XFM_2_COORD_KERNEL_ARGUMENT];
        std::string dV_kernel    = args[DELTAN_XFM_2_DV_KERNEL_ARGUMENT];
        std::string flattener    = args[DELTAN_XFM_2_FLATTEN_ARGUMENT];
        std::string f_flattener  = args[DELTAN_XFM_2_FIELD_FLATTEN_ARGUMENT];

        std::unique_ptr< std::vector<GiNaC::symbol> > params = this->parameter_list(param_kernel);
        std::unique_ptr< std::vector<GiNaC::symbol> > fields = this->field_list(coord_kernel, flattener);
        std::unique_ptr< std::vector<GiNaC::symbol> > derivs = this->deriv_list(coord_kernel, flattener);
        std::unique_ptr< std::vector<GiNaC::symbol> > dV     = this->index1_field_list(dV_kernel, f_flattener);

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_deltaN_xfm_2(*params, *fields, *derivs, *dV, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
	    }


  } // namespace macro_packages
