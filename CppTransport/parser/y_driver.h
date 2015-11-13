//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __y_driver_H_
#define __y_driver_H_


#include <iostream>
#include <string>

#include "lexeme.h"
#include "lexical.h"
#include "semantic_data.h"
#include "script.h"

#include "symbol_factory.h"

#include "y_common.h"


namespace y
	{

    class y_driver
	    {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        y_driver(symbol_factory& s);

        //! destructor is default
        ~y_driver() = default;


		    // INTERFACE

      public:

        //! detect error conditions reported during processing
        bool failed() const { return(this->root->failed()); }

        void error(std::string msg);

        const script& get_script();

        void add_field(lexeme_type* lex, attributes* a);

        void add_parameter(lexeme_type* lex, attributes* a);

		    void add_subexpr(lexeme_type* lex, subexpr* e);

        void set_name(lexeme_type* lex);

        void set_author(lexeme_type* lex);

        void set_tag(lexeme_type* lex);

        void set_core(lexeme_type* lex);

        void set_implementation(lexeme_type* lex);

        void set_model(lexeme_type* lex);

        void set_potential(GiNaC::ex* V);

        void set_indexorder_left();

        void set_indexorder_right();

        void add_latex_attribute(attributes* a, lexeme_type* lex);
		    
		    void add_latex_attribute(subexpr* e, lexeme_type* lex);

		    void add_value_attribute(subexpr* e, GiNaC::ex* v);

        void set_abserr(struct stepper* s, lexeme_type* lex);

        void set_relerr(struct stepper* s, lexeme_type* lex);

        void set_stepper(struct stepper* s, lexeme_type* lex);

        void set_stepsize(struct stepper* s, lexeme_type* lex);

        void set_background_stepper(struct stepper* s);

        void set_perturbations_stepper(struct stepper* s);

        GiNaC::ex* get_integer(lexeme_type* lex);

        GiNaC::ex* get_decimal(lexeme_type* lex);

        GiNaC::ex* get_identifier(lexeme_type* lex);

        GiNaC::ex* add(GiNaC::ex* l, GiNaC::ex* r);

        GiNaC::ex* sub(GiNaC::ex* l, GiNaC::ex* r);

        GiNaC::ex* mul(GiNaC::ex* l, GiNaC::ex* r);

        GiNaC::ex* div(GiNaC::ex* l, GiNaC::ex* r);

        GiNaC::ex* pow(GiNaC::ex* l, GiNaC::ex* r);

        GiNaC::ex* unary_minus(GiNaC::ex* l);

        GiNaC::ex* abs(GiNaC::ex* arg);

        GiNaC::ex* step(GiNaC::ex* arg);

        GiNaC::ex* sqrt(GiNaC::ex* arg);

        GiNaC::ex* sin(GiNaC::ex* arg);

        GiNaC::ex* cos(GiNaC::ex* arg);

        GiNaC::ex* tan(GiNaC::ex* arg);

        GiNaC::ex* asin(GiNaC::ex* arg);

        GiNaC::ex* acos(GiNaC::ex* arg);

        GiNaC::ex* atan(GiNaC::ex* arg);

        GiNaC::ex* atan2(GiNaC::ex* a1, GiNaC::ex* a2);

        GiNaC::ex* sinh(GiNaC::ex* arg);

        GiNaC::ex* cosh(GiNaC::ex* arg);

        GiNaC::ex* tanh(GiNaC::ex* arg);

        GiNaC::ex* asinh(GiNaC::ex* arg);

        GiNaC::ex* acosh(GiNaC::ex* arg);

        GiNaC::ex* atanh(GiNaC::ex* arg);

        GiNaC::ex* exp(GiNaC::ex* arg);

        GiNaC::ex* log(GiNaC::ex* arg);

        GiNaC::ex* Li2(GiNaC::ex* arg);

        GiNaC::ex* Li(GiNaC::ex* a1, GiNaC::ex* a2);

        GiNaC::ex* G(GiNaC::ex* a1, GiNaC::ex* a2);

        GiNaC::ex* G(GiNaC::ex* a1, GiNaC::ex* a2, GiNaC::ex* a3);

        GiNaC::ex* S(GiNaC::ex* a1, GiNaC::ex* a2, GiNaC::ex* a3);

        GiNaC::ex* H(GiNaC::ex* a1, GiNaC::ex* a2);

        GiNaC::ex* zeta(GiNaC::ex* arg);

        GiNaC::ex* zeta(GiNaC::ex* a1, GiNaC::ex* a2);

        GiNaC::ex* zetaderiv(GiNaC::ex* a1, GiNaC::ex* a2);

        GiNaC::ex* tgamma(GiNaC::ex* arg);

        GiNaC::ex* lgamma(GiNaC::ex* arg);

        GiNaC::ex* beta(GiNaC::ex* a1, GiNaC::ex* a2);

        GiNaC::ex* psi(GiNaC::ex* arg);

        GiNaC::ex* psi(GiNaC::ex* a1, GiNaC::ex* a2);

        GiNaC::ex* factorial(GiNaC::ex* arg);

        GiNaC::ex* binomial(GiNaC::ex* a1, GiNaC::ex* a2);


		    // INTERNAL DATA

      private:

        std::unique_ptr<script> root;

		    symbol_factory& sym_factory;

	    };

	}


#endif //__y_driver_H_
