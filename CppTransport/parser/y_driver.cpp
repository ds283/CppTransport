//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <string>
#include <sstream>

#include "y_driver.h"
#include "error.h"

#include "ginac/ginac.h"

namespace y
  {
      y_driver::y_driver()
        {
          root = new script;
          return;
        }

      y_driver::~y_driver()
        {
          delete this->root;

          return;
        }

      void y_driver::error(std::string msg)
        {
          ::error(msg);
        }

      script* y_driver::get_script()
        {
          return(this->root);
        }

      declaration* y_driver::make_field(lexeme::lexeme<enum keyword_type, enum character_type>* lex, attributes* a)
        {
          // extract identifier name from lexeme
          std::string  id;
          bool         ok = lex->get_identifier(id);
			    declaration* d  = NULL;

          if(ok)
            {
              quantity s(id, *a, lex->get_line(), lex->get_path());

              d = new field_declaration(s, lex->get_line(), lex->get_path());
            }
          else
            {
              ::error(ERROR_IDENTIFIER_LOOKUP, lex->get_line(), lex->get_path());
            }

          delete a; // otherwise, attributes block would never be deallocated
			 
			 return(d);
        }

      declaration* y_driver::make_parameter(lexeme::lexeme<enum keyword_type, enum character_type>* lex, attributes* a)
        {
          // extract identifier name from lexeme
          std::string  id;
          bool         ok = lex->get_identifier(id);
			    declaration* d  = NULL;

          if(ok)
            {
              quantity s(id, *a, lex->get_line(), lex->get_path());

              d = new parameter_declaration(s, lex->get_line(), lex->get_path());
            }
          else
            {
              ::error(ERROR_IDENTIFIER_LOOKUP, lex->get_line(), lex->get_path());
            }

          delete a; // otherwise, attributes block would never be deallocated
			 
			 return(d);
        }

      void y_driver::add_latex_attribute(attributes *a, lexeme::lexeme<keyword_type, character_type> *lex)
        {
          // extract string name from lexeme
          std::string latex_name;
          bool        ok = lex->get_string(latex_name);

          if(ok)
            {
              a->set_latex(latex_name);
            }
          else
            {
              ::error(ERROR_STRING_LOOKUP, lex->get_line(), lex->get_path());
            }
        }

      void y_driver::set_name(lexeme::lexeme<keyword_type, character_type> *lex)
        {
          std::string str;
          bool        ok = lex->get_string(str);

          if(ok)
            {
              this->root->set_name(str);
            }
          else
            {
              ::error(ERROR_STRING_LOOKUP, lex->get_line(), lex->get_path());
            }
        }

      void y_driver::set_author(lexeme::lexeme<keyword_type, character_type> *lex)
        {
          std::string str;
          bool        ok = lex->get_string(str);

          if(ok)
            {
              this->root->set_author(str);
            }
          else
            {
              ::error(ERROR_STRING_LOOKUP, lex->get_line(), lex->get_path());
            }
        }

      void y_driver::set_tag(lexeme::lexeme<keyword_type, character_type> *lex)
        {
          std::string str;
          bool        ok = lex->get_string(str);

          if(ok)
            {
              this->root->set_tag(str);
            }
          else
            {
              ::error(ERROR_STRING_LOOKUP, lex->get_line(), lex->get_path());
            }
        }

      GiNaC::ex* y_driver::add(GiNaC::ex *l, GiNaC::ex *r)
        {
          GiNaC::ex* rval = new GiNaC::ex((*l) + (*r));

          delete l;
          delete r;

          return(rval);
        }

      GiNaC::ex* y_driver::sub(GiNaC::ex *l, GiNaC::ex *r)
        {
          GiNaC::ex* rval = new GiNaC::ex((*l) - (*r));

          delete l;
          delete r;

          return(rval);
        }

      GiNaC::ex* y_driver::mul(GiNaC::ex *l, GiNaC::ex *r)
        {
          GiNaC::ex* rval = new GiNaC::ex((*l) * (*r));

          delete l;
          delete r;

          return(rval);
        }

      GiNaC::ex* y_driver::div(GiNaC::ex *l, GiNaC::ex *r)
        {
          GiNaC::ex* rval = new GiNaC::ex((*l) / (*r));

          delete l;
          delete r;

          return(rval);
        }

      GiNaC::ex* y_driver::pow(GiNaC::ex *l, GiNaC::ex *r)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::pow((*l), (*r)));

          delete l;
          delete r;

          return(rval);
        }

      GiNaC::ex* y_driver::abs(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::abs(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::step(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::step(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::sqrt(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::sqrt(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::sin(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::sin(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::cos(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::cos(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::tan(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::tan(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::asin(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::asin(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::acos(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::acos(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::atan(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::atan(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::atan2(GiNaC::ex *a1, GiNaC::ex *a2)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::atan2(*a1, *a2));

          delete a1;
          delete a2;

          return(rval);
        }

      GiNaC::ex* y_driver::sinh(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::sinh(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::cosh(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::cosh(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::tanh(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::tanh(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::asinh(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::asinh(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::acosh(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::acosh(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::atanh(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::atanh(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::exp(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::exp(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::log(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::log(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::Li2(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::Li2(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::Li(GiNaC::ex *a1, GiNaC::ex *a2)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::Li(*a1, *a2));

          delete a1;
          delete a2;

          return(rval);
        }

      GiNaC::ex* y_driver::G(GiNaC::ex *a1, GiNaC::ex *a2)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::G(*a1, *a2));

          delete a1;
          delete a2;

          return(rval);
        }

      GiNaC::ex* y_driver::G(GiNaC::ex *a1, GiNaC::ex *a2, GiNaC::ex *a3)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::G(*a1, *a2, *a3));

          delete a1;
          delete a2;
          delete a3;

          return(rval);
        }

      GiNaC::ex* y_driver::S(GiNaC::ex *a1, GiNaC::ex *a2, GiNaC::ex *a3)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::S(*a1, *a2, *a3));

          delete a1;
          delete a2;
          delete a3;

          return(rval);
        }

      GiNaC::ex* y_driver::H(GiNaC::ex *a1, GiNaC::ex *a2)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::H(*a1, *a2));

          delete a1;
          delete a2;

          return(rval);
        }

      GiNaC::ex* y_driver::zeta(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::zeta(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::zeta(GiNaC::ex *a1, GiNaC::ex *a2)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::zeta(*a1, *a2));

          delete a1;
          delete a2;

          return(rval);
        }

      GiNaC::ex* y_driver::zetaderiv(GiNaC::ex *a1, GiNaC::ex *a2)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::zetaderiv(*a1, *a2));

          delete a1;
          delete a2;

          return(rval);
        }

      GiNaC::ex* y_driver::tgamma(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::tgamma(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::lgamma(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::lgamma(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::beta(GiNaC::ex *a1, GiNaC::ex *a2)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::beta(*a1, *a2));

          delete a1;
          delete a2;

          return(rval);
        }

      GiNaC::ex* y_driver::psi(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::psi(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::psi(GiNaC::ex *a1, GiNaC::ex *a2)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::psi(*a1, *a2));

          delete a1;
          delete a2;

          return(rval);
        }

      GiNaC::ex* y_driver::factorial(GiNaC::ex *arg)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::factorial(*arg));

          delete arg;

          return(rval);
        }

      GiNaC::ex* y_driver::binomial(GiNaC::ex *a1, GiNaC::ex *a2)
        {
          GiNaC::ex* rval = new GiNaC::ex(GiNaC::binomial(*a1, *a2));

          delete a1;
          delete a2;

          return(rval);
        }

      GiNaC::ex* y_driver::get_integer(lexeme::lexeme<keyword_type, character_type> *lex)
        {
          int  i  = 1;
          bool ok = lex->get_integer(i);

          GiNaC::ex* rval = new GiNaC::ex(i);

          if(!ok)
            {
              ::error(ERROR_INTEGER_LOOKUP, lex->get_line(), lex->get_path());
            }

          return(rval);
        }

      GiNaC::ex* y_driver::get_decimal(lexeme::lexeme<keyword_type, character_type> *lex)
        {
          double d  = 1.0;
          bool   ok = lex->get_decimal(d);

          GiNaC::ex* rval = new GiNaC::ex(d);

          if(!ok)
            {
              ::error(ERROR_DECIMAL_LOOKUP, lex->get_line(), lex->get_path());
            }

          return(rval);
        }

      GiNaC::ex* y_driver::get_identifier(lexeme::lexeme<keyword_type, character_type> *lex)
        {
          std::string id;
          bool        ok = lex->get_identifier(id);

          GiNaC::ex*  rval = NULL;

          if(ok)
            {
              quantity* s = NULL;

              bool exists = this->root->lookup_symbol(id, s);

              if(exists)
                {
                  rval = new GiNaC::ex(*(s->get_ginac_symbol()));
                }
              else
                {
                  std::ostringstream msg;

                  msg << ERROR_UNKNOWN_IDENTIFIER << " '" << id << "'";
                  ::error(msg.str(), lex->get_line(), lex->get_path());
                }
            }
          else
            {
              ::error(ERROR_IDENTIFIER_LOOKUP, lex->get_line(), lex->get_path());
            }

          if(rval == NULL)
            {
              rval = new GiNaC::ex(1);
            }

          return(rval);
        }
  }
