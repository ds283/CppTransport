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
                = { "name", "author", "tag", "field", "potential", "parameter" };

enum keyword_type keyword_map[]
                = { name, author, tag, field, potential, parameter };

std::string symbol_table[]
                = { "{", "}", "(", ")",
                    "[", "]", ",", ".", ";",
                    "+", "-", "*", "/", "\\", "~",
                    "&", "^", "@", "...", "->" };

enum symbol_type symbol_map[]
                = { open_brace, close_brace, open_bracket, close_bracket,
                    open_square, close_square, comma, period, semicolon,
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
    bool ok = false;

    stream << "Lexeme " << this->unique << " = ";

    switch(type)
      {
        case keyword:
          stream << "keyword ";
          for(int i = 0; i < NUMBER_KEYWORDS; i++)
            {
              if(keyword_map[i] == this->k)
                {
                  stream << "'" << keyword_table[i] << "'\n";
                  ok = true;
                }
            }
          if(ok == false)
            {
              stream << "UNKNOWN! (" << this->k << ")\n";
            }
          break;

        case symbol:
          stream << "symbol ";
          for(int i = 0; i < NUMBER_SYMBOLS; i++)
            {
              if(symbol_map[i] == this->s)
                {
                  stream << "'" << symbol_table[i] << "'\n";
                  ok = true;
                }
            }
          if(ok == false)
            {
              stream << "UNKNOWN! (" << this->s << ")\n";
            }
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
