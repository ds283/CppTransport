//
// Created by David Seery on 13/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __lexeme_H_
#define __lexeme_H_

#include <iostream>
#include <deque>
#include <string>

#include "core.h"

enum lexeme_buffer_type
  {
    buf_string, buf_number, buf_symbol, buf_string_literal
  };

enum lexeme_type
  {
    keyword, symbol, ident, integer, decimal, string, unknown
  };

enum keyword_type
  {
    name, author, tag, field, potential, parameter, latex,
    f_abs, f_step, f_sqrt,
    f_sin, f_cos, f_tan,
    f_asin, f_acos, f_atan, f_atan2,
    f_sinh, f_cosh, f_tanh,
    f_asinh, f_acosh, f_atanh,
    f_exp, f_log, f_Li2, f_Li, f_G, f_S, f_H,
    f_zeta, f_zetaderiv, f_tgamma, f_lgamma,
    f_beta, f_psi, f_factorial, f_binomial
  };

#define NUMBER_KEYWORDS (38)

enum symbol_type
  {
    open_brace, close_brace, open_bracket, close_bracket,
    open_square, close_square, comma, period, colon, semicolon,
    plus, minus, star, backslash, foreslash, tilde,
    ampersand, circumflex, ampersat, ellipsis, rightarrow
  };

#define NUMBER_SYMBOLS (23)

class lexeme
  {
    public:
      lexeme(std::string buffer, enum lexeme_buffer_type t, std::deque<struct inclusion> p, unsigned int l);
      ~lexeme();

      // write details to specified output stream
      void dump(std::ostream& stream);

      // get information
      enum lexeme_type  get_type();
      bool              get_keyword   (enum keyword_type& keyword);
      bool              get_symbol    (enum symbol_type& symbol);
      bool              get_identifier(std::string& id);
      bool              get_integer   (int& z);
      bool              get_decimal   (double& d);
      bool              get_string    (std::string& str);

    private:
      enum lexeme_type  type;
      unsigned int      unique;

      enum keyword_type k;
      enum symbol_type  s;
      int               z;
      double            d;
      std::string       str;

      std::deque<struct inclusion> path;
      unsigned int                 line;
  };


#endif //__lexeme_H_
