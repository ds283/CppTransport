//
//  main.cpp
//  CppTransport
//
//  Created by David Seery on 12/06/2013.
//  Copyright (c) 2013 University of Sussex. All rights reserved.
//

#include <iostream>
#include <deque>

#include "core.h"
#include "error.h"
#include "msg_en.h"
#include "finder.h"
#include "lexical.h"
#include "lexstream.h"
#include "y_lexer.h"
#include "y_driver.h"
#include "y_parser.tab.hh"

// FAIL return code for Bison parser
#ifndef FAIL
#define FAIL (-1)
#endif

struct input
  {
    lexstream<enum keyword_type, enum symbol_type>* stream;
    y::y_lexer*                                     lexer;
    y::y_driver*                                    driver;
    y::y_parser*                                    parser;
    std::string                                     name;
  };


// lexical analyser definition

std::string keyword_table[] =
  {
      "name", "author", "tag", "field", "potential", "parameter", "latex",
      "abs", "step", "sqrt", "sin", "cos", "tan",
      "asin", "acos", "atan", "atan2", "sinh", "cosh", "tanh",
      "asinh", "acosh", "atanh", "exp", "log", "Li2", "Li", "G", "S", "H",
      "zeta", "zetaderiv", "tgamma", "lgamma", "beta", "psi", "factorial", "binomial"
  };

enum keyword_type keyword_map[] =
  {
      name, author, tag, field, potential, parameter,
      f_abs, f_step, f_sqrt,
      f_sin, f_cos, f_tan,
      f_asin, f_acos, f_atan, f_atan2,
      f_sinh, f_cosh, f_tanh,
      f_asinh, f_acosh, f_atanh,
      f_exp, f_log, f_Li2, f_Li, f_G, f_S, f_H,
      f_zeta, f_zetaderiv, f_tgamma, f_lgamma,
      f_beta, f_psi, f_factorial, f_binomial
  };

std::string symbol_table[] =
  {
      "{", "}", "(", ")",
      "[", "]", ",", ".", ":", ";",
      "+", "-", "*", "/", "\\", "~",
      "&", "^", "@", "...", "->"
  };

enum symbol_type symbol_map[] =
   {
       open_brace, close_brace, open_bracket, close_bracket,
       open_square, close_square, comma, period, colon, semicolon,
       plus, minus, star, backslash, foreslash, tilde,
       ampersand, circumflex, ampersat, ellipsis, rightarrow
   };


int main(int argc, const char *argv[])
  {
    std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << "\n";

    // set up the initial search path to consist only of CWD
    finder path("~");

    std::deque<struct input> inputs;

    for(int i = 1; i < argc; i++)
      {
        if(strcmp(argv[i], "-I") == 0)
          {
            path.add(std::string(argv[++i]));
          }
        else  // assume to be an input file we are processing
          {
            struct input in;

            in.name   = (std::string)argv[i];
            in.stream = new lexstream<enum keyword_type, enum symbol_type>((std::string)argv[i], &path,
                                                                           keyword_table, keyword_map, NUMBER_KEYWORDS,
                                                                           symbol_table, symbol_map, NUMBER_SYMBOLS);

            in.stream->dump(std::cout);

            in.lexer  = new y::y_lexer(in.stream);
            in.driver = new y::y_driver();
            in.parser = new y::y_parser(in.lexer, in.driver);

            if(in.parser->parse() == FAIL)
              {
                warn(WARNING_PARSING_FAILED + (std::string)(" '") + in.name + (std::string)("'"));
              }

            inputs.push_back(in);
          }
      }

    // deallocate lexeme storage
    for(int i = 0; i < inputs.size(); i++)
      {
        delete inputs[i].stream;
        delete inputs[i].parser;
        delete inputs[i].lexer;
        delete inputs[i].driver;
      }

    return 0;
  }
