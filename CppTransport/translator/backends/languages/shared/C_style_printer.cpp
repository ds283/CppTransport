//
// Created by David Seery on 09/12/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
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


#include <sstream>

#include "C_style_printer.h"

#include "to_printable.h"


std::string C_style_printer::ginac(const GiNaC::ex& expr) const
  {
    std::ostringstream out;

    out << GiNaC::csrc << expr;

    return(out.str());
  }


std::string C_style_printer::comment(const std::string tag) const
  {
    std::ostringstream stmt;
    stmt << this->comment_prefix << this->comment_pad << tag << this->comment_pad << this->comment_postfix;
    return(stmt.str());
  }


boost::optional<std::string> C_style_printer::get_start_block_delimiter() const
  {
    return this->open_brace;
  }


boost::optional<std::string> C_style_printer::get_end_block_delimiter() const
  {
    return this->close_brace;
  }


unsigned int C_style_printer::get_block_delimiter_indent() const
  {
    return this->brace_indent;
  }


unsigned int C_style_printer::get_block_indent() const
  {
    return this->block_indent;
  }


std::string C_style_printer::for_loop(const std::string& loop_variable, unsigned int min, unsigned int max) const
  {
    std::ostringstream stmt;
    stmt << this->for_keyword << "(" << this->loop_type << " " << loop_variable << " = " << min << "; " << loop_variable << " < " << max << "; ++" << loop_variable << ")";
    return(stmt.str());
  }


template <typename ItemType>
std::string C_style_printer::make_array_subscript(const std::string& kernel, const ItemType& a, unsigned int offset) const
  {
    std::ostringstream stmt;
    stmt << kernel << this->array_open << a;
    if(offset != 0) stmt << "+" << offset;
    stmt << this->array_close;
    return(stmt.str());
  }


template <typename ItemType>
std::string C_style_printer::make_array_subscript(const std::string& kernel, const ItemType& a, const std::string& flatten, unsigned int offset) const
  {
    std::ostringstream stmt;
    stmt << kernel << this->array_open << flatten << this->function_open << a;
    if(offset != 0) stmt << "+" << offset;
    stmt << this->function_close << this->array_close;
    return(stmt.str());
  }


template <typename ItemType>
std::string C_style_printer::make_array_subscript(const std::string& kernel, const ItemType& a, const ItemType& b, const std::string& flatten) const
  {
    std::ostringstream stmt;
    stmt << kernel << this->array_open << flatten << this->function_open << a << "," << b << this->function_close << this->array_close;
    return(stmt.str());
  }


template <typename ItemType>
std::string C_style_printer::make_array_subscript(const std::string& kernel, const ItemType& a, const ItemType& b, const ItemType& c, const std::string& flatten) const
  {
    std::ostringstream stmt;
    stmt << kernel << this->array_open << flatten << this->function_open << a << "," << b << "," << c << this->function_close << this->array_close;
    return(stmt.str());
  }


std::string C_style_printer::array_subscript(const std::string& kernel, unsigned int a, unsigned int offset) const
  {
    return this->make_array_subscript(kernel, a, offset);
  }


std::string C_style_printer::array_subscript(const std::string& kernel, unsigned int a, const std::string& flatten, unsigned int offset) const
  {
    return this->make_array_subscript(kernel, a, flatten, offset);
  }


std::string C_style_printer::array_subscript(const std::string& kernel, unsigned int a, unsigned int b, const std::string& flatten) const
  {
    return this->make_array_subscript(kernel, a, b, flatten);
  }


std::string C_style_printer::array_subscript(const std::string& kernel, unsigned int a, unsigned int b, unsigned int c, const std::string& flatten) const
  {
    return this->make_array_subscript(kernel, a, b, c, flatten);
  }


std::string C_style_printer::array_subscript(const std::string& kernel, const abstract_index& a, unsigned int offset) const
  {
    return this->make_array_subscript(kernel, a.get_loop_variable(), offset);
  }


std::string C_style_printer::array_subscript(const std::string& kernel, const abstract_index& a, const std::string& flatten, unsigned int offset) const
  {
    return this->make_array_subscript(kernel, a.get_loop_variable(), flatten, offset);
  }


std::string C_style_printer::array_subscript(const std::string& kernel, const abstract_index& a, const abstract_index& b, const std::string& flatten) const
  {
    return this->make_array_subscript(kernel, a.get_loop_variable(), b.get_loop_variable(), flatten);
  }


std::string C_style_printer::array_subscript(const std::string& kernel, const abstract_index& a, const abstract_index& b, const abstract_index& c, const std::string& flatten) const
  {
    return this->make_array_subscript(kernel, a.get_loop_variable(), b.get_loop_variable(), c.get_loop_variable(), flatten);
  }


std::string C_style_printer::initialization_list(const std::vector<std::string>& list, bool quote) const
  {
    std::ostringstream stmt;
    stmt << this->initializer_open << " ";

    unsigned int count = 0;
    for(const std::string& ele : list)
      {
        if(count > 0) stmt << this->initializer_sep << " ";
        stmt << to_printable(ele, quote);
        ++count;
      }

    stmt << " " << this->initializer_close;
    return(stmt.str());
  }


std::string C_style_printer::lambda_invokation(const std::string& name, const generic_lambda& lambda,
                                               const abstract_index_database& indices) const
  {
    std::ostringstream stmt;
    stmt << name << this->function_open;

    unsigned int count = 0;
    for(const abstract_index& idx : indices)
      {
        if(count > 0) stmt << this->argument_sep;
        stmt << idx.get_loop_variable();
        ++count;
      }

    stmt << this->function_close;

    return(stmt.str());
  }


std::string C_style_printer::open_lambda(const abstract_index_database& indices, const std::string& return_type) const
  {
    std::ostringstream stmt;

    stmt << this->lambda_capture << this->function_open;

    unsigned int count = 0;
    for(const abstract_index& idx : indices)
      {
        if(count > 0) stmt << this->argument_sep << " ";
        stmt << this->lambda_argument_type << " " << idx.get_loop_variable();
        ++count;
      }

    stmt << this->function_close << " " << this->lambda_return << " " << return_type << " " << this->open_brace;

    return(stmt.str());
  }


std::string C_style_printer::close_lambda() const
  {
    std::ostringstream stmt;

    stmt << " " << this->close_brace;

    return(stmt.str());
  }


std::string C_style_printer::format_return(const GiNaC::ex& expr) const
  {
    std::ostringstream stmt;

    stmt << this->return_keyword << " " << this->ginac(expr) << this->semicolon;

    return(stmt.str());
  }


std::string C_style_printer::format_return(const std::string& expr) const
  {
    std::ostringstream stmt;

    stmt << this->return_keyword << " " << expr << this->semicolon;

    return(stmt.str());
  }


std::string C_style_printer::format_if(const std::list<GiNaC::ex>& conditions) const
  {
    return this->format_if_stmt(this->if_keyword, conditions);
  }


std::string C_style_printer::format_elseif(const std::list<GiNaC::ex>& conditions) const
  {
    std::ostringstream else_if;
    else_if << this->else_keyword << " " << this->if_keyword;

    return this->format_if_stmt(else_if.str(), conditions);
  }


std::string C_style_printer::format_else(const GiNaC::ex& expr) const
  {
    std::ostringstream stmt;

    stmt << this->else_keyword << " " << this->format_return(expr);

    return(stmt.str());
  }


std::string C_style_printer::format_if_stmt(const std::string& kw, const std::list<GiNaC::ex>& conditions) const
  {
    std::ostringstream stmt;

    stmt << kw << "(";

    unsigned int count = 0;
    for(const GiNaC::ex& expr : conditions)
      {
        if(count > 0) stmt << " && ";
        stmt << expr;
        ++count;
      }

    stmt << ")";

    return stmt.str();
  }
