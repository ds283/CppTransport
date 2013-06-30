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

namespace lexeme    // package in a unique namespace to protect common words like 'keyword', 'character' used below
  {

      enum lexeme_buffer_type
        {
          buf_string, buf_number, buf_character, buf_string_literal
        };

      enum lexeme_type
        {
          keyword, character, ident, integer, decimal, string, unknown
        };

      template <class keywords, class characters>
      class lexeme
        {
          public:
          lexeme(const std::string buffer, const enum lexeme_buffer_type t,
                 const std::deque<struct inclusion> p, unsigned int l, unsigned int u,
                 const std::string* kt, const keywords* km, unsigned int num_k,
                 const std::string* ct, const characters* cm, unsigned int num_c);

          ~lexeme();

          // write details to specified output stream
          void dump(std::ostream& stream);

          // get information
          enum lexeme_type                    get_type();

          bool                                get_keyword(keywords& keyword);

          bool                                get_symbol(characters& symbol);

          bool                                get_identifier(std::string& id);

          bool                                get_integer(int& z);

          bool                                get_decimal(double& d);

          bool                                get_string(std::string& str);

          const unsigned int&                 get_line();

          const std::deque<struct inclusion>& get_path();

          private:
          enum lexeme_type                    type;
          const unsigned int                  unique;

          // lexeme value - not all of these are used by any single lexeme
          keywords                            k;
          characters                          s;
          int                                 z;
          double                              d;
          std::string                         str;

          // origin: line number and sequence of inclusions
          const std::deque<struct inclusion>  path;
          const unsigned int                  line;

          const std::string*                  ktable;
          const keywords*                     kmap;
          const unsigned int                  Nk;

          const std::string*                  ctable;
          const characters*                   cmap;
          unsigned int                        Nc;
        };

//  IMPLEMENTATION

      template <class keywords, class characters>
      lexeme<keywords, characters>::lexeme(const std::string buffer, const enum lexeme_buffer_type t,
                                           const std::deque<struct inclusion> p, unsigned int l, unsigned int u,
                                           const std::string* kt, const keywords* km, unsigned int num_k,
                                           const std::string* ct, const characters* cm, unsigned int num_c)
      : path(p), line(l), unique(u),
        ktable(kt), kmap(km), Nk(num_k),
        ctable(ct), cmap(cm), Nc(num_c)
        {
          bool ok     = false;
          int  offset = 0;

          switch (t)
            {
              case buf_string:
                type = ident;
                str  = buffer;

                for (int i = 0; i < Nk; i++)
                  {
                    if (buffer == ktable[i])
                      {
                        type = keyword;
                        k    = km[i];
                        str  = "";
                     }
                  }
                break;

              case buf_number:
                type = unknown;

                if ((sscanf(buffer.c_str(), "%i%n", &z, &offset) == 1) && offset == buffer.length())
                  {
                    type = integer;
                  }
                else if ((sscanf(buffer.c_str(), "%lf%n", &d, &offset) == 1) && offset == buffer.length())
                  {
                    type = decimal;
                  }
                else if ((sscanf(buffer.c_str(), "%lg%n", &d, &offset) == 1) && offset == buffer.length())
                  {
                    type = decimal;
                  }
                else if ((sscanf(buffer.c_str(), "%lG%n", &d, &offset) == 1) && offset == buffer.length())
                  {
                    type = decimal;
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << ERROR_UNRECOGNIZED_NUMBER << " '" << buffer << "'";
                    error(msg.str(), l, p);
                  }

                if (type == decimal && buffer[0] == '0' && buffer[1] == 'x')
                  {
                    std::ostringstream msg;
                    msg << WARNING_HEX_CONVERSION_A << " '" << buffer << "' " << WARNING_HEX_CONVERSION_B;
                    warn(msg.str(), l, p);
                  }
                else if (type == decimal && buffer[0] == '0')
                  {
                    std::ostringstream msg;
                    msg << WARNING_OCTAL_CONVERSION_A << " '" << buffer << "' " << WARNING_OCTAL_CONVERSION_B;
                    warn(msg.str(), l, p);
                  }
                break;

              case buf_character:
                type = unknown;

                for(int i = 0; i < Nc; i++)
                  {
                    if(buffer == ctable[i])
                      {
                        type = character;
                        s    = cmap[i];
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
                str    = buffer;
                break;

              default:
                assert(false);
            }
        }

      template <class keywords, class characters>
      lexeme<keywords, characters>::~lexeme()
        {
          return;
        }


// ******************************************************************


      template <class keywords, class characters>
      void lexeme<keywords, characters>::dump(std::ostream& stream)
        {
          stream << "Lexeme " << this->unique << " = ";

          switch (type)
            {
              case keyword:
                stream << "keyword ";

              assert(this->k >= 0);
              assert(this->k < this->Nk);

              stream << "'" << this->ktable[(int) this->k] << "'\n";
              break;

              case character:
                stream << "symbol ";

              assert(this->s >= 0);
              assert(this->s < this->Nc);

              stream << "'" << this->ctable[(int) this->s] << "'\n";
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


      template <class keywords, class characters>
      enum lexeme_type lexeme<keywords, characters>::get_type()
        {
          return this->type;
        }

      template <class keywords, class characters>
      bool lexeme<keywords, characters>::get_keyword(keywords& kw)
        {
          bool rval = false;

          if (this->type == keyword)
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

          return (rval);
        }

      template <class keywords, class characters>
      bool lexeme<keywords, characters>::get_symbol(characters& sym)
        {
          bool rval = false;

          if (this->type == character)
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

          return (rval);
        }

      template <class keywords, class characters>
      bool lexeme<keywords, characters>::get_identifier(std::string& id)
        {
          bool rval = false;

          if (this->type == ident)
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

          return (rval);
        }

      template <class keywords, class characters>
      bool lexeme<keywords, characters>::get_integer(int& z)
        {
          bool rval = false;

          if (this->type == integer)
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

          return (rval);
        }

      template <class keywords, class characters>
      bool lexeme<keywords, characters>::get_decimal(double& d)
        {
          bool rval = false;

          if (this->type == d)
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

          return (rval);
        }

      template <class keywords, class characters>
      bool lexeme<keywords, characters>::get_string(std::string& str)
        {
          bool rval = false;

          if (this->type == string)
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

          return (rval);
        }


      template <class keywords, class characters>
      const unsigned int& lexeme<keywords, characters>::get_line()
        {
          return (this->line);
        }


      template <class keywords, class characters>
      const std::deque<struct inclusion>& lexeme<keywords, characters>::get_path()
        {
          return (this->path);
        }

  }

#endif //__lexeme_H_
