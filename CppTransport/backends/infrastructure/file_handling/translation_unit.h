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


		// UTILITIES

  public:

		// print an advisory message, if the current verbosity level is set sufficiently high
    void print_advisory(const std::string& msg);

		// print a warning message
		void warn(const std::string& msg);

		// print an error message
		void error(const std::string& msg);


		// INTERFACE - EXTRACT INFORMATION ABOUT THIS TRANSLATION UNIT

  public:

    const std::string&                                 get_model_input() const;

    const std::string&                                 get_core_output() const;
    const std::string&                                 get_implementation_output() const;

    const std::string&                                 get_core_guard() const;
    const std::string&                                 get_implementation_guard() const;

    bool                                               get_do_cse() const;

    const std::string&                                 get_name() const;
    const std::string&                                 get_author() const;
    const std::string&                                 get_model() const;
    const std::string&                                 get_tag() const;

    unsigned int                                       get_number_fields() const;
    unsigned int                                       get_number_parameters() const;
    enum indexorder                                    get_index_order() const;

    const GiNaC::symbol&                               get_Mp_symbol() const;
    const GiNaC::ex                                    get_potential() const;

    const std::vector<GiNaC::symbol>                   get_field_symbols() const;
    const std::vector<GiNaC::symbol>                   get_deriv_symbols() const;
    const std::vector<GiNaC::symbol>                   get_parameter_symbols() const;

    const std::vector<std::string>                     get_field_list() const;
    const std::vector<std::string>                     get_latex_list() const;
    const std::vector<std::string>                     get_param_list() const;
    const std::vector<std::string>                     get_platx_list() const;

    const struct stepper&                              get_background_stepper() const;
    const struct stepper&                              get_perturbations_stepper() const;

    finder&                                            get_finder() const { return(this->path); }
    output_stack&                                      get_stack() const { return(*this->stack); }
    translator&                                        get_translator() const { return(*this->outstream); }

		symbol_factory&                                    get_symbol_factory();

  protected:

    std::string                                        mangle_output_name(std::string input, std::string tag);
    std::string                                        get_template_suffix(std::string input);

  private:

    std::unique_ptr<y::lexstream_type>                 stream;
    std::unique_ptr<y::y_lexer>                        lexer;
    std::unique_ptr<y::y_driver>                       driver;
    std::unique_ptr<y::y_parser>                       parser;

		// GiNaC symbol factory
		symbol_factory                                     sym_factory;

    std::string                                        name;                    // name of input script
		bool                                               parse_failed;

    finder&                                            path;
    argument_cache&                                    cache;
    local_environment&                                 env;

    std::unique_ptr<output_stack>                      stack;
    std::unique_ptr<translator>                        outstream;

    // cached details about the translation unit
    std::string                                        core_output;             // name of core .h file
    std::string                                        implementation_output;   // name of implementation .h file

    std::string                                        core_guard;              // tag for #ifndef guard -- core file
    std::string                                        implementation_guard;    // tag for #ifndef guard -- implementation file
  };


#endif // __input_H_
