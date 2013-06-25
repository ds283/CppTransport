//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __lexstream_H_
#define __lexstream_H_

#include <iostream>
#include <string>
#include <deque>
#include <fstream>
#include <sstream>
#include <cctype>

#include "lexeme.h"
#include "finder.h"
#include "lexfile.h"
#include "error.h"


template <class keywords, class characters>
class lexstream
  {
    public:
      lexstream(const std::string filename, finder* search,
								std::string* kt, keywords* km, unsigned int num_k,
								std::string* ct, characters* cm, unsigned int num_c);
      ~lexstream();

      void							                    reset();
      lexeme::lexeme<keywords, characters>* get();
      bool							                    eat();
      bool							                    state();

      void								                  print(std::ostream& stream);

    private:
		  bool                                  parse     (std::string file, finder* search, std::deque<struct inclusion>& inclusions);

		  void                                  lexicalize(finder* search, std::deque<struct inclusion>& inclusions, lexfile& input);

		  std::string                           get_lexeme(lexfile& input, enum lexeme::lexeme_buffer_type& type, std::deque<struct inclusion>& p);

      std::deque< lexeme::lexeme<keywords, characters> >                    lexeme_list; // list of lexemes obtained from the file

      typename std::deque< lexeme::lexeme<keywords, characters> >::iterator ptr;         // pointer to current lexeme position (when reading)
      bool                                                                  ptr_valid;   // sentry - validity of ptr?

		  unsigned int															                            unique;

      std::string*                                                          ktable;      // table of keywords
      keywords*                                                             kmap;        // table of keyword token types
      unsigned int                                                          Nk;          // number of keywords

      std::string*                                                          ctable;      // table of 'characters'
      characters*                                                           cmap;        // table of character token types
      unsigned int                                                          Nc;          // number of characters
  };


// IMPLEMENTATION


// ******************************************************************


// convert the contents of 'filename' to a string of lexemes, descending into
// included files as necessary
template <class keywords, class characters>
lexstream<keywords, characters>::lexstream(const std::string filename, finder* search,
                                           std::string* kt, keywords* km, unsigned int num_k,
                                           std::string*ct, characters*cm, unsigned int num_c)
  : ptr_valid(false), ktable(kt), kmap(km), Nk(num_k), ctable(ct), cmap(cm), Nc(num_c)
  {
    assert(search != NULL);
    assert(ktable != NULL);
    assert(kmap   != NULL);
    assert(ctable != NULL);
    assert(cmap != NULL);

    std::deque<struct inclusion> inclusions;

    if(parse(filename, search, inclusions) == false)
      {
        std::ostringstream msg;
        msg << ERROR_OPEN_TOPLEVEL << " '" << filename << "'";
        error(msg.str());
      }
  }

// destroy lexeme stream
template <class keywords, class characters>
lexstream<keywords, characters>::~lexstream(void)
  {
	 return;
  }


// ******************************************************************


// output list of lexemes
template <class keywords, class characters>
void lexstream<keywords, characters>::print(std::ostream& stream)
  {
    for(int i = 0; i < this->lexeme_list.size(); i++)
      {
        this->lexeme_list[i].dump(stream);
      }
  }

// ******************************************************************


template <class keywords, class characters>
void lexstream<keywords, characters>::reset()
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

template <class keywords, class characters>
lexeme::lexeme<keywords, characters>* lexstream<keywords, characters>::get()
  {
    lexeme::lexeme<keywords, characters>* rval = NULL;

    if(this->ptr_valid)
      {
        rval = &(*(this->ptr));
      }

    return(rval);
  }

template <class keywords, class characters>
bool lexstream<keywords, characters>::eat()
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

template <class keywords, class characters>
bool lexstream<keywords, characters>::state()
  {
    return(this->ptr_valid);
  }


// ******************************************************************


template <class keywords, class characters>
bool lexstream<keywords, characters>::parse(std::string file, finder* search, std::deque<struct inclusion>& inclusions)
  {
    assert(search != NULL);

    std::string path = "";
    bool        found = search->fqpn(file, path);

    if(found)
      {
        lexfile input(path);

        // remember this inclusion
        struct inclusion inc;
        inc.line = 0;                // line doesn't count for the topmost file on the stack
        inc.name = path;

        inclusions.push_front(inc);  // note this takes a copy of inc

        lexicalize(search, inclusions, input);

        inclusions.pop_front();      // pop this file from the stack
      }

    return(found);
  }

template <class keywords, class characters>
void lexstream<keywords, characters>::lexicalize(finder* search, std::deque<struct inclusion>& inclusions, lexfile& input)
  {
    while(input.current_state() == lex_ok)
      {
        enum lexeme::lexeme_buffer_type type;
        std::string             word = get_lexeme(input, type, inclusions);

        if(word != "")
          {
            switch(type)
              {
                case lexeme::buf_character:
                  if(word == "#")                                     // treat as a preprocessor directive
                    {
                      word = get_lexeme(input, type, inclusions);     // get next lexeme

                      if(word == "include")                           // inclusion directive
                        {
                          word = get_lexeme(input, type, inclusions);

                          if(type != lexeme::buf_string_literal)
                            {
                              error(ERROR_INCLUDE_DIRECTIVE, input.current_line(), inclusions);
                            }
                          else
                            {
                              inclusions[0].line = input.current_line(); // update which line this file was included from
                              if(parse(word, search, inclusions) == false)
                                {
                                  std::ostringstream msg;
                                  msg << ERROR_INCLUDE_FILE << " '" << word << "'";
                                  error(msg.str(), input.current_line(), inclusions);
                                }
                            }
                        }
                    }
                  else
                    {
                      lexeme_list.push_back(lexeme::lexeme<enum keyword_type, enum character_type>
                                                (word, type, inclusions, input.current_line(), this->unique++,
                                                 this->ktable, this->kmap, this->Nk,
                                                 this->ctable, this->cmap, this->Nc));
                    }
                  break;

                case lexeme::buf_string:
                case lexeme::buf_number:
                case lexeme::buf_string_literal:
                  lexeme_list.push_back(lexeme::lexeme<enum keyword_type, enum character_type>
                                            (word, type, inclusions, input.current_line(), this->unique++,
                                             this->ktable, this->kmap, this->Nk,
                                             this->ctable, this->cmap, this->Nc));
                  break;

                default:
                  assert(false);                          // should never get here
              }
          }
      }
  }

template <class keywords, class characters>
std::string lexstream<keywords, characters>::get_lexeme(lexfile& input, enum lexeme::lexeme_buffer_type& type, std::deque<struct inclusion>& p)
  {
    enum lexfile_outcome state = lex_ok;
    char                 c     = 0;

    std::string          word   = "";

	  while(word == "" && state == lex_ok)
      {
        c = input.get(state);

        if(state == lex_ok)
          {
            if(isalpha(c) || c == '_' || c == '$')            // looks like identifier or reserved work
              {
                word = c;
                input.eat();

                while((isalnum(c = input.get(state)) || c == '_' || c == '$') && state == lex_ok)
                  { word += c;
                    input.eat();
                  }
                type = lexeme::buf_string;
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
                      && state == lex_ok)
                  {
                    word += c;

                    if(c == '.') dot = true;
                    if(c == 'x') hex = true;

                    eng = false;  // not expecting an exponent (so '+' and '-' should not be considered part of this lexeme, unless:
                    if((c == 'e' || c == 'E') && hex == false) eng = true;

                    input.eat();
                  }
                type = lexeme::buf_number;
              }
            else if(c == '%')                                 // single-line comment
              {
                input.eat();

                while((c = input.get(state)) != '\n' && state == lex_ok)
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
                if((c = input.get(state)) == '>' && state == lex_ok)
                  {
                    word += c;
                    input.eat();
                  }
                type = lexeme::buf_character;
              }
            else if(c == '.')                                 // could be a '.', or could be the first char of '...'
              {
                word = c;
                input.eat();
                if((c = input.get(state)) == '.' && state == lex_ok)
                  {
                    word += c;
                    input.eat();
                    if((c = input.get(state)) == '.' && state == lex_ok)
                      {
                        word += c;
                        input.eat();
                      }
                    else
                      {
                        error(ERROR_EXPECTED_ELLIPSIS, input.current_line(), p);
                        word += '.';                          // make up to a proper ellipsis anyway
                      }
                  }
                type = lexeme::buf_character;
              }
            else if(c == '"')                                 // looks like a string literal
              {
                input.eat();
                c = input.get(state);
                while(c != '"' && state == lex_ok)
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
                    error(ERROR_EXPECTED_CLOSE_QUOTE, input.current_line(), p);
                  }
                type = lexeme::buf_string_literal;
              }
            else
              { word = c;
                input.eat();
                type = lexeme::buf_character;
              }
          }
      }

    return(word);
  }


#endif //__lexstream_H_
