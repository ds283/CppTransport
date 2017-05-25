//
// Created by David Seery on 12/06/2013.
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

#ifndef CPPTRANSPORT_LEXSTREAM_H
#define CPPTRANSPORT_LEXSTREAM_H

#include <iostream>
#include <string>
#include <deque>
#include <fstream>
#include <sstream>
#include <cctype>
#include <memory>

#include "lexeme.h"
#include "finder.h"
#include "lexfile.h"
#include "error.h"
#include "input_stack.h"
#include "lexstream_data.h"


//! lexstream parses a specified input file into a traversable list of lexemes
template <typename Keywords, typename Characters>
class lexstream
  {

    // TYPES

  public:

    //! shorthand for basic lexeme
    typedef lexeme::lexeme<Keywords, Characters> lexeme_type;

  protected:

    //! lexeme table
    typedef std::deque< std::shared_ptr<lexeme_type> > lexeme_table_type;

    //! lexeme table iterator
    typedef typename lexeme_table_type::iterator lexeme_table_ptr;
    
    //! lexeme data bundle
    typedef std::tuple< lexeme::lexeme_buffer, std::shared_ptr<std::string>, unsigned int > lexeme_bundle;


    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    lexstream(lexstream_data& p,
              const std::vector<std::string>& kt, const std::vector<Keywords>& km,
              const std::vector<std::string>& ct, const std::vector<Characters>& cm, const std::vector<bool>& ctx);

    //! destructor
    ~lexstream() = default;


    // INTERFACE

  public:

    //! reset stream to beginning
    void reset();

    //! get next lexeme from the stream; either use get() or dereference -- these are synonymous
    std::shared_ptr<lexeme_type> get();
    std::shared_ptr<lexeme_type> operator*() { return this->get(); }

    //! consume current lexeme, ie. moves to next lexeme
    lexstream<Keywords, Characters>& operator++();

    //! interrogate current state: true if get() will return a valid lexeme,
    //! false otherwise
    bool state();

    //! dump contents to specified stream
    void print(std::ostream& stream);


    // INTERNAL API

  private:

    //! parse a named file
    bool parse(const boost::filesystem::path& file);

    //! convert the stream associated with a lexfile descriptor into lexemes;
    //! called by parse()
    void lexicalize(lexfile& input);

    //! identify and handle directives
    void handle_directive(lexfile& input);

    //! parse a lexeme from a specified lexfile descriptor
    lexeme::lexeme_buffer get_lexeme(lexfile& input);


    // INTERNAL DATA

  private:

    // DELEGATES

    //! data object passed to us by the translation unit
    lexstream_data& data_payload;

    //! input filestack, used for generating contexted messages
    input_stack stack;


    // LEXEME DATA

    //! list of lexemes parsed from the input file
    lexeme_table_type lexeme_list;

    //! current position of the stream within this list
    lexeme_table_ptr ptr;

    //! counter to assign unique numbers to lexemes
    unsigned int unique;


    // PARSING TABLES

    //! table of keywords
    const std::vector<std::string>& keyword_table;

    //! table of keyword tokens
    const std::vector<Keywords>& keyword_tokens;

    //! table of characters
    const std::vector<std::string>& chars_table;

    //! table of character tokens
    const std::vector<Characters>& chars_tokens;

    //! table of unary minus sign 'contexts' that apply *after* each character
    const std::vector<bool>& unary_context;

  };


// IMPLEMENTATION


// ******************************************************************


// convert the contents of 'filename' to a string of lexemes, descending into
// included files as necessary
template <typename Keywords, typename Characters>
lexstream<Keywords, Characters>::lexstream(lexstream_data& p,
                                           const std::vector<std::string>& kt, const std::vector<Keywords>& km,
                                           const std::vector<std::string>& ct, const std::vector<Characters>& cm,
                                           const std::vector<bool>& ctx)
  : keyword_table(kt),
    keyword_tokens(km),
    chars_table(ct),
    chars_tokens(cm),
    unary_context(ctx),
    data_payload(p),
    unique(1)
  {
    if(!this->parse(data_payload.get_input_file()))
      {
        std::ostringstream msg;
        msg << ERROR_OPEN_TOPLEVEL << " '" << data_payload.get_input_file().string() << "'";

        error_context err_context(stack, data_payload.get_error_handler(), data_payload.get_warning_handler());
        err_context.error(msg.str());
      }
  }


// ******************************************************************


// output list of lexemes
template <typename Keywords, typename Characters>
void lexstream<Keywords, Characters>::print(std::ostream& stream)
  {
    for(const auto& lex : this->lexeme_list)
      {
        lex->dump(stream);
      }
  }


// ******************************************************************


template <typename Keywords, typename Characters>
void lexstream<Keywords, Characters>::reset()
  {
    this->ptr = this->lexeme_list.begin();
  }


template <typename Keywords, typename Characters>
std::shared_ptr< typename lexstream<Keywords, Characters>::lexeme_type > lexstream<Keywords, Characters>::get()
  {
    if(this->state()) return *this->ptr;

    return nullptr;
  }


template <typename Keywords, typename Characters>
lexstream<Keywords, Characters>& lexstream<Keywords, Characters>::operator++()
  {
    if(this->state()) ++this->ptr;

    return *this;
  };


template <typename Keywords, typename Characters>
bool lexstream<Keywords, Characters>::state()
  {
    return (this->ptr != this->lexeme_list.end());
  }


// ******************************************************************


template <typename Keywords, typename Characters>
bool lexstream<Keywords, Characters>::parse(const boost::filesystem::path& file)
  {
    boost::filesystem::path path;
    bool found = this->data_payload.get_finder().fqpn(file, path);

    if(!found) return false;

    // push parent directory into search paths
    if(path.has_parent_path())
      {
        this->data_payload.get_finder().add(path.parent_path());
      }

    // the lexfile object responsible for reading in a file persists only within this block,
    // but the lines it reads in are managed with std::shared_ptr<>
    // the lexemes which are generated during lexicalization
    // then inherit ownership of these lines, so even though the
    // lexfile object itself is destroyed we are not left with dangling pointers
    lexfile input(path, this->stack);

    this->stack.push(path);
    this->lexicalize(input);
    this->stack.pop();

    return (true);
  }


template <typename Keywords, typename Characters>
void lexstream<Keywords, Characters>::lexicalize(lexfile& input)
  {
    // keep track of whether we expect unary or binary minus sign
    typename lexeme_type::minus_context context = lexeme_type::minus_context::unary;

    while(input.get_state() == lexfile::status::ok)
      {
        // get next lexeme from input stream
        auto word = this->get_lexeme(input);

        // if no token read, skip
        if(word.empty()) continue;

        switch(word.get_type())
          {
            case lexeme::lexeme_buffer::type::character:
              {
                if(word == "#") // treat as a preprocessor directive
                  {
                    this->handle_directive(input);
                  }
                else
                  {
                    error_context err_ctx(this->stack, word.get_context_line(),
                                          word.get_context_start_position(), word.get_context_end_position(),
                                          this->data_payload.get_error_handler(),
                                          this->data_payload.get_warning_handler());

                    this->lexeme_list.emplace_back(
                      std::make_shared<lexeme_type>(word, context, this->unique++, err_ctx,
                                                    this->keyword_table, this->keyword_tokens,
                                                    this->chars_table, this->chars_tokens, this->unary_context));
                  }
                break;
              }

            case lexeme::lexeme_buffer::type::string:
            case lexeme::lexeme_buffer::type::number:
            case lexeme::lexeme_buffer::type::string_literal:
              {
                error_context err_ctx(this->stack, word.get_context_line(),
                                      word.get_context_start_position(), word.get_context_end_position(),
                                      this->data_payload.get_error_handler(),
                                      this->data_payload.get_warning_handler());

                this->lexeme_list.emplace_back(
                  std::make_shared<lexeme_type>(word, context, this->unique++, err_ctx,
                                                this->keyword_table, this->keyword_tokens,
                                                this->chars_table, this->chars_tokens, this->unary_context));
                break;
              }
          }
      }
  }


template <typename Keywords, typename Characters>
void lexstream<Keywords, Characters>::handle_directive(lexfile& input)
  {
    // get next lexeme
    auto word = this->get_lexeme(input);

    if(word == "include") // inclusion directive
      {
        auto file = this->get_lexeme(input);

        if(file.get_type() != lexeme::lexeme_buffer::type::string_literal)
          {
            error_context err_ctx(this->stack, word.get_context_line(),
                                  word.get_context_start_position(), word.get_context_end_position(),
                                  this->data_payload.get_error_handler(),
                                  this->data_payload.get_warning_handler());

            std::ostringstream msg;
            msg << ERROR_INCLUDE_DIRECTIVE << " '" << *word << "'";
            err_ctx.error(msg.str());
          }
        else
          {
            if(!this->parse(*file))
              {
                error_context err_ctx(this->stack, file.get_context_line(),
                                      file.get_context_start_position(), file.get_context_end_position(),
                                      this->data_payload.get_error_handler(),
                                      this->data_payload.get_warning_handler());

                std::ostringstream msg;
                msg << ERROR_INCLUDE_FILE << " '" << *file << "'";
                err_ctx.error(msg.str());
              }
          }
      }
    else
      {
        error_context err_ctx(this->stack, word.get_context_line(),
                              word.get_context_start_position(), word.get_context_end_position(),
                              this->data_payload.get_error_handler(),
                              this->data_payload.get_warning_handler());

        std::ostringstream msg;
        msg << ERROR_UNKNOWN_DIRECTIVE << " '" << *word << "'";
        err_ctx.error(msg.str());
      }
  }


template <typename Keywords, typename Characters>
lexeme::lexeme_buffer lexstream<Keywords, Characters>::get_lexeme(lexfile& input)
  {
    lexfile::status state = lexfile::status::ok;
    char c = 0;

    lexeme::lexeme_buffer word;

    // skip over white space characters and comments
    while(state == lexfile::status::ok)
      {
        std::tie(c, state) = *input;

        if(c == '%')
          {
            ++input;
    
            std::tie(c, state) = *input;
            while(c != '\n' && state == lexfile::status::ok)
              {
                ++input;
                std::tie(c, state) = *input;
              }
    
            ++input;  // skip past final '\n';
          }
        else if(c == ' ' || c == '\t' || c == '\n')
          {
            ++input;
          }
        else break;
      }

    // extract lexeme from next characters on the input stream
    if(state != lexfile::status::ok) return word;
    
    // cache current input position
    word.set_context_line(input.get_current_line());
    word.set_context_end_position(input.get_current_char_pos());
    
    if(isalpha(c) || c == '_' || c == '$')            // looks like identifier or reserved work
      {
        word = c;
        ++input;

        std::tie(c, state) = *input;
        while((isalnum(c) || c == '_' || c == '$') && state == lexfile::status::ok)
          {
            word += c;
            ++input;
            std::tie(c, state) = *input;
          }

        word.set_type(lexeme::lexeme_buffer::type::string);
      }
    else if(isdigit(c))                               // looks like a number
      {
        bool dot = false;                             // does the number have a decimal point?
        bool hex = false;                             // is this number in hexadecimal?
        bool eng = false;                             // expecting an exponent?

        word = c;
        ++input;

        std::tie(c, state) = *input;
        while((isdigit(c)
               || (dot == false && c == '.')
               || (hex == false && dot == false && c == 'x')
               || (hex == true && (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f'
                                   || c == 'A' || c == 'B' || c == 'D' || c == 'E' || c == 'F'))
               || (hex == false && (c == 'e' || c == 'E'))
               || (eng == true && (c == '+' || c == '-')))
              && state == lexfile::status::ok)
          {
            word += c;

            if(c == '.') dot = true;
            if(c == 'x') hex = true;

            // not expecting an exponent (so '+' and '-' should not be considered part of this lexeme, unless
            // they immediately follow an 'e' or an 'E'
            eng = false;
            if((c == 'e' || c == 'E') && hex == false) eng = true;

            ++input;
            std::tie(c, state) = *input;
          }

        word.set_type(lexeme::lexeme_buffer::type::number);
      }
    else if(c == '-')                                 // could be a '-' sign, or could be the first char of '->'
      {
        word = c;
        ++input;

        std::tie(c, state) = *input;
        if(c == '>' && state == lexfile::status::ok)
          {
            word += c;
            ++input;
          }

        word.set_type(lexeme::lexeme_buffer::type::character);
      }
    else if(c == '.')                                 // could be a '.', or could be the first char of '...'
      {
        word = c;
        ++input;

        std::tie(c, state) = *input;
        if(c == '.' && state == lexfile::status::ok)
          {
            word += c;
            ++input;

            std::tie(c, state) = *input;
            if(c == '.' && state == lexfile::status::ok)
              {
                word += c;
                ++input;
              }
            else
              {
                error_context err_ctx(this->stack, word.get_context_line(),
                                      word.get_context_start_position(), input.get_current_char_pos(),
                                      this->data_payload.get_error_handler(),
                                      this->data_payload.get_warning_handler());
                err_ctx.error(ERROR_EXPECTED_ELLIPSIS);
                word += '.';                          // make up to a proper ellipsis anyway
              }
          }

        word.set_type(lexeme::lexeme_buffer::type::character);
      }
    else if(c == '"')                                 // looks like a string literal
      {
        ++input;

        std::tie(c, state) = *input;
        while(c != '"' && state == lexfile::status::ok)
          {
            word += c;
            ++input;
            std::tie(c, state) = *input;
          }

        if(c == '"')
          {
            ++input;
          }
        else
          {
            error_context err_ctx(this->stack, word.get_context_line(),
                                  word.get_context_start_position(), input.get_current_char_pos(),
                                  this->data_payload.get_error_handler(),
                                  this->data_payload.get_warning_handler());
            err_ctx.error(ERROR_EXPECTED_CLOSE_QUOTE);
          }

        word.set_type(lexeme::lexeme_buffer::type::string_literal);
      }
    else
      {
        word = c;
        ++input;

        word.set_type(lexeme::lexeme_buffer::type::character);
      }
    
    word.set_current_end_position(input.get_current_char_pos());
    
    return word;
  }


#endif //CPPTRANSPORT_LEXSTREAM_H
