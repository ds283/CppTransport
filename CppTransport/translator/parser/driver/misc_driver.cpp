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


#include "misc_driver.h"
#include "generics_driver.h"
#include "parse_error.h"


namespace y
  {
    
    misc_driver::misc_driver(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    void misc_driver::set_attribute_latex(attributes& a, lexeme_type& lex)
      {
        try
          {
            auto Setter = [&](auto& name, auto& lex) -> auto { return a.set_latex(name, lex); };
            SetStringValue(Setter, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }

    // Four functions that allow us to set priors & values on fields + parameters in the model file for use
    // with writing to the CosmoSIS .ini files
    void misc_driver::set_attribute_value(attributes& a, lexeme_type& lex)
    {
      try
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return a.set_value(name, lex); };
        SetStringValue(Setter, lex);
      }
      catch(parse_error& xe)
      {
        this->root.report_error();
      }
    }

    void misc_driver::set_attribute_derivvalue(attributes& a, lexeme_type& lex)
    {
      try
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return a.set_derivvalue(name, lex); };
        SetStringValue(Setter, lex);
      }
      catch(parse_error& xe)
      {
        this->root.report_error();
      }
    }

    void misc_driver::set_attribute_prior(attributes& a, lexeme_type& lex)
    {
      try
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return a.set_prior(name, lex); };
        SetStringValue(Setter, lex);
      }
      catch(parse_error& xe)
      {
        this->root.report_error();
      }
    }

    void misc_driver::set_attribute_derivprior(attributes& a, lexeme_type& lex)
    {
      try
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return a.set_derivprior(name, lex); };
        SetStringValue(Setter, lex);
      }
      catch(parse_error& xe)
      {
        this->root.report_error();
      }
    }

    void misc_driver::set_attribute_log_value(attributes &a, lexeme_type &lex)
    {
      try
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return a.set_log_values(name, lex); };
        SetStringValue(Setter, lex);
      }
      catch(parse_error& xe)
      {
        this->root.report_error();
      }
    }

    
    void misc_driver::add_string(string_array& a, lexeme_type& lex)
      {
        try
          {
            auto Setter = [&](auto& name, auto& lex) -> auto { a.push_element(name, lex); return true; };
            SetStringValue(Setter, lex);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void misc_driver::set_indexorder_left()
      {
        try
          {
            this->root.misc.set_indexorder(index_order::left);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void misc_driver::set_indexorder_right()
      {
        try
          {
            this->root.misc.set_indexorder(index_order::right);
          }
        catch(parse_error& xe)
          {
            this->root.report_error();
          }
      }
    
    
    void misc_driver::set_required_version(lexeme_type& lex)
      {
        auto Setter = [&](auto& ver, auto& lex) -> auto { return this->root.misc.set_required_version(ver, lex); };
        SetIntegerValue(Setter, lex);
      }
    
  }   // namespace y
