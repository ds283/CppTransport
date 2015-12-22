//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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


std::string C_style_printer::initialization_list(const std::vector<std::string>& list) const
  {
    std::ostringstream stmt;
    stmt << this->initializer_open << " ";

    unsigned int count = 0;
    for(const std::string& ele : list)
      {
        if(count > 0) stmt << this->initializer_sep << " ";
        stmt << to_printable(ele);
        ++count;
      }

    stmt << " " << this->initializer_close;
    return(stmt.str());
  }
