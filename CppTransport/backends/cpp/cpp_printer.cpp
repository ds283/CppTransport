//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "cpp_printer.h"


namespace cpp
  {

    static std::string print(const GiNaC::ex& expr)
      {
        std::ostringstream out;
        std::string        name;

        if(GiNaC::is_a<GiNaC::function>(expr)) name = GiNaC::ex_to<GiNaC::function>(expr).get_name();
        else                                   name = GiNaC::ex_to<GiNaC::basic>(expr).class_name();

        if     (name == "numeric") out << expr;
        else if(name == "symbol")  out << expr;
        else if(name == "add")     out << this->print_operands(expr, "+");
        else if(name == "mul")     out << this->print_operands(expr, "*");
        else if(name == "power")   out << "pow(" << this->print_operands(expr, ",") << ")";
        else                       out << name << "(" << this->print_operands(expr, ",") << ")";

        return(out.str());
      }

  } // namespace cpp
