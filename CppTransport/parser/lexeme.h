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
    name, author, tag, field, potential, parameter
  };

#define NUMBER_KEYWORDS (6)

enum symbol_type
  {
    open_brace, close_brace, open_bracket, close_bracket,
    open_square, close_square, comma, period, semicolon, plus, minus, star, backslash, foreslash, tilde,
    ampersand, circumflex, ampersat, ellipsis, rightarrow
  };

#define NUMBER_SYMBOLS (22)

class lexeme
  {
    public:
      lexeme(std::string buffer, enum lexeme_buffer_type t, std::deque<struct inclusion> p, unsigned int l);
      ~lexeme();

      // write details to specified output stream
      void dump(std::ostream& stream);

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
