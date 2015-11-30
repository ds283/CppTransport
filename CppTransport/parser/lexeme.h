//
// Created by David Seery on 13/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __lexeme_H_
#define __lexeme_H_


#include <assert.h>

#include <iostream>
#include <deque>
#include <vector>
#include <string>
#include <sstream>
#include <cstdio>
#include <memory>

#include "core.h"
#include "error.h"
#include "filestack.h"


constexpr auto UNARY_TAG                  = "@unary";
constexpr unsigned int LENGTH_UNARY_TAG   = 6;
constexpr auto BINARY_TAG                 = "@binary";
constexpr unsigned int LENGTH_BINARY_TAG  = 7;


namespace lexeme    // package in a unique namespace to protect common words like 'keyword', 'character' used below
	{

    enum class buffer_type
	    {
        string, number, character, string_literal
	    };

    enum class lexeme_type
	    {
        keyword, character, identifier, integer, decimal, string, unknown
	    };

    enum class minus_context
	    {
        unary, binary
	    };


    template <typename Keywords, typename Characters>
    class lexeme
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a lexeme
        //! accepts a shared_ptr<> to the line from which it originates.
        //! Ownership is shared with the lexfile object which originally read in this file,
        //! but all lexemes generated from the same line will co-own it, so it
        //! survives even if the lexfile object is destroyed
        lexeme(const std::string& buffer, const enum buffer_type t,
               enum minus_context& context, filestack& p, unsigned int u,
               const std::shared_ptr<std::string>& ln, unsigned int cpos,
               const std::vector<std::string>& kt, const std::vector<Keywords>& km,
               const std::vector<std::string>& ct, const std::vector<Characters>& cm, const std::vector<bool>& ctx);

        //! destructor is default
        ~lexeme() = default;


        // WRITE SELF-DETAILS TO OUTPUT STREAM

      public:

        void dump(std::ostream& stream);


		    // INTERFACE

      public:

        // get information
        enum lexeme_type get_type() const { return(this->type); }

        bool get_keyword(Keywords& keyword);

        bool get_symbol(Characters& symbol);

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
        Keywords    k;
        Characters  s;
        int         z;
        double      d;
        std::string str;

        // copy of filestack object
        std::unique_ptr<filestack> path;

        // shared pointer to line; these lexeme is one of the shared owners
        // of the line from which it originated
        const std::shared_ptr<std::string> line;
        unsigned int char_pos;

        // cache keyword and character tables;
        // these are technically needed only during construction, but we keep a copy
        // in order that we can use them if we write out the value of the lexeme
        // we take references, so the object referenced should not be invalidated
        // during the lifetime of the lexeme
        const std::vector<std::string>& ktable;
        const std::vector<Keywords>&    kmap;

        const std::vector<std::string>& ctable;
        const std::vector<Characters>&  cmap;
        const std::vector<bool>&        ccontext;
	    };


//  IMPLEMENTATION

    template <typename Keywords, typename Characters>
    lexeme<Keywords, Characters>::lexeme(const std::string& buffer, const enum buffer_type t,
                                         enum minus_context& context, filestack& p, unsigned int u,
                                         const std::shared_ptr<std::string>& ln, unsigned int cpos,
                                         const std::vector<std::string>& kt, const std::vector<Keywords>& km,
                                         const std::vector<std::string>& ct, const std::vector<Characters>& cm, const std::vector<bool>& ctx)
	    : path(p.clone()),    // use clone to take a copy; otherwise, there is only one filestack object and after processing it is empty! so all file location data is lost
	      unique(u),
	      ktable(kt),
	      kmap(km),
	      ctable(ct),
	      cmap(cm),
	      ccontext(ctx),
        line(ln),
        char_pos(cpos)
	    {
        bool ok     = false;
        int  offset = 0;

        assert(path);
        assert(line);

        switch(t)
	        {
            case buffer_type::string:
              {
                type = lexeme_type::identifier;
                str    = buffer;

                context = minus_context::binary; // unary minus can't follow an identifier

                for(unsigned int i = 0; i < this->ktable.size(); ++i)
                  {
                    if(buffer == ktable[i])
                      {
                        type    = lexeme_type::keyword;
                        k       = km[i];
                        str     = "";
                        context = minus_context::unary; // unary minus can always follow a keyword (eg. built-in function)
                      }
                  }
                break;
              }

            case buffer_type::number:
              {
                type = lexeme_type::unknown;

                if((sscanf(buffer.c_str(), "%i%n", &z, &offset) == 1) && offset == buffer.length())
                  {
                    type = lexeme_type::integer;
                  }
                else if((sscanf(buffer.c_str(), "%lf%n", &d, &offset) == 1) && offset == buffer.length())
                  {
                    type = lexeme_type::decimal;
                  }
                else if((sscanf(buffer.c_str(), "%lg%n", &d, &offset) == 1) && offset == buffer.length())
                  {
                    type = lexeme_type::decimal;
                  }
                else if((sscanf(buffer.c_str(), "%lG%n", &d, &offset) == 1) && offset == buffer.length())
                  {
                    type = lexeme_type::decimal;
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << ERROR_UNRECOGNIZED_NUMBER << " '" << buffer << "'";
                    error(msg.str(), *path, *line, char_pos);
                  }

                if(type == lexeme_type::integer && buffer[0] == '0' && buffer[1] == 'x')
                  {
                    std::ostringstream msg;
                    msg << WARNING_HEX_CONVERSION_A << " '" << buffer << "' " << WARNING_HEX_CONVERSION_B;
                    warn(msg.str(), *path, *line, char_pos);
                  }
                else if(type == lexeme_type::integer && buffer[0] == '0')
                  {
                    std::ostringstream msg;
                    msg << WARNING_OCTAL_CONVERSION_A << " '" << buffer << "' " << WARNING_OCTAL_CONVERSION_B;
                    warn(msg.str(), *path, *line, char_pos);
                  }

                context = minus_context::binary; // unary minus can't follow a number
                break;
              }

            case buffer_type::character:
              {
                type = lexeme_type::unknown;

                for(unsigned int i = 0; i < this->ctable.size(); ++i)
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
                       && (!unary || (unary && context == minus_context::unary))
                       && (!binary || (binary && context == minus_context::binary)))
                      {
                        type    = lexeme_type::character;
                        s       = cmap[i];
                        context = ccontext[i] ? minus_context::unary : minus_context::binary;
                        ok      = true;
                      }
                  }

                if(!ok)
                  {
                    std::ostringstream msg;
                    msg << ERROR_UNRECOGNIZED_SYMBOL << " '" << buffer << "'";
                    error(msg.str(), *path, *line, char_pos);
                    context = minus_context::unary; // reset the context
                  }
                break;
              }

            case buffer_type::string_literal:
              {
                type  = lexeme_type::string;
                str     = buffer;
                context = minus_context::unary; // binary minus can't follow a string
                break;
              }
	        }
	    }


// ******************************************************************


    template <typename Keywords, typename Characters>
    void lexeme<Keywords, Characters>::dump(std::ostream& stream)
	    {
        stream << "Lexeme " << this->unique << " = ";

        switch(type)
	        {
            case lexeme_type::keyword:
              {
                stream << "keyword ";

                assert(this->k >= 0);
                assert(this->k < this->ktable.size());

                stream << "'" << this->ktable[(int) this->k] << "'" << '\n';
                break;
              }

            case lexeme_type::character:
              {
                stream << "symbol ";

                assert(this->s >= 0);
                assert(this->s < this->ctable.size());

                stream << "'" << this->ctable[(int) this->s] << "'" << '\n';
                break;
              }

            case lexeme_type::identifier:
              {
                stream << "identifier '" << this->str << "'" << '\n';
                break;
              }

            case lexeme_type::integer:
              {
                stream << "integer literal '" << this->z << "'" << '\n';
                break;
              }

            case lexeme_type::decimal:
              {
                stream << "decimal literal '" << this->d << "'" << '\n';
                break;
              }

            case lexeme_type::string:
              {
                stream << "string literal '" << this->str << "'" << '\n';
                break;
              }

            case lexeme_type::unknown:
              {
                stream << "UNKNOWN LEXEME" << '\n';
                break;
              }
	        }
	    }


// ******************************************************************


    template <typename Keywords, typename Characters>
    bool lexeme<Keywords, Characters>::get_keyword(Keywords& kw)
	    {
        bool rval = false;

        if(this->type == lexeme_type::keyword)
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


    template <typename Keywords, typename Characters>
    bool lexeme<Keywords, Characters>::get_symbol(Characters& sym)
	    {
        bool rval = false;

        if(this->type == lexeme_type::character)
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


    template <typename Keywords, typename Characters>
    bool lexeme<Keywords, Characters>::get_identifier(std::string& id)
	    {
        bool rval = false;

        if(this->type == lexeme_type::identifier)
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


    template <typename Keywords, typename Characters>
    bool lexeme<Keywords, Characters>::get_integer(int& z)
	    {
        bool rval = false;

        if(this->type == lexeme_type::integer)
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


    template <typename Keywords, typename Characters>
    bool lexeme<Keywords, Characters>::get_decimal(double& d)
	    {
        bool rval = false;

        if(this->type == lexeme_type::decimal)
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


    template <typename Keywords, typename Characters>
    bool lexeme<Keywords, Characters>::get_string(std::string& str)
	    {
        bool rval = false;

        if(this->type == lexeme_type::string)
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
