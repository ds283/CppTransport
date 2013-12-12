//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "cse.h"
#include "cpp_cse.h"
#include "error.h"
#include "msg_en.h"


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
        else if(name == "power")   out << this->print_power(expr);
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


    // treat powers specially, because it's better to unroll them and
    // avoid a function call if we have an exact integral power
    std::string cpp_cse::print_power(const GiNaC::ex& expr)
      {
        std::ostringstream out;
        size_t             n = expr.nops();

        if(n != 2)
          {
            error(ERROR_CSE_POWER_ARGUMENTS);
            out << "pow(" << this->print_operands(expr, ",") << ")";
          }
        else
          {
            const GiNaC::ex& exp_generic = expr.op(1);

            if(GiNaC::is_a<GiNaC::numeric>(exp_generic))
              {
                const GiNaC::numeric& exp_numeric = GiNaC::ex_to<GiNaC::numeric>(exp_generic);

                if(GiNaC::is_integer(exp_numeric))
                  {
                    if(GiNaC::is_nonneg_integer(exp_numeric))
                      {
                        if      (exp_numeric.to_int() == 0) out << "1.0";
                        else if (exp_numeric.to_int() == 1) out << this->symbol(expr.op(0));
                        else if (exp_numeric.to_int() == 2) out << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0));
                        else if (exp_numeric.to_int() == 3) out << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0));
                        else if (exp_numeric.to_int() == 4) out << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0));
                        else                                out << "pow(" << this->symbol(expr.op(0)) << "," << exp_numeric.to_int() << ")";
                      }
                    else  // negative integer
                      {
                        out << "1.0/";
                        if      (exp_numeric.to_int() == -0) out << "1.0";
                        else if (exp_numeric.to_int() == -1) out << this->symbol(expr.op(0));
                        else if (exp_numeric.to_int() == -2) out << "(" << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0)) << ")";
                        else if (exp_numeric.to_int() == -3) out << "(" << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0)) << ")";
                        else if (exp_numeric.to_int() == -4) out << "(" << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0)) << "*" << this->symbol(expr.op(0)) << ")";
                        else                                 out << "pow(" << this->symbol(expr.op(0)) << "," << -exp_numeric.to_int() << ")";
                      }
                  }
                else  // not an integer
                  {
                    out << "pow(" << this->symbol(expr.op(0)) << "," << this->symbol(expr.op(1)) << ")";
                  }
              }
          }

        return(out.str());
      }

  } // namespace cpp
