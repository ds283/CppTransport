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

#ifndef CPPTRANSPORT_EXPRESSION_TREE_H
#define CPPTRANSPORT_EXPRESSION_TREE_H


#include "semantic_data.h"
#include "model_descriptor.h"
#include "symbol_factory.h"
#include "argument_cache.h"
#include "local_environment.h"


namespace y
  {

    class expression_tree
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor
        expression_tree(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le);
        
        //! destructor is default
        ~expression_tree() = default;
        
        
        // INTERFACE -- GET EXPRESSION TERMINALS
  
      public:
    
        std::shared_ptr<GiNaC::ex> get_integer(lexeme_type& lex);
    
        std::shared_ptr<GiNaC::ex> get_decimal(lexeme_type& lex);
    
        std::shared_ptr<GiNaC::ex> get_identifier(lexeme_type& lex);
    
        
        // INTERFACE -- BUILD EXPRESSION TREES
        
      public:
    
        std::shared_ptr<GiNaC::ex> add(GiNaC::ex& l, GiNaC::ex& r);
    
        std::shared_ptr<GiNaC::ex> sub(GiNaC::ex& l, GiNaC::ex& r);
    
        std::shared_ptr<GiNaC::ex> mul(GiNaC::ex& l, GiNaC::ex& r);
    
        std::shared_ptr<GiNaC::ex> div(GiNaC::ex& l, GiNaC::ex& r);
    
        std::shared_ptr<GiNaC::ex> pow(GiNaC::ex& l, GiNaC::ex& r);
    
        std::shared_ptr<GiNaC::ex> unary_minus(GiNaC::ex& l);
    
        std::shared_ptr<GiNaC::ex> abs(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> step(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> sqrt(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> sin(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> cos(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> tan(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> asin(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> acos(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> atan(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> atan2(GiNaC::ex& a1, GiNaC::ex& a2);
    
        std::shared_ptr<GiNaC::ex> sinh(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> cosh(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> tanh(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> asinh(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> acosh(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> atanh(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> exp(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> log(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> Li2(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> Li(GiNaC::ex& a1, GiNaC::ex& a2);
    
        std::shared_ptr<GiNaC::ex> G(GiNaC::ex& a1, GiNaC::ex& a2);
    
        std::shared_ptr<GiNaC::ex> G(GiNaC::ex& a1, GiNaC::ex& a2, GiNaC::ex& a3);
    
        std::shared_ptr<GiNaC::ex> S(GiNaC::ex& a1, GiNaC::ex& a2, GiNaC::ex& a3);
    
        std::shared_ptr<GiNaC::ex> H(GiNaC::ex& a1, GiNaC::ex& a2);
    
        std::shared_ptr<GiNaC::ex> zeta(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> zeta(GiNaC::ex& a1, GiNaC::ex& a2);
    
        std::shared_ptr<GiNaC::ex> zetaderiv(GiNaC::ex& a1, GiNaC::ex& a2);
    
        std::shared_ptr<GiNaC::ex> tgamma(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> lgamma(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> beta(GiNaC::ex& a1, GiNaC::ex& a2);
    
        std::shared_ptr<GiNaC::ex> psi(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> psi(GiNaC::ex& a1, GiNaC::ex& a2);
    
        std::shared_ptr<GiNaC::ex> factorial(GiNaC::ex& arg);
    
        std::shared_ptr<GiNaC::ex> binomial(GiNaC::ex& a1, GiNaC::ex& a2);
    
    
        // INTERNAL DATA
        
      protected:
        
        //! model description container
        model_descriptor& root;
        
        //! delegated symbol factory
        symbol_factory& sym_factory;
        
        //! delegated argument cache
        argument_cache& cache;
        
        //! delegated local environment
        local_environment& env;
        
      };
    
  }


#endif //CPPTRANSPORT_EXPRESSION_TREE_H
