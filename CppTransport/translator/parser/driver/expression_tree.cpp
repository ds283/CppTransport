//
// Created by David Seery on 22/05/2017.
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


#include "expression_tree.h"


namespace y
  {
    
    expression_tree::expression_tree(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::get_integer(lexeme_type& lex)
      {
        boost::optional<int> i = lex.get_integer();
        
        if(!i)
          {
            lex.error(ERROR_INTEGER_LOOKUP);
          }
        
        return std::make_shared<GiNaC::ex>(i ? *i : 1);
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::get_decimal(lexeme_type& lex)
      {
        boost::optional<double> d = lex.get_decimal();
        
        if(!d)
          {
            lex.error(ERROR_DECIMAL_LOOKUP);
          }

        return std::make_shared<GiNaC::ex>(d ? *d : 1.0);
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::get_identifier(lexeme_type& lex)
      {
        boost::optional<std::string> id = lex.get_identifier();

        if(id)
          {
            boost::optional<declaration&> record = this->root.check_symbol_exists(*id);
            
            if(record)
              {
                return std::make_shared<GiNaC::ex>(record->get_expression());
              }
            else
              {
                std::ostringstream msg;
                
                msg << ERROR_UNKNOWN_IDENTIFIER << " '" << *id << "'";
                lex.error(msg.str());
              }
          }
        else
          {
            lex.error(ERROR_IDENTIFIER_LOOKUP);
          }
        
        return std::make_shared<GiNaC::ex>(1);
      }

    
    std::shared_ptr<GiNaC::ex> expression_tree::add(GiNaC::ex& l, GiNaC::ex& r)
      {
        return std::make_shared<GiNaC::ex>(l + r);
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::sub(GiNaC::ex& l, GiNaC::ex& r)
      {
        return std::make_shared<GiNaC::ex>(l - r);
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::mul(GiNaC::ex& l, GiNaC::ex& r)
      {
        return std::make_shared<GiNaC::ex>(l * r);
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::div(GiNaC::ex& l, GiNaC::ex& r)
      {
        return std::make_shared<GiNaC::ex>(l / r);
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::pow(GiNaC::ex& l, GiNaC::ex& r)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::pow(l, r));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::unary_minus(GiNaC::ex& l)
      {
        return std::make_shared<GiNaC::ex>(-l);
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::abs(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::abs(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::step(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::step(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::sqrt(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::sqrt(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::sin(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::sin(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::cos(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::cos(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::tan(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::tan(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::asin(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::asin(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::acos(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::acos(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::atan(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::atan(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::atan2(GiNaC::ex& a1, GiNaC::ex& a2)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::atan2(a1, a2));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::sinh(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::sinh(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::cosh(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::cosh(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::tanh(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::tanh(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::asinh(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::asinh(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::acosh(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::acosh(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::atanh(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::atanh(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::exp(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::exp(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::log(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::log(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::Li2(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::Li2(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::Li(GiNaC::ex& a1, GiNaC::ex& a2)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::Li(a1, a2));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::G(GiNaC::ex& a1, GiNaC::ex& a2)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::G(a1, a2));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::G(GiNaC::ex& a1, GiNaC::ex& a2, GiNaC::ex& a3)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::G(a1, a2, a3));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::S(GiNaC::ex& a1, GiNaC::ex& a2, GiNaC::ex& a3)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::S(a1, a2, a3));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::H(GiNaC::ex& a1, GiNaC::ex& a2)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::H(a1, a2));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::zeta(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::zeta(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::zeta(GiNaC::ex& a1, GiNaC::ex& a2)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::zeta(a1, a2));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::zetaderiv(GiNaC::ex& a1, GiNaC::ex& a2)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::zetaderiv(a1, a2));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::tgamma(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::tgamma(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::lgamma(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::lgamma(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::beta(GiNaC::ex& a1, GiNaC::ex& a2)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::beta(a1, a2));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::psi(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::psi(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::psi(GiNaC::ex& a1, GiNaC::ex& a2)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::psi(a1, a2));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::factorial(GiNaC::ex& arg)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::factorial(arg));
      }
    
    
    std::shared_ptr<GiNaC::ex> expression_tree::binomial(GiNaC::ex& a1, GiNaC::ex& a2)
      {
        return std::make_shared<GiNaC::ex>(GiNaC::binomial(a1, a2));
      }
    
  }   // namespace y
