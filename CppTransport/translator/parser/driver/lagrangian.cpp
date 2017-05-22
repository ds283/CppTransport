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


#include "lagrangian.h"


namespace y
  {
    
    lagrangian::lagrangian(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    void lagrangian::add_field(lexeme_type* lex, attributes* a)
      {
        // extract identifier name from lexeme
        boost::optional<std::string> id = lex->get_identifier();
        
        if(id)
          {
            GiNaC::symbol sym = this->sym_factory.get_symbol(*id, a->get_latex());
            this->root.add_field(*id, sym, *lex, a);
          }
        else
          {
            lex->error(ERROR_IDENTIFIER_LOOKUP);
          }
        
        delete a; // otherwise, attributes block would never be deallocated
      }
    
    
    void lagrangian::add_parameter(lexeme_type* lex, attributes* a)
      {
        // extract identifier name from lexeme
        boost::optional<std::string> id = lex->get_identifier();
        
        if(id)
          {
            GiNaC::symbol sym = this->sym_factory.get_symbol(*id, a->get_latex());
            this->root.add_parameter(*id, sym, *lex, a);
          }
        else
          {
            lex->error(ERROR_IDENTIFIER_LOOKUP);
          }
        
        delete a; // otherwise, attributes block would never be deallocated
      }
    
    
    void lagrangian::add_subexpr(lexeme_type* lex, subexpr* e)
      {
        // extract identifier name from lexeme
        boost::optional<std::string> id = lex->get_identifier();
        
        subexpr_declaration* d = nullptr;
        
        if(id)
          {
            GiNaC::symbol sym = this->sym_factory.get_symbol(*id, e->get_latex());
            this->root.add_subexpr(*id, sym, *lex, e);
          }
        else
          {
            lex->error(ERROR_IDENTIFIER_LOOKUP);
          }
        
        delete e;
      }
    
    
    void lagrangian::add_latex_attribute(subexpr* e, lexeme_type* lex)
      {
        // extract string name from lexeme
        boost::optional<std::string> latex_name = lex->get_string();
        
        if(latex_name)
          {
            e->set_latex(*latex_name, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void lagrangian::add_value_attribute(subexpr* e, GiNaC::ex* v, lexeme_type* lex)
      {
        e->set_value(*v, *lex);
      }
    
    
    void lagrangian::set_potential(GiNaC::ex* V, lexeme_type* lex)
      {
        this->root.set_potential(*V, *lex);
        
        delete V;
      }
    
  }   // namespace y
