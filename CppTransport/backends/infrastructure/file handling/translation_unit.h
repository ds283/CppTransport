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

#include "replacement_rule_package.h"


// data structure for tracking input scripts as they progress through the pipeline
class translation_unit
  {
  public:
    translation_unit(std::string file, finder* p,
          std::string core_out="", std::string implementation_out="", bool cse=true);
    ~translation_unit();

    unsigned int do_replacement();

  protected:
    lexstream<enum keyword_type, enum character_type>* stream;
    y::y_lexer*                                        lexer;
    y::y_driver*                                       driver;
    y::y_parser*                                       parser;
    std::string                                        name;
    bool                                               do_cse;

    finder*                                            path;

    std::string                                        core_output;
    std::string                                        implementation_output;

    unsigned int                                       apply(std::string in, std::string out);
    unsigned int                                       process(macro_packages::replacement_data& data);

    std::string                                        mangle_output_name(std::string input, std::string tag);
    std::string                                        get_template_suffix(std::string input);
  };


#endif // __input_H_
