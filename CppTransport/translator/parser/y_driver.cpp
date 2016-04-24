//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>

#include "y_driver.h"


namespace y
	{

    y_driver::y_driver(symbol_factory& s, argument_cache& c, local_environment& e, error_context err_ctx)
	    : sym_factory(s),
        cache(c),
        env(e),
        root(s, err_ctx)
	    {
	    }


    void y_driver::error(std::string msg)
	    {
        ::error(msg, this->cache, this->env);
	    }


    const script& y_driver::get_script()
	    {
        return(this->root);
	    }


    void y_driver::add_field(lexeme_type* lex, attributes* a)
	    {
        // extract identifier name from lexeme
        std::string id;
        bool        ok = lex->get_identifier(id);

        if(ok)
	        {
            GiNaC::symbol sym = this->sym_factory.get_symbol(id, a->get_latex());
            this->root.add_field(id, sym, *lex, a);
	        }
        else
	        {
            lex->error(ERROR_IDENTIFIER_LOOKUP);
	        }

        delete a; // otherwise, attributes block would never be deallocated
	    }


    void y_driver::add_parameter(lexeme_type* lex, attributes* a)
	    {
        // extract identifier name from lexeme
        std::string            id;
        bool                   ok = lex->get_identifier(id);

        if(ok)
	        {
            GiNaC::symbol sym = this->sym_factory.get_symbol(id, a->get_latex());
		        this->root.add_parameter(id, sym, *lex, a);
	        }
        else
	        {
            lex->error(ERROR_IDENTIFIER_LOOKUP);
	        }

        delete a; // otherwise, attributes block would never be deallocated
	    }


		void y_driver::add_subexpr(lexeme_type* lex, subexpr* e)
			{
				// extract identifier name from lexeme
		    std::string id;
		    bool        ok = lex->get_identifier(id);

				subexpr_declaration* d = nullptr;

				if(ok)
					{
				    GiNaC::symbol sym = this->sym_factory.get_symbol(id, e->get_latex());
						this->root.add_subexpr(id, sym, *lex, e);
					}
				else
					{
            lex->error(ERROR_IDENTIFIER_LOOKUP);
					}

				delete e;
			}


    void y_driver::add_latex_attribute(attributes* a, lexeme_type* lex)
	    {
        // extract string name from lexeme
        std::string latex_name;
        bool        ok = lex->get_string(latex_name);

        if(ok)
	        {
            a->set_latex(latex_name, *lex);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::add_string(string_array* a, lexeme_type* lex)
      {
        // extract string from lexeme
        std::string value;
        bool        ok = lex->get_string(value);

        if(ok)
          {
            a->push_element(value, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }


    void y_driver::add_email(author* a, lexeme_type* lex)
      {
        // extract string from lexeme
        std::string value;
        bool        ok = lex->get_string(value);

        if(ok)
          {
            a->set_email(value, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }


    void y_driver::add_institute(author* a, lexeme_type* lex)
      {
        // extract string from lexeme
        std::string value;
        bool        ok = lex->get_string(value);

        if(ok)
          {
            a->set_institute(value, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }


		void y_driver::add_latex_attribute(subexpr* e, lexeme_type* lex)
			{
				// extract string name from lexeme
		    std::string latex_name;
				bool ok = lex->get_string(latex_name);

				if(ok)
					{
						e->set_latex(latex_name, *lex);
					}
				else
					{
            lex->error(ERROR_STRING_LOOKUP);
					}
			}


		void y_driver::add_value_attribute(subexpr* e, GiNaC::ex* v)
			{
				e->set_value(*v);
			}


    void y_driver::set_abserr(struct stepper* s, lexeme_type* lex)
	    {
        // extract decimal value from lexeme
        double d  = DEFAULT_ABS_ERR;
        bool   ok = lex->get_decimal(d);

        if(ok)
	        {
            s->set_abserr(std::abs(d), *lex);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::set_relerr(struct stepper* s, lexeme_type* lex)
	    {
        // extract decimal value from lexeme
        double d  = DEFAULT_REL_ERR;
        bool   ok = lex->get_decimal(d);

        if(ok)
	        {
            s->set_relerr(std::abs(d), *lex);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::set_stepper(struct stepper* s, lexeme_type* lex)
	    {
        // extract string name from lexeme
        std::string stepper_name = DEFAULT_STEPPER;
        bool        ok           = lex->get_string(stepper_name);

        if(ok)
	        {
            s->set_name(stepper_name, *lex);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::set_stepsize(struct stepper* s, lexeme_type* lex)
	    {
        // extract decimal value from lexeme
        double d  = DEFAULT_STEP_SIZE;
        bool   ok = lex->get_decimal(d);

        if(ok)
	        {
            s->set_stepsize(d, *lex);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::set_background_stepper(stepper* s)
	    {
        this->root.set_background_stepper(s);

        delete s;
	    }


    void y_driver::set_perturbations_stepper(stepper* s)
	    {
        this->root.set_perturbations_stepper(s);

        delete s;
	    }


    void y_driver::set_name(lexeme_type* lex)
	    {
        std::string str;
        bool        ok = lex->get_string(str);

        if(ok)
	        {
            this->root.set_name(str, *lex);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::add_author(lexeme_type* lex, author* a)
	    {
        std::string str;
        bool        ok = lex->get_string(str);

        if(ok)
	        {
            a->set_name(str, *lex);
            this->root.add_author(str, *lex, a);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::set_citeguide(lexeme_type* lex)
	    {
        std::string str;
        bool        ok = lex->get_string(str);

        if(ok)
	        {
            this->root.set_citeguide(str, *lex);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::set_description(lexeme_type* lex)
      {
        std::string str;
        bool        ok = lex->get_string(str);

        if(ok)
          {
            this->root.set_description(str, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }


    void y_driver::set_revision(lexeme_type* lex)
      {
        int  d = 1;
        bool ok = lex->get_integer(d);

        if(ok)
          {
            this->root.set_revision(d, *lex);
          }
        else
          {
            lex->error(ERROR_INTEGER_LOOKUP);
          }
      }


    void y_driver::set_license(lexeme_type* lex)
      {
        std::string str;
        bool        ok = lex->get_string(str);

        if(ok)
          {
            this->root.set_license(str, *lex);
          }
        else
          {
            lex->error(ERROR_STRING_LOOKUP);
          }
      }


    void y_driver::set_references(string_array* a)
      {
        this->root.set_references(a->get_array());
      }


    void y_driver::set_urls(string_array* a)
      {
        this->root.set_urls(a->get_array());
      }


    void y_driver::set_core(lexeme_type* lex)
	    {
        std::string str;
        bool        ok = lex->get_string(str);

        if(ok)
	        {
            this->root.set_core(str, *lex);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::set_implementation(lexeme_type* lex)
	    {
        std::string str;
        bool        ok = lex->get_string(str);

        if(ok)
	        {
            this->root.set_implementation(str, *lex);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::set_model(lexeme_type* lex)
	    {
        std::string str;
        bool        ok = lex->get_string(str);

        if(ok)
	        {
            this->root.set_model(str, *lex);
	        }
        else
	        {
            lex->error(ERROR_STRING_LOOKUP);
	        }
	    }


    void y_driver::set_potential(GiNaC::ex* V)
	    {
        this->root.set_potential(*V);

        delete V;
	    }


    void y_driver::set_indexorder_left()
	    {
        this->root.set_indexorder(index_order::left);
	    }


    void y_driver::set_indexorder_right()
	    {
        this->root.set_indexorder(index_order::right);
	    }


    GiNaC::ex* y_driver::add(GiNaC::ex* l, GiNaC::ex* r)
	    {
        GiNaC::ex* rval = new GiNaC::ex((*l) + (*r));

        delete l;
        delete r;

        return (rval);
	    }


    GiNaC::ex* y_driver::sub(GiNaC::ex* l, GiNaC::ex* r)
	    {
        GiNaC::ex* rval = new GiNaC::ex((*l) - (*r));

        delete l;
        delete r;

        return (rval);
	    }


    GiNaC::ex* y_driver::mul(GiNaC::ex* l, GiNaC::ex* r)
	    {
        GiNaC::ex* rval = new GiNaC::ex((*l) * (*r));

        delete l;
        delete r;

        return (rval);
	    }


    GiNaC::ex* y_driver::div(GiNaC::ex* l, GiNaC::ex* r)
	    {
        GiNaC::ex* rval = new GiNaC::ex((*l) / (*r));

        delete l;
        delete r;

        return (rval);
	    }


    GiNaC::ex* y_driver::pow(GiNaC::ex* l, GiNaC::ex* r)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::pow((*l), (*r)));

        delete l;
        delete r;

        return (rval);
	    }


    GiNaC::ex* y_driver::unary_minus(GiNaC::ex* l)
	    {
        GiNaC::ex* rval = new GiNaC::ex(-(*l));

        delete l;

        return (rval);
	    }


    GiNaC::ex* y_driver::abs(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::abs(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::step(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::step(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::sqrt(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::sqrt(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::sin(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::sin(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::cos(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::cos(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::tan(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::tan(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::asin(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::asin(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::acos(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::acos(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::atan(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::atan(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::atan2(GiNaC::ex* a1, GiNaC::ex* a2)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::atan2(*a1, *a2));

        delete a1;
        delete a2;

        return (rval);
	    }


    GiNaC::ex* y_driver::sinh(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::sinh(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::cosh(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::cosh(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::tanh(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::tanh(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::asinh(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::asinh(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::acosh(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::acosh(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::atanh(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::atanh(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::exp(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::exp(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::log(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::log(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::Li2(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::Li2(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::Li(GiNaC::ex* a1, GiNaC::ex* a2)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::Li(*a1, *a2));

        delete a1;
        delete a2;

        return (rval);
	    }


    GiNaC::ex* y_driver::G(GiNaC::ex* a1, GiNaC::ex* a2)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::G(*a1, *a2));

        delete a1;
        delete a2;

        return (rval);
	    }


    GiNaC::ex* y_driver::G(GiNaC::ex* a1, GiNaC::ex* a2, GiNaC::ex* a3)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::G(*a1, *a2, *a3));

        delete a1;
        delete a2;
        delete a3;

        return (rval);
	    }


    GiNaC::ex* y_driver::S(GiNaC::ex* a1, GiNaC::ex* a2, GiNaC::ex* a3)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::S(*a1, *a2, *a3));

        delete a1;
        delete a2;
        delete a3;

        return (rval);
	    }


    GiNaC::ex* y_driver::H(GiNaC::ex* a1, GiNaC::ex* a2)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::H(*a1, *a2));

        delete a1;
        delete a2;

        return (rval);
	    }


    GiNaC::ex* y_driver::zeta(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::zeta(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::zeta(GiNaC::ex* a1, GiNaC::ex* a2)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::zeta(*a1, *a2));

        delete a1;
        delete a2;

        return (rval);
	    }


    GiNaC::ex* y_driver::zetaderiv(GiNaC::ex* a1, GiNaC::ex* a2)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::zetaderiv(*a1, *a2));

        delete a1;
        delete a2;

        return (rval);
	    }


    GiNaC::ex* y_driver::tgamma(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::tgamma(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::lgamma(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::lgamma(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::beta(GiNaC::ex* a1, GiNaC::ex* a2)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::beta(*a1, *a2));

        delete a1;
        delete a2;

        return (rval);
	    }


    GiNaC::ex* y_driver::psi(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::psi(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::psi(GiNaC::ex* a1, GiNaC::ex* a2)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::psi(*a1, *a2));

        delete a1;
        delete a2;

        return (rval);
	    }


    GiNaC::ex* y_driver::factorial(GiNaC::ex* arg)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::factorial(*arg));

        delete arg;

        return (rval);
	    }


    GiNaC::ex* y_driver::binomial(GiNaC::ex* a1, GiNaC::ex* a2)
	    {
        GiNaC::ex* rval = new GiNaC::ex(GiNaC::binomial(*a1, *a2));

        delete a1;
        delete a2;

        return (rval);
	    }


    GiNaC::ex* y_driver::get_integer(lexeme_type* lex)
	    {
        int  i  = 1;
        bool ok = lex->get_integer(i);

        GiNaC::ex* rval = new GiNaC::ex(i);

        if(!ok)
	        {
            lex->error(ERROR_INTEGER_LOOKUP);
	        }

        return (rval);
	    }


    GiNaC::ex* y_driver::get_decimal(lexeme_type* lex)
	    {
        double d  = 1.0;
        bool   ok = lex->get_decimal(d);

        GiNaC::ex* rval = new GiNaC::ex(d);

        if(!ok)
	        {
            lex->error(ERROR_DECIMAL_LOOKUP);
	        }

        return (rval);
	    }


    GiNaC::ex* y_driver::get_identifier(lexeme_type* lex)
	    {
        std::string id;
        bool        ok = lex->get_identifier(id);

        GiNaC::ex* rval = nullptr;

        if(ok)
	        {
            boost::optional<declaration&> record = this->root.check_symbol_exists(id);

            if(record)
	            {
                rval = new GiNaC::ex(record->get_expression());
	            }
            else
	            {
                std::ostringstream msg;

                msg << ERROR_UNKNOWN_IDENTIFIER << " '" << id << "'";
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


	}
