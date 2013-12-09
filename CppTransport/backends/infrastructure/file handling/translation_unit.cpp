//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "core.h"
#include "msg_en.h"

#include "translation_unit.h"

#include "boost/algorithm/string.hpp"
#include "boost/range/algorithm/remove_if.hpp"
#include "boost/lexical_cast.hpp"

#include "buffer.h"
#include "replacement_rule_package.h"
#include "package_group_factory.h"
#include "macro.h"

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
                      "asinh", "acosh", "atanh", "exp", "log", "Li2", "Li", "G_func", "S_func", "H_func",
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
                            f_exp, f_log, f_Li2, f_Li, f_G, f_S, f_H,
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


translation_unit::translation_unit(std::string file, finder* p, std::string core_out, std::string implementation_out, bool cse)
  : name(file), do_cse(cse), path(p)
  {
    // lexicalize this input file
    stream = new lexstream<keyword_type, character_type>(name, path,
                                                         keyword_table, keyword_map, NUMBER_KEYWORDS,
                                                         character_table, character_map, character_unary_context, NUMBER_CHARACTERS);
    // in.stream->print(std::cerr);

    // now pass to the parser for syntactic analysis
    lexer  = new y::y_lexer(stream);
    driver = new y::y_driver();
    parser = new y::y_parser(lexer, driver);

    if(parser->parse() == FAIL) warn(WARNING_PARSING_FAILED + (std::string)(" '") + name + (std::string)("'"));
    // in.driver->get_script()->print(std::cerr);

    if(core_out != "")
      {
        core_output = core_out;
      }
    else
      {
        core_output = this->mangle_output_name(name, this->get_template_suffix(driver->get_script()->get_core()));
      }

    if(implementation_out != "")
      {
        implementation_output = implementation_out;
      }
    else
      {
        implementation_output = this->mangle_output_name(name, this->get_template_suffix(driver->get_script()->get_implementation()));
      }
  }


translation_unit::~translation_unit()
  {
    // deallocate storage
    delete this->stream;
    delete this->parser;
    delete this->lexer;
    delete this->driver;
  }


// ******************************************************************


unsigned int translation_unit::do_replacement()
  {
    unsigned int rval = 0;

    script* s = this->driver->get_script();

    std::string in = s->get_core();
    if(in != "")
      {
        rval += this->apply(in, this->core_output);
      }
    else
      {
        error(ERROR_NO_CORE_TEMPLATE);
        exit(EXIT_FAILURE);
      }

    in = s->get_implementation();
    if(in != "")
      {
        rval += this->apply(in, this->implementation_output);
      }
    else
      {
        error(ERROR_NO_IMPLEMENTATION_TEMPLATE);
        exit(EXIT_FAILURE);
      }

    return(rval);
  }


unsigned int translation_unit::apply(std::string in, std::string out)
  {
    unsigned int rval = 0;
    std::string  template_in;

    if(this->path->fqpn(in + ".h", template_in) == true)
      {
        macro_packages::replacement_data data;

        data.parse_tree         = this->driver->get_script();
        data.script_in          = this->name;

        data.template_in        = template_in;
        data.file_out           = out;

        data.core_out           = this->core_output;
        data.implementation_out = this->implementation_output;

        data.guard              = boost::to_upper_copy(leafname(out));
        data.guard.erase(boost::remove_if(data.guard, boost::is_any_of(INVALID_GUARD_CHARACTERS)));

        // this information is available via parse_tree->get_X() anyway,
        // but is helpful to have it nearer the top of the data structure
        data.num_fields         = data.parse_tree->get_number_fields();
        data.num_params         = data.parse_tree->get_number_params();
        data.index_order        = data.parse_tree->get_indexorder();

        // set up a 'fake' inclusion list, so that errors can include the line number in the template
        struct inclusion incl;
        incl.line = 0;
        incl.name = template_in;
        data.path.push_back(incl);
        data.current_line = 1;

        rval += this->process(data);
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_MISSING_TEMPLATE << " '" << in << ".h'";
        error(msg.str());
      }

    return(rval);
  }


unsigned int translation_unit::process(macro_packages::replacement_data& data)
  {
    unsigned int  rval = 0;
    std::ifstream in;

    in.open(data.template_in.c_str());
    if(in.is_open() && !in.fail())
      {
        std::string line;
        std::getline(in, line);

        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_any_of(" ,:;."));
        if(tokens.size() < 3)
          {
            std::ostringstream msg;
            msg << ERROR_TOKENIZE_TEMPLATE_HEADER << " '" << data.template_in << "'";
            error(msg.str());
          }
        else
          {
            double minver = boost::lexical_cast<double>(tokens[2]);
            if(minver <= CPPTRANSPORT_NUMERIC_VERSION)
              {
                // generate an output buffer and an appropriate backend
                buffer*        buf     = new buffer;
                package_group* backend = package_group_factory(tokens[1], data, buf, this->do_cse);

                if(backend != nullptr)
                  {
                    macro_package* macro = new macro_package(data.num_fields, data.num_params, data.index_order,
                                                             BACKEND_MACRO_PREFIX, BACKEND_LINE_SPLIT, backend);

                    // inform the selected backend of the final macro package
                    // TODO consider replacing this
                    backend->set_macros(macro);

                    while(in.eof() == false && in.fail() == false)
                      {
                        std::getline(in, line);
                        rval += macro->apply(line);

                        buf->write_to_end(line);
                        data.current_line++;
                      }

                    // no need to flush temporaries before writing out the buffer, because that
                    // happens automatically via the closure handlers
                    buf->emit(data.file_out);

                    delete buf;
                  }
              }
            else
              {
                std::ostringstream msg;
                msg << ERROR_TEMPLATE_TOO_RECENT_A << " '" << data.template_in << "' " << ERROR_TEMPLATE_TOO_RECENT_B << minver << ")";
                error(msg.str());
              }
          }
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_READING_TEMPLATE << " '" << data.template_in << "'";
        error(msg.str());
      }

    in.close();

    return(rval);
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
