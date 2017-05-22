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

#ifndef CPPTRANSPORT_GENERICS_H
#define CPPTRANSPORT_GENERICS_H


#include "semantic_data.h"
#include "model_descriptor.h"
#include "symbol_factory.h"
#include "argument_cache.h"
#include "local_environment.h"


namespace y
  {
    
    template <typename SymbolFactory, typename InsertSymbol, typename AttributeBlock>
    void GenericInsertSymbol(SymbolFactory& sym_factory, InsertSymbol& insert, lexeme_type* lex, AttributeBlock* a)
      {
        // extract identifier name from lexeme
        boost::optional<std::string> id = lex->get_identifier();
        
        if(id) // lex points to a valid identifier
          {
            GiNaC::symbol sym = sym_factory(*id, a->get_latex());
            insert(*id, sym, *lex, a);
          }
        else
          {
            lex->error(ERROR_IDENTIFIER_LOOKUP);
          }
        
        delete a; // release semantic value inherited from Bison, otherwise this would never get done
      };
    
    
    template <typename ValueSetter>
    bool SetStringValue(ValueSetter& set, lexeme_type* lex)
      {
        // extract string name from lexeme
        boost::optional<std::string> str = lex->get_string();
        
        if(!str)
          {
            lex->error(ERROR_STRING_LOOKUP);
            return false;
          }

        return set(*str, *lex);
      };
    
    
    template <typename ValueSetter>
    bool SetIntegerValue(ValueSetter& set, lexeme_type* lex)
      {
        // extract string name from lexeme
        boost::optional<int> i = lex->get_integer();
        
        if(!i)
          {
            lex->error(ERROR_INTEGER_LOOKUP);
            return false;
          }
        
        return set(*i, *lex);
      };
    
    
    template <typename ValueSetter>
    bool SetDecimalValue(ValueSetter& set, lexeme_type* lex)
      {
        // extract string name from lexeme
        boost::optional<double> d = lex->get_decimal();
        
        if(!d)
          {
            lex->error(ERROR_DECIMAL_LOOKUP);
            return false;
          }

        return set(*d, *lex);
      };

  }   // namespace y



#endif //CPPTRANSPORT_GENERICS_H
