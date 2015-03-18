//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <sstream>

#include "C_style_printer.h"


std::string C_style_printer::ginac(const GiNaC::ex& expr)
  {
    std::ostringstream out;

    out << GiNaC::csrc << expr;

    return(out.str());
  }


std::string C_style_printer::comment(const std::string tag)
  {
    return(this->comment_prefix + this->comment_pad + tag + this->comment_pad + this->comment_postfix);
  }
