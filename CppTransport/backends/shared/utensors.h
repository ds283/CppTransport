//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MACROS_UTENSORS_H
#define CPPTRANSPORT_MACROS_UTENSORS_H


#include "replacement_rule_package.h"
#include "cse_map_phase_indices.h"


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


    class replace_U1 : public cse_map_phase1
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_U1(std::string n, tensor_factory& f, cse& cw, symbol_factory& s, language_printer& prn)
          : cse_map_phase1(std::move(n), U1_PREDEF_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            shared(f.get_shared_resources()),
            sym_factory(s)
          {
            u1_tensor = f.make_u1(prn);
          }

        //! destructor
        virtual ~replace_U1() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return(unroll_behaviour::allow); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

        //! symbol factory
        symbol_factory& sym_factory;

        //! u1-tensor object
        std::unique_ptr<u1> u1_tensor;

      };


    class replace_U2 : public cse_map_phase2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_U2(std::string n, tensor_factory& f, cse& cw, symbol_factory& s, language_printer& prn)
          : cse_map_phase2(std::move(n), U2_PREDEF_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            shared(f.get_shared_resources()),
            sym_factory(s)
          {
            u2_tensor = f.make_u2(prn);
          }

        //! destructor
        virtual ~replace_U2() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return(unroll_behaviour::allow); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

        //! symbol factory
        symbol_factory& sym_factory;

        //! u1-tensor object
        std::unique_ptr<u2> u2_tensor;

      };


    class replace_U3 : public cse_map_phase3
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_U3(std::string n, tensor_factory& f, cse& cw, symbol_factory& s, language_printer& prn)
          : cse_map_phase3(std::move(n), U3_PREDEF_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            shared(f.get_shared_resources()),
            sym_factory(s)
          {
            u3_tensor = f.make_u3(prn);
          }

        //! destructor
        virtual ~replace_U3() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return(unroll_behaviour::allow); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

        //! symbol factory
        symbol_factory& sym_factory;

        //! u1-tensor object
        std::unique_ptr<u3> u3_tensor;

      };


    class utensors: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        utensors(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor
        virtual ~utensors() = default;

      };
  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROS_UTENSORS_H
