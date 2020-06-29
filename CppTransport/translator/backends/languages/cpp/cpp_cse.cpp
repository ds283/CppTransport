//
// Created by David Seery on 04/12/2013.
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


#include <string>
#include <sstream>

#include "cse.h"
#include "cpp_cse.h"
#include "error.h"
#include "msg_en.h"


namespace cpp
  {
    
    const std::map< std::string, std::string > func_convert
      {
        {"abs", "std::abs"},
        {"sqrt", "std::sqrt"},
        {"sin", "std::sin"},
        {"cos", "std::cos"},
        {"tan", "std::tan"},
        {"asin", "std::asin"},
        {"acos", "std::acos"},
        {"atan", "std::atan"},
        {"atan2", "std::atan2"},
        {"sinh", "std::sinh"},
        {"cosh", "std::cosh"},
        {"tanh", "std::tanh"},
        {"asinh", "std::asinh"},
        {"acosh", "std::acosh"},
        {"atanh", "std::atanh"},
        {"exp", "std::exp"},
        {"log", "std::log"},
        {"pow", "std::pow"},
        {"tgamma", "std::tgamma"},
        {"lgamma", "std::lgamma"}
      };
    

    std::string cpp_cse::print(const GiNaC::ex& expr, bool use_count)
      {
        std::string name;
    
        if(GiNaC::is_a<GiNaC::function>(expr)) name = GiNaC::ex_to<GiNaC::function>(expr).get_name();
        else name = GiNaC::ex_to<GiNaC::basic>(expr).class_name();

        if     (name == "numeric")   return this->printer.ginac(expr);
        else if(name == "symbol")    return this->printer.ginac(expr);
        else if(name == "add")       return this->print_operands(expr, "+", use_count);
        else if(name == "mul")       return this->print_operands(expr, "*", use_count);
        else if(name == "power")     return this->print_power(expr, use_count);
        else if(name == "tensdelta") return this->printer.ginac(expr);
        else if(name == "idx")       return this->printer.ginac(expr);
        else if(name == "varidx")    return this->printer.ginac(expr);
        else if(name == "indexed")   return this->printer.ginac(expr);

        // not a standard operation, so assume it must be a special function
        // look up its C++ form in func_map, and then format its arguments,
        // taking care to keep track of use counts
        auto t = func_convert.find(name);

        // if no match was found, we don't know what to do
        // give up in despair
        if(t == func_convert.end())
          {
            error_context err_ctx = this->data_payload.make_error_context();

            std::ostringstream msg;
            msg << ERROR_UNIMPLEMENTED_MATHS_FUNCTION << " '" << name << "', "
                << ERROR_USED_IN_EXPRESSION << " '" << expr << "'";
            err_ctx.error(msg.str());

            return std::string{};
          }

        std::string rval{t->second};
        rval.append("(");
        rval.append(this->print_operands(expr, ",", use_count));
        rval.append(")");

        return rval;
      }


    std::string cpp_cse::print_operands(const GiNaC::ex& expr, std::string op, bool use_count)
      {
        std::string rval;

        unsigned int c = 0;
        for(const auto& arg : expr)
          {
            if(c > 0) rval.append(op);

            // determine whether we should bracket this operand
            // this should happen any time the operand has lower precedence than the current operand, but at the moment
            // we only deal with + and * so we can simply check for +
            // we can kill the brackets if the operands are a comma-separated list of arguments, though
            bool bracket = op != "," && GiNaC::is_a<GiNaC::add>(arg);

            if(bracket) rval.append("(");
            if(use_count) rval.append(this->get_symbol_with_use_count(arg));
            else          rval.append(this->get_symbol_without_use_count(arg));
            if(bracket) rval.append(")");

            ++c;
          }
        
        return rval;
      }


    // utility function to properly bracket the base of a power expression
    static std::string unwrap_power(const std::string& base_str, const GiNaC::ex& base_expr, unsigned int factors)
      {
        bool simple = GiNaC::is_a<GiNaC::symbol>(base_expr)
                      || GiNaC::is_a<GiNaC::constant>(base_expr)
                      || GiNaC::is_a<GiNaC::function>(base_expr);

        std::ostringstream buf;

        unsigned int count = 0;
        while(factors > 0)
          {
            if(count > 0) buf << "*";
            if(simple) buf << base_str;
            else       buf << "(" << base_str << ")";

            --factors;
            ++count;
          }

        return buf.str();
      }


    // treat powers specially, because it's better to unroll them and
    // avoid a function call if we have an exact integral power
    std::string cpp_cse::print_power(const GiNaC::ex& expr, bool use_count)
      {
        std::ostringstream out;
        size_t n = expr.nops();

        if(n != 2)
          {
            error_context err_ctx = this->data_payload.make_error_context();
            err_ctx.error(ERROR_CSE_POWER_ARGUMENTS);
            out << "std::pow(" << this->print_operands(expr, ",", use_count) << ")";
            return std::string{};
          }

        // set up aliases for base and exponent expressions
        const auto& base_expr = expr.op(0);
        const auto& exponent_expr = expr.op(1);
    
        // perform use-counting on base and exponent exponent, which is necessary since their values may have been
        // used elsewhere in the CSE tree, even if it is an integer that we will unroll and not use explicitly
        std::string base;
        if(use_count) base = this->get_symbol_with_use_count(base_expr);
        else base = this->get_symbol_without_use_count(base_expr);

        std::string exponent;
        if(use_count) exponent = this->get_symbol_with_use_count(exponent_expr);
        else exponent = this->get_symbol_without_use_count(exponent_expr);

        // if exponent is not an integer, emit a standard power directive
        if(!GiNaC::is_a<GiNaC::numeric>(exponent_expr))
          {
            out << "std::pow(" << base << "," << exponent << ")";
            return out.str();
          }

        const auto& exp_numeric = GiNaC::ex_to<GiNaC::numeric>(exponent_expr);

        if(!GiNaC::is_integer(exp_numeric))
          {
            out << "std::pow(" << base << "," << exponent << ")";
            return out.str();
          }

        if(GiNaC::is_nonneg_integer(exp_numeric))
          {
            if(exp_numeric.to_int() == 0) out << "1.0";
            else if(exp_numeric.to_int() == 1) out << unwrap_power(base, base_expr, 1);
            else if(exp_numeric.to_int() == 2) out << "(" << unwrap_power(base, base_expr, 2) << ")";
            else if(exp_numeric.to_int() == 3) out << "(" << unwrap_power(base, base_expr, 3) << ")";
            else if(exp_numeric.to_int() == 4) out << "(" << unwrap_power(base, base_expr, 4) << ")";
            else out << "std::pow(" << base << "," << exp_numeric.to_int() << ")";
          }
        else  // negative integer
          {
            if(exp_numeric.to_int() == -0) out << "1.0";
            else if(exp_numeric.to_int() == -1) out << "1.0/" << unwrap_power(base, base_expr, 1);
            else if(exp_numeric.to_int() == -2) out << "1.0/" << "(" << unwrap_power(base, base_expr, 2) << ")";
            else if(exp_numeric.to_int() == -3) out << "1.0/" << "(" << unwrap_power(base, base_expr, 3) << ")";
            else if(exp_numeric.to_int() == -4) out << "1.0/" << "(" << unwrap_power(base, base_expr, 4) << ")";
            else out << "std::pow(" << base << "," << exp_numeric.to_int() << ")";
          }

        return out.str();
      }

  } // namespace cpp
