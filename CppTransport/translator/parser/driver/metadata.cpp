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


#include "metadata.h"


namespace y
  {
    
    metadata::metadata(script& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    void metadata::set_model(lexeme_type* lex)
      {
        boost::optional<std::string> str = lex->get_string();
        
        if(str)
          {
            this->root.set_model(*str, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void metadata::add_email(author* a, lexeme_type* lex)
      {
        // extract string from lexeme
        boost::optional<std::string> value = lex->get_string();
        
        if(value)
          {
            a->set_email(*value, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void metadata::add_institute(author* a, lexeme_type* lex)
      {
        // extract string from lexeme
        boost::optional<std::string> value = lex->get_string();
        
        if(value)
          {
            a->set_institute(*value, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void metadata::set_name(lexeme_type* lex)
      {
        boost::optional<std::string> str = lex->get_string();
        
        if(str)
          {
            this->root.set_name(*str, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void metadata::add_author(lexeme_type* lex, author* a)
      {
        boost::optional<std::string> str = lex->get_string();
        
        if(str)
          {
            a->set_name(*str, *lex);
            this->root.add_author(*str, *lex, a);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void metadata::set_citeguide(lexeme_type* lex)
      {
        boost::optional<std::string> str = lex->get_string();
        
        if(str)
          {
            this->root.set_citeguide(*str, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void metadata::set_description(lexeme_type* lex)
      {
        boost::optional<std::string> str = lex->get_string();
        
        if(str)
          {
            this->root.set_description(*str, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void metadata::set_revision(lexeme_type* lex)
      {
        boost::optional<int> d = lex->get_integer();
        
        if(d)
          {
            this->root.set_revision(*d, *lex);
          }
        else
          {
            lex->error(ERROR_INTEGER_LOOKUP);
          }
      }
    
    
    void metadata::set_license(lexeme_type* lex)
      {
        boost::optional<std::string> str = lex->get_string();
        
        if(str)
          {
            this->root.set_license(*str, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void metadata::set_references(string_array* a)
      {
        this->root.set_references(a->get_array());
      }
    
    
    void metadata::set_urls(string_array* a)
      {
        this->root.set_urls(a->get_array());
      }
    
  }   // namespace y
