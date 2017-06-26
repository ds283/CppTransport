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



#ifndef CPPTRANSPORT_MACROS_LAGRANGIAN_TENSORS_H
#define CPPTRANSPORT_MACROS_LAGRANGIAN_TENSORS_H


#include "replacement_rule_package.h"
#include "cse_map_field_indices.h"


namespace macro_packages
  {

    constexpr unsigned int A_K1_ARGUMENT = 0;
    constexpr unsigned int A_K2_ARGUMENT = 1;
    constexpr unsigned int A_K3_ARGUMENT = 2;
    constexpr unsigned int A_A_ARGUMENT = 3;
    constexpr unsigned int A_TOTAL_ARGUMENTS = 4;
    constexpr unsigned int A_TOTAL_INDICES = 3;
    
    constexpr unsigned int ATILDE_K1_ARGUMENT = 0;
    constexpr unsigned int ATILDE_K2_ARGUMENT = 1;
    constexpr unsigned int ATILDE_K3_ARGUMENT = 2;
    constexpr unsigned int ATILDE_A_ARGUMENT = 3;
    constexpr unsigned int ATILDE_TOTAL_ARGUMENTS = 4;
    constexpr unsigned int ATILDE_TOTAL_INDICES = 3;
    
    constexpr unsigned int B_K1_ARGUMENT = 0;
    constexpr unsigned int B_K2_ARGUMENT = 1;
    constexpr unsigned int B_K3_ARGUMENT = 2;
    constexpr unsigned int B_A_ARGUMENT = 3;
    constexpr unsigned int B_TOTAL_ARGUMENTS = 4;
    constexpr unsigned int B_TOTAL_INDICES = 3;

    constexpr unsigned int C_K1_ARGUMENT = 0;
    constexpr unsigned int C_K2_ARGUMENT = 1;
    constexpr unsigned int C_K3_ARGUMENT = 2;
    constexpr unsigned int C_A_ARGUMENT = 3;
    constexpr unsigned int C_TOTAL_ARGUMENTS = 4;
    constexpr unsigned int C_TOTAL_INDICES = 3;

    constexpr unsigned int M_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int M_TOTAL_INDICES = 2;


    class replace_A : public cse_map_field3
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_A(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_field3(std::move(n), A_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            sym_factory(s)
          {
            A_tensor = f.make_A(prn, cw);
          }

        //! destructor
        virtual ~replace_A() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_behaviour get_unroll(const index_literal_list& idx_list) const override { return this->A_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:

        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! symbol factory
        symbol_factory& sym_factory;

        //! A-tensor object
        std::unique_ptr<A> A_tensor;

      };
    
    
    class replace_Atilde : public cse_map_field3
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        replace_Atilde(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_field3(std::move(n), ATILDE_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            sym_factory(s)
          {
            Atilde_tensor = f.make_Atilde(prn, cw);
          }
        
        //! destructor
        virtual ~replace_Atilde() = default;
        
        
        // INTERFACE
      
      public:
        
        //! determine unroll status
        unroll_behaviour get_unroll(const index_literal_list& idx_list) const override { return this->Atilde_tensor->get_unroll(idx_list); }
        
        
        // INTERNAL API
      
      protected:
        
        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;
        
        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;
        
        
        // INTERNAL DATA
      
      private:
        
        //! CSE worker
        cse& cse_worker;
        
        //! lambda manager
        lambda_manager& lambda_mgr;
        
        //! language printer
        language_printer& printer;
        
        //! symbol factory
        symbol_factory& sym_factory;
        
        //! A-tensor object
        std::unique_ptr<Atilde> Atilde_tensor;
        
      };


    class replace_B : public cse_map_field3
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_B(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_field3(std::move(n), B_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            sym_factory(s)
          {
            B_tensor = f.make_B(prn, cw);
          }

        //! destructor
        virtual ~replace_B() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_behaviour get_unroll(const index_literal_list& idx_list) const override { return this->B_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:

        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! symbol factory
        symbol_factory& sym_factory;

        //! B-tensor object
        std::unique_ptr<B> B_tensor;

      };


    class replace_C : public cse_map_field3
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_C(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_field3(std::move(n), C_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            sym_factory(s)
          {
            C_tensor = f.make_C(prn, cw);
          }

        //! destructor
        virtual ~replace_C() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_behaviour get_unroll(const index_literal_list& idx_list) const override { return this->C_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:

        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! symbol factory
        symbol_factory& sym_factory;

        //! C-tensor object
        std::unique_ptr<C> C_tensor;

      };


    class replace_M : public cse_map_field2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_M(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_field2(std::move(n), M_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            sym_factory(s)
          {
            M_tensor = f.make_M(prn, cw);
          }

        //! destructor
        virtual ~replace_M() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_behaviour get_unroll(const index_literal_list& idx_list) const override { return this->M_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:

        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! symbol factory
        symbol_factory& sym_factory;

        //! M-tensor object
        std::unique_ptr<M> M_tensor;

      };


    class lagrangian_tensors: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        lagrangian_tensors(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~lagrangian_tensors() = default;

      };

  }


#endif //CPPTRANSPORT_MACROS_LAGRANGIAN_TENSORS_H
