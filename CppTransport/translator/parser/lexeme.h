//
// Created by David Seery on 13/06/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//



#ifndef CPPTRANSPORT_LEXEME_H
#define CPPTRANSPORT_LEXEME_H


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
#include "error_context.h"
#include "filestack.h"

#include "boost/optional.hpp"


constexpr auto UNARY_TAG                  = "@unary";
constexpr unsigned int LENGTH_UNARY_TAG   = 6;
constexpr auto BINARY_TAG                 = "@binary";
constexpr unsigned int LENGTH_BINARY_TAG  = 7;


namespace lexeme    // package in a unique namespace
	{

    //! tag the type of a lexeme
    enum class lexeme_type
	    {
        keyword, character, identifier, integer, decimal, string, unknown
	    };

    //! tag the 'context' of a minus sign -- should it be interpreted as a unary minus, or a binary minus?
    enum class minus_context
	    {
        unary, binary
	    };


    //! lexeme buffer holds a string representation of a lexeme,
    //! together with a flag identifying what sort of content the string contains
    class lexeme_buffer
      {

        // TYPES

      public:

        enum class type
          {
            string, number, character, string_literal
          };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! default constructor
        lexeme_buffer() = default;

        //! value constructor
        lexeme_buffer(std::string b, type t)
          : buffer(std::move(b)),
            buffer_type(t)
          {
          }

        //! destructor
        ~lexeme_buffer() = default;


        // INTERFACE

      public:

        //! get type
        type get_type() const { return this->buffer_type; }

        //! set type
        lexeme_buffer& set_type(type t) { this->buffer_type = t; return *this; }

        //! get buffer contents
        const std::string& get() const { return this->buffer; }

        //! get buffer size / length
        size_t size() const { return this->buffer.size(); }
        size_t length() const { return this->buffer.length(); }

        //! is buffer empty?
        bool empty() const { return this->buffer.empty(); }


        // BUFFER OPERATIONS

      public:

        //! assign a string
        lexeme_buffer& operator=(const std::string& str) { this->buffer = str; return *this; }

        //! assign a character
        lexeme_buffer& operator=(char c) { this->buffer = c; return *this; }

        //! append a string
        lexeme_buffer& operator+=(const std::string& str) { this->buffer += str; return *this; }

        //! append a character
        lexeme_buffer& operator+=(char c) { this->buffer += c; return *this; }

        //! subscript
        char operator[](unsigned p) const
          {
            if(p < this->buffer.size()) return this->buffer[p];

            std::ostringstream msg;
            msg << ERROR_BUFFER_ACCESS_OUT_OF_RANGE << " (" << this->buffer << " | " << p << ", " << this->buffer.size() << ")";
            throw std::out_of_range(msg.str());
          }

        //! find
        size_t find(const std::string str, size_t pos=0) const { return this->buffer.find(str,pos); }

        //! convert to C string
        const char* c_str() const { return this->buffer.c_str(); }


        // BUFFER COMPARISONS

      public:

        //! equality comparison
        bool operator==(const std::string& str) const { return this->buffer == str; }


        // DEREFERENCE

      public:

        //! dereference to get buffer
        const std::string& operator*() const { return this->buffer; }


        // INTERNAL DATA

      private:

        //! buffer
        std::string buffer;

        //! buffer type
        type buffer_type;

      };


    template <typename Keywords, typename Characters>
    class lexeme
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a lexeme
        //! accepts an error_context object which is used in generating context for
        //! later error messages
        lexeme(const lexeme_buffer& buffer, minus_context& context, unsigned int u, error_context err_ctx,
               const std::vector<std::string>& kt, const std::vector<Keywords>& km, const std::vector<std::string>& ct,
               const std::vector<Characters>& cm, const std::vector<bool>& ctx);

        //! destructor is default
        ~lexeme() = default;


        // WRITE SELF-DETAILS TO OUTPUT STREAM

      public:

        //! dump lexeme list to specified stream
        void dump(std::ostream& stream);


		    // INTERFACE -- EXTRACT INFORMATION ABOUT THIS LEXEME

      public:

        //! get lexeme type
        lexeme_type get_type() const { return(this->type); }

        //! if this lexeme is a keyword, returns the keyword tag
        //! otherwise, returns boost::none
        boost::optional<Keywords> get_keyword();

        //! if this lexeme is a symbol, returns the symbol tag
        //! otherwise, returns boost::none
        boost::optional<Characters> get_symbol();

        //! if this this lexeme is an identifier, returns its string representation
        //! otherwise, returns boost::none
        boost::optional<std::string> get_identifier();

        //! if this lexeme is an integer, returns its value
        //! otherwise, returns boost::none
        boost::optional<int> get_integer();

        //! if this lexeme is a decimal, returns its value
        //! otherwise, returns boost::none
        boost::optional<double> get_decimal();

        //! if this lexeme is a string literal, returns its value
        //! otherwise returns boost::none
        boost::optional<std::string> get_string();


        // INTERFACE -- RAISE ERROR, CONTEXTUALIZED TO THIS LEXEME

      public:

        //! raise error using the context defined by this lexeme
        void error(const std::string& msg) const { this->err_context.error(msg); }

        //! raise warning using the context defined by this lexeme
        void warn(const std::string& msg) const { this->err_context.warn(msg); }


        // INTERFACE -- GET ERROR CONTEXT

      public:

        //! return error context
        const error_context& get_error_context() const { return(this->err_context); }


        // INTERNAL DATA

      protected:

        //! lexeme type
        enum lexeme_type type;

        //! unique identifier
        const unsigned int unique;

        //! lexeme values - typically only one of these will be used by any single lexeme
        Keywords    k;
        Characters  s;
        int         z;
        double      d;
        std::string str;

        //! error context
        error_context err_context;

        //! cache keyword and character tables;
        //! these are technically needed only during construction, but we keep a copy
        //! in order that we can use them if we write out the value of the lexeme.
        //! We take references, so the object referenced should not be invalidated
        //! during the lifetime of the lexeme
        const std::vector<std::string>& ktable;
        const std::vector<Keywords>&    kmap;

        const std::vector<std::string>& ctable;
        const std::vector<Characters>&  cmap;
        const std::vector<bool>&        ccontext;
	    };


//  IMPLEMENTATION

    template <typename Keywords, typename Characters>
    lexeme<Keywords, Characters>::lexeme(const lexeme_buffer& buffer, minus_context& context, unsigned int u,
                                         error_context err_ctx,
                                         const std::vector<std::string>& kt, const std::vector<Keywords>& km,
                                         const std::vector<std::string>& ct,
                                         const std::vector<Characters>& cm, const std::vector<bool>& ctx)
	    : unique(u),
	      ktable(kt),
	      kmap(km),
	      ctable(ct),
	      cmap(cm),
	      ccontext(ctx),
        err_context(std::move(err_ctx))
	    {
        int offset = 0;

        switch(buffer.get_type())
	        {
            case lexeme_buffer::type::string:
              {
                type = lexeme_type::identifier;
                str = *buffer;

                context = minus_context::binary; // unary minus can't follow an identifier

                // check whether this literal matches a keyword
                for(unsigned int i = 0; i < this->ktable.size(); ++i)
                  {
                    if(buffer == ktable[i])
                      {
                        type    = lexeme_type::keyword;
                        k       = km[i];
                        str     = "";
                        context = minus_context::unary; // unary minus can always follow a keyword (eg. built-in function)
                        break;
                      }
                  }
                break;
              }

            case lexeme_buffer::type::number:
              {
                type = lexeme_type::unknown;

                if(buffer.length() >= 2 && buffer[0] == '0' && buffer[1] == 'x')    // allow >= 2 to catch just '0x' on its own
                  {
                    if((sscanf(buffer.c_str(), "%x%n", &z, &offset) == 1) && offset == buffer.length())
                      {
                        type = lexeme_type::integer;
                      }
                    else
                      {
                        std::ostringstream msg;
                        msg << WARNING_HEX_CONVERSION_A << " '" << *buffer << "' " << WARNING_HEX_CONVERSION_B;
                        err_context.warn(msg.str());
                      }
                  }
                else if(buffer.length() > 1 && buffer[0] == '0'
                        && buffer.find(".", 1) == std::string::npos
                        && buffer.find("e", 1) == std::string::npos && buffer.find("E", 1) == std::string::npos)    // don't attempt to convert strings containing a decimal point or E notation
                  {
                    if((sscanf(buffer.c_str(), "%o%n", &z, &offset) == 1) && offset == buffer.length())
                      {
                        type = lexeme_type::integer;
                      }
                    else
                      {
                        std::ostringstream msg;
                        msg << WARNING_OCTAL_CONVERSION_A << " '" << *buffer << "' " << WARNING_OCTAL_CONVERSION_B;
                        err_context.warn(msg.str());
                      }
                  }
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
                  
                // check for successful conversion
                if(type == lexeme_type::unknown)
                  {
                    std::ostringstream msg;
                    msg << ERROR_UNRECOGNIZED_NUMBER << " '" << *buffer << "'";
                    err_context.error(msg.str());
                  }

                context = minus_context::binary; // unary minus can't follow a number
                break;
              }

            case lexeme_buffer::type::character:
              {
                bool found_match = false;

                type = lexeme_type::unknown;

                for(unsigned int i = 0; i < this->ctable.size(); ++i)
                  {
                    bool unary = false;
                    bool binary = false;
                    std::string match = ctable[i];
                    size_t pos;

                    // check for @binary or @unary distinguishing tags in the character
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
                        type = lexeme_type::character;
                        s = cmap[i];
                        context = ccontext[i] ? minus_context::unary : minus_context::binary;
                        found_match = true;
                        break;
                      }
                  }

                if(!found_match)
                  {
                    std::ostringstream msg;
                    msg << ERROR_UNRECOGNIZED_SYMBOL << " '" << *buffer << "'";
                    err_context.error(msg.str());
                    context = minus_context::unary; // reset the context
                  }
                break;
              }

            case lexeme_buffer::type::string_literal:
              {
                type = lexeme_type::string;
                str = *buffer;
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

                stream << "'" << this->ktable[static_cast<int>(this->k)] << "'" << '\n';
                break;
              }

            case lexeme_type::character:
              {
                stream << "symbol ";

                stream << "'" << this->ctable[static_cast<int>(this->s)] << "'" << '\n';
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
    boost::optional<Keywords> lexeme<Keywords, Characters>::get_keyword()
	    {
        if(this->type == lexeme_type::keyword) return this->k;

        std::ostringstream msg;
        msg << WARNING_LEXEME_KEYWORD << " (id " << this->unique << ")";
        this->err_context.warn(msg.str());
        return boost::none;
	    }


    template <typename Keywords, typename Characters>
    boost::optional<Characters> lexeme<Keywords, Characters>::get_symbol()
	    {
        if(this->type == lexeme_type::character) return this->s;

        std::ostringstream msg;
        msg << WARNING_LEXEME_SYMBOL << " (id " << this->unique << ")";
        this->err_context.warn(msg.str());
        return boost::none;
	    }


    template <typename Keywords, typename Characters>
    boost::optional<std::string> lexeme<Keywords, Characters>::get_identifier()
	    {
        if(this->type == lexeme_type::identifier) return this->str;

        std::ostringstream msg;
        msg << WARNING_LEXEME_IDENTIFIER << " (id " << this->unique << ")";
        this->err_context.warn(msg.str());
        return boost::none;
	    }


    template <typename Keywords, typename Characters>
    boost::optional<int> lexeme<Keywords, Characters>::get_integer()
	    {
        if(this->type == lexeme_type::integer) return this->z;

        std::ostringstream msg;
        msg << WARNING_LEXEME_INTEGER << " (id " << this->unique << ")";
        this->err_context.warn(msg.str());
        return boost::none;
	    }


    template <typename Keywords, typename Characters>
    boost::optional<double> lexeme<Keywords, Characters>::get_decimal()
	    {
        if(this->type == lexeme_type::decimal) return this->d;

        std::ostringstream msg;
        msg << WARNING_LEXEME_DECIMAL << " (id " << this->unique << ")";
        this->err_context.warn(msg.str());
        return boost::none;
	    }


    template <typename Keywords, typename Characters>
    boost::optional<std::string> lexeme<Keywords, Characters>::get_string()
	    {
        if(this->type == lexeme_type::string) return this->str;

        std::ostringstream msg;
        msg << WARNING_LEXEME_STRING << " (id " << this->unique << ")";
        this->err_context.warn(msg.str());
        return boost::none;
	    }


	}   // namespace lexeme


#endif //CPPTRANSPORT_LEXEME_H
