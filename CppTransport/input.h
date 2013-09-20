//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __input_H_
#define __input_H_

#include "lexstream.h"
#include "y_lexer.h"
#include "y_driver.h"
#include "y_parser.tab.hh"


// data structure for tracking input scripts as they progress through the pipeline
struct input
  {
    lexstream<enum keyword_type, enum character_type>* stream;
    y::y_lexer*                                        lexer;
    y::y_driver*                                       driver;
    y::y_parser*                                       parser;
    std::string                                        name;

    std::string                                        core_output;
    std::string                                        implementation_output;
  };


#endif // __input_H_