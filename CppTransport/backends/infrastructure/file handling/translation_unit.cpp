//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include "core.h"
#include "translation_unit.h"
#include "parse_tree.h"
#include "output_stack.h"

#include "boost/algorithm/string.hpp"
#include "boost/range/algorithm/remove_if.hpp"


// FAIL return code for Bison parser
#ifndef FAIL
#define FAIL (1)
#endif


// lexical analyser data


const std::string keyword_table[] =
                    {
                      "name", "author", "tag", "field", "potential",
                      "parameter", "latex", "core", "implementation", "model",
                      "abserr", "relerr", "stepper", "stepsize",
                      "background", "perturbations",
                      "indexorder", "left", "right",
                      "abs", "step", "sqrt", "sin", "cos", "tan",
                      "asin", "acos", "atan", "atan2", "sinh", "cosh", "tanh",
                      "asinh", "acosh", "atanh", "exp", "log", "pow", "Li2", "Li", "G_func", "S_func", "H_func",
                      "zeta_func", "zetaderiv", "tgamma_func", "lgamma_func", "beta_func", "psi_func", "factorial", "binomial"
                    };

const enum keyword_type keyword_map[] =
                          {
                            k_name, k_author, k_tag, k_field, k_potential,
                            k_parameter, k_latex, k_core, k_implementation, k_model,
                            k_abserr, k_relerr, k_stepper, k_stepsize,
                            k_background, k_perturbations,
                            k_indexorder, k_left, k_right,
                            f_abs, f_step, f_sqrt,
                            f_sin, f_cos, f_tan,
                            f_asin, f_acos, f_atan, f_atan2,
                            f_sinh, f_cosh, f_tanh,
                            f_asinh, f_acosh, f_atanh,
                            f_exp, f_log, f_pow, f_Li2, f_Li, f_G, f_S, f_H,
                            f_zeta, f_zetaderiv, f_tgamma, f_lgamma,
                            f_beta, f_psi, f_factorial, f_binomial
                          };

const std::string character_table[] =
                    {
                      "{", "}", "(", ")",
                      "[", "]", ",", ".", ":", ";",
                      "=", "+", "-@binary", "-@unary", "*", "/", "\\", "~",
                      "&", "^", "@", "...", "->"
                    };

const enum character_type character_map[] =
                            {
                              open_brace, close_brace, open_bracket, close_bracket,
                              open_square, close_square, comma, period, colon, semicolon,
                              equals, plus, binary_minus, unary_minus, star, backslash, foreslash, tilde,
                              ampersand, circumflex, ampersat, ellipsis, rightarrow
                            };

// keep track of which characters can precede a unary minus
// this is an open bracket '(', and the binary operators
// which bind tighter: *, /, ^
// plus anything which isn't part of an expression
const bool character_unary_context[] =
             {
               true, true, true, false,
               true, true, true, true, true, true,
               false, false, false, false, true, true, true, true,
               true, true, true, true, true
             };


static std::string  strip_dot_h(const std::string& pathname);
static std::string  leafname   (const std::string& pathname);


translation_unit::translation_unit(std::string file, finder* p, std::string core_out, std::string implementation_out, bool cse, bool v)
  : name(file), do_cse(cse), verbose(v), path(p)
  {
    // lexicalize this input file
    stream = new lexstream<keyword_type, character_type>(name, path,
                                                         keyword_table, keyword_map, NUMBER_KEYWORDS,
                                                         character_table, character_map, character_unary_context, NUMBER_CHARACTERS);
//    stream->print(std::cerr);

    // now pass to the parser for syntactic analysis
    lexer  = new y::y_lexer(stream);
    driver = new y::y_driver(sym_factory);
    parser = new y::y_parser(lexer, driver);

    if(parser->parse() == FAIL) warn(WARNING_PARSING_FAILED + (std::string)(" '") + name + (std::string)("'"));
    // in.driver->get_script()->print(std::cerr);

    // cache details about this translation unit
    if(core_out != "")
      {
        core_output = core_out;
      }
    else
      {
        core_output = this->mangle_output_name(name, this->get_template_suffix(driver->get_script()->get_core()));
      }
    core_guard = boost::to_upper_copy(leafname(core_output));
    core_guard.erase(boost::remove_if(core_guard, boost::is_any_of(INVALID_GUARD_CHARACTERS)), core_guard.end());

    if(implementation_out != "")
      {
        implementation_output = implementation_out;
      }
    else
      {
        implementation_output = this->mangle_output_name(name, this->get_template_suffix(driver->get_script()->get_implementation()));
      }
    implementation_guard = boost::to_upper_copy(leafname(implementation_output));
    implementation_guard.erase(boost::remove_if(implementation_guard, boost::is_any_of(INVALID_GUARD_CHARACTERS)), implementation_guard.end());

    stack = new output_stack;
    outstream = new translator(this);
  }


translation_unit::~translation_unit()
  {
    // deallocate storage

    assert(this->stream != nullptr);
    assert(this->parser != nullptr);
    assert(this->lexer != nullptr);
    assert(this->driver != nullptr);

    delete this->stream;
    delete this->parser;
    delete this->lexer;
    delete this->driver;

    assert(this->stack != nullptr);
    assert(this->outstream != nullptr);

    delete this->stack;
    delete this->outstream;
  }


// ******************************************************************


unsigned int translation_unit::do_replacement()
  {
    unsigned int rval = 0;

    const script* s = this->driver->get_script();

    std::string in = s->get_core();
    if(in != "")
      {
        rval += this->outstream->translate(in, this->core_output, process_core);
      }
    else
      {
        error(ERROR_NO_CORE_TEMPLATE);
        exit(EXIT_FAILURE);
      }

    in = s->get_implementation();
    if(in != "")
      {
        rval += this->outstream->translate(in, this->implementation_output, process_implementation);
      }
    else
      {
        error(ERROR_NO_IMPLEMENTATION_TEMPLATE);
        exit(EXIT_FAILURE);
      }

    return(rval);
  }


const std::string& translation_unit::get_model_input() const
  {
    return(this->name);
  }


const std::string& translation_unit::get_core_output() const
  {
    return(this->core_output);
  }


const std::string& translation_unit::get_implementation_output() const
  {
    return(this->implementation_output);
  }


const std::string& translation_unit::get_core_guard() const
  {
    return(this->core_guard);
  }


const std::string& translation_unit::get_implementation_guard() const
  {
    return(this->implementation_guard);
  }


bool translation_unit::get_do_cse() const
  {
    return(this->do_cse);
  }


const std::string& translation_unit::get_name() const
  {
    return(this->driver->get_script()->get_name());
  }


const std::string& translation_unit::get_author() const
  {
    return(this->driver->get_script()->get_author());
  }


const std::string& translation_unit::get_model() const
  {
    return(this->driver->get_script()->get_model());
  }


const std::string& translation_unit::get_tag() const
  {
    return(this->driver->get_script()->get_tag());
  }


unsigned int translation_unit::get_number_fields() const
  {
    return(this->driver->get_script()->get_number_fields());
  }


unsigned int translation_unit::get_number_parameters() const
  {
    return(this->driver->get_script()->get_number_params());
  }


enum indexorder translation_unit::get_index_order() const
  {
    return(this->driver->get_script()->get_indexorder());
  }


const GiNaC::symbol& translation_unit::get_Mp_symbol() const
  {
    return(this->driver->get_script()->get_Mp_symbol());
  }


const GiNaC::ex translation_unit::get_potential() const
  {
    return(this->driver->get_script()->get_potential());
  }


const std::vector<GiNaC::symbol> translation_unit::get_field_symbols() const
  {
    return(this->driver->get_script()->get_field_symbols());
  }


const std::vector<GiNaC::symbol> translation_unit::get_deriv_symbols() const
  {
    return(this->driver->get_script()->get_deriv_symbols());
  }


const std::vector<GiNaC::symbol> translation_unit::get_parameter_symbols() const
  {
    return(this->driver->get_script()->get_param_symbols());
  }


const std::vector<std::string> translation_unit::get_field_list() const
  {
    return(this->driver->get_script()->get_field_list());
  }


const std::vector<std::string> translation_unit::get_latex_list() const
  {
    return(this->driver->get_script()->get_latex_list());
  }


const std::vector<std::string> translation_unit::get_param_list() const
  {
    return(this->driver->get_script()->get_param_list());
  }


const std::vector<std::string> translation_unit::get_platx_list() const
  {
    return(this->driver->get_script()->get_platx_list());
  }


const struct stepper& translation_unit::get_background_stepper() const
  {
    return(this->driver->get_script()->get_background_stepper());
  }


const struct stepper& translation_unit::get_perturbations_stepper() const
  {
    return(this->driver->get_script()->get_perturbations_stepper());
  }


finder* translation_unit::get_finder()
  {
    return(this->path);
  }


output_stack* translation_unit::get_stack()
  {
    return(this->stack);
  }


translator* translation_unit::get_translator()
  {
    return(this->outstream);
  }


symbol_factory& translation_unit::get_symbol_factory()
	{
		return(this->sym_factory);
	}


// ******************************************************************


std::string translation_unit::mangle_output_name(std::string input, std::string tag)
  {
    size_t      pos = 0;
    std::string output;

    if((pos = input.find(MODEL_SCRIPT_SUFFIX)) != std::string::npos)
      {
        if(pos == input.length() - MODEL_SCRIPT_SUFFIX_LENGTH) output = input.erase(input.length() - MODEL_SCRIPT_SUFFIX_LENGTH, std::string::npos) + "_" + tag;
        else                                                   output = input + "_" + tag;
      }

    return(output + ".h");
  }


std::string translation_unit::get_template_suffix(std::string input)
  {
    size_t      pos = 0;
    std::string output;

    if((pos = input.find(TEMPLATE_TAG_SUFFIX)) != std::string::npos) output = input.erase(0, pos+1);
    else                                                             output = input;

    return(output);
  }


void translation_unit::print_advisory(const std::string& msg)
	{
		if(this->verbose)
			{
		    std::cout << CPPTRANSPORT_NAME << ": " << msg << std::endl;
			}
	}


static std::string strip_dot_h(const std::string& pathname)
  {
    std::string rval;

    if(pathname.substr(pathname.length() - 3) == ".h")
      {
        rval = pathname.substr(0, pathname.length() - 3);
      }
    else if(pathname.substr(pathname.length() - 5) == ".hpp")
      {
        rval = pathname.substr(0, pathname.length() - 5);
      }
    else
      {
        rval = pathname;
      }

    return(rval);
  }


static std::string leafname(const std::string& pathname)
  {
    std::string rval = pathname;
    size_t      pos;

    while((pos = rval.find("/")) != std::string::npos)
      {
        rval.erase(0, pos+1);
      }

    while((pos = rval.find(".")) != std::string::npos)
      {
        rval.erase(pos, std::string::npos);
      }

    return(rval);
  }
