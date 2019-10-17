//
// Created by David Seery on 05/12/2013.
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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#include "core.h"
#include "defaults.h"
#include "translation_unit.h"
#include "model_descriptor.h"
#include "output_stack.h"

#include "boost/algorithm/string.hpp"
#include "boost/range/algorithm/remove_if.hpp"


// FAIL return code for Bison parser
#ifndef FAIL
#define FAIL (1)
#endif


// lexical analyser data


const y::lexeme_type::keyword_map keywords =
  {
    { "metadata",           keyword_type::metadata },
    { "require_version",    keyword_type::require_version },
    { "lagrangian",         keyword_type::lagrangian },
    { "canonical",          keyword_type::canonical },
    { "nontrivial_metric",  keyword_type::nontrivial_metric },
    { "name",               keyword_type::name },
    { "author",             keyword_type::author },
    { "citeguide",          keyword_type::citeguide },
    { "description",        keyword_type::description },
    { "license",            keyword_type::license },
    { "revision",           keyword_type::revision },
    { "references",         keyword_type::references },
    { "urls",               keyword_type::urls },
    { "email",              keyword_type::email },
    { "institute",          keyword_type::institute },
    { "field",              keyword_type::field },
    { "potential",          keyword_type::potential },
    { "metric",             keyword_type::metric },
    { "subexpr",            keyword_type::subexpr },
    { "value",              keyword_type::value },
    { "prior",              keyword_type::prior },
    { "deriv_value",        keyword_type::deriv_value },
    { "deriv_prior",        keyword_type::deriv_prior },
    { "parameter",          keyword_type::parameter },
    { "latex",              keyword_type::latex },
    { "templates",          keyword_type::templates },
    { "settings",           keyword_type::settings },
    { "core",               keyword_type::core },
    { "implementation",     keyword_type::impl },
    { "sampling",           keyword_type::sampling },
    { "sampling_template",  keyword_type::sampling_template },
    { "generate_sampling",  keyword_type::generate_sampling },
    { "sampler",            keyword_type::sampler },
    { "model",              keyword_type::model },
    { "abserr",             keyword_type::abserr },
    { "relerr",             keyword_type::relerr },
    { "stepper",            keyword_type::stepper },
    { "stepsize",           keyword_type::stepsize },
    { "background",         keyword_type::background },
    { "perturbations",      keyword_type::perturbations },
    { "indexorder",         keyword_type::indexorder },
    { "left",               keyword_type::left },
    { "right",              keyword_type::right },
    { "abs",                keyword_type::abs },
    { "step",               keyword_type::step },
    { "sqrt",               keyword_type::sqrt },
    { "sin",                keyword_type::sin },
    { "cos",                keyword_type::cos },
    { "tan",                keyword_type::tan },
    { "sec",                keyword_type::sec },
    { "csc",                keyword_type::cosec },
    { "cosec",              keyword_type::cosec },
    { "cot",                keyword_type::cot },
    { "asin",               keyword_type::asin },
    { "acos",               keyword_type::acos },
    { "atan",               keyword_type::atan },
    { "atan2",              keyword_type::atan2 },
    { "sinh",               keyword_type::sinh },
    { "cosh",               keyword_type::cosh },
    { "tanh",               keyword_type::tanh },
    { "sech",               keyword_type::sech },
    { "csch",               keyword_type::cosech },
    { "cosech",             keyword_type::cosech },
    { "coth",               keyword_type::coth },
    { "asinh",              keyword_type::asinh },
    { "acosh",              keyword_type::acosh },
    { "atanh",              keyword_type::atanh },
    { "exp",                keyword_type::exp },
    { "log",                keyword_type::log },
    { "pow",                keyword_type::pow },
    { "Li2",                keyword_type::Li2 },
    { "Li",                 keyword_type::Li },
    { "G_func",             keyword_type::G },
    { "S_func",             keyword_type::S },
    { "H_func",             keyword_type::H },
    { "zeta_func",          keyword_type::zeta },
    { "zetaderiv",          keyword_type::zetaderiv },
    { "tgamma_func",        keyword_type::tgamma },
    { "lgamma_func",        keyword_type::lgamma },
    { "beta_func",          keyword_type::beta },
    { "psi_func",           keyword_type::psi },
    { "factorial",          keyword_type::factorial },
    { "binomial",           keyword_type::binomial }
  };

// final column is *true* if the corresponding symbol can *precede*
// a unary minus
// This is most characters except the close bracket ')',
// which signals the end of a subexpression and must be followed by a binary minus
// otherwise, arithmetic operators such as +, -, *, /, ^ can all be followed by a unary minus
// A unary minus can itself be followed by a unary minus, too
const y::lexeme_type::character_map symbols =
  {
    { "{", { character_type::open_brace, true } },
    { "}", { character_type::close_brace, true } },
    { "(", { character_type::open_bracket, true } },
    { ")", { character_type::close_bracket, false } },
    { "[", { character_type::open_square, true } },
    { "]", { character_type::close_square, true } },
    { ",", { character_type::comma, true } },
    { ".", { character_type::period, true } },
    { ":", { character_type::colon, true } },
    { ";", { character_type::semicolon, true } },
    { "=", { character_type::equals, true } },
    { "+", { character_type::plus, true } },
    { "-@binary", { character_type::binary_minus, true } },
    { "-@unary", { character_type::unary_minus, true } },
    { "*", { character_type::star, true } },
    { "\\", { character_type::backslash, true } },
    { "/", { character_type::foreslash, true } },
    { "~", { character_type::tilde, true } },
    { "&", { character_type::ampersand, true } },
    { "^", { character_type::circumflex, true } },
    { "@", { character_type::ampersat, true } },
    { "...", { character_type::ellipsis, true } },
    { "->", { character_type::rightarrow, true } }
    };


static std::string strip_dot_h(const std::string& pathname);
static std::string leafname   (const std::string& pathname);


translation_unit::translation_unit(boost::filesystem::path file, finder& p, argument_cache& c, local_environment& e,
                                   version_policy& vp)
  : name(file),
    path(p),
    cache(c),
    env(e),
    policy(vp),
    parse_failed(false),
    errors(0),
    file_errors(0),
    warnings(0),
    lexstream_payload(file,
                      std::bind(&translation_unit::context_error, this, std::placeholders::_1, std::placeholders::_2),
                      std::bind(&translation_unit::context_warn, this, std::placeholders::_1, std::placeholders::_2),
                      path, cache),
    instream(lexstream_payload, keywords, symbols),
    lexer(instream),
    model(sym_factory, vp,
          error_context(stack,
                        std::bind(&translation_unit::context_error, this, std::placeholders::_1, std::placeholders::_2),
                        std::bind(&translation_unit::context_warn, this, std::placeholders::_1, std::placeholders::_2))),
    driver(model, sym_factory, c, e),
    parser(lexer, driver),
    translator_payload(file,
                       std::bind(&translation_unit::context_error, this, std::placeholders::_1, std::placeholders::_2),
                       std::bind(&translation_unit::context_warn, this, std::placeholders::_1, std::placeholders::_2),
                       std::bind(&translation_unit::print_advisory, this, std::placeholders::_1),
                       path, stack, sym_factory, model, cache, vp),
    outstream(translator_payload)
  {
    // 'instream' constructor lexicalizes the input file
    
    // the instream object owns the list of lexemes, which persist as long as it exists
    // therefore the lexeme list should be persistent while all transactions involving this unit are active
    
    // dump lexeme stream to output -- for debugging
    // instream.print(std::cerr);
    
    // combination of lexer, driver and parser performs syntactic analysis
    
    if(parser.parse() == FAIL || model.failed())
      {
        std::ostringstream msg;
        msg << WARNING_PARSING_FAILED << " " << name;
        this->warn(msg.str());
        parse_failed = true;
      }
    
    // dump results of syntactic analysis -- for debugging
    // this->model.print(std::cerr);
    
    // ask model descriptor to validate itself
    auto validation_errors = model.validate();
    if(validation_errors.empty()) return;
    
    this->warn(WARNING_VALIDATION_ERRORS);
    for(const auto& t : validation_errors)
      {
        if(t->first)
          {
            this->error(std::string(FATAL_TOKEN) + t->second);
            this->parse_failed = true;
          }
        else
          {
            this->warn(t->second);
          }
      }
  }
  

void translation_unit::populate_output_filenames()
  {
    boost::filesystem::path core_output;
    std::string             core_guard;
    boost::filesystem::path implementation_output;
    std::string             implementation_guard;

    if(this->cache.core_out().length() > 0 ) core_output = this->cache.core_out();
    else
      {
        boost::optional< contexted_value<std::string>& > core = this->model.templates.get_core_template();
        if(core) core_output = this->mangle_output_name(name, this->get_template_suffix(*core));
      }
    core_guard = boost::to_upper_copy(leafname(core_output.string()));
    core_guard.erase(boost::remove_if(core_guard, boost::is_any_of(INVALID_GUARD_CHARACTERS)), core_guard.end());

    if(this->cache.implementation_out().length() > 0) implementation_output = this->cache.implementation_out();
    else
      {
        boost::optional< contexted_value<std::string>& > impl = this->model.templates.get_implementation_template();
        if(impl) implementation_output = this->mangle_output_name(name, this->get_template_suffix(*impl));
      }
    implementation_guard = boost::to_upper_copy(leafname(implementation_output.string()));
    implementation_guard.erase(boost::remove_if(implementation_guard, boost::is_any_of(INVALID_GUARD_CHARACTERS)), implementation_guard.end());

    // assign these values to the translator data payload
    this->translator_payload.set_core_implementation(core_output, core_guard, implementation_output, implementation_guard);

    // If we are doing sampling, we need to assign the sampling filenames.
    if(*this->translator_payload.templates.get_sampling())
    {
      // Initialise filepath variables for for sampling that we will write to
      boost::filesystem::path sampling_output;
      boost::filesystem::path sampling_values_output;
      boost::filesystem::path sampling_priors_output;
      boost::filesystem::path sampling_mcmc_output;
      boost::filesystem::path sampling_cmake_output;
      boost::filesystem::path sampling_getdist_python_output;
      boost::filesystem::path sampling_getdist_latex_output;
      std::string             sampling_guard;

      // This sets up our filepath for the sampling output file
      sampling_output        = this->mangle_output_name(name, "sampling");
      sampling_values_output = this->mangle_output_name(name, "values");
      sampling_priors_output = this->mangle_output_name(name, "priors");
      sampling_mcmc_output   = this->mangle_output_name(name, "mcmc");
      sampling_cmake_output  = this->mangle_output_name(name, "cmake");
      sampling_getdist_python_output  = this->mangle_output_name(name, "getdist_python");
      sampling_getdist_latex_output   = this->mangle_output_name(name, "getdist_latex");

      sampling_guard = boost::to_upper_copy(leafname(sampling_output.string()));
      sampling_guard.erase(boost::remove_if(sampling_guard, boost::is_any_of(INVALID_GUARD_CHARACTERS)), sampling_guard.end());

      this->translator_payload.set_sampling_output(sampling_output, sampling_guard, sampling_values_output,
              sampling_priors_output, sampling_mcmc_output, sampling_cmake_output,
              sampling_getdist_python_output, sampling_getdist_latex_output);
    }
  }


// ******************************************************************


unsigned int translation_unit::apply()
  {
    unsigned int rval = 0;

    // don't attempt translation if parsing failed
		if(this->parse_failed || this->errors > 0) return rval;
    
    this->populate_output_filenames();

    const boost::filesystem::path& core_output = this->translator_payload.get_core_filename();
    const boost::filesystem::path& impl_output = this->translator_payload.get_implementation_filename();
    
    boost::optional< contexted_value<std::string>& > core = this->model.templates.get_core_template();
    if(core)
      {
        this->file_errors = 0;
        try
          {
            rval += this->outstream.translate(*core, (*core).get_declaration_point(), core_output, process_type::process_core);
          }
        catch(exit_parse& xe)
          {
            std::ostringstream msg;
            msg << NOTIFY_PARSE_TERMINATED << ": " << xe.what();
            this->warn(msg.str());
          }
      }
    else
      {
        this->error(ERROR_NO_CORE_TEMPLATE);
      }

    boost::optional< contexted_value<std::string>& > impl = this->model.templates.get_implementation_template();
    if(impl)
      {
        this->file_errors = 0;
        try
          {
            rval += this->outstream.translate(*impl, (*impl).get_declaration_point(), impl_output, process_type::process_implementation);
          }
        catch(exit_parse& xe)
          {
            std::ostringstream msg;
            msg << NOTIFY_PARSE_TERMINATED << ": " << xe.what();
            this->warn(msg.str());
          }
      }
    else
      {
        this->error(ERROR_NO_IMPLEMENTATION_TEMPLATE);
      }

    // Sampling template file parser & translator
    if (*this->translator_payload.templates.get_sampling())
    {
      const boost::filesystem::path& sampling_output = this->translator_payload.get_sampling_filename();
      boost::optional< contexted_value<std::string>& > sampling = this->model.templates.get_sampling_template();
      if(sampling)
      {
        this->file_errors = 0;
        try
        {
          rval += this->outstream.translate(*sampling, (*sampling).get_declaration_point(), sampling_output, process_type::process_sampling);
        }
        catch(exit_parse& xe)
        {
          std::ostringstream msg;
          msg << NOTIFY_PARSE_TERMINATED << ": " << xe.what();
          this->warn(msg.str());
        }
      }
      else
      {
        this->error(ERROR_NO_CORE_TEMPLATE);
      }

      const boost::filesystem::path& sampling_priors_output = this->translator_payload.get_sampling_priors_filename();
      std::string sampling_priors = "cosmosis_priors_template";
      this->file_errors = 0;
      try
      {
        rval += this->outstream.translate(sampling_priors, (*sampling).get_declaration_point(), sampling_priors_output, process_type::process_sampling_ini);
      }
      catch(exit_parse& xe)
      {
        std::ostringstream msg;
        msg << NOTIFY_PARSE_TERMINATED << ": " << xe.what();
        this->warn(msg.str());
      }

      const boost::filesystem::path& sampling_values_output = this->translator_payload.get_sampling_values_filename();
      std::string sampling_values = "cosmosis_values_template";
      this->file_errors = 0;
      try
      {
        rval += this->outstream.translate(sampling_values, (*sampling).get_declaration_point(), sampling_values_output, process_type::process_sampling_ini);
      }
      catch(exit_parse& xe)
      {
        std::ostringstream msg;
        msg << NOTIFY_PARSE_TERMINATED << ": " << xe.what();
        this->warn(msg.str());
      }

      const boost::filesystem::path& sampling_mcmc_output = this->translator_payload.get_sampling_mcmc_filename();
      std::string sampling_mcmc = "cosmosis_mcmc_template";
      this->file_errors = 0;
      try
      {
        rval += this->outstream.translate(sampling_mcmc, (*sampling).get_declaration_point(), sampling_mcmc_output, process_type::process_sampling_ini);
      }
      catch(exit_parse& xe)
      {
        std::ostringstream msg;
        msg << NOTIFY_PARSE_TERMINATED << ": " << xe.what();
        this->warn(msg.str());
      }

      if (this->cache.do_cmake()){
        const boost::filesystem::path& sampling_cmake_output = this->translator_payload.get_sampling_cmake_filename();
        std::string sampling_cmake = "cpptsample_CMakeLists_template";
        this->file_errors = 0;
        try
        {
          rval += this->outstream.translate(sampling_cmake, (*sampling).get_declaration_point(), sampling_cmake_output, process_type::process_sampling_txt);
        }
        catch(exit_parse& xe)
        {
          std::ostringstream msg;
          msg << NOTIFY_PARSE_TERMINATED << ": " << xe.what();
          this->warn(msg.str());
        }
      }

      // Translate the GetDist Python file
      const boost::filesystem::path& sampling_getdist_python_output = this->translator_payload.get_sampling_getdist_python_filename();
      std::string sampling_getdist_python = "cosmosis_getdist_python_template";
      this->file_errors = 0;
      try
      {
        rval += this->outstream.translate(sampling_getdist_python, (*sampling).get_declaration_point(), sampling_getdist_python_output, process_type::process_sampling_getdist_python);
      }
      catch(exit_parse& xe)
      {
        std::ostringstream msg;
        msg << NOTIFY_PARSE_TERMINATED << ": " << xe.what();
        this->warn(msg.str());
      }

      // Translate the GetDist LaTeX file
      const boost::filesystem::path& sampling_getdist_latex_output = this->translator_payload.get_sampling_getdist_latex_filename();
      std::string sampling_getdist_latex = "cosmosis_getdist_latex_template";
      this->file_errors = 0;
      try
      {
        rval += this->outstream.translate(sampling_getdist_latex, (*sampling).get_declaration_point(), sampling_getdist_latex_output, process_type::process_sampling_getdist_latex);
      }
      catch(exit_parse& xe)
      {
        std::ostringstream msg;
        msg << NOTIFY_PARSE_TERMINATED << ": " << xe.what();
        this->warn(msg.str());
      }

    }

    if(this->errors > 0)
      {
        this->parse_failed = true;

        // remove output files
        if(boost::filesystem::exists(core_output)) boost::filesystem::remove(core_output);
        if(boost::filesystem::exists(impl_output)) boost::filesystem::remove(impl_output);
      }

    return(rval);
  }


// ******************************************************************


boost::filesystem::path translation_unit::mangle_output_name(const boost::filesystem::path& input, const std::string& tag)
  {
    std::string output;

    auto Sampling = *this->translator_payload.templates.get_sampling();
    // Determine if we are generating sampling templates as well or not

    if (!*Sampling)
    {
      if (tag == "core" || tag == "mpi")
      {
        boost::filesystem::path h_extension(".h");

        boost::filesystem::path stem = input.stem();
        boost::filesystem::path leaf = stem.leaf().string() + "_" + tag;

        return(leaf.replace_extension(h_extension));
      }
    }
    else
    {
      // This is used to create a ./build directory, as I am planning to run the CppT command in the main directory.
      boost::filesystem::path build_prefix("build");
      const char* build_path = build_prefix.string().c_str();
      boost::filesystem::path build_dir(build_path);
      boost::filesystem::create_directory(build_dir);

      if (tag == "core" || tag == "mpi")
      {
        boost::filesystem::path h_extension(".h");

        boost::filesystem::path stem = input.stem();
        boost::filesystem::path leaf = "./" + build_prefix.string() + "/" + stem.leaf().string() + "_" + tag; // stem.leaf().string() + "_" + tag;

        return(leaf.replace_extension(h_extension));

      }
      else if (tag == "sampling")
      {
        boost::filesystem::path cpp_extension(".cpp");

        boost::filesystem::path stem = input.stem();
        boost::filesystem::path leaf = "./" + build_prefix.string() + "/" + stem.leaf().string() + "_" + tag;

        return(leaf.replace_extension(cpp_extension));
      }
      else if (tag == "priors" || tag == "values" || tag == "mcmc")
      {
        boost::filesystem::path ini_extension(".ini");

        boost::filesystem::path stem = input.stem();
        std::string ModelName = stem.leaf().string();

        boost::filesystem::path mcmc_prefix(ModelName + "_mcmc");
        const char* mcmc_path = mcmc_prefix.string().c_str();
        boost::filesystem::path mcmc_dir(mcmc_path);
        boost::filesystem::create_directory(mcmc_dir);

        boost::filesystem::path leaf = "./" + mcmc_prefix.string() + "/" + stem.leaf().string() + "_" + tag;

        return(leaf.replace_extension(ini_extension));
      }
      else if (tag == "cmake")
      {
        boost::filesystem::path txt_extension(".txt");
        boost::filesystem::path leaf = "CMakeLists" ;

        return(leaf.replace_extension(txt_extension));
      }
      else if (tag == "getdist_python")
      {
        boost::filesystem::path py_extension(".py");

        boost::filesystem::path stem = input.stem();
        std::string ModelName = stem.leaf().string();
        boost::filesystem::path mcmc_prefix(ModelName + "_mcmc");

        boost::filesystem::path leaf = "./" + mcmc_prefix.string() + "/" + stem.leaf().string() + "_GetDist";

        return(leaf.replace_extension(py_extension));
      }
      else if (tag == "getdist_latex")
      {
        boost::filesystem::path getdist_extension(".paramnames");

        boost::filesystem::path stem = input.stem();
        std::string ModelName = stem.leaf().string();
        boost::filesystem::path mcmc_GetDist_prefix(ModelName + "_mcmc/GetDistFiles");
        const char* mcmc_GetDist_path = mcmc_GetDist_prefix.string().c_str();
        boost::filesystem::path mcmc_GetDist_dir(mcmc_GetDist_path);
        boost::filesystem::create_directory(mcmc_GetDist_dir);

        boost::filesystem::path leaf = "./" + mcmc_GetDist_prefix.string() + "/" + stem.leaf().string() + "_MCMCGetDistData";

        return(leaf.replace_extension(getdist_extension));
      }
    }
  }


std::string translation_unit::get_template_suffix(std::string input)
  {
    size_t      pos = 0;
    std::string output;
    
    if((pos = input.find_last_of(TEMPLATE_TAG_SUFFIX)) != std::string::npos) output = input.erase(0, pos + 1);
    else output = input;

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
		::error(msg, this->cache, this->env);
    
    ++this->errors;
    ++this->file_errors;
    if(this->file_errors > DEFAULT_MAX_ERROR_COUNT) throw exit_parse(NOTIFY_TOO_MANY_ERRORS);
	}


void translation_unit::warn(const std::string& msg)
	{
		::warn(msg, this->cache, this->env);
    ++this->warnings;
	}


void translation_unit::context_error(const std::string& msg, const error_context& ctx)
  {
    ::error(msg, this->cache, this->env, ctx);

    ++this->errors;
    ++this->file_errors;
    if(this->file_errors > DEFAULT_MAX_ERROR_COUNT) throw exit_parse(NOTIFY_TOO_MANY_ERRORS);
  }


void translation_unit::context_warn(const std::string& msg, const error_context& ctx)
  {
    ::warn(msg, this->cache, this->env, ctx);
    ++this->warnings;
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
