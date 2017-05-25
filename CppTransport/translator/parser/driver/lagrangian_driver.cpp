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


namespace y
  {
    
    lagrangian_driver::lagrangian_driver(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    void lagrangian_driver::add_field(lexeme_type& lex, attributes& a)
      {
        auto SymbolFactory = [&](auto& name, auto& latex) -> auto { return this->sym_factory.get_symbol(name, latex); };
        auto InsertSymbol = [&](auto& name, auto& symbol, auto& lexeme, auto& attrs) -> auto { return this->root.add_field(name, symbol, lexeme, attrs); };
        
        GenericInsertSymbol(SymbolFactory, InsertSymbol, lex, a);
      }
    
    
    void lagrangian_driver::add_parameter(lexeme_type& lex, attributes& a)
      {
        auto SymbolFactory = [&](auto& name, auto& latex) -> auto { return this->sym_factory.get_symbol(name, latex); };
        auto InsertSymbol = [&](auto& name, auto& symbol, auto& lexeme, auto& attrs) -> auto { return this->root.add_parameter(name, symbol, lexeme, attrs); };
    
        GenericInsertSymbol(SymbolFactory, InsertSymbol, lex, a);
      }
    
    
    void lagrangian_driver::add_subexpr(lexeme_type& lex, subexpr& e)
      {
        auto SymbolFactory = [&](auto& name, auto& latex) -> auto { return this->sym_factory.get_symbol(name, latex); };
        auto InsertSymbol = [&](auto& name, auto& symbol, auto& lexeme, auto& expr) -> auto { return this->root.add_subexpr(name, symbol, lexeme, expr); };
    
        GenericInsertSymbol(SymbolFactory, InsertSymbol, lex, e);
      }
    
    
    void lagrangian_driver::set_subexpr_latex(subexpr& e, lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return e.set_latex(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void lagrangian_driver::set_subexpr_value(subexpr& e, GiNaC::ex& v, lexeme_type& lex)
      {
        e.set_value(v,lex);
      }
    
    
    void lagrangian_driver::set_potential(GiNaC::ex& V, lexeme_type& lex)
      {
        this->root.set_potential(V, lex);
      }
    
  }   // namespace y
