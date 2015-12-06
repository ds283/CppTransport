//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <iostream>

#include <unordered_map>

#include "lexical.h"
#include "y_lexer.h"

std::unordered_map<enum keyword_type, enum y::y_parser::token::yytokentype> keyword_tokens =
  {
    { keyword_type::name,             y::y_parser::token::name },
    { keyword_type::author,           y::y_parser::token::author },
    { keyword_type::tag,              y::y_parser::token::tag },
    { keyword_type::field,            y::y_parser::token::field },
    { keyword_type::potential,        y::y_parser::token::potential },
    { keyword_type::subexpr,          y::y_parser::token::subexpr },
    { keyword_type::value,            y::y_parser::token::value },
    { keyword_type::parameter,        y::y_parser::token::parameter },
    { keyword_type::latex,            y::y_parser::token::latex },
    { keyword_type::core,             y::y_parser::token::core },
    { keyword_type::impl,             y::y_parser::token::implementation },
    { keyword_type::model,            y::y_parser::token::model },
    { keyword_type::abserr,           y::y_parser::token::abserr },
    { keyword_type::relerr,           y::y_parser::token::relerr },
    { keyword_type::k_stepper,        y::y_parser::token::stepper },
    { keyword_type::stepsize,         y::y_parser::token::stepsize },
    { keyword_type::background,       y::y_parser::token::background },
    { keyword_type::perturbations,    y::y_parser::token::perturbations },
    { keyword_type::indexorder,       y::y_parser::token::indexorder },
    { keyword_type::left,             y::y_parser::token::left },
    { keyword_type::right,            y::y_parser::token::right },
    { keyword_type::f_abs,            y::y_parser::token::abs },
    { keyword_type::f_step,           y::y_parser::token::step },
    { keyword_type::f_sqrt,           y::y_parser::token::sqrt },
    { keyword_type::f_sin,            y::y_parser::token::sin },
    { keyword_type::f_cos,            y::y_parser::token::cos },
    { keyword_type::f_tan,            y::y_parser::token::tan },
    { keyword_type::f_asin,           y::y_parser::token::asin },
    { keyword_type::f_acos,           y::y_parser::token::acos },
    { keyword_type::f_atan,           y::y_parser::token::atan },
    { keyword_type::f_atan2,          y::y_parser::token::atan2 },
    { keyword_type::f_sinh,           y::y_parser::token::sinh },
    { keyword_type::f_cosh,           y::y_parser::token::cosh },
    { keyword_type::f_tanh,           y::y_parser::token::tanh },
    { keyword_type::f_asinh,          y::y_parser::token::asinh },
    { keyword_type::f_acosh,          y::y_parser::token::acosh },
    { keyword_type::f_atanh,          y::y_parser::token::atanh },
    { keyword_type::f_exp,            y::y_parser::token::exp },
    { keyword_type::f_log,            y::y_parser::token::log },
    { keyword_type::f_pow,            y::y_parser::token::pow },
    { keyword_type::f_Li2,            y::y_parser::token::Li2 },
    { keyword_type::f_Li,             y::y_parser::token::Li },
    { keyword_type::f_G,              y::y_parser::token::G },
    { keyword_type::f_S,              y::y_parser::token::S },
    { keyword_type::f_H,              y::y_parser::token::H },
    { keyword_type::f_zeta,           y::y_parser::token::zeta },
    { keyword_type::f_zetaderiv,      y::y_parser::token::zetaderiv },
    { keyword_type::f_tgamma,         y::y_parser::token::tgamma },
    { keyword_type::f_lgamma,         y::y_parser::token::lgamma },
    { keyword_type::f_beta,           y::y_parser::token::beta },
    { keyword_type::f_psi,            y::y_parser::token::psi },
    { keyword_type::f_factorial,      y::y_parser::token::factorial },
    { keyword_type::f_binomial,       y::y_parser::token::binomial }
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
