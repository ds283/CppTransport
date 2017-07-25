//
// Created by David Seery on 26/05/2017.
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


#include "declarations.h"


// ******************************************************************


declaration::declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l)
  : name(n),
    symbol(s),
    declaration_point(l),
    my_id(current_id++)
  {
  }

// initialize static member
unsigned int declaration::current_id = 0;


// ******************************************************************


field_declaration::field_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l,
                                     std::shared_ptr<attributes> a)
  : declaration(n, s, l),
    attrs(a)
  {
  }


std::string field_declaration::get_latex_name() const
  {
    std::string latex_name = this->attrs->get_latex();

    if(latex_name.length() == 0) latex_name = this->name;

    return(latex_name);
  }


void field_declaration::print(std::ostream& stream) const
  {
    stream << "Field declaration for symbol '" << this->get_name()
           << "', GiNaC symbol '" << this->get_ginac_symbol() << "'" << '\n';
  }


// ******************************************************************


parameter_declaration::parameter_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l,
                                             std::shared_ptr<attributes> a)
  : declaration(n, s, l),
    attrs(a)
  {
  }


std::string parameter_declaration::get_latex_name() const
  {
    std::string latex_name = this->attrs->get_latex();

    if(latex_name.length() == 0) latex_name = this->name;

    return(latex_name);
  }


void parameter_declaration::print(std::ostream& stream) const
  {
    stream << "Parameter declaration for symbol '" << this->get_name()
           << "', GiNaC symbol '" << this->get_ginac_symbol() << "'" << '\n';
  }


// ******************************************************************


subexpr_declaration::subexpr_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l,
                                         std::shared_ptr<subexpr> e)
  : declaration(n, s, l),
    sexpr(e)
  {
  }


std::string subexpr_declaration::get_latex_name() const
  {
    std::string latex_name = this->sexpr->get_latex();

    if(latex_name.length() == 0) latex_name = this->name;

    return(latex_name);
  }


GiNaC::ex subexpr_declaration::get_value() const
  {
    return this->sexpr->get_value();
  }


void subexpr_declaration::print(std::ostream& stream) const
  {
    stream << "Subexpression declaration for symbol '" << this->get_name()
           << "', GiNaC symbol '" << this->get_ginac_symbol() << "'" << '\n';
  }
