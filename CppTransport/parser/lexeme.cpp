//
// Created by David Seery on 13/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>
#include <sstream>
#include <cstdio>

#include "lexeme.h"
#include "error.h"

// ******************************************************************


static unsigned int unique_count = 0;

std::string keyword_table[]
                = { "name", "author", "tag", "field", "potential", "parameter", "latex",
                    "abs", "step", "sqrt", "sin", "cos", "tan",
                    "asin", "acos", "atan", "atan2", "sinh", "cosh", "tanh",
                    "asinh", "acosh", "atanh", "exp", "log", "Li2", "Li", "G", "S", "H",
                    "zeta", "zetaderiv", "tgamma", "lgamma", "beta", "psi", "factorial", "binomial" };

enum keyword_type keyword_map[]
                = { name, author, tag, field, potential, parameter,
                    f_abs, f_step, f_sqrt,
                    f_sin, f_cos, f_tan,
                    f_asin, f_acos, f_atan, f_atan2,
                    f_sinh, f_cosh, f_tanh,
                    f_asinh, f_acosh, f_atanh,
                    f_exp, f_log, f_Li2, f_Li, f_G, f_S, f_H,
                    f_zeta, f_zetaderiv, f_tgamma, f_lgamma,
                    f_beta, f_psi, f_factorial, f_binomial };

std::string symbol_table[]
                = { "{", "}", "(", ")",
                    "[", "]", ",", ".", ":", ";",
                    "+", "-", "*", "/", "\\", "~",
                    "&", "^", "@", "...", "->" };

enum symbol_type symbol_map[]
                = { open_brace, close_brace, open_bracket, close_bracket,
                    open_square, close_square, comma, period, colon, semicolon,
                    plus, minus, star, backslash, foreslash, tilde,
                    ampersand, circumflex, ampersat, ellipsis, rightarrow };

// ******************************************************************


lexeme::lexeme(std::string buffer, enum lexeme_buffer_type t, std::deque<struct inclusion> p, unsigned int l)
  : path(p), line(l)
  {
    bool ok     = false;
    int  offset = 0;

    switch(t)
      {
        case buf_string:
          type = ident;
          str  = buffer;

          for(int i = 0; i < NUMBER_KEYWORDS; i++)
            {
              if(buffer == keyword_table[i])
                {
                  type = keyword;
                  k    = keyword_map[i];
                  str  = "";
                }
            }
          break;

        case buf_number:
          type = unknown;

          if((sscanf(buffer.c_str(), "%i%n", &z, &offset) == 1) && offset == buffer.length())
            {
              type = integer;
            }
          else if((sscanf(buffer.c_str(), "%lf%n", &d, &offset) == 1) && offset == buffer.length())
            {
              type = decimal;
            }
          else if((sscanf(buffer.c_str(), "%g%n", &d, &offset) == 1) && offset == buffer.length())
            {
              type = decimal;
            }
          else if((sscanf(buffer.c_str(), "%G%n", &d, &offset) == 1) && offset == buffer.length())
            {
              type = decimal;
            }
          else
            {
              std::ostringstream msg;
              msg << ERROR_UNRECOGNIZED_NUMBER << " '" << buffer << "'";
              error(msg.str(), l, p);
            }

          if(type == decimal && buffer[0] == '0' && buffer[1] == 'x')
            {
              std::ostringstream msg;
              msg << WARNING_HEX_CONVERSION_A << " '" << buffer << "' " << WARNING_HEX_CONVERSION_B;
              warn(msg.str(), l, p);
            }
          else if(type == decimal && buffer[0] == '0')
            {
              std::ostringstream msg;
              msg << WARNING_OCTAL_CONVERSION_A << " '" << buffer << "' " << WARNING_OCTAL_CONVESRION_B;
              warn(msg.str(), l, p);
            }
          break;

        case buf_symbol:
          type = unknown;

          for(int i = 0; i < NUMBER_SYMBOLS; i++)
            {
              if(buffer == symbol_table[i])
                {
                  type = symbol;
                  s    = symbol_map[i];
                  ok   = true;
                }
            }

          if(ok == false)
            {
              std::ostringstream msg;
              msg << ERROR_UNRECOGNIZED_SYMBOL << " '" << buffer << "'";
              error(msg.str(), l, p);
            }
          break;

        case buf_string_literal:
          type = string;
          str  = buffer;
          break;

        default:
          assert(false);
      }

    unique = unique_count++;
  }

lexeme::~lexeme()
  {
    return;
  }


// ******************************************************************


void lexeme::dump(std::ostream& stream)
  {
    stream << "Lexeme " << this->unique << " = ";

    switch(type)
      {
        case keyword:
          stream << "keyword ";

          assert(this->k >= 0);
          assert(this->k < NUMBER_KEYWORDS);

          stream << "'" << keyword_table[(int)this->k] << "'\n";
          break;

        case symbol:
          stream << "symbol ";

          assert(this->s >= 0);
          assert(this->s < NUMBER_SYMBOLS);

          stream << "'" << symbol_table[(int)this->s] << "'\n";
          break;

        case ident:
          stream << "identifier '" << this->str << "'\n";
          break;

        case integer:
          stream << "integer literal '" << this->z << "'\n";
          break;

        case decimal:
          stream << "decimal literal '" << this->d << "'\n";
          break;

        case string:
          stream << "string literal '" << this->str << "'\n";
          break;

        case unknown:
          stream << "UNKNOWN LEXEME\n";
          break;

        default:
          assert(false);
      }
  }


// ******************************************************************


enum lexeme_type lexeme::get_type()
  {
    return this->type;
  }

bool lexeme::get_keyword(enum keyword_type& keyword)
  {
    bool rval = false;

    if(this->type == keyword)
      {
        keyword = this->k;
        rval    = true;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_LEXEME_KEYWORD << " (id " << this->unique << ")";
        warn(msg.str(), this->line, this->path);
      }

    return(rval);
  }

bool lexeme::get_symbol(enum symbol_type& symbol)
  {
    bool rval = false;

    if(this->type == symbol)
      {
        symbol = this->s;
        rval   = true;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_LEXEME_SYMBOL << " (id " << this->unique << ")";
        warn(msg.str(), this->line, this->path);
      }

    return(rval);
  }

bool lexeme::get_identifier(std::string& id)
  {
    bool rval = false;

    if(this->type == ident)
      {
        id   = this->str;
        rval = true;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_LEXEME_IDENTIFIER << " (id " << this->unique << ")";
        warn(msg.str(), this->line, this->path);
      }

    return(rval);
  }

bool lexeme::get_integer(int& z)
  {
    bool rval = false;

    if(this->type == z)
      {
        z    = this->z;
        rval = true;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_LEXEME_INTEGER << " (id " << this->unique << ")";
        warn(msg.str(), this->line, this->path);
      }

    return(rval);
  }

bool lexeme::get_decimal(double& d)
  {
    bool rval = false;

    if(this->type == d)
      {
        d    = this->d;
        rval = true;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_LEXEME_DECIMAL << " (id " << this->unique << ")";
        warn(msg.str(), this->line, this->path);
      }

    return(rval);
  }

bool lexeme::get_string(std::string& str)
  {
    bool rval = false;

    if(this->type == string)
      {
        str  = this->str;
        rval = true;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_LEXEME_STRING << " (id " << this->unique << ")";
        warn(msg.str(), this->line, this->path);
      }

    return(rval);
  }
