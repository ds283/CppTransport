//
// Created by David Seery on 17/06/2013.
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



#ifndef CPPTRANSPORT_Y_DRIVER_H
#define CPPTRANSPORT_Y_DRIVER_H


#include <iostream>
#include <string>

#include "semantic_data.h"
#include "model_descriptor.h"

#include "symbol_factory.h"
#include "argument_cache.h"
#include "local_environment.h"

#include "y_common.h"

#include "expression_tree_driver.h"
#include "lagrangian_driver.h"
#include "metadata_driver.h"
#include "templates_driver.h"
#include "misc_driver.h"


namespace y
	{

    //! y_driver contains the main methods called by the Bison parser
    class y_driver
	    {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        //! error_context is passed down from parent translation_unit.
        //! It is only used to pass to our script object 'root' and is used to construct
        //! fake error contexts for default reserved symbols such as M_Planck
        y_driver(symbol_factory& s, argument_cache& c, local_environment& e, error_context err_ctx);

        //! destructor is default
        ~y_driver() = default;


		    // INTERFACE -- FAILURE AND ERROR CONDITIONS

      public:

        //! detect error conditions reported during processing
        bool failed() const { return(this->root.failed()); }

        //! raise error, bypassing normal context-reporting system (eg. if no context can be found)
        void error(std::string msg);
        
        
        // INTERFACE -- GET MODEL DESCRIPTION CONTAINER
        
      public:

        const model_descriptor& get_descriptor();
    
    
        // LAGRANGIAN MANAGEMENT
  
      public:
    
        void add_field(lexeme_type& lex, attributes& a) { this->lag.add_field(lex, a); }
    
        void add_parameter(lexeme_type& lex, attributes& a) { this->lag.add_parameter(lex, a); }
    
        void add_subexpr(lexeme_type& lex, subexpr& e) { this->lag.add_subexpr(lex, e); }
    
        void set_potential(GiNaC::ex& V, lexeme_type& lex) { this->lag.set_potential(V, lex); }
    
        void add_latex_attribute(subexpr& e, lexeme_type& lex) { this->lag.add_latex_attribute(e, lex); }
    
        void add_value_attribute(subexpr& e, GiNaC::ex& v, lexeme_type& lex) { this->lag.add_value_attribute(e, v, lex); }
    
    
        // SET BASIC METADATA
  
      public:
    
        void set_model(lexeme_type& lex) { this->meta.set_model(lex); }
    
        void set_name(lexeme_type& lex) { this->meta.set_name(lex); }
    
        void add_author(lexeme_type& lex, author& a) { this->meta.add_author(lex, a); }
    
        void set_citeguide(lexeme_type& lex) { this->meta.set_citeguide(lex); }
    
        void set_description(lexeme_type& lex) { this->meta.set_description(lex); }
    
        void set_revision(lexeme_type& lex) { this->meta.set_revision(lex); }
    
        void set_license(lexeme_type& lex) { this->meta.set_license(lex); }
    
        void set_references(string_array& a) { this->meta.set_references(a); }
    
        void set_urls(string_array& a) { this->meta.set_urls(a); }
    
        void add_email(author& a, lexeme_type& lex) { this->meta.add_email(a, lex); }
    
        void add_institute(author& a, lexeme_type& institute) { this->meta.add_institute(a, institute); }
    
    
        // MISCELLANEOUS SETTINGS
  
      public:
    
        void set_indexorder_left() { this->misc.set_indexorder_left(); }
    
        void set_indexorder_right() { this->misc.set_indexorder_right(); }
    
        void add_latex_attribute(attributes& a, lexeme_type& lex) { this->misc.add_latex_attribute(a, lex); }
    
        void add_string(string_array& a, lexeme_type& lex) { this->misc.add_string(a, lex); }
    
    
        // TEMPLATE DATA
  
      public:
    
        void set_core(lexeme_type& lex) { this->templ.set_core(lex); }
    
        void set_implementation(lexeme_type& lex) { this->templ.set_implementation(lex); }
    
        void set_abserr(struct stepper& s, lexeme_type& lex) { this->templ.set_abserr(s, lex); }
    
        void set_relerr(struct stepper& s, lexeme_type& lex) { this->templ.set_relerr(s, lex); }
    
        void set_stepper(struct stepper& s, lexeme_type& lex) { this->templ.set_stepper(s, lex); }
    
        void set_stepsize(struct stepper& s, lexeme_type& lex) { this->templ.set_stepsize(s, lex); }
    
        void set_background_stepper(struct stepper& s, lexeme_type& lex) { this->templ.set_background_stepper(s, lex); }
    
        void set_perturbations_stepper(struct stepper& s, lexeme_type& lex) { this->templ.set_perturbations_stepper(s, lex); }
    
    
        // GET EXPRESSION TERMINALS
  
      public:
    
        std::shared_ptr<GiNaC::ex> get_integer(lexeme_type& lex) { return this->etree.get_integer(lex); }
    
        std::shared_ptr<GiNaC::ex> get_decimal(lexeme_type& lex) { return this->etree.get_decimal(lex); }
    
        std::shared_ptr<GiNaC::ex> get_identifier(lexeme_type& lex) { return this->etree.get_identifier(lex); }
    
    
        // BUILD EXPRESSION TREES
  
      public:
    
        std::shared_ptr<GiNaC::ex> add(GiNaC::ex& l, GiNaC::ex& r) { return this->etree.add(l,r); }
    
        std::shared_ptr<GiNaC::ex> sub(GiNaC::ex& l, GiNaC::ex& r) { return this->etree.sub(l,r); }
        
        std::shared_ptr<GiNaC::ex> mul(GiNaC::ex& l, GiNaC::ex& r) { return this->etree.mul(l,r); }
    
        std::shared_ptr<GiNaC::ex> div(GiNaC::ex& l, GiNaC::ex& r) { return this->etree.div(l,r); }
    
        std::shared_ptr<GiNaC::ex> pow(GiNaC::ex& l, GiNaC::ex& r) { return this->etree.pow(l,r); }
    
        std::shared_ptr<GiNaC::ex> unary_minus(GiNaC::ex& l) { return this->etree.unary_minus(l); }
    
        std::shared_ptr<GiNaC::ex> abs(GiNaC::ex& arg) { return this->etree.abs(arg); }
    
        std::shared_ptr<GiNaC::ex> step(GiNaC::ex& arg) { return this->etree.step(arg); }
    
        std::shared_ptr<GiNaC::ex> sqrt(GiNaC::ex& arg) { return this->etree.sqrt(arg); }
    
        std::shared_ptr<GiNaC::ex> sin(GiNaC::ex& arg) { return this->etree.sin(arg); }
    
        std::shared_ptr<GiNaC::ex> cos(GiNaC::ex& arg) { return this->etree.cos(arg); }
    
        std::shared_ptr<GiNaC::ex> tan(GiNaC::ex& arg) { return this->etree.tan(arg); }
    
        std::shared_ptr<GiNaC::ex> asin(GiNaC::ex& arg) { return this->etree.asin(arg); }
    
        std::shared_ptr<GiNaC::ex> acos(GiNaC::ex& arg) { return this->etree.acos(arg); }
    
        std::shared_ptr<GiNaC::ex> atan(GiNaC::ex& arg) { return this->etree.atan(arg); }
    
        std::shared_ptr<GiNaC::ex> atan2(GiNaC::ex& a1, GiNaC::ex& a2) { return this->etree.atan2(a1, a2); }
    
        std::shared_ptr<GiNaC::ex> sinh(GiNaC::ex& arg) { return this->etree.sinh(arg); }
    
        std::shared_ptr<GiNaC::ex> cosh(GiNaC::ex& arg) { return this->etree.cosh(arg); }
    
        std::shared_ptr<GiNaC::ex> tanh(GiNaC::ex& arg) { return this->etree.tanh(arg); }
    
        std::shared_ptr<GiNaC::ex> asinh(GiNaC::ex& arg) { return this->etree.asinh(arg); }
    
        std::shared_ptr<GiNaC::ex> acosh(GiNaC::ex& arg) { return this->etree.acosh(arg); }
    
        std::shared_ptr<GiNaC::ex> atanh(GiNaC::ex& arg) { return this->etree.atanh(arg); }
    
        std::shared_ptr<GiNaC::ex> exp(GiNaC::ex& arg) { return this->etree.exp(arg); }
    
        std::shared_ptr<GiNaC::ex> log(GiNaC::ex& arg) { return this->etree.log(arg); }
    
        std::shared_ptr<GiNaC::ex> Li2(GiNaC::ex& arg) { return this->etree.Li2(arg); }
    
        std::shared_ptr<GiNaC::ex> Li(GiNaC::ex& a1, GiNaC::ex& a2) { return this->etree.Li(a1, a2); }
    
        std::shared_ptr<GiNaC::ex> G(GiNaC::ex& a1, GiNaC::ex& a2) { return this->etree.G(a1, a2); }
    
        std::shared_ptr<GiNaC::ex> G(GiNaC::ex& a1, GiNaC::ex& a2, GiNaC::ex& a3) { return this->etree.G(a1, a2, a3); }
    
        std::shared_ptr<GiNaC::ex> S(GiNaC::ex& a1, GiNaC::ex& a2, GiNaC::ex& a3) { return this->etree.S(a1, a2, a3); }
    
        std::shared_ptr<GiNaC::ex> H(GiNaC::ex& a1, GiNaC::ex& a2) { return this->etree.H(a1, a2); }
    
        std::shared_ptr<GiNaC::ex> zeta(GiNaC::ex& arg) { return this->etree.zeta(arg); }
    
        std::shared_ptr<GiNaC::ex> zeta(GiNaC::ex& a1, GiNaC::ex& a2) { return this->etree.zeta(a1, a2); }
    
        std::shared_ptr<GiNaC::ex> zetaderiv(GiNaC::ex& a1, GiNaC::ex& a2) { return this->etree.zetaderiv(a1, a2); }
    
        std::shared_ptr<GiNaC::ex> tgamma(GiNaC::ex& arg) { return this->etree.tgamma(arg); }
    
        std::shared_ptr<GiNaC::ex> lgamma(GiNaC::ex& arg) { return this->etree.lgamma(arg); }
    
        std::shared_ptr<GiNaC::ex> beta(GiNaC::ex& a1, GiNaC::ex& a2) { return this->etree.beta(a1, a2); }
    
        std::shared_ptr<GiNaC::ex> psi(GiNaC::ex& arg) { return this->etree.psi(arg); }
    
        std::shared_ptr<GiNaC::ex> psi(GiNaC::ex& a1, GiNaC::ex& a2) { return this->etree.psi(a1, a2); }
    
        std::shared_ptr<GiNaC::ex> factorial(GiNaC::ex& arg) { return this->etree.factorial(arg); }
    
        std::shared_ptr<GiNaC::ex> binomial(GiNaC::ex& a1, GiNaC::ex& a2) { return this->etree.binomial(a1, a2); }
    
    
        // INTERNAL DATA

      private:

        // MODEL DESCRIPTOR
        
        //! container for model descriptor information
        model_descriptor root;


        // REFERENCES TO DELEGATE OBJECTS

        //! reference to symbol factory
		    symbol_factory& sym_factory;

        //! reference to argument cache
        argument_cache& cache;

        //! reference to local environment
        local_environment& env;
    
    
        // COMPONENT FUNCTIONALITY OBJECTS
    
        //! expression trees
        expression_tree_driver etree;
    
        //! lagrangian data
        lagrangian_driver lag;
    
        //! metadata
        metadata_driver meta;
    
        //! template data
        templates_driver templ;
    
        //! miscellaneous settings and methods
        misc_driver misc;

	    };

	}


#endif // CPPTRANSPORT_Y_DRIVER_H
