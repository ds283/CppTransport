//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "cse.h"
#include "cpp_cse.h"


namespace cpp
  {

    std::string cpp_cse::print(const GiNaC::ex& expr)
      {
        std::ostringstream out;
        std::string        name;

        if(GiNaC::is_a<GiNaC::function>(expr)) name = GiNaC::ex_to<GiNaC::function>(expr).get_name();
        else                                   name = GiNaC::ex_to<GiNaC::basic>(expr).class_name();

        if     (name == "numeric") out << this->printer.ginac(expr);
        else if(name == "symbol")  out << this->printer.ginac(expr);
        else if(name == "add")     out << this->print_operands(expr, "+");
        else if(name == "mul")     out << this->print_operands(expr, "*");
        else if(name == "power")   out << "pow(" << this->print_operands(expr, ",") << ")";
        else                       out << name << "(" << this->print_operands(expr, ",") << ")";

        return(out.str());
      }


    std::string cpp_cse::print_operands(const GiNaC::ex& expr, std::string op)
      {
        std::ostringstream out;
        size_t             n = expr.nops();

        if(n > 0)
          {
            for(size_t i = 0; i < n; i++)
              {
                out << (i > 0 ? op : "") << this->symbol(expr.op(i));
              }
          }
        else
          {
            out << this->printer.ginac(expr);
          }

        return(out.str());
      }

  } // namespace cpp
