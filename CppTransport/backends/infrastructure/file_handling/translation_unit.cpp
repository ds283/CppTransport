//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include "core.h"
#include "translation_unit.h"
#include "script.h"
#include "output_stack.h"

#include "boost/algorithm/string.hpp"
#include "boost/range/algorithm/remove_if.hpp"


// FAIL return code for Bison parser
#ifndef FAIL
#define FAIL (1)
#endif


// lexical analyser data


const std::vector<std::string> keyword_table =
  {
    "name", "author", "tag", "field", "potential", "subexpr", "value",
    "parameter", "latex", "core", "implementation", "model",
    "abserr", "relerr", "stepper", "stepsize",
    "background", "perturbations",
    "indexorder", "left", "right",
    "abs", "step", "sqrt", "sin", "cos", "tan",
    "asin", "acos", "atan", "atan2", "sinh", "cosh", "tanh",
    "asinh", "acosh", "atanh", "exp", "log", "pow", "Li2", "Li", "G_func", "S_func",
    "H_func",
    "zeta_func", "zetaderiv", "tgamma_func", "lgamma_func", "beta_func", "psi_func",
    "factorial", "binomial"
                                 };

const std::vector<enum keyword_type> keyword_map =
  {
    keyword_type::name, keyword_type::author, keyword_type::tag,
    keyword_type::field, keyword_type::potential, keyword_type::subexpr,
    keyword_type::value,
    keyword_type::parameter, keyword_type::latex,
    keyword_type::core, keyword_type::impl, keyword_type::model,
    keyword_type::abserr, keyword_type::relerr, keyword_type::k_stepper,
    keyword_type::stepsize,
    keyword_type::background, keyword_type::perturbations,
    keyword_type::indexorder, keyword_type::left, keyword_type::right,
    keyword_type::f_abs, keyword_type::f_step, keyword_type::f_sqrt,
    keyword_type::f_sin, keyword_type::f_cos, keyword_type::f_tan,
    keyword_type::f_asin, keyword_type::f_acos, keyword_type::f_atan,
    keyword_type::f_atan2,
    keyword_type::f_sinh, keyword_type::f_cosh, keyword_type::f_tanh,
    keyword_type::f_asinh, keyword_type::f_acosh, keyword_type::f_atanh,
    keyword_type::f_exp, keyword_type::f_log, keyword_type::f_pow,
    keyword_type::f_Li2,
    keyword_type::f_Li, keyword_type::f_G, keyword_type::f_S, keyword_type::f_H,
    keyword_type::f_zeta, keyword_type::f_zetaderiv, keyword_type::f_tgamma,
    keyword_type::f_lgamma,
    keyword_type::f_beta, keyword_type::f_psi, keyword_type::f_factorial,
    keyword_type::f_binomial
  };

const std::vector<std::string> character_table =
  {
    "{", "}", "(", ")",
    "[", "]", ",", ".", ":", ";",
    "=", "+", "-@binary", "-@unary", "*", "/", "\\", "~",
    "&", "^", "@", "...", "->"
                                 };

const std::vector<enum character_type> character_map =
  {
    character_type::open_brace, character_type::close_brace,
    character_type::open_bracket, character_type::close_bracket,
    character_type::open_square, character_type::close_square,
    character_type::comma,
    character_type::period, character_type::colon, character_type::semicolon,
    character_type::equals, character_type::plus,
    character_type::binary_minus, character_type::unary_minus,
    character_type::star, character_type::backslash, character_type::foreslash,
    character_type::tilde, character_type::ampersand, character_type::circumflex,
    character_type::ampersat, character_type::ellipsis,
    character_type::rightarrow
                                         };

// keep track of which characters can precede a unary minus
// this is an open bracket '(', and the binary operators
// which bind tighter: *, /, ^
// plus anything which isn't part of an expression
const std::vector<bool> character_unary_context =
  {
    true, true, true, false,
    true, true, true, true, true, true,
    false, false, false, false, true, true, true, true,
    true, true, true, true, true
  };


static std::string  strip_dot_h(const std::string& pathname);
static std::string  leafname   (const std::string& pathname);


translation_unit::translation_unit(std::string file, finder& p, argument_cache& c, local_environment& e)
  : name(file),
    path(p),
    cache(c),
    env(e),
    parse_failed(false)
  {
    // lexicalize this input file
    stream = std::make_unique<y::lexstream_type>(name, path, keyword_table, keyword_map, character_table, character_map, character_unary_context);

    // dump lexeme stream to output -- for debugging
    // stream->print(std::cerr);

    // now pass to the parser for syntactic analysis
    lexer  = std::make_unique<y::y_lexer>(*stream);
    driver = std::make_unique<y::y_driver>(sym_factory);
    parser = std::make_unique<y::y_parser>(*lexer, *driver);

    outstream = std::make_unique<translator>(this);

    if(parser->parse() == FAIL || driver->failed())
	    {
        ::warn(WARNING_PARSING_FAILED + (std::string)(" '") + name + (std::string)("'"));
		    parse_failed = true;
	    }

    // dump results of syntactic analysis -- for debugging
    // in.driver->get_script()->print(std::cerr);

    // cache details about this translation unit
    if(cache.core_out().length() > 0 )
      {
        core_output = cache.core_out();
      }
    else
      {
        core_output = this->mangle_output_name(name, this->get_template_suffix(driver->get_script().get_core()));
      }
    core_guard = boost::to_upper_copy(leafname(core_output));
    core_guard.erase(boost::remove_if(core_guard, boost::is_any_of(INVALID_GUARD_CHARACTERS)), core_guard.end());

    if(cache.implementation_out().length() > 0)
      {
        implementation_output = cache.implementation_out();
      }
    else
      {
        implementation_output = this->mangle_output_name(name, this->get_template_suffix(driver->get_script().get_implementation()));
      }
    implementation_guard = boost::to_upper_copy(leafname(implementation_output));
    implementation_guard.erase(boost::remove_if(implementation_guard, boost::is_any_of(INVALID_GUARD_CHARACTERS)), implementation_guard.end());
  }


// ******************************************************************


unsigned int translation_unit::apply()
  {
    unsigned int rval = 0;

		if(this->parse_failed) return rval;

    const script& s = this->driver->get_script();

    std::string in = s.get_core();
    if(in != "")
      {
        rval += this->outstream->translate(in, this->core_output, process_core);
      }
    else
      {
        this->error(ERROR_NO_CORE_TEMPLATE);
        exit(EXIT_FAILURE);
      }

    in = s.get_implementation();
    if(in != "")
      {
        rval += this->outstream->translate(in, this->implementation_output, process_implementation);
      }
    else
      {
        this->error(ERROR_NO_IMPLEMENTATION_TEMPLATE);
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
    return(this->cache.do_cse());
  }


const std::string& translation_unit::get_name() const
  {
    return(this->driver->get_script().get_name());
  }


const std::string& translation_unit::get_author() const
  {
    return(this->driver->get_script().get_author());
  }


const std::string& translation_unit::get_model() const
  {
    return(this->driver->get_script().get_model());
  }


const std::string& translation_unit::get_tag() const
  {
    return(this->driver->get_script().get_tag());
  }


unsigned int translation_unit::get_number_fields() const
  {
    return(this->driver->get_script().get_number_fields());
  }


unsigned int translation_unit::get_number_parameters() const
  {
    return(this->driver->get_script().get_number_params());
  }


enum indexorder translation_unit::get_index_order() const
  {
    return(this->driver->get_script().get_indexorder());
  }


const GiNaC::symbol& translation_unit::get_Mp_symbol() const
  {
    return(this->driver->get_script().get_Mp_symbol());
  }


const GiNaC::ex translation_unit::get_potential() const
  {
    return(this->driver->get_script().get_potential());
  }


const std::vector<GiNaC::symbol> translation_unit::get_field_symbols() const
  {
    return(this->driver->get_script().get_field_symbols());
  }


const std::vector<GiNaC::symbol> translation_unit::get_deriv_symbols() const
  {
    return(this->driver->get_script().get_deriv_symbols());
  }


const std::vector<GiNaC::symbol> translation_unit::get_parameter_symbols() const
  {
    return(this->driver->get_script().get_param_symbols());
  }


const std::vector<std::string> translation_unit::get_field_list() const
  {
    return(this->driver->get_script().get_field_list());
  }


const std::vector<std::string> translation_unit::get_latex_list() const
  {
    return(this->driver->get_script().get_latex_list());
  }


const std::vector<std::string> translation_unit::get_param_list() const
  {
    return(this->driver->get_script().get_param_list());
  }


const std::vector<std::string> translation_unit::get_platx_list() const
  {
    return(this->driver->get_script().get_platx_list());
  }


const struct stepper& translation_unit::get_background_stepper() const
  {
    return(this->driver->get_script().get_background_stepper());
  }


const struct stepper& translation_unit::get_perturbations_stepper() const
  {
    return(this->driver->get_script().get_perturbations_stepper());
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
		if(this->cache.verbose())
			{
		    std::cout << CPPTRANSPORT_NAME << ": " << msg << '\n';
			}
	}


void translation_unit::error(const std::string& msg)
	{
		::error(msg, this->stack);
	}


void translation_unit::warn(const std::string& msg)
	{
		::warn(msg, this->stack);
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
