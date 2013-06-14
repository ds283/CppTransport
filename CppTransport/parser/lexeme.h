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

enum lexeme_type
  {
    keyword, symbol, ident, integer, decimal, string
  };

enum keyword_type
  {
    name, author, tag, field, potential, parameter
  };

enum symbol_type
  {
    open_brace, close_brace, open_bracket, close_bracket,
    open_square, close_square, comma, period, semicolon, plus, minus, star, backslash, foreslash, tilde,
    ampersand, dollar, percent, circumflex, ampersat
  };

class identifier
  {
    public:
      identifier(std::string id);
      ~identifier();

    private:
      std::string name;
  };

class lexeme
  {
    public:
      lexeme(enum keyword_type kw, std::deque<struct inclusion> p, unsigned int l);
      lexeme(enum symbol_type sym, std::deque<struct inclusion> p, unsigned int l);
      lexeme(identifier& id,       std::deque<struct inclusion> p, unsigned int l);
      lexeme(int intg,             std::deque<struct inclusion> p, unsigned int l);
      lexeme(double decm,          std::deque<struct inclusion> p, unsigned int l);
      lexeme(std::string strg,     std::deque<struct inclusion> p, unsigned int l);

      ~lexeme();

    private:
      enum lexeme_type type;

      enum keyword_type k;
      enum symbol_type  s;
      identifier*       i;
      int               z;
      double            d;
      std::string       str;

      std::deque<struct inclusion> path;
      unsigned int                 line;
  };


#endif //__lexeme_H_
