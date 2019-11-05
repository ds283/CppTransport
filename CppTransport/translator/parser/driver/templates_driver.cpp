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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#include "templates_driver.h"
#include "generics_driver.h"
#include "parse_error.h"


namespace y
  {
    
    templates_driver::templates_driver(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    void templates_driver::set_model(lexeme_type& lex)
      {
        try
          {
            auto Setter = [&](auto& name, auto& lex) -> auto { return this->root.templates.set_model(name, lex); };
            SetStringValue(Setter, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void templates_driver::set_core(lexeme_type& lex)
      {
        try
          {
            auto Setter = [&](auto& name, auto& lex) -> auto { return this->root.templates.set_core(name, lex); };
            SetStringValue(Setter, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void templates_driver::set_implementation(lexeme_type& lex)
      {
        try
          {
            auto Setter = [&](auto& name, auto& lex) -> auto { return this->root.templates.set_implementation(name, lex); };
            SetStringValue(Setter, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }

    void templates_driver::set_sampling(lexeme_type& lex)
    {
      try
      {
        auto Setter = [&](auto& sample, auto& lex) -> auto { return this->root.templates.set_sampling(sample, lex); };
        SetStringValue(Setter, lex);
      }
      catch(parse_error& xe)
      {
        this->root.report_error();
      }
    }

    void templates_driver::set_sampling_template(lexeme_type& lex)
    {
      try
      {
        auto Setter = [&](auto& sample, auto& lex) -> auto { return this->root.templates.set_sampling_template(sample, lex); };
        SetStringValue(Setter, lex);
      }
      catch(parse_error& xe)
      {
        this->root.report_error();
      }
    }
    
    
    void templates_driver::set_abserr(struct stepper& s, lexeme_type& lex)
      {
        try
          {
            auto Setter = [&](auto err, auto& lex) -> auto { return s.set_abserr(std::abs(err), lex); };
            SetDecimalValue(Setter, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void templates_driver::set_relerr(struct stepper& s, lexeme_type& lex)
      {
        try
          {
            auto Setter = [&](auto err, auto& lex) -> auto { return s.set_relerr(std::abs(err), lex); };
            SetDecimalValue(Setter, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void templates_driver::set_stepper(struct stepper& s, lexeme_type& lex)
      {
        try
          {
            auto Setter = [&](auto& name, auto& lex) -> auto { return s.set_name(name, lex); };
            SetStringValue(Setter, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void templates_driver::set_stepsize(struct stepper& s, lexeme_type&lex)
      {
        try
          {
            auto Setter = [&](auto& name, auto& lex) -> auto { return s.set_stepsize(name, lex); };
            SetDecimalValue(Setter, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void templates_driver::set_background_stepper(lexeme_type& lex, std::shared_ptr<stepper> s)
      {
        try
          {
            this->root.templates.set_background_stepper(lex, s);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void templates_driver::set_perturbations_stepper(lexeme_type& lex, std::shared_ptr<stepper> s)
      {
        try
          {
            this->root.templates.set_perturbations_stepper(lex, s);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
  }   // namespace y
