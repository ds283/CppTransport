//
// Created by David Seery on 13/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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
#include "filestack.h"


#define UNARY_TAG         "@unary"
#define LENGTH_UNARY_TAG  6
#define BINARY_TAG        "@binary"
#define LENGTH_BINARY_TAG 7


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

      enum lexeme_minus_context
        {
          unary_context, binary_context
        };

      template <typename keywords, typename characters>
      class lexeme
        {
          public:
            lexeme(const std::string buffer, const enum lexeme_buffer_type t,
                   enum lexeme_minus_context& context,
                   const filestack* p, unsigned int u,
                   const std::string* kt, const keywords* km, unsigned int num_k,
                   const std::string* ct, const characters* cm, const bool* ctx, unsigned int num_c);

            // need an explicit copy constructor to handle cloning of the filestack* object
            lexeme(const lexeme<keywords, characters>& obj);

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

            const filestack*                    get_path();

          protected:
            enum lexeme_type                    type;
            const unsigned int                  unique;

            // lexeme value - not all of these are used by any single lexeme
            keywords                            k;
            characters                          s;
            int                                 z;
            double                              d;
            std::string                         str;

            // copy of a filestack object recording where this lexeme came from
            // we take a copy and mark it const so that it cannot subsequently be
            // modified
            const filestack*                    path;

            const std::string*                  ktable;
            const keywords*                     kmap;
            const unsigned int                  Nk;

            const std::string*                  ctable;
            const characters*                   cmap;
            const bool*                         ccontext;
            unsigned int                        Nc;
        };

//  IMPLEMENTATION

      template <typename keywords, typename characters>
      lexeme<keywords, characters>::lexeme(const std::string buffer, const enum lexeme_buffer_type t,
                                           enum lexeme_minus_context& context,
                                           const filestack* p, unsigned int u,
                                           const std::string* kt, const keywords* km, unsigned int num_k,
                                           const std::string* ct, const characters* cm, const bool* ctx, unsigned int num_c)
      : path(p->clone()), unique(u),
        ktable(kt), kmap(km), Nk(num_k),
        ctable(ct), cmap(cm), ccontext(ctx), Nc(num_c)
        {
          bool ok     = false;
          int  offset = 0;

          // cloning of the filestack* object implies that we have to be cautious
          // when this object gets copied, otherwise it will just inherit
          // a reference to the same cloned filestack.
          // that will lead to delete errors when the destructor is called on whichever
          // object is destroyed second
          assert(path   != nullptr);

          assert(kmap   != nullptr);
          assert(ktable != nullptr);
          assert(cmap   != nullptr);
          assert(ctable != nullptr);
          assert(ctx    != nullptr);

          switch (t)
            {
              case buf_string:
                type = ident;
                str  = buffer;

                context = binary_context; // unary minus can't follow an identifier

                for (int i = 0; i < Nk; ++i)
                  {
                    if (buffer == ktable[i])
                      {
                        type    = keyword;
                        k       = km[i];
                        str     = "";
                        context = unary_context; // unary minus can always follow a keyword (eg. built-in function)
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
                    error(msg.str(), path);
                  }

                if(type == integer && buffer[0] == '0' && buffer[1] == 'x')
                  {
                    std::ostringstream msg;
                    msg << WARNING_HEX_CONVERSION_A << " '" << buffer << "' " << WARNING_HEX_CONVERSION_B;
                    warn(msg.str(), path);
                  }
                else if(type == integer && buffer[0] == '0')
                  {
                    std::ostringstream msg;
                    msg << WARNING_OCTAL_CONVERSION_A << " '" << buffer << "' " << WARNING_OCTAL_CONVERSION_B;
                    warn(msg.str(), path);
                  }

                context = binary_context; // unary minus can't follow a number
                break;

              case buf_character:
                type = unknown;

                for(int i = 0; i < Nc; ++i)
                  {
                    bool unary  = false;
                    bool binary = false;
                    std::string match = ctable[i];
                    size_t pos;

                    if((pos = match.find(UNARY_TAG)) != std::string::npos)
                      {
                        unary = true;
                        match.erase(pos, LENGTH_UNARY_TAG);
                      }
                    if((pos = match.find(BINARY_TAG)) != std::string::npos)
                      {
                        binary = true;
                        match.erase(pos, LENGTH_BINARY_TAG);
                      }
                    if(buffer == match
                       && (!unary  || (unary  && context == unary_context))
                       && (!binary || (binary && context == binary_context)))
                      {
                        type    = character;
                        s       = cmap[i];
                        context = ccontext[i] ? unary_context : binary_context;
                        ok      = true;
                      }
                  }

                if(!ok)
                  {
                    std::ostringstream msg;
                    msg << ERROR_UNRECOGNIZED_SYMBOL << " '" << buffer << "'";
                    error(msg.str(), p);
                    context = unary_context; // reset the context
                  }
                break;

              case buf_string_literal:
                type    = string;
                str     = buffer;
                context = unary_context; // binary minus can't follow a string
                break;

              default:
                assert(false);
            }
        }

      template <typename keywords, typename characters>
      lexeme<keywords, characters>::lexeme(const lexeme<keywords, characters>& obj)
        : type(obj.type), unique(obj.unique),
          k(obj.k), s(obj.s), z(obj.z), d(obj.d), str(obj.str),
          path(obj.path->clone()),
          ktable(obj.ktable), kmap(obj.kmap), Nk(obj.Nk),
          ctable(obj.ctable), cmap(obj.cmap), ccontext(obj.ccontext), Nc(obj.Nc)
        {
        }

      template <typename keywords, typename characters>
      lexeme<keywords, characters>::~lexeme()
        {
          // delete copy of filestack object
          assert(this->path != nullptr);
          delete this->path;
        }


// ******************************************************************


      template <typename keywords, typename characters>
      void lexeme<keywords, characters>::dump(std::ostream& stream)
        {
          stream << "Lexeme " << this->unique << " = ";

          switch (type)
            {
              case keyword:
                stream << "keyword ";

              assert(this->k >= 0);
              assert(this->k < this->Nk);

              stream << "'" << this->ktable[(int) this->k] << "'" << std::endl;
              break;

              case character:
                stream << "symbol ";

              assert(this->s >= 0);
              assert(this->s < this->Nc);

              stream << "'" << this->ctable[(int) this->s] << "'" << std::endl;
              break;

              case ident:
                stream << "identifier '" << this->str << "'" << std::endl;
              break;

              case integer:
                stream << "integer literal '" << this->z << "'" << std::endl;
              break;

              case decimal:
                stream << "decimal literal '" << this->d << "'" << std::endl;
              break;

              case string:
                stream << "string literal '" << this->str << "'" << std::endl;
              break;

              case unknown:
                stream << "UNKNOWN LEXEME" << std::endl;
              break;

              default:
                assert(false);
            }
        }


// ******************************************************************


			// introduce typedef to avoid writing 'enum lexeme_type' in the get_type() function definition
			// for some reason, the Intel compiler chokes on this, although Clang is OK
			typedef enum lexeme_type lt;

      template <typename keywords, typename characters>
      lt lexeme<keywords, characters>::get_type()
        {
          return this->type;
        }

      template <typename keywords, typename characters>
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
              warn(msg.str(), this->path);
            }

          return (rval);
        }

      template <typename keywords, typename characters>
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
              warn(msg.str(), this->path);
            }

          return (rval);
        }

      template <typename keywords, typename characters>
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
              warn(msg.str(), this->path);
            }

          return (rval);
        }

      template <typename keywords, typename characters>
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
              warn(msg.str(), this->path);
            }

          return (rval);
        }

      template <typename keywords, typename characters>
      bool lexeme<keywords, characters>::get_decimal(double& d)
        {
          bool rval = false;

          if (this->type == decimal)
            {
              d    = this->d;
              rval = true;
            }
          else
            {
              std::ostringstream msg;
              msg << WARNING_LEXEME_DECIMAL << " (id " << this->unique << ")";
              warn(msg.str(), this->path);
            }

          return (rval);
        }

      template <typename keywords, typename characters>
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
              warn(msg.str(), this->path);
            }

          return(rval);
        }


      template <typename keywords, typename characters>
      const filestack* lexeme<keywords, characters>::get_path()
        {
          return(this->path);
        }

  }

#endif //__lexeme_H_
