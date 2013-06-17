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
    lexstream*   stream;
    y::y_lexer*  lexer;
    y::y_driver* driver;
    y::y_parser* parser;
    std::string  name;
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
            in.stream = new lexstream((std::string)argv[i], &path);

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
