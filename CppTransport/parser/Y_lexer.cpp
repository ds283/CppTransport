//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <iostream>
#include <assert.h>

#include "y_lexer.h"

enum y::y_parser::token::yytokentype keyword_tokens[] =
  {
      y::y_parser::token::name,
      y::y_parser::token::author,
      y::y_parser::token::tag,
      y::y_parser::token::field,
      y::y_parser::token::potential,
      y::y_parser::token::parameter,
      y::y_parser::token::latex,
      y::y_parser::token::k_class,
      y::y_parser::token::model,
      y::y_parser::token::abs,
      y::y_parser::token::step,
      y::y_parser::token::sqrt,
      y::y_parser::token::sin,
      y::y_parser::token::cos,
      y::y_parser::token::tan,
      y::y_parser::token::asin,
      y::y_parser::token::acos,
      y::y_parser::token::atan,
      y::y_parser::token::atan2,
      y::y_parser::token::sinh,
      y::y_parser::token::cosh,
      y::y_parser::token::tanh,
      y::y_parser::token::asinh,
      y::y_parser::token::acosh,
      y::y_parser::token::atanh,
      y::y_parser::token::exp,
      y::y_parser::token::log,
      y::y_parser::token::Li2,
      y::y_parser::token::Li,
      y::y_parser::token::G,
      y::y_parser::token::S,
      y::y_parser::token::H,
      y::y_parser::token::zeta,
      y::y_parser::token::zetaderiv,
      y::y_parser::token::tgamma,
      y::y_parser::token::lgamma,
      y::y_parser::token::beta,
      y::y_parser::token::psi,
      y::y_parser::token::factorial,
      y::y_parser::token::binomial
  };

enum y::y_parser::token::yytokentype symbol_tokens[] =
  {
      y::y_parser::token::open_brace,
      y::y_parser::token::close_brace,
      y::y_parser::token::open_bracket,
      y::y_parser::token::close_bracket,
      y::y_parser::token::open_square,
      y::y_parser::token::close_square,
      y::y_parser::token::comma,
      y::y_parser::token::period,
      y::y_parser::token::colon,
      y::y_parser::token::semicolon,
      y::y_parser::token::plus,
      y::y_parser::token::minus,
      y::y_parser::token::star,
      y::y_parser::token::backslash,
      y::y_parser::token::foreslash,
      y::y_parser::token::tilde,
      y::y_parser::token::ampersand,
      y::y_parser::token::circumflex,
      y::y_parser::token::ampersat,
      y::y_parser::token::ellipsis,
      y::y_parser::token::rightarrow
  };

namespace y
  {

    y_lexer::y_lexer(lexstream<enum keyword_type, enum character_type>* s)
      : stream(s)
      {
        assert(s != NULL);

        stream->reset();
      }

    y_lexer::~y_lexer()
      {
        return;
      }

    int y_lexer::yylex(y::y_parser::semantic_type* lval)
      {
        assert(lval != NULL);

        lval->lex = this->stream->get();
        this->stream->eat();

        int     rval = 0;

        if(lval->lex != NULL)
          {
            enum lexeme::lexeme_type type = lval->lex->get_type();

            switch(type)
              {
                case lexeme::keyword:
                  {
                    enum keyword_type kw;

                    if(lval->lex->get_keyword(kw))
                      {
                        rval = keyword_tokens[(int)kw];
                      }
                  }
                  break;

                case lexeme::character:
                  {
                    enum character_type sym;

                    if(lval->lex->get_symbol(sym))
                      {
                        rval = symbol_tokens[(int)sym];
                      }
                  }
                  break;

                case lexeme::ident:
                  rval = y::y_parser::token::identifier;
                  break;

                case lexeme::integer:
                  rval = y::y_parser::token::integer;
                  break;

                case lexeme::decimal:
                  rval = y::y_parser::token::decimal;
                  break;

                case lexeme::string:
                  rval = y::y_parser::token::string;
                  break;

                default:
                  assert(false);
              }
          }
        else
          {
            /* return eof? */
          }

        // std::cerr << "Returning token number " << rval << "\n";

        return(rval);
      }

  }
