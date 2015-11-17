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
#include <memory>

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

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a lexeme
        //! accepts a shared_ptr<> to the line from which it originates.
        //! Ownership is shared with the lexfile object which originally read in this file,
        //! but all lexemes generated from the same line will co-own it, so it
        //! survives even if the lexfile object is destroyed
        lexeme(const std::string buffer, const enum lexeme_buffer_type t,
               enum lexeme_minus_context& context,
               filestack& p, unsigned int u,
               const std::shared_ptr<std::string>& ln, unsigned int cpos,
               const std::string* kt, const keywords* km, unsigned int num_k,
               const std::string* ct, const characters* cm, const bool* ctx, unsigned int num_c);

        //! destructor is default
        ~lexeme() = default;


        // WRITE SELF-DETAILS TO OUTPUT STREAM

      public:

        void dump(std::ostream& stream);


		    // INTERFACE

      public:

        // get information
        enum lexeme_type get_type() const { return(this->type); }

        bool get_keyword(keywords& keyword);

        bool get_symbol(characters& symbol);

        bool get_identifier(std::string& id);

        bool get_integer(int& z);

        bool get_decimal(double& d);

        bool get_string(std::string& str);

        const filestack& get_path()   const { return(*this->path); }

        const std::string& get_line() const { return(*this->line); }

        unsigned int get_char_pos()   const { return(this->char_pos); }


        // INTERNAL DATA

      protected:

        enum lexeme_type   type;
        const unsigned int unique;

        // lexeme value - not all of these are used by any single lexeme
        keywords    k;
        characters  s;
        int         z;
        double      d;
        std::string str;

        // copy of filestack object
        std::unique_ptr<filestack> path;

        // shared pointer to line; these lexeme is one of the shared owners
        // of the line from which it originated
        const std::shared_ptr<std::string> line;
        unsigned int char_pos;

        const std::string* ktable;
        const keywords   * kmap;
        const unsigned int Nk;

        const std::string* ctable;
        const characters * cmap;
        const bool       * ccontext;
        unsigned int       Nc;
	    };


//  IMPLEMENTATION

    template <typename keywords, typename characters>
    lexeme<keywords, characters>::lexeme(const std::string buffer, const enum lexeme_buffer_type t,
                                         enum lexeme_minus_context& context,
                                         filestack& p, unsigned int u,
                                         const std::shared_ptr<std::string>& ln, unsigned int cpos,
                                         const std::string* kt, const keywords* km, unsigned int num_k,
                                         const std::string* ct, const characters* cm, const bool* ctx, unsigned int num_c)
	    : path(p.clone()),    // use clone to take a copy; otherwise, there is only one filestack object and after processing it is empty! so all file location data is lost
	      unique(u),
	      ktable(kt),
	      kmap(km),
	      Nk(num_k),
	      ctable(ct),
	      cmap(cm),
	      ccontext(ctx),
	      Nc(num_c),
        line(ln),
        char_pos(cpos)
	    {
        bool ok     = false;
        int  offset = 0;

        assert(path);
        assert(line);

        assert(kmap != nullptr);
        assert(ktable != nullptr);
        assert(cmap != nullptr);
        assert(ctable != nullptr);
        assert(ctx != nullptr);

        switch(t)
	        {
            case buf_string:
              {
                type = ident;
                str    = buffer;

                context = binary_context; // unary minus can't follow an identifier

                for(int i = 0; i < Nk; ++i)
                  {
                    if(buffer == ktable[i])
                      {
                        type    = keyword;
                        k       = km[i];
                        str     = "";
                        context = unary_context; // unary minus can always follow a keyword (eg. built-in function)
                      }
                  }
                break;
              }

            case buf_number:
              {
                type = unknown;

                if((sscanf(buffer.c_str(), "%i%n", &z, &offset) == 1) && offset == buffer.length())
                  {
                    type = integer;
                  }
                else if((sscanf(buffer.c_str(), "%lf%n", &d, &offset) == 1) && offset == buffer.length())
                  {
                    type = decimal;
                  }
                else if((sscanf(buffer.c_str(), "%lg%n", &d, &offset) == 1) && offset == buffer.length())
                  {
                    type = decimal;
                  }
                else if((sscanf(buffer.c_str(), "%lG%n", &d, &offset) == 1) && offset == buffer.length())
                  {
                    type = decimal;
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << ERROR_UNRECOGNIZED_NUMBER << " '" << buffer << "'";
                    error(msg.str(), *path, *line, char_pos);
                  }

                if(type == integer && buffer[0] == '0' && buffer[1] == 'x')
                  {
                    std::ostringstream msg;
                    msg << WARNING_HEX_CONVERSION_A << " '" << buffer << "' " << WARNING_HEX_CONVERSION_B;
                    warn(msg.str(), *path, *line, char_pos);
                  }
                else if(type == integer && buffer[0] == '0')
                  {
                    std::ostringstream msg;
                    msg << WARNING_OCTAL_CONVERSION_A << " '" << buffer << "' " << WARNING_OCTAL_CONVERSION_B;
                    warn(msg.str(), *path, *line, char_pos);
                  }

                context = binary_context; // unary minus can't follow a number
                break;
              }

            case buf_character:
              {
                type = unknown;

                for(int i = 0; i < Nc; ++i)
                  {
                    bool        unary  = false;
                    bool        binary = false;
                    std::string match  = ctable[i];
                    size_t      pos;

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
                       && (!unary || (unary && context == unary_context))
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
                    error(msg.str(), *path, *line, char_pos);
                    context = unary_context; // reset the context
                  }
                break;
              }

            case buf_string_literal:
              {
                type  = string;
                str     = buffer;
                context = unary_context; // binary minus can't follow a string
                break;
              }

            default:
	            assert(false);
	        }
	    }


// ******************************************************************


    template <typename keywords, typename characters>
    void lexeme<keywords, characters>::dump(std::ostream& stream)
	    {
        stream << "Lexeme " << this->unique << " = ";

        switch(type)
	        {
            case keyword:
              {
                stream << "keyword ";

                assert(this->k >= 0);
                assert(this->k < this->Nk);

                stream << "'" << this->ktable[(int) this->k] << "'" << '\n';
                break;
              }

            case character:
              {
                stream << "symbol ";

                assert(this->s >= 0);
                assert(this->s < this->Nc);

                stream << "'" << this->ctable[(int) this->s] << "'" << '\n';
                break;
              }

            case ident:
              {
                stream << "identifier '" << this->str << "'" << '\n';
                break;
              }

            case integer:
              {
                stream << "integer literal '" << this->z << "'" << '\n';
                break;
              }

            case decimal:
              {
                stream << "decimal literal '" << this->d << "'" << '\n';
                break;
              }

            case string:
              {
                stream << "string literal '" << this->str << "'" << '\n';
                break;
              }

            case unknown:
              {
                stream << "UNKNOWN LEXEME" << '\n';
                break;
              }

            default:
	            assert(false);
	        }
	    }


// ******************************************************************


    template <typename keywords, typename characters>
    bool lexeme<keywords, characters>::get_keyword(keywords& kw)
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
            warn(msg.str(), *this->path);
	        }

        return (rval);
	    }


    template <typename keywords, typename characters>
    bool lexeme<keywords, characters>::get_symbol(characters& sym)
	    {
        bool rval = false;

        if(this->type == character)
	        {
            sym  = this->s;
            rval = true;
	        }
        else
	        {
            std::ostringstream msg;
            msg << WARNING_LEXEME_SYMBOL << " (id " << this->unique << ")";
            warn(msg.str(), *this->path);
	        }

        return (rval);
	    }


    template <typename keywords, typename characters>
    bool lexeme<keywords, characters>::get_identifier(std::string& id)
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
            warn(msg.str(), *this->path);
	        }

        return (rval);
	    }


    template <typename keywords, typename characters>
    bool lexeme<keywords, characters>::get_integer(int& z)
	    {
        bool rval = false;

        if(this->type == integer)
	        {
            z    = this->z;
            rval = true;
	        }
        else
	        {
            std::ostringstream msg;
            msg << WARNING_LEXEME_INTEGER << " (id " << this->unique << ")";
            warn(msg.str(), *this->path);
	        }

        return (rval);
	    }


    template <typename keywords, typename characters>
    bool lexeme<keywords, characters>::get_decimal(double& d)
	    {
        bool rval = false;

        if(this->type == decimal)
	        {
            d    = this->d;
            rval = true;
	        }
        else
	        {
            std::ostringstream msg;
            msg << WARNING_LEXEME_DECIMAL << " (id " << this->unique << ")";
            warn(msg.str(), *this->path);
	        }

        return (rval);
	    }


    template <typename keywords, typename characters>
    bool lexeme<keywords, characters>::get_string(std::string& str)
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
            warn(msg.str(), *this->path);
	        }

        return (rval);
	    }


	}

#endif //__lexeme_H_
