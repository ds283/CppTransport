//
// Created by David Seery on 22/05/2017.
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


#include "lagrangian_driver.h"
#include "generics_driver.h"
#include "parse_error.h"


namespace y
  {
    
    lagrangian_driver::lagrangian_driver(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    void lagrangian_driver::add_field(lexeme_type& lex, std::shared_ptr<attributes> a)
      {
        try
          {
            auto SymbolFactory = [&](auto& name, auto& latex) -> auto
              {
                return this->sym_factory.get_real_symbol(name, latex);
              };

            auto InsertSymbol = [&](auto& name, auto& symbol, auto& lexeme, auto& attrs) -> auto
              {
                return this->root.model.add_field(name, symbol, lexeme, attrs);
              };
    
            GenericInsertSymbol(SymbolFactory, InsertSymbol, lex, a);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void lagrangian_driver::add_parameter(lexeme_type& lex, std::shared_ptr<attributes> a)
      {
        try
          {
            auto SymbolFactory = [&](auto& name, auto& latex) -> auto
              {
                return this->sym_factory.get_real_symbol(name, latex);
              };

            auto InsertSymbol = [&](auto& name, auto& symbol, auto& lexeme, auto& attrs) -> auto
              {
                return this->root.model.add_parameter(name, symbol, lexeme, attrs);
              };
    
            GenericInsertSymbol(SymbolFactory, InsertSymbol, lex, a);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void lagrangian_driver::add_subexpr(lexeme_type& lex, std::shared_ptr<subexpr> e)
      {
        try
          {
            auto SymbolFactory = [&](auto& name, auto& latex) -> auto
              {
                return this->sym_factory.get_real_symbol(name, latex);
              };

            auto InsertSymbol = [&](auto& name, auto& symbol, auto& lexeme, auto& expr) -> auto
              {
                return this->root.model.add_subexpr(name, symbol, lexeme, expr);
              };
    
            GenericInsertSymbol(SymbolFactory, InsertSymbol, lex, e);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void lagrangian_driver::set_subexpr_latex(subexpr& e, lexeme_type& lex)
      {
        try
          {
            auto Setter = [&](auto& name, auto& lex) -> auto { return e.set_latex(name, lex); };
            SetStringValue(Setter, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void lagrangian_driver::set_subexpr_value(subexpr& e, GiNaC::ex& v, lexeme_type& lex)
      {
        try
          {
            e.set_value(v,lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void
    lagrangian_driver::add_metric_component(field_metric_base& metric, lexeme_type& i, lexeme_type& j, GiNaC::ex& e,
                                            lexeme_type& context)
      {
        try
          {
            // capture i, j coordinates
            std::string i_val;
            std::string j_val;
            auto Set_i = [&](auto& name, auto& lex) -> auto { i_val = name; return true; };
            auto Set_j = [&](auto& name, auto& lex) -> auto { j_val = name; return true; };
            SetIdentifierValue(Set_i, i);
            SetIdentifierValue(Set_j, j);

            metric.set_component(std::make_pair(i_val, j_val), std::make_pair(std::ref(i), std::ref(j)), e, context);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void lagrangian_driver::set_potential(lexeme_type& lex, std::shared_ptr<GiNaC::ex> V)
      {
        try
          {
            this->root.model.set_potential(lex, V);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void lagrangian_driver::set_metric(lexeme_type& lex, std::shared_ptr<field_metric> f)
      {
        try
          {
            this->root.model.set_metric(lex, f);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void lagrangian_driver::set_lagrangian_type(model_type t, lexeme_type& lex)
      {
        try
          {
            this->root.model.set_lagrangian_type(t, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }


    std::shared_ptr<field_metric_base> lagrangian_driver::make_field_metric_base() const
      {
        return this->root.model.make_field_metric_base();
      }

  }   // namespace y
