//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>

#include "lagrangian_tensors.h"
#include "cse.h"
#include "u_tensor_factory.h"


#define BIND1(X) std::bind(&lagrangian_tensors::X, this, std::placeholders::_1)
#define BIND3(X) std::bind(&lagrangian_tensors::X, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)


namespace macro_packages
  {

    constexpr unsigned int A_TENSOR_K1_ARGUMENT = 0;
    constexpr unsigned int A_TENSOR_K2_ARGUMENT = 1;
    constexpr unsigned int A_TENSOR_K3_ARGUMENT = 2;
    constexpr unsigned int A_TENSOR_A_ARGUMENT = 3;
    constexpr unsigned int A_TENSOR_TOTAL_ARGUMENTS = 4;
    constexpr unsigned int A_TENSOR_TOTAL_INDICES = 3;

    constexpr unsigned int B_TENSOR_K1_ARGUMENT = 0;
    constexpr unsigned int B_TENSOR_K2_ARGUMENT = 1;
    constexpr unsigned int B_TENSOR_K3_ARGUMENT = 2;
    constexpr unsigned int B_TENSOR_A_ARGUMENT = 3;
    constexpr unsigned int B_TENSOR_TOTAL_ARGUMENTS = 4;
    constexpr unsigned int B_TENSOR_TOTAL_INDICES = 3;

    constexpr unsigned int C_TENSOR_K1_ARGUMENT = 0;
    constexpr unsigned int C_TENSOR_K2_ARGUMENT = 1;
    constexpr unsigned int C_TENSOR_K3_ARGUMENT = 2;
    constexpr unsigned int C_TENSOR_A_ARGUMENT = 3;
    constexpr unsigned int C_TENSOR_TOTAL_ARUGMENTS = 4;
    constexpr unsigned int C_TENSOR_TOTAL_INDICES = 3;

    constexpr unsigned int A_PREDEF_K1_ARGUMENT = 0;
    constexpr unsigned int A_PREDEF_K2_ARGUMENT = 1;
    constexpr unsigned int A_PREDEF_K3_ARGUMENT = 2;
    constexpr unsigned int A_PREDEF_A_ARGUMENT = 3;
    constexpr unsigned int A_PREDEF_HSQ_ARGUMENT = 4;
    constexpr unsigned int A_PREDEF_EPSILON_ARGUMENT = 5;
    constexpr unsigned int A_PREDEF_TOTAL_ARGUMENTS = 6;
    constexpr unsigned int A_PREDEF_TOTAl_INDICES = 3;

    constexpr unsigned int B_PREDEF_K1_ARGUMENT = 0;
    constexpr unsigned int B_PREDEF_K2_ARGUMENT = 1;
    constexpr unsigned int B_PREDEF_K3_ARGUMENT = 2;
    constexpr unsigned int B_PREDEF_A_ARGUMENT = 3;
    constexpr unsigned int B_PREDEF_HSQ_ARGUMENT = 4;
    constexpr unsigned int B_PREDEF_EPSILON_ARGUMENT = 5;
    constexpr unsigned int B_PREDEF_TOTAL_ARGUMENTS = 6;
    constexpr unsigned int B_PREDEF_TOTAL_INDICES = 3;

    constexpr unsigned int C_PREDEF_K1_ARGUMENT = 0;
    constexpr unsigned int C_PREDEF_K2_ARGUMENT = 1;
    constexpr unsigned int C_PREDEF_K3_ARGUMENT = 2;
    constexpr unsigned int C_PREDEF_A_ARGUMENT = 3;
    constexpr unsigned int C_PREDEF_HSQ_ARGUMENT = 4;
    constexpr unsigned int C_PREDEF_EPSILON_ARGUMENT = 5;
    constexpr unsigned int C_PREDEF_TOTAL_ARGUMENTS = 6;
    constexpr unsigned int C_PREDEF_TOTAL_INDICES = 3;

    constexpr unsigned int M_TENSOR_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int M_TENSOR_TOTAL_INDICES = 2;

    constexpr unsigned int M_PREDEF_HSQ_ARGUMENT = 0;
    constexpr unsigned int M_PREDEF_EPSILON_ARGUMENT = 1;
    constexpr unsigned int M_PREDEF_TOTAL_ARGUMENTS = 2;
    constexpr unsigned int M_PREDEF_TOTAL_INDICES = 2;


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
          { BIND1(pre_A_tensor),                BIND1(pre_B_tensor),                BIND1(pre_C_tensor),
            BIND1(pre_A_predef),                BIND1(pre_B_predef),                BIND1(pre_C_predef),
            BIND1(pre_M_tensor),                BIND1(pre_M_predef)
          };

        const std::vector<replacement_rule_post> posts =
          { nullptr,                            nullptr,                            nullptr,
            nullptr,                            nullptr,                            nullptr,
            nullptr,                            nullptr
          };

        const std::vector<replacement_rule_index> rules =
          { BIND3(replace_3index_field_tensor), BIND3(replace_3index_field_tensor), BIND3(replace_3index_field_tensor),
            BIND3(replace_3index_field_tensor), BIND3(replace_3index_field_tensor), BIND3(replace_3index_field_tensor),
            BIND3(replace_2index_field_tensor), BIND3(replace_2index_field_tensor)
          };

        const std::vector<std::string> names =
          { "A_TENSOR",                         "B_TENSOR",                         "C_TENSOR",
            "A_PREDEF",                         "B_PREDEF",                         "C_PREDEF",
            "M_TENSOR",                         "M_PREDEF"
          };

        const std::vector<unsigned int> args =
          { A_TENSOR_TOTAL_ARGUMENTS,           B_TENSOR_TOTAL_ARGUMENTS,           C_TENSOR_TOTAL_ARUGMENTS,
            A_PREDEF_TOTAL_ARGUMENTS,           B_PREDEF_TOTAL_ARGUMENTS,           C_PREDEF_TOTAL_ARGUMENTS,
            M_TENSOR_TOTAL_ARGUMENTS,           M_PREDEF_TOTAL_ARGUMENTS
          };

        const std::vector<unsigned int> indices =
          { A_TENSOR_TOTAL_INDICES,             B_TENSOR_TOTAL_INDICES,             C_TENSOR_TOTAL_INDICES,
            A_PREDEF_TOTAl_INDICES,             B_PREDEF_TOTAL_INDICES,             C_PREDEF_TOTAL_INDICES,
            M_TENSOR_TOTAL_INDICES,             M_PREDEF_TOTAL_INDICES
          };

        const std::vector<enum index_class> ranges =
          { index_class::field_only,            index_class::field_only,            index_class::field_only,
            index_class::field_only,            index_class::field_only,            index_class::field_only,
            index_class::field_only,            index_class::field_only
          };

        const std::vector<enum unroll_behaviour> unroll =
          { unroll_behaviour::allow,            unroll_behaviour::allow,            unroll_behaviour::allow,
            unroll_behaviour::allow,            unroll_behaviour::allow,            unroll_behaviour::allow,
            unroll_behaviour::allow,            unroll_behaviour::allow
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


    std::unique_ptr<cse_map> lagrangian_tensors::pre_A_tensor(const macro_argument_list& args)
      {
		    symbol_factory& sym_factory = this->data_payload.get_symbol_factory();

        GiNaC::symbol k1 = sym_factory.get_symbol(args[A_TENSOR_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[A_TENSOR_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[A_TENSOR_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[A_TENSOR_A_ARGUMENT]);

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_A(k1, k2, k3, a, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> lagrangian_tensors::pre_A_predef(const macro_argument_list& args)
      {
        symbol_factory& sym_factory = this->data_payload.get_symbol_factory();

        GiNaC::symbol k1 = sym_factory.get_symbol(args[A_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[A_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[A_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[A_PREDEF_TOTAl_INDICES]);

        GiNaC::symbol Hsq_symbol = sym_factory.get_symbol(args[A_PREDEF_HSQ_ARGUMENT]);
        GiNaC::symbol eps_symbol = sym_factory.get_symbol(args[A_PREDEF_EPSILON_ARGUMENT]);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_A(k1, k2, k3, a, Hsq, eps, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


      // ******************************************************************


    std::unique_ptr<cse_map> lagrangian_tensors::pre_B_tensor(const macro_argument_list& args)
      {
        symbol_factory& sym_factory = this->data_payload.get_symbol_factory();

        GiNaC::symbol k1 = sym_factory.get_symbol(args[B_TENSOR_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[B_TENSOR_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[B_TENSOR_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[B_TENSOR_A_ARGUMENT]);

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_B(k1, k2, k3, a, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> lagrangian_tensors::pre_B_predef(const macro_argument_list& args)
      {
        symbol_factory& sym_factory = this->data_payload.get_symbol_factory();

        GiNaC::symbol k1 = sym_factory.get_symbol(args[B_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[B_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[B_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[B_PREDEF_A_ARGUMENT]);

        GiNaC::symbol Hsq_symbol(args[B_PREDEF_HSQ_ARGUMENT]);
        GiNaC::symbol eps_symbol(args[B_PREDEF_EPSILON_ARGUMENT]);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_B(k1, k2, k3, a, Hsq, eps, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    // ******************************************************************


    std::unique_ptr<cse_map> lagrangian_tensors::pre_C_tensor(const macro_argument_list& args)
      {
        symbol_factory& sym_factory = this->data_payload.get_symbol_factory();

        GiNaC::symbol k1 = sym_factory.get_symbol(args[C_TENSOR_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[C_TENSOR_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[C_TENSOR_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[C_TENSOR_A_ARGUMENT]);

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_C(k1, k2, k3, a, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> lagrangian_tensors::pre_C_predef(const macro_argument_list& args)
      {
        symbol_factory& sym_factory = this->data_payload.get_symbol_factory();

        GiNaC::symbol k1 = sym_factory.get_symbol(args[C_PREDEF_K1_ARGUMENT]);
        GiNaC::symbol k2 = sym_factory.get_symbol(args[C_PREDEF_K2_ARGUMENT]);
        GiNaC::symbol k3 = sym_factory.get_symbol(args[C_PREDEF_K3_ARGUMENT]);
        GiNaC::symbol  a = sym_factory.get_symbol(args[C_PREDEF_A_ARGUMENT]);

        GiNaC::symbol Hsq_symbol = sym_factory.get_symbol(args[C_PREDEF_HSQ_ARGUMENT]);
        GiNaC::symbol eps_symbol = sym_factory.get_symbol(args[C_PREDEF_EPSILON_ARGUMENT]);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_C(k1, k2, k3, a, Hsq, eps, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


// ******************************************************************


    std::unique_ptr<cse_map> lagrangian_tensors::pre_M_tensor(const macro_argument_list& args)
      {
        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_M(*container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }


    std::unique_ptr<cse_map> lagrangian_tensors::pre_M_predef(const macro_argument_list& args)
      {
        symbol_factory& sym_factory = this->data_payload.get_symbol_factory();

        GiNaC::symbol Hsq_symbol = sym_factory.get_symbol(args[M_PREDEF_HSQ_ARGUMENT]);
        GiNaC::symbol eps_symbol = sym_factory.get_symbol(args[M_PREDEF_EPSILON_ARGUMENT]);
        GiNaC::ex     Hsq = Hsq_symbol;
        GiNaC::ex     eps = eps_symbol;

        std::unique_ptr< std::vector<GiNaC::ex> > container = std::make_unique< std::vector<GiNaC::ex> >();
        this->u_factory.compute_M(Hsq, eps, *container);

        return std::make_unique<cse_map>(std::move(container), this->cse_worker);
      }

  } // namespace macro_packages
