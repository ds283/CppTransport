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


#include "utilities.h"
#include "generics.h"


namespace y
  {
    
    utilities::utilities(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    void utilities::add_latex_attribute(attributes* a, lexeme_type* lex)
      {
        auto Setter = [&](auto name, auto lex) -> auto { return a->set_latex(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void utilities::add_string(string_array* a, lexeme_type* lex)
      {
        auto Setter = [&](auto name, auto lex) -> auto { a->push_element(name, lex); return true; };
        SetStringValue(Setter, lex);
      }
    
    
    void utilities::set_indexorder_left()
      {
        this->root.set_indexorder(index_order::left);
      }
    
    
    void utilities::set_indexorder_right()
      {
        this->root.set_indexorder(index_order::right);
      }
    
  }   // namespace y
