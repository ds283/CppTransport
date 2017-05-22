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


#include "templates.h"


namespace y
  {
    
    templates::templates(script& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    void templates::set_core(lexeme_type* lex)
      {
        boost::optional<std::string> str = lex->get_string();
        
        if(str)
          {
            this->root.set_core(*str, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void templates::set_implementation(lexeme_type* lex)
      {
        boost::optional<std::string> str = lex->get_string();
        
        if(str)
          {
            this->root.set_implementation(*str, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void templates::set_abserr(struct stepper* s, lexeme_type* lex)
      {
        // extract decimal value from lexeme
        boost::optional<double> d = lex->get_decimal();
        
        if(d)
          {
            s->set_abserr(std::abs(*d), *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void templates::set_relerr(struct stepper* s, lexeme_type* lex)
      {
        // extract decimal value from lexeme
        boost::optional<double> d = lex->get_decimal();
        
        if(d)
          {
            s->set_relerr(std::abs(*d), *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void templates::set_stepper(struct stepper* s, lexeme_type* lex)
      {
        // extract string name from lexeme
        boost::optional<std::string> stepper_name = lex->get_string();
        
        if(stepper_name)
          {
            s->set_name(*stepper_name, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void templates::set_stepsize(struct stepper* s, lexeme_type* lex)
      {
        // extract decimal value from lexeme
        boost::optional<double> d = lex->get_decimal();
        
        if(d)
          {
            s->set_stepsize(*d, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }
    
    
    void templates::set_background_stepper(stepper* s, lexeme_type* lex)
      {
        this->root.set_background_stepper(s, *lex);
        
        delete s;
      }
    
    
    void templates::set_perturbations_stepper(stepper* s, lexeme_type* lex)
      {
        this->root.set_perturbations_stepper(s, *lex);
        
        delete s;
      }
    
  }   // namespace y
