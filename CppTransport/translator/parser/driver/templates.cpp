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
#include "generics.h"


namespace y
  {
    
    templates::templates(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    void templates::set_core(lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return this->root.set_core(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void templates::set_implementation(lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return this->root.set_implementation(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void templates::set_abserr(struct stepper& s, lexeme_type& lex)
      {
        auto Setter = [&](auto err, auto& lex) -> auto { return s.set_abserr(std::abs(err), lex); };
        SetDecimalValue(Setter, lex);
      }
    
    
    void templates::set_relerr(struct stepper& s, lexeme_type& lex)
      {
        auto Setter = [&](auto err, auto& lex) -> auto { return s.set_relerr(std::abs(err), lex); };
        SetDecimalValue(Setter, lex);
      }
    
    
    void templates::set_stepper(struct stepper& s, lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return s.set_name(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void templates::set_stepsize(struct stepper& s, lexeme_type&lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return s.set_stepsize(name, lex); };
        SetDecimalValue(Setter, lex);
      }
    
    
    void templates::set_background_stepper(stepper& s, lexeme_type& lex)
      {
        this->root.set_background_stepper(s, lex);
      }
    
    
    void templates::set_perturbations_stepper(stepper& s, lexeme_type& lex)
      {
        this->root.set_perturbations_stepper(s, lex);
      }
    
  }   // namespace y
