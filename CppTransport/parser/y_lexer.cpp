//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#include <iostream>

#include <unordered_map>

#include "lexical.h"
#include "y_lexer.h"


std::unordered_map<enum keyword_type, enum y::y_parser::token::yytokentype> keyword_tokens =
  {
    { keyword_type::name,             y::y_parser::token::name },
    { keyword_type::author,           y::y_parser::token::author },
    { keyword_type::citeguide,        y::y_parser::token::citeguide },
    { keyword_type::description,      y::y_parser::token::description },
    { keyword_type::license,          y::y_parser::token::license },
    { keyword_type::revision,         y::y_parser::token::revision },
    { keyword_type::references,       y::y_parser::token::references },
    { keyword_type::urls,             y::y_parser::token::urls },
    { keyword_type::email,            y::y_parser::token::email },
    { keyword_type::institute,        y::y_parser::token::institute },
    { keyword_type::field,            y::y_parser::token::field },
    { keyword_type::potential,        y::y_parser::token::potential },
    { keyword_type::subexpr,          y::y_parser::token::subexpr },
    { keyword_type::value,            y::y_parser::token::value },
    { keyword_type::parameter,        y::y_parser::token::parameter },
    { keyword_type::latex,            y::y_parser::token::latex },
    { keyword_type::templates,        y::y_parser::token::templates },
    { keyword_type::settings,         y::y_parser::token::settings },
    { keyword_type::core,             y::y_parser::token::core },
    { keyword_type::impl,             y::y_parser::token::implementation },
    { keyword_type::model,            y::y_parser::token::model },
    { keyword_type::abserr,           y::y_parser::token::abserr },
    { keyword_type::relerr,           y::y_parser::token::relerr },
    { keyword_type::stepper,          y::y_parser::token::stepper },
    { keyword_type::stepsize,         y::y_parser::token::stepsize },
    { keyword_type::background,       y::y_parser::token::background },
    { keyword_type::perturbations,    y::y_parser::token::perturbations },
    { keyword_type::indexorder,       y::y_parser::token::indexorder },
    { keyword_type::left,             y::y_parser::token::left },
    { keyword_type::right,            y::y_parser::token::right },
    { keyword_type::abs,              y::y_parser::token::abs },
    { keyword_type::step,             y::y_parser::token::step },
    { keyword_type::sqrt,             y::y_parser::token::sqrt },
    { keyword_type::sin,              y::y_parser::token::sin },
    { keyword_type::cos,              y::y_parser::token::cos },
    { keyword_type::tan,              y::y_parser::token::tan },
    { keyword_type::asin,             y::y_parser::token::asin },
    { keyword_type::acos,             y::y_parser::token::acos },
    { keyword_type::atan,             y::y_parser::token::atan },
    { keyword_type::atan2,            y::y_parser::token::atan2 },
    { keyword_type::sinh,             y::y_parser::token::sinh },
    { keyword_type::cosh,             y::y_parser::token::cosh },
    { keyword_type::tanh,             y::y_parser::token::tanh },
    { keyword_type::asinh,            y::y_parser::token::asinh },
    { keyword_type::acosh,            y::y_parser::token::acosh },
    { keyword_type::atanh,            y::y_parser::token::atanh },
    { keyword_type::exp,              y::y_parser::token::exp },
    { keyword_type::log,              y::y_parser::token::log },
    { keyword_type::pow,              y::y_parser::token::pow },
    { keyword_type::Li2,              y::y_parser::token::Li2 },
    { keyword_type::Li,               y::y_parser::token::Li },
    { keyword_type::G,                y::y_parser::token::G },
    { keyword_type::S,                y::y_parser::token::S },
    { keyword_type::H,                y::y_parser::token::H },
    { keyword_type::zeta,             y::y_parser::token::zeta },
    { keyword_type::zetaderiv,        y::y_parser::token::zetaderiv },
    { keyword_type::tgamma,           y::y_parser::token::tgamma },
    { keyword_type::lgamma,           y::y_parser::token::lgamma },
    { keyword_type::beta,             y::y_parser::token::beta },
    { keyword_type::psi,              y::y_parser::token::psi },
    { keyword_type::factorial,        y::y_parser::token::factorial },
    { keyword_type::binomial,         y::y_parser::token::binomial }
  };

std::unordered_map<enum character_type, enum y::y_parser::token::yytokentype> symbol_tokens =
  {
    { character_type::open_brace,     y::y_parser::token::open_brace },
    { character_type::close_brace,    y::y_parser::token::close_brace },
    { character_type::open_bracket,   y::y_parser::token::open_bracket },
    { character_type::close_bracket,  y::y_parser::token::close_bracket },
    { character_type::open_square,    y::y_parser::token::open_square },
    { character_type::close_square,   y::y_parser::token::close_square },
    { character_type::comma,          y::y_parser::token::comma },
    { character_type::period,         y::y_parser::token::period },
    { character_type::colon,          y::y_parser::token::colon },
    { character_type::semicolon,      y::y_parser::token::semicolon },
    { character_type::equals,         y::y_parser::token::equals },
    { character_type::plus,           y::y_parser::token::plus },
    { character_type::binary_minus,   y::y_parser::token::binary_minus },
    { character_type::unary_minus,    y::y_parser::token::unary_minus },
    { character_type::star,           y::y_parser::token::star },
    { character_type::backslash,      y::y_parser::token::backslash },
    { character_type::foreslash,      y::y_parser::token::foreslash },
    { character_type::tilde,          y::y_parser::token::tilde },
    { character_type::ampersand,      y::y_parser::token::ampersand },
    { character_type::circumflex,     y::y_parser::token::circumflex },
    { character_type::ampersat,       y::y_parser::token::ampersat },
    { character_type::ellipsis,       y::y_parser::token::ellipsis },
    { character_type::rightarrow,     y::y_parser::token::rightarrow }
  };

namespace y
  {

    y_lexer::y_lexer(lexstream_type& s)
      : stream(s)
      {
        stream.reset();
		    current_lex = nullptr;
      }

    int y_lexer::yylex(y::y_parser::semantic_type* lval)
      {
        assert(lval != nullptr);

        lval->lex = this->current_lex = this->stream.get();
        this->stream.eat();

        int     rval = 0;

        if(lval->lex != nullptr)
          {
            enum lexeme::lexeme_type type = lval->lex->get_type();

            switch(type)
              {
                case lexeme::lexeme_type::keyword:
                  {
                    enum keyword_type kw;

                    if(lval->lex->get_keyword(kw))
                      {
                        rval = keyword_tokens[kw];
                      }
                  }
                  break;

                case lexeme::lexeme_type::character:
                  {
                    enum character_type sym;

                    if(lval->lex->get_symbol(sym))
                      {
                        rval = symbol_tokens[sym];
                      }
                  }
                  break;

                case lexeme::lexeme_type::identifier:
                  rval = y::y_parser::token::identifier;
                  break;

                case lexeme::lexeme_type::integer:
                  rval = y::y_parser::token::integer;
                  break;

                case lexeme::lexeme_type::decimal:
                  rval = y::y_parser::token::decimal;
                  break;

                case lexeme::lexeme_type::string:
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

        // std::cerr << "Returning token number " << rval << '\n';

        return(rval);
      }

  }
