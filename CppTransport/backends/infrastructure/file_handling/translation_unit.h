//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __input_H_
#define __input_H_

#include "lexstream.h"
#include "y_common.h"
#include "y_lexer.h"
#include "y_driver.h"
#include "y_parser.tab.hh"

#include "stepper.h"
#include "indexorder.h"
#include "output_stack.h"
#include "translator.h"
#include "translator_data.h"
#include "local_environment.h"
#include "argument_cache.h"

#include "symbol_factory.h"


// data structure for tracking input scripts as they progress through the pipeline
class translation_unit
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    translation_unit(std::string file, finder& p, argument_cache& c, local_environment& e);

    //! destructor is default
    ~translation_unit() = default;


		// INTERFACE

  public:

		// perform translation
    unsigned int apply();


		// INTERNAL API

  protected:

		// print an advisory message, if the current verbosity level is set sufficiently high
    void print_advisory(const std::string& msg);

		// print a warning message
		void warn(const std::string& msg);

		// print an error message
		void error(const std::string& msg);

    //! construct output name (input not const or taken by reference because modified internally)
    std::string mangle_output_name(std::string input, const std::string& tag);

    //! deduce template suffix frmo input name (input not const or taken by reference because modified internally)
    std::string get_template_suffix(std::string input);


		// INTERFACE - EXTRACT INFORMATION ABOUT THIS TRANSLATION UNIT

  public:

    translator&                                        get_translator() { return(this->outstream); }


    // INTERNAL DATA

  private:

    // GiNaC symbol factory
    symbol_factory                                     sym_factory;

    std::unique_ptr<y::lexstream_type>                 stream;
    std::unique_ptr<y::y_lexer>                        lexer;
    y::y_driver                                        driver;
    std::unique_ptr<y::y_parser>                       parser;

    std::string                                        name;                    // name of input script
		bool                                               parse_failed;

    finder&                                            path;
    argument_cache&                                    cache;
    local_environment&                                 env;
    output_stack                                       stack;

    translator_data                                    data_payload;

    translator                                         outstream;               // must be constructed *after* data_payload, path, stack, sym_factory

  };


#endif // __input_H_
