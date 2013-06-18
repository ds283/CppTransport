//
// Created by David Seery on 13/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __lexeme_H_
#define __lexeme_H_

#include <assert.h>

#include <iostream>
#include <deque>
#include <string>
#include <sstream>
#include <cstdio>

#include "core.h"
#include "error.h"

enum lexeme_buffer_type
  {
    buf_string, buf_number, buf_symbol, buf_string_literal
  };

enum lexeme_type
  {
    keyword, symbol, ident, integer, decimal, string, unknown
  };

template <class keywords, class symbols>
class lexeme
  {
    public:
      lexeme(std::string buffer, enum lexeme_buffer_type t,
				 std::deque<struct inclusion> p, unsigned int l, unsigned int u,
             std::string* kt, keywords* km, unsigned int num_k,
             std::string* st, symbols*  sm, unsigned int num_s);
      ~lexeme();

      // write details to specified output stream
      void dump(std::ostream& stream);

      // get information
      enum lexeme_type  get_type();
      bool              get_keyword   (keywords& keyword);
      bool              get_symbol    (symbols& symbol);
      bool              get_identifier(std::string& id);
      bool              get_integer   (int& z);
      bool              get_decimal   (double& d);
      bool              get_string    (std::string& str);

    private:
      enum lexeme_type             type;
      unsigned int                 unique;

      keywords                     k;
      symbols                      s;
      int                          z;
      double                       d;
      std::string                  str;

      std::deque<struct inclusion> path;
      unsigned int                 line;

      std::string*                 ktable;
      keywords*                    kmap;
      unsigned int                 Nk;

      std::string*                 stable;
      symbols*                     smap;
      unsigned int                 Ns;
  };

// IMPLEMENTATION

template <class keywords, class symbols>
lexeme<keywords, symbols>::lexeme(std::string buffer, enum lexeme_buffer_type t,
                                  std::deque<struct inclusion> p, unsigned int l, unsigned int u,
                                  std::string* kt, keywords* km, unsigned int num_k,
                                  std::string* st, symbols* sm, unsigned int num_s)
  : path(p), line(l), unique(u),
    ktable(kt), kmap(km), Nk(num_k),
    stable(st), smap(sm), Ns(num_s)
  {
    bool ok     = false;
    int  offset = 0;

    switch(t)
      {
        case buf_string:
          type = ident;
          str  = buffer;

          for(int i = 0; i < Nk; i++)
            {
              if(buffer == ktable[i])
                {
                  type = keyword;
                  k    = km[i];
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

          for(int i = 0; i < Ns; i++)
            {
              if(buffer == stable[i])
                {
                  type = symbol;
                  s    = smap[i];
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
  }

template <class keywords, class symbols>
lexeme<keywords, symbols>::~lexeme()
  {
    return;
  }


// ******************************************************************


template <class keywords, class symbols>
void lexeme<keywords, symbols>::dump(std::ostream& stream)
  {
    stream << "Lexeme " << this->unique << " = ";

    switch(type)
      {
        case keyword:
          stream << "keyword ";

          assert(this->k >= 0);
          assert(this->k < this->Nk);

          stream << "'" << this->ktable[(int)this->k] << "'\n";
          break;

        case symbol:
          stream << "symbol ";

          assert(this->s >= 0);
          assert(this->s < this->Ns);

          stream << "'" << this->stable[(int)this->s] << "'\n";
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


template <class keywords, class symbols>
enum lexeme_type lexeme<keywords, symbols>::get_type()
  {
    return this->type;
  }

template <class keywords, class symbols>
bool lexeme<keywords, symbols>::get_keyword(keywords& kw)
  {
    bool rval = false;

    if(this->type == keyword)
      {
        kw   = this->k;
        rval = true;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_LEXEME_KEYWORD << " (id " << this->unique << ")";
        warn(msg.str(), this->line, this->path);
      }

    return(rval);
  }

template <class keywords, class symbols>
bool lexeme<keywords, symbols>::get_symbol(symbols& sym)
  {
    bool rval = false;

    if(this->type == symbol)
      {
        sym  = this->s;
        rval = true;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_LEXEME_SYMBOL << " (id " << this->unique << ")";
        warn(msg.str(), this->line, this->path);
      }

    return(rval);
  }

template <class keywords, class symbols>
bool lexeme<keywords, symbols>::get_identifier(std::string& id)
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

template <class keywords, class symbols>
bool lexeme<keywords, symbols>::get_integer(int& z)
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

template <class keywords, class symbols>
bool lexeme<keywords, symbols>::get_decimal(double& d)
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

template <class keywords, class symbols>
bool lexeme<keywords, symbols>::get_string(std::string& str)
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


#endif //__lexeme_H_
