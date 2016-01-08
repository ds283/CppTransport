//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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


template <class Keywords, class Characters>
class lexstream
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    lexstream(lexstream_data& p,
              const std::vector<std::string>& kt, const std::vector<Keywords>& km,
              const std::vector<std::string>& ct, const std::vector<Characters>& cm, const std::vector<bool>& ctx);

    ~lexstream() = default;


    // INTERFACE

  public:

    void reset();

    lexeme::lexeme<Keywords, Characters>* get();

    bool eat();

    bool state();

    void print(std::ostream& stream);


    // INTERNAL API

  private:

    bool parse(const boost::filesystem::path& file);

    void lexicalize(lexfile& input);

    std::string get_lexeme(lexfile& input, enum lexeme::buffer_type& type);


    // INTERNAL DATA

  private:

    // data object passed to us by the translation unit
    lexstream_data& data_payload;

    input_stack stack;       // stack of included files

    std::deque< lexeme::lexeme<Keywords, Characters> > lexeme_list; // list of lexemes obtained from the file

    typename std::deque< lexeme::lexeme<Keywords, Characters> >::iterator ptr;         // pointer to current lexeme position (when reading)
    bool                                                                  ptr_valid;   // sentry - validity of ptr?

    unsigned int unique;

    const std::vector<std::string>& ktable;      // table of Keywords
    const std::vector<Keywords>&    kmap;        // table of keyword token types

    const std::vector<std::string>& ctable;      // table of 'characters'
    const std::vector<Characters>&  cmap;        // table of character token types
    const std::vector<bool>&        ccontext;    // keeps track of 'unary' context; true if can precede a unary minus

	};


// IMPLEMENTATION


// ******************************************************************


// convert the contents of 'filename' to a string of lexemes, descending into
// included files as necessary
template <class Keywords, class Characters>
lexstream<Keywords, Characters>::lexstream(lexstream_data& p,
                                           const std::vector<std::string>& kt, const std::vector<Keywords>& km,
                                           const std::vector<std::string>& ct, const std::vector<Characters>& cm, const std::vector<bool>& ctx)
  : ptr_valid(false),
    ktable(kt),
    kmap(km),
    ctable(ct),
    cmap(cm),
    ccontext(ctx),
    data_payload(p),
    unique(1)
  {
    if(!this->parse(data_payload.get_model_input()))
      {
        std::ostringstream msg;
        msg << ERROR_OPEN_TOPLEVEL << " '" << data_payload.get_model_input() << "'";

        error_context err_context(stack, data_payload.get_error_handler(), data_payload.get_warning_handler());
        err_context.error(msg.str());
      }
  }


// ******************************************************************


// output list of lexemes
template <class Keywords, class Characters>
void lexstream<Keywords, Characters>::print(std::ostream& stream)
  {
    for(int i = 0; i < this->lexeme_list.size(); ++i)
      {
        this->lexeme_list[i].dump(stream);
      }
  }


// ******************************************************************


template <class Keywords, class Characters>
void lexstream<Keywords, Characters>::reset()
  {
    if(this->lexeme_list.size() > 0)
      {
        this->ptr       = this->lexeme_list.begin();
        this->ptr_valid = true;
      }
    else
      {
        this->ptr_valid = false;
      }
  }

template <class Keywords, class Characters>
lexeme::lexeme<Keywords, Characters>* lexstream<Keywords, Characters>::get()
  {
    lexeme::lexeme<Keywords, Characters>* rval = nullptr;

    if(this->ptr_valid)
      {
        rval = &(*(this->ptr));
      }

    return(rval);
  }

template <class Keywords, class Characters>
bool lexstream<Keywords, Characters>::eat()
  {
    if(this->ptr_valid)
      {
        this->ptr++;
        if(this->ptr == this->lexeme_list.end())
          {
            this->ptr_valid = false;
          }
      }
    else
      {
        this->ptr_valid = false;
      }

    return(this->ptr_valid);
  }

template <class Keywords, class Characters>
bool lexstream<Keywords, Characters>::state()
  {
    return(this->ptr_valid);
  }


// ******************************************************************


template <class Keywords, class Characters>
bool lexstream<Keywords, Characters>::parse(const boost::filesystem::path& file)
  {
    boost::filesystem::path path;
    bool found = this->data_payload.get_finder().fqpn(file, path);

    if(found)
      {
        // the lexfile object reponsible for reading in a file persists only within this block,
        // but the lines it reads in are managed with std::shared_ptr<>

        // the lexemes which are generated during lexicalization
        // then inherit ownership of these lines, so even though the
        // lexfile object itself is destroyed we are not left with dangling pointers

        // push parent directory into search paths
        if(path.has_parent_path())
          {
            this->data_payload.get_finder().add(path.parent_path());
          }

        lexfile input(path, this->stack);

        this->stack.push(path);
        this->lexicalize(input);
        this->stack.pop();
      }

    return(found);
  }

template <class Keywords, class Characters>
void lexstream<Keywords, Characters>::lexicalize(lexfile& input)
  {
    enum lexeme::minus_context context = lexeme::minus_context::unary;      // keep track of whether we expect unary or binary minus sign

    while(input.current_state() == lexfile_outcome::ok)
      {
        enum lexeme::buffer_type type;
        std::string              word = this->get_lexeme(input, type);

        if(word != "")
          {
            switch(type)
              {
                case lexeme::buffer_type::character:
                  {
                    if(word == "#")                                               // treat as a preprocessor directive
                      {
                        word = this->get_lexeme(input, type);                     // get next lexeme

                        if(word == "include")                                     // inclusion directive
                          {
                            word = this->get_lexeme(input, type);

                            if(type != lexeme::buffer_type::string_literal)
                              {
                                error_context err_ctx(this->stack, input.get_current_line(), input.get_current_char_pos(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                                err_ctx.error(ERROR_INCLUDE_DIRECTIVE);
                              }
                            else
                              {
                                if(!this->parse(word))
                                  {
                                    error_context err_ctx(this->stack, input.get_current_line(), input.get_current_char_pos(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());

                                    std::ostringstream msg;
                                    msg << ERROR_INCLUDE_FILE << " '" << word << "'";
                                    err_ctx.error(msg.str());
                                  }
                              }
                          }
                      }
                    else
                      {
                        // note: this updates context, depending what the lexeme is recognized as
                        error_context err_ctx(this->stack, input.get_current_line(), input.get_current_char_pos(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                        this->lexeme_list.emplace_back(word, type, context, this->unique++, err_ctx,
                                                       this->ktable, this->kmap, this->ctable, this->cmap, this->ccontext);
                      }
                    break;
                  }

                case lexeme::buffer_type::string:
                case lexeme::buffer_type::number:
                case lexeme::buffer_type::string_literal:
                  {
                    // note: this updates context, depending what the lexeme is recognized as
                    error_context err_ctx(this->stack, input.get_current_line(), input.get_current_char_pos(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                    this->lexeme_list.emplace_back(word, type, context, this->unique++, err_ctx,
                                                   this->ktable, this->kmap, this->ctable, this->cmap, this->ccontext);
                    break;
                  }
              }
          }
      }
  }

template <class Keywords, class Characters>
std::string lexstream<Keywords, Characters>::get_lexeme(lexfile& input, enum lexeme::buffer_type& type)
  {
    enum lexfile_outcome state = lexfile_outcome::ok;
    char                 c     = 0;

    std::string          word   = "";

	  while(word == "" && state == lexfile_outcome::ok)
      {
        c = input.get(state);

        if(state == lexfile_outcome::ok)
          {
            if(isalpha(c) || c == '_' || c == '$')            // looks like identifier or reserved work
              {
                word = c;
                input.eat();

                while((isalnum(c = input.get(state)) || c == '_' || c == '$') && state == lexfile_outcome::ok)
                  { word += c;
                    input.eat();
                  }
                type = lexeme::buffer_type::string;
              }
            else if(isdigit(c))                               // looks like a number
              {
                bool dot = false;                             // does the number have a decimal point?
                bool hex = false;                             // is this number in hexadecimal?
                bool eng = false;                             // expecting an exponent?

                word = c;
                input.eat();

                while((isdigit(c = input.get(state))
                       || (dot == false && c == '.')
                       || (hex == false && dot == false && c == 'x')
                       || (hex == true  && (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f'
                                            || c == 'A' || c == 'B' || c == 'D' || c == 'E' || c == 'F'))
                       || (hex == false && (c == 'e' || c == 'E'))
                       || (eng == true  && (c == '+' || c == '-')))
                      && state == lexfile_outcome::ok)
                  {
                    word += c;

                    if(c == '.') dot = true;
                    if(c == 'x') hex = true;

                    eng = false;  // not expecting an exponent (so '+' and '-' should not be considered part of this lexeme, unless:
                    if((c == 'e' || c == 'E') && hex == false) eng = true;

                    input.eat();
                  }
                type = lexeme::buffer_type::number;
              }
            else if(c == '%')                                 // single-line comment
              {
                input.eat();

                while((c = input.get(state)) != '\n' && state == lexfile_outcome::ok)
                  {
                    input.eat();
                  }
                input.eat();
              }
            else if(c == ' ' || c == '\t' || c == '\n')       // skip over white-space characters
              {
                input.eat();
              }
            else if(c == '-')                                 // could be a '-' sign, or could be the first char of '->'
              {
                word = c;
                input.eat();
                if((c = input.get(state)) == '>' && state == lexfile_outcome::ok)
                  {
                    word += c;
                    input.eat();
                  }
                type = lexeme::buffer_type::character;
              }
            else if(c == '.')                                 // could be a '.', or could be the first char of '...'
              {
                word = c;
                input.eat();
                if((c = input.get(state)) == '.' && state == lexfile_outcome::ok)
                  {
                    word += c;
                    input.eat();
                    if((c = input.get(state)) == '.' && state == lexfile_outcome::ok)
                      {
                        word += c;
                        input.eat();
                      }
                    else
                      {
                        error_context err_ctx(this->stack, input.get_current_line(), input.get_current_char_pos(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                        err_ctx.error(ERROR_EXPECTED_ELLIPSIS);
                        word += '.';                          // make up to a proper ellipsis anyway
                      }
                  }
                type = lexeme::buffer_type::character;
              }
            else if(c == '"')                                 // looks like a string literal
              {
                input.eat();
                c = input.get(state);
                while(c != '"' && state == lexfile_outcome::ok)
                  {
                    word += c;
                    input.eat();
                    c = input.get(state);
                  }
                if(c == '"')
                  {
                    input.eat();
                  }
                else
                  {
                    error_context err_ctx(this->stack, input.get_current_line(), input.get_current_char_pos(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                    err_ctx.error(ERROR_EXPECTED_CLOSE_QUOTE);
                  }
                type = lexeme::buffer_type::string_literal;
              }
            else
              {
                word = c;
                input.eat();
                type = lexeme::buffer_type::character;
              }
          }
      }

    return(word);
  }


#endif //CPPTRANSPORT_LEXSTREAM_H
