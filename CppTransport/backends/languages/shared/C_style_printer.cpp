//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <sstream>

#include "C_style_printer.h"


std::string C_style_printer::ginac(const GiNaC::ex& expr) const
  {
    std::ostringstream out;

    out << GiNaC::csrc << expr;

    return(out.str());
  }


std::string C_style_printer::comment(const std::string tag) const
  {
    return(this->comment_prefix + this->comment_pad + tag + this->comment_pad + this->comment_postfix);
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


std::string C_style_printer::plant_for_loop(const std::string& loop_variable, unsigned int min, unsigned int max) const
  {
    std::ostringstream stmt;
    stmt << this->for_keyword << "(" << this->loop_type << " " << loop_variable << " = " << min << "; " << loop_variable << " < " << max << "; ++" << loop_variable << ")";
    return(stmt.str());
  }
