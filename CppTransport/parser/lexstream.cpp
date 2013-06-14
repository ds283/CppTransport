//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <assert.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

#include "lexstream.h"
#include "lexfile.h"
#include "error.h"


// ******************************************************************


static bool parse            (const std::string filename, finder* search,
                              std::deque<struct lexeme>& lexeme_list, std::deque<struct inclusion>& inclusions);

static void lexicalize       (std::deque<struct lexeme>& lexeme_list, finder* search,
                              std::deque<struct inclusion>& inclusions, lexfile& input);

static std::string get_lexeme(lexfile& input, enum lexeme_buffer_type& type, std::deque<struct inclusion>& p);

// ******************************************************************


// convert the contents of 'filename' to a string of lexemes, descending into
// included files as necessary
lexstream::lexstream(const std::string filename, finder* search)
  {
    assert(search != NULL);

    std::deque<struct inclusion> inclusions;

    if(parse(filename, search, lexeme_list, inclusions) == false)
      {
        std::ostringstream msg;
        msg << ERROR_OPEN_TOPLEVEL << " '" << filename << "'";
        error(msg.str());
      }
  }

// destroy lexeme stream
lexstream::~lexstream(void)
  {
  }


// ******************************************************************


// output list of lexemes
void lexstream::dump(std::ostream& stream)
  {
    for(int i = 0; i < this->lexeme_list.size(); i++)
      {
        this->lexeme_list[i].dump(stream);
      }
  }

// ******************************************************************

static bool parse(const std::string filename, finder* search,
                  std::deque<struct lexeme>& lexeme_list, std::deque<struct inclusion>& inclusions)
  {
    assert(search != NULL);

    unsigned int  line = 0;
    std::string   path = "";

    bool found = search->fqpn(filename, path);

    if(found)
      {
        lexfile input(path);

        // remember this inclusion
        struct inclusion inc;
        inc.line = 0;                // line doesn't count for the topmost file on the stack
        inc.name = path;

        inclusions.push_front(inc);  // note this takes a copy of inc

        lexicalize(lexeme_list, search, inclusions, input);

        inclusions.pop_front();      // pop this file from the stack
      }

    return(found);
  }

static void lexicalize(std::deque<struct lexeme>& lexeme_list, finder* search,
                       std::deque<struct inclusion>& inclusions, lexfile& input)
  {
    while(input.current_state() == lex_ok)
      {
        enum lexeme_buffer_type type;
        std::string             word = get_lexeme(input, type, inclusions);

        if(word != "")
          {
            switch(type)
              {
                case buf_symbol:
                  if(word == "#")                         // treat as a preprocessor directive
                    {
                      word = get_lexeme(input, type, inclusions);     // get next lexeme

                      if(word == "include")               // inclusion directive
                        {
                          word = get_lexeme(input, type, inclusions);

                          if(type != buf_string_literal)
                            {
                              error(ERROR_INCLUDE_DIRECTIVE, input.current_line(), inclusions);
                            }
                          else
                            {
                              inclusions[0].line = input.current_line(); // update which line this file was included from
                              if(parse(word, search, lexeme_list, inclusions) == false)
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
                      lexeme_list.push_back(lexeme(word, type, inclusions, input.current_line()));
                    }
                  break;

                case buf_string:
                case buf_number:
                case buf_string_literal:
                  lexeme_list.push_back(lexeme(word, type, inclusions, input.current_line()));
                  break;

                default:
                  assert(false);                          // should never get here
              }
          }
      }
  }

static std::string get_lexeme(lexfile& input, enum lexeme_buffer_type& type, std::deque<struct inclusion>& p)
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
                type = buf_string;
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
                type = buf_number;
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
                type = buf_symbol;
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
                type = buf_symbol;
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
                type = buf_string_literal;
              }
            else
              { word = c;
                input.eat();
                type = buf_symbol;
              }
          }
      }

    return(word);
  }
