//
// Created by David Seery on 25/06/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_TRANSLATION_UNIT_H
#define CPPTRANSPORT_TRANSLATION_UNIT_H

#include <stdexcept>

#include "lexstream.h"
#include "y_common.h"
#include "y_lexer.h"
#include "parser/driver/y_driver.h"
#include "y_parser.hpp"

#include "stepper.h"
#include "model_settings.h"
#include "output_stack.h"
#include "translator.h"
#include "translator_data.h"
#include "local_environment.h"
#include "argument_cache.h"
#include "version_policy.h"

#include "symbol_factory.h"


class exit_parse : public std::runtime_error
  {
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    exit_parse(std::string m)
      : std::runtime_error(std::move(m))
      {
      }
    
    //! destructor is default
    ~exit_parse() = default;
    
  };


//! encapsulates the entire pipeline for translating an input script
class translation_unit
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    translation_unit(boost::filesystem::path file, finder& p, argument_cache& c, local_environment& e,
                     version_policy& vp);

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
    
    //! push names of output files and header guards to translator data payload
    void populate_output_filenames();

		//! print an advisory message, if the current verbosity level is set sufficiently high
    void print_advisory(const std::string& msg);

		//! print a warning message - no context data
		void warn(const std::string& msg);

		//! print an error message - no context data
		void error(const std::string& msg);

    //! print a warning message - with context data
    void context_warn(const std::string& msg, const error_context& ctx);

    //! print an error message - with context data
    void context_error(const std::string& msg, const error_context& ctx);

    //! construct output name (input not const or taken by reference because modified internally)
    boost::filesystem::path mangle_output_name(const boost::filesystem::path& input, const std::string& tag);

    //! deduce template suffix from input name (input not const or taken by reference because modified internally)
    std::string get_template_suffix(std::string input);


    // INTERNAL DATA

  private:

    // POLICY OBJECTS

    //! reference to finder
    finder& path;
    
    //! reference to argument cache
    argument_cache& cache;
    
    //! reference to local environment
    local_environment& env;
    
    //! version policy registry
    version_policy& policy;
    
    // TRANSLATION UNIT RESOURCES
    
    //! GiNaC symbol factory; needed because we must have only *one* instance of each
    //! symbol, due to the way GiNaC works (lexically equivalent symbols are not mathematically
    //! equivalent if they are not copied from the same base object).
    //! There is one symbol factory for each translation unit.
    symbol_factory sym_factory;

    //! filestack recording where we are in the template inclusion tree
    output_stack stack;

    //! cache name of input script
    boost::filesystem::path name;

    
    // ERROR HANDLING
    
    //! flag to capture state of parse
    bool parse_failed;

    //! number of reported errors (global)
    unsigned int errors;
    
    //! number of reported errors (per file)
    unsigned int file_errors;

    //! number of reported warnings
    unsigned int warnings;
    
    
    // TRANSLATION
    
    //! model descriptor -- encapsulates all details about the model;
    //! is populated during parsing by y_driver
    model_descriptor model;
    
    //! data bundle passed to translator
    translator_data translator_payload;
    
    //! translator object: performs actual translation of template files
    //! must be constructed *after* translator_payload, path, stack, sym_factory
    translator outstream;
    
    // PARSING
    
    //! data bundle passed to lexstream object
    //! must be constructed *after* path, cache
    lexstream_data lexstream_payload;
    
    //! lexeme stream representing input
    y::lexstream_type instream;
    
    //! lexer object interfacing between lexstream and Bison
    y::y_lexer lexer;
    
    //! Bison parser driver
    y::y_driver driver;
    
    //! Bison parser
    y::y_parser parser;

  };


#endif // CPPTRANSPORT_TRANSLATION_UNIT_H
