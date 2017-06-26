//
// Created by David Seery on 26/06/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_CURVATURE_TENSORS_H
#define CPPTRANSPORT_CURVATURE_TENSORS_H


#include "replacement_rule_package.h"
#include "cse_map_field_indices.h"


namespace macro_packages
  {

    constexpr unsigned int METRIC_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int METRIC_TOTAL_INDICES = 2;

    constexpr unsigned int CONNEXION_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int CONNEXION_TOTAL_INDICES = 2;


    class replace_metric : public cse_map_field2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_metric(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_field2(std::move(n), METRIC_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            sym_factory(s)
          {
            auto& cf = dynamic_cast<curvature_tensor_factory&>(f);
            G = cf.make_metric(prn, cw);
            Ginv = cf.make_metric_inverse(prn, cw);
          }

        //! destructor
        virtual ~replace_metric() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_behaviour get_unroll(const index_literal_list& idx_list) const override;


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

        //! metric-tensor object
        std::unique_ptr<metric> G;

        //! inverse-metric-tensor object
        std::unique_ptr<metric_inverse> Ginv;

      };


    class replace_connexion : public cse_map_field3
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_connexion(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, symbol_factory& s, language_printer& prn)
          : cse_map_field3(std::move(n), CONNEXION_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm),
            sym_factory(s)
          {
            auto& cf = dynamic_cast<curvature_tensor_factory&>(f);
            Gamma = cf.make_connexion(prn, cw);
          }

        //! destructor
        virtual ~replace_connexion() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_behaviour get_unroll(const index_literal_list& idx_list) const override;


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

        //! connexion object
        std::unique_ptr<connexion> Gamma;

      };


    class curvature_tensors: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        curvature_tensors(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~curvature_tensors() = default;

      };


  }   // namespace macro_packages


#endif //CPPTRANSPORT_CURVATURE_TENSORS_H
