//
// Created by David Seery on 26/11/2019.
// --@@
// Copyright (c) 2019 University of Sussex. All rights reserved.
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

#include <initializer_list>
#include <string>
#include <memory>
#include <parser/symbol_list.h>

#include "read_expressions.h"


namespace reader
  {

    static std::unique_ptr<GiNaC::symtab> build_symbol_table(const symbol_list& symbols, const std::string&& prefix)
      {
        auto table = std::make_unique<GiNaC::symtab>();

        for(size_t i = 0; i < symbols.size(); ++i)
          {
            const auto& item = symbols[i];
            auto label = prefix + std::to_string(i);

            (*table)[label] = item;
          }

        return table;
      }


    static std::unique_ptr<GiNaC::symtab> build_field_symbol_table(const symbol_list& fields)
      {
        return build_symbol_table(fields, "f_");
      }


    static std::unique_ptr<GiNaC::symtab> build_deriv_symbol_table(const symbol_list& derivs)
      {
        return build_symbol_table(derivs, "d_");
      }


    static std::unique_ptr<GiNaC::symtab> build_parameter_symbol_table(const symbol_list& params)
      {
        return build_symbol_table(params, "p_");
      }


    std::unique_ptr<GiNaC::symtab>
    build_total_symbol_table(const symbol_list& fields, const symbol_list& derivs, const symbol_list& params)
      {
        auto field_table = build_field_symbol_table(fields);
        auto deriv_table = build_deriv_symbol_table(derivs);
        auto parameter_table = build_parameter_symbol_table(params);

        auto full_table = std::make_unique<GiNaC::symtab>();
        std::move(field_table->begin(), field_table->end(), std::back_inserter(*full_table));
        std::move(parameter_table->begin(), parameter_table->end(), std::back_inserter(*full_table));

        return full_table;
      }


    std::unique_ptr<GiNaC::matrix>
    read_inverse_metric(boost::filesystem::path p, const GiNaC::symtab& subst_table, size_t N)
      {
        // allocate space for inverse metric
        auto Ginv = std::make_unique<GiNaC::matrix>(N, N);

        // open text file containing pre-computed inverse metric
        std::ifstream in(p.string(), std::ios::in);

        for(size_t i = 0; i < N; ++i)
          {
            for(size_t j = 0; j < N; ++j)
              {
                Ginv->set(i, j, detail::read_expr(in, subst_table, std::initializer_list<size_t>{i, j}));
              }
          }

        // return result matrix
        return Ginv;
      }

  }   //  namespace reader
