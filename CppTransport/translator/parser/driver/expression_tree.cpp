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
    
    expression_tree::expression_tree(script& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    GiNaC::ex* expression_tree::get_integer(lexeme_type* lex)
      {
        boost::optional<int> i = lex->get_integer();
        
        GiNaC::ex* rval = new GiNaC::ex(i ? *i : 1);
        
        if(!i)
          {
            lex->error(ERROR_INTEGER_LOOKUP);
          }
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::get_decimal(lexeme_type* lex)
      {
        boost::optional<double> d  = lex->get_decimal();
        
        GiNaC::ex* rval = new GiNaC::ex(d ? *d : 1.0);
        
        if(!d)
          {
            lex->error(ERROR_DECIMAL_LOOKUP);
          }
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::get_identifier(lexeme_type* lex)
      {
        boost::optional<std::string> id = lex->get_identifier();
        
        GiNaC::ex* rval = nullptr;
        
        if(id)
          {
            boost::optional<declaration&> record = this->root.check_symbol_exists(*id);
            
            if(record)
              {
                rval = new GiNaC::ex(record->get_expression());
              }
            else
              {
                std::ostringstream msg;
                
                msg << ERROR_UNKNOWN_IDENTIFIER << " '" << *id << "'";
                lex->error(msg.str());
              }
          }
        else
          {
            lex->error(ERROR_IDENTIFIER_LOOKUP);
          }
        
        if(rval == nullptr)
          {
            rval = new GiNaC::ex(1);
          }
        
        return (rval);
      }

    
    GiNaC::ex* expression_tree::add(GiNaC::ex* l, GiNaC::ex* r)
      {
        GiNaC::ex* rval = new GiNaC::ex((*l) + (*r));
        
        delete l;
        delete r;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::sub(GiNaC::ex* l, GiNaC::ex* r)
      {
        GiNaC::ex* rval = new GiNaC::ex((*l) - (*r));
        
        delete l;
        delete r;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::mul(GiNaC::ex* l, GiNaC::ex* r)
      {
        GiNaC::ex* rval = new GiNaC::ex((*l) * (*r));
        
        delete l;
        delete r;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::div(GiNaC::ex* l, GiNaC::ex* r)
      {
        GiNaC::ex* rval = new GiNaC::ex((*l) / (*r));
        
        delete l;
        delete r;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::pow(GiNaC::ex* l, GiNaC::ex* r)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::pow((*l), (*r)));
        
        delete l;
        delete r;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::unary_minus(GiNaC::ex* l)
      {
        GiNaC::ex* rval = new GiNaC::ex(-(*l));
        
        delete l;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::abs(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::abs(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::step(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::step(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::sqrt(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::sqrt(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::sin(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::sin(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::cos(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::cos(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::tan(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::tan(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::asin(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::asin(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::acos(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::acos(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::atan(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::atan(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::atan2(GiNaC::ex* a1, GiNaC::ex* a2)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::atan2(*a1, *a2));
        
        delete a1;
        delete a2;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::sinh(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::sinh(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::cosh(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::cosh(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::tanh(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::tanh(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::asinh(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::asinh(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::acosh(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::acosh(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::atanh(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::atanh(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::exp(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::exp(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::log(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::log(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::Li2(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::Li2(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::Li(GiNaC::ex* a1, GiNaC::ex* a2)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::Li(*a1, *a2));
        
        delete a1;
        delete a2;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::G(GiNaC::ex* a1, GiNaC::ex* a2)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::G(*a1, *a2));
        
        delete a1;
        delete a2;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::G(GiNaC::ex* a1, GiNaC::ex* a2, GiNaC::ex* a3)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::G(*a1, *a2, *a3));
        
        delete a1;
        delete a2;
        delete a3;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::S(GiNaC::ex* a1, GiNaC::ex* a2, GiNaC::ex* a3)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::S(*a1, *a2, *a3));
        
        delete a1;
        delete a2;
        delete a3;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::H(GiNaC::ex* a1, GiNaC::ex* a2)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::H(*a1, *a2));
        
        delete a1;
        delete a2;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::zeta(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::zeta(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::zeta(GiNaC::ex* a1, GiNaC::ex* a2)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::zeta(*a1, *a2));
        
        delete a1;
        delete a2;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::zetaderiv(GiNaC::ex* a1, GiNaC::ex* a2)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::zetaderiv(*a1, *a2));
        
        delete a1;
        delete a2;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::tgamma(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::tgamma(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::lgamma(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::lgamma(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::beta(GiNaC::ex* a1, GiNaC::ex* a2)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::beta(*a1, *a2));
        
        delete a1;
        delete a2;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::psi(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::psi(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::psi(GiNaC::ex* a1, GiNaC::ex* a2)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::psi(*a1, *a2));
        
        delete a1;
        delete a2;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::factorial(GiNaC::ex* arg)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::factorial(*arg));
        
        delete arg;
        
        return (rval);
      }
    
    
    GiNaC::ex* expression_tree::binomial(GiNaC::ex* a1, GiNaC::ex* a2)
      {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::binomial(*a1, *a2));
        
        delete a1;
        delete a2;
        
        return (rval);
      }
    
  }   // namespace y
