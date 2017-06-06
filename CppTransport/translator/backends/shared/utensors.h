//
// Created by David Seery on 04/12/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_MACROS_UTENSORS_H
#define CPPTRANSPORT_MACROS_UTENSORS_H


#include "replacement_rule_package.h"
#include "cse_map_phase_indices.h"


namespace macro_packages
  {

    constexpr unsigned int U1_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int U1_TOTAL_INDICES = 1;

    constexpr unsigned int U2_K_ARGUMENT = 0;
    constexpr unsigned int U2_A_ARGUMENT = 1;
    constexpr unsigned int U2_TOTAL_ARGUMENTS = 2;
    constexpr unsigned int U2_TOTAL_INDICES = 2;

    constexpr unsigned int U3_K1_ARGUMENT = 0;
    constexpr unsigned int U3_K2_ARGUMENT = 1;
    constexpr unsigned int U3_K3_ARGUMENT = 2;
    constexpr unsigned int U3_A_ARGUMENT = 3;
    constexpr unsigned int U3_TOTAL_ARGUMENTS = 4;
    constexpr unsigned int U3_TOTAL_INDICES = 3;


    class replace_U1 : public cse_map_phase1
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_U1(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_phase1(std::move(n), U1_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            shared(f.get_shared_resources()),
            sym_factory(s)
          {
            u1_tensor = f.make_u1(prn, cw);
          }

        //! destructor
        virtual ~replace_U1() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_behaviour get_unroll() const override { return this->u1_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_hook(const macro_argument_list& args) override;

        //! evaluate
        virtual std::string roll(const macro_argument_list& args, const abstract_index_database& indices) override;


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

        //! u1-tensor object
        std::unique_ptr<u1> u1_tensor;

      };


    class replace_U2 : public cse_map_phase2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_U2(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_phase2(std::move(n), U2_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            shared(f.get_shared_resources()),
            sym_factory(s)
          {
            u2_tensor = f.make_u2(prn, cw);
          }

        //! destructor
        virtual ~replace_U2() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_behaviour get_unroll() const override { return this->u2_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_hook(const macro_argument_list& args) override;

        //! evaluate
        virtual std::string roll(const macro_argument_list& args, const abstract_index_database& indices) override;


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

        //! u1-tensor object
        std::unique_ptr<u2> u2_tensor;

      };


    class replace_U3 : public cse_map_phase3
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_U3(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_phase3(std::move(n), U3_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            shared(f.get_shared_resources()),
            sym_factory(s)
          {
            u3_tensor = f.make_u3(prn, cw);
          }

        //! destructor
        virtual ~replace_U3() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_behaviour get_unroll() const override { return this->u3_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_hook(const macro_argument_list& args) override;

        //! evaluate
        virtual std::string roll(const macro_argument_list& args, const abstract_index_database& indices) override;


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

        //! u1-tensor object
        std::unique_ptr<u3> u3_tensor;

      };


    class utensors: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        utensors(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor
        virtual ~utensors() = default;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROS_UTENSORS_H
