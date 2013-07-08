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
#include "input.h"
#include "backends.h"

// FAIL return code for Bison parser
#ifndef FAIL
#define FAIL (1)
#endif


// ******************************************************************

// lexical analyser data


const std::string keyword_table[] =
  {
      "name", "author", "tag", "field", "potential",
      "parameter", "latex", "class", "model",
      "abserr", "relerr", "stepper", "stepsize",
      "background", "perturbations",
      "abs", "step", "sqrt", "sin", "cos", "tan",
      "asin", "acos", "atan", "atan2", "sinh", "cosh", "tanh",
      "asinh", "acosh", "atanh", "exp", "log", "Li2", "Li", "G", "S", "H",
      "zeta", "zetaderiv", "tgamma", "lgamma", "beta", "psi", "factorial", "binomial"
  };

const enum keyword_type keyword_map[] =
  {
      k_name, k_author, k_tag, k_field, k_potential,
      k_parameter, k_latex, k_class, k_model,
      k_abserr, k_relerr, k_stepper, k_stepsize,
      k_background, k_perturbations,
      f_abs, f_step, f_sqrt,
      f_sin, f_cos, f_tan,
      f_asin, f_acos, f_atan, f_atan2,
      f_sinh, f_cosh, f_tanh,
      f_asinh, f_acosh, f_atanh,
      f_exp, f_log, f_Li2, f_Li, f_G, f_S, f_H,
      f_zeta, f_zetaderiv, f_tgamma, f_lgamma,
      f_beta, f_psi, f_factorial, f_binomial
  };

const std::string character_table[] =
  {
      "{", "}", "(", ")",
      "[", "]", ",", ".", ":", ";",
      "=", "+", "-", "*", "/", "\\", "~",
      "&", "^", "@", "...", "->"
  };

const enum character_type character_map[] =
   {
       open_brace, close_brace, open_bracket, close_bracket,
       open_square, close_square, comma, period, colon, semicolon,
       equals, plus, minus, star, backslash, foreslash, tilde,
       ampersand, circumflex, ampersat, ellipsis, rightarrow
   };


// ******************************************************************


const std::string backend_table[] =
  {
    "cpp"
  };

const backend_function backend_dispatcher[] =
  {
    cpp_backend
  };

bool backend_selector[] =
  {
    true
  };


// ******************************************************************


static std::string mangle_output_name(std::string input);


// ******************************************************************


int main(int argc, const char *argv[])
  {
    std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << std::endl;

    // set up the initial search path to consist only of CWD
    finder path("~");

    std::deque<struct input> inputs;

    for(int i = 1; i < argc; i++)
      {
        if(strcmp(argv[i], "-I") == 0)
          {
            if(i + 1 < argc)
              {
                path.add(std::string(argv[++i]));
              }
            else
              {
                std::ostringstream msg;
                msg << ERROR_MISSING_PATHNAME << " -I";
                error(msg.str());
              }
          }
        else if(strcmp(argv[i], "--backend") == 0)
          {
            std::string backend = "";
            bool        found   = false;

            if(i + 1 < argc)
              {
                backend = argv[++i];
              }
            else
              {
                error(ERROR_MISSING_BACKEND);
              }

            for(int i = 0; i < NUMBER_BACKENDS; i++)
              {
                if(backend == backend_table[i])
                  {
                    backend_selector[i] = true;
                    found = true;
                  }
              }
            if(!found)
              {
                std::ostringstream msg;
                msg << ERROR_UNKNOWN_BACKEND << " '" << backend << "'";
                error(msg.str());
              }
          }
        else  // assume to be an input file we are processing
          {
            struct input in;

            // lexicalize this input file
            in.name   = (std::string)argv[i];
            in.stream = new lexstream<enum keyword_type, enum character_type>((std::string)argv[i], &path,
                                                                              keyword_table, keyword_map, NUMBER_KEYWORDS,
                                                                              character_table, character_map, NUMBER_CHARACTERS);

            // in.stream->print(std::cerr);

            // now pass to the parser for syntactic analysis
            in.lexer  = new y::y_lexer(in.stream);
            in.driver = new y::y_driver();
            in.parser = new y::y_parser(in.lexer, in.driver);

            if(in.parser->parse() == FAIL)
              {
                warn(WARNING_PARSING_FAILED + (std::string)(" '") + in.name + (std::string)("'"));
              }

            // in.driver->get_script()->print(std::cerr);
            in.output = mangle_output_name(in.name);

            inputs.push_back(in);
          }
      }

    // pass each translation unit off to the selected backends
    for(int i = 0; i < inputs.size(); i++)
      {
        for(int j = 0; j < NUMBER_BACKENDS; j++)
          {
            if(backend_selector[j])
              {
                if((*(backend_dispatcher[j]))(inputs[i], &path) == false)
                  {
                    std::ostringstream msg;
                    msg << ERROR_BACKEND_FAILURE << " '" << backend_table[j] << "'";
                    error(msg.str());
                  };
              }
          }
      }

    // deallocate  storage
    for(int i = 0; i < inputs.size(); i++)
      {
        delete inputs[i].stream;
        delete inputs[i].parser;
        delete inputs[i].lexer;
        delete inputs[i].driver;
      }

    return 0;
  }


// ******************************************************************


static std::string mangle_output_name(std::string input)
  {
    size_t      pos = 0;
    std::string output;

    if((pos = input.find(MODEL_SCRIPT_SUFFIX)) != std::string::npos)
      {
        if(pos == input.length() - MODEL_SCRIPT_SUFFIX_LENGTH)
          {
            output = input.erase(input.length() - MODEL_SCRIPT_SUFFIX_LENGTH, std::string::npos);
          }
        else
          {
            output = input;
          }
      }

    return(output);
  }