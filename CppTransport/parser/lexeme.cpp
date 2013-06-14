//
// Created by David Seery on 13/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include "lexeme.h"


// ******************************************************************


lexeme::lexeme(enum keyword_type kw, std::deque<struct inclusion> p, unsigned int l)
  : path(p), line(l), i(NULL)
  {
    type = keyword;
    k    = kw;
  }

lexeme::lexeme(enum symbol_type sym, std::deque<struct inclusion> p, unsigned int l)
  : path(p), line(l), i(NULL)
  {
    type = symbol;
    s    = sym;
  }

lexeme::lexeme(identifier& id, std::deque<struct inclusion> p, unsigned int l)
  : path(p), line(l)
  {
    identifier* copy_id = new identifier(id);

    type = ident;
    i    = copy_id;
  }

lexeme::lexeme(int intg, std::deque<struct inclusion> p, unsigned int l)
  : path(p), line(l), i(NULL)
  {
    type = integer;
    z    = intg;
  }

lexeme::lexeme(double decm, std::deque<struct inclusion> p, unsigned int l)
: path(p), line(l), i(NULL)
  {
    type = decimal;
    d    = decm;
  }

lexeme::lexeme(std::string strg, std::deque<struct inclusion> p, unsigned int l)
: path(p), line(l), i(NULL)
  {
    type = keyword;
    str  = strg;
  }

lexeme::~lexeme()
  {
    // delete identifier, if it is present
    if(this->i != NULL)
      {
        delete this->i;
      }
  }


// ******************************************************************


identifier::identifier(std::string id)
  : name(id)
  {
    return;
  }

identifier::~identifier()
  {
    return;
  }