//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef CPPTRANSPORT_MACROS_GAUGE_XFM_H
#define CPPTRANSPORT_MACROS_GAUGE_XFM_H


#include "replacement_rule_package.h"
#include "cse_map_phase_indices.h"


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


    class replace_zeta1 : public cse_map_phase1
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_zeta1(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : cse_map_phase1(std::move(n), ZETA_XFM_1_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            shared(f.get_shared_resources())
          {
            zeta1_tensor = f.make_zeta1(prn);
          }

        //! destructor
        virtual ~replace_zeta1() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return this->zeta1_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_hook(const macro_argument_list& args) override;

        //! evaluate
        virtual std::string roll(const macro_argument_list& args, const abstract_index_list& indices) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! zeta1 tensor
        std::unique_ptr<zeta1> zeta1_tensor;

      };


    class replace_zeta2 : public cse_map_phase2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_zeta2(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_phase2(std::move(n), ZETA_XFM_2_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            shared(f.get_shared_resources()),
            sym_factory(s)
          {
            zeta2_tensor = f.make_zeta2(prn);
          }

        //! destructor
        virtual ~replace_zeta2() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return this->zeta2_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_hook(const macro_argument_list& args) override;

        //! evaluate
        virtual std::string roll(const macro_argument_list& args, const abstract_index_list& indices) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! symbol factory
        symbol_factory& sym_factory;

        //! zeta2 tensor
        std::unique_ptr<zeta2> zeta2_tensor;

      };


    class replace_dN1 : public cse_map_phase1
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_dN1(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : cse_map_phase1(std::move(n), DELTAN_XFM_1_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            shared(f.get_shared_resources())
          {
            dN1_tensor = f.make_dN1(prn);
          }

        //! destructor
        virtual ~replace_dN1() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return this->dN1_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_hook(const macro_argument_list& args) override;

        //! evaluate
        virtual std::string roll(const macro_argument_list& args, const abstract_index_list& indices) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! dN1 tensor
        std::unique_ptr<dN1> dN1_tensor;

      };


    class replace_dN2 : public cse_map_phase2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_dN2(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : cse_map_phase2(std::move(n), DELTAN_XFM_1_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            shared(f.get_shared_resources())
          {
            dN2_tensor = f.make_dN2(prn);
          }

        //! destructor
        virtual ~replace_dN2() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return this->dN2_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_hook(const macro_argument_list& args) override;

        //! evaluate
        virtual std::string roll(const macro_argument_list& args, const abstract_index_list& indices) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! dN2 tensor
        std::unique_ptr<dN2> dN2_tensor;

      };


    class gauge_xfm: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        gauge_xfm(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~gauge_xfm() = default;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROS_GAUGE_XFM_H
