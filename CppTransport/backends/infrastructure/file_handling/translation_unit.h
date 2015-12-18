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
#include "y_parser.hpp"

#include "stepper.h"
#include "model_settings.h"
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
    translation_unit(boost::filesystem::path file, finder& p, argument_cache& c, local_environment& e);

    //! destructor is default
    ~translation_unit() = default;


		// INTERFACE

  public:

		//! perform translation
    unsigned int apply();

    //! detect errors encountered during processing
    bool fail() const { return(this->parse_failed); }

		// INTERNAL API

  protected:

		// print an advisory message, if the current verbosity level is set sufficiently high
    void print_advisory(const std::string& msg);

		// print a warning message - no context data
		void warn(const std::string& msg);

		// print an error message - no context data
		void error(const std::string& msg);

    // print a warning message - with context data
    void context_warn(const std::string& msg, const error_context& ctx);

    // print an error message - with context data
    void context_error(const std::string& msg, const error_context& ctx);

    //! construct output name (input not const or taken by reference because modified internally)
    boost::filesystem::path mangle_output_name(const boost::filesystem::path& input, const std::string& tag);

    //! deduce template suffix from input name (input not const or taken by reference because modified internally)
    std::string get_template_suffix(std::string input);


    // INTERNAL DATA

  private:

    // GiNaC symbol factory
    symbol_factory          sym_factory;

    boost::filesystem::path name;                    // name of input script
    bool                    parse_failed;

    unsigned int            errors;
    unsigned int            warnings;

    finder&                 path;
    argument_cache&         cache;
    local_environment&      env;
    output_stack            stack;

    lexstream_data          lexstream_payload;       // must be constructed *after* path, cache
    y::lexstream_type       instream;
    y::y_lexer              lexer;
    y::y_driver             driver;
    y::y_parser             parser;

    translator_data         translator_payload;
    translator              outstream;               // must be constructed *after* data_payload, path, stack, sym_factory

  };


#endif // __input_H_
