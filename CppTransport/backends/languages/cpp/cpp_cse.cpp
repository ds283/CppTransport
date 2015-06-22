//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "cse.h"
#include "cpp_cse.h"
#include "error.h"
#include "msg_en.h"


namespace cpp
  {

    std::string cpp_cse::print(const GiNaC::ex& expr, symbol_f symf)
      {
        std::ostringstream out;
        std::string        name;

        if(GiNaC::is_a<GiNaC::function>(expr)) name = GiNaC::ex_to<GiNaC::function>(expr).get_name();
        else                                   name = GiNaC::ex_to<GiNaC::basic>(expr).class_name();

        if     (name == "numeric") out << this->printer.ginac(expr);
        else if(name == "symbol")  out << this->printer.ginac(expr);
        else if(name == "add")     out << this->print_operands(expr, "+", symf);
        else if(name == "mul")     out << this->print_operands(expr, "*", symf);
        else if(name == "power")   out << this->print_power(expr, symf);
        else                       out << this->printer.ginac(expr);

        return(out.str());
      }


    std::string cpp_cse::print_operands(const GiNaC::ex& expr, std::string op, symbol_f symf)
      {
        std::ostringstream out;

        unsigned int c = 0;
        for(GiNaC::const_iterator t = expr.begin(); t != expr.end(); ++t)
          {
            if(c > 0) out << op;
            out << symf(*t);
            c++;
          }

        return(out.str());
      }


    // treat powers specially, because it's better to unroll them and
    // avoid a function call if we have an exact integral power
    std::string cpp_cse::print_power(const GiNaC::ex& expr, symbol_f symf)
      {
        std::ostringstream out;
        size_t             n = expr.nops();

        if(n != 2)
          {
            error(ERROR_CSE_POWER_ARGUMENTS);
            out << "std::pow(" << this->print_operands(expr, ",", symf) << ")";
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
                        else if (exp_numeric.to_int() == 1) out << symf(expr.op(0));
                        else if (exp_numeric.to_int() == 2) out << symf(expr.op(0)) << "*" << symf(expr.op(0));
                        else if (exp_numeric.to_int() == 3) out << symf(expr.op(0)) << "*" << symf(expr.op(0)) << "*" << symf(expr.op(0));
                        else if (exp_numeric.to_int() == 4) out << symf(expr.op(0)) << "*" << symf(expr.op(0)) << "*" << symf(expr.op(0)) << "*" << symf(expr.op(0));
                        else                                out << "std::pow(" << symf(expr.op(0)) << "," << exp_numeric.to_int() << ")";
                      }
                    else  // negative integer
                      {
                        out << "1.0/";
                        if      (exp_numeric.to_int() == -0) out << "1.0";
                        else if (exp_numeric.to_int() == -1) out << symf(expr.op(0));
                        else if (exp_numeric.to_int() == -2) out << "(" << symf(expr.op(0)) << "*" << symf(expr.op(0)) << ")";
                        else if (exp_numeric.to_int() == -3) out << "(" << symf(expr.op(0)) << "*" << symf(expr.op(0)) << "*" << symf(expr.op(0)) << ")";
                        else if (exp_numeric.to_int() == -4) out << "(" << symf(expr.op(0)) << "*" << symf(expr.op(0)) << "*" << symf(expr.op(0)) << "*" << symf(expr.op(0)) << ")";
                        else                                 out << "std::pow(" << symf(expr.op(0)) << "," << -exp_numeric.to_int() << ")";
                      }
                  }
                else  // not an integer
                  {
                    out << "std::pow(" << symf(expr.op(0)) << "," << symf(expr.op(1)) << ")";
                  }
              }
          }

        return(out.str());
      }

  } // namespace cpp
