//
// Created by David Seery on 30/11/2015.
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

#ifndef CPPTRANSPORT_TRANSLATOR_DATA_H
#define CPPTRANSPORT_TRANSLATOR_DATA_H


#include <functional>

#include "finder.h"
#include "output_stack.h"
#include "model_settings.h"
#include "symbol_factory.h"
#include "parser/driver/y_driver.h"
#include "error_context.h"
#include "contexted_value.h"
#include "argument_cache.h"
#include "version_policy.h"


typedef std::function<void(const std::string&)> message_handler;


class translator_data
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    translator_data(const boost::filesystem::path& file, error_context::error_handler e,
                    error_context::warning_handler w, message_handler m, finder& f, output_stack& os, symbol_factory& s,
                    model_descriptor& desc, argument_cache& c, version_policy& vp);

    //! destructor is default
    ~translator_data() = default;


    // GET ERROR_CONTEXT HANDLERS

  public:

    //! make an error_context object
    error_context make_error_context() const { return error_context(this->outstack, this->err, this->wrn); }
    
    //! make an error_context object with full context data
    error_context make_error_context(std::shared_ptr<std::string> l, unsigned int s, unsigned int e) const
      { return error_context(this->outstack, std::move(l), s, e, this->err, this->wrn); }
    
    //! get error handler
    const error_context::error_handler& get_error_handler() const { return this->err; }
    
    //! get warning handler
    const error_context::warning_handler& get_warning_handler() const { return this->wrn; }
    
    
    // SET CORE, IMPLEMENTATION DATA

  public:

    //! set core and implementation paths
    void set_core_implementation(const boost::filesystem::path& co, const std::string& cg,
                                 const boost::filesystem::path& io, const std::string& ig,
                                 const boost::filesystem::path& cono, const std::string& cong);

    void set_sampling_output(const boost::filesystem::path& so, const std::string& sg,
                             const boost::filesystem::path& sv, const boost::filesystem::path& sp,
                             const boost::filesystem::path& sm, const boost::filesystem::path& sc,
                             const boost::filesystem::path& samplinggetdistPy,
                             const boost::filesystem::path& samplinggetdistLatex);
    // in order: sampling output, guard, values, priors, mcmc, cmake, GetDist python, GetDist latex

    // GET CORE, IMPLEMENTATION DATA

  public:

    //! get filename of model descriptor
    const boost::filesystem::path& get_source_filename() const { return(this->filename); }

    //! get filename of translated core
    const boost::filesystem::path& get_core_filename() const { return(this->core_output); }

    //! get filename of translated implementation
    const boost::filesystem::path& get_implementation_filename() const { return(this->implementation_output); }

    //! get filename of translated module constructor file
    const boost::filesystem::path& get_constructor_filename() const { return(this->constructor_output); }

    //! get filename of the sample output file
    const boost::filesystem::path& get_sampling_filename() const { return(this->sampling_output); }

    //! get filename of the sample values file
    const boost::filesystem::path& get_sampling_values_filename() const { return(this->sampling_values_output); }

    //! get filename of the sample priors file
    const boost::filesystem::path& get_sampling_priors_filename() const { return(this->sampling_priors_output); }

    //! get filename of the sample mcmc file
    const boost::filesystem::path& get_sampling_mcmc_filename() const { return(this->sampling_mcmc_output); }

    //! get filename of the sample cmake file
    const boost::filesystem::path& get_sampling_cmake_filename() const { return(this->sampling_cmake_output); }

    //! get filename of the sample GetDist Python output file
    const boost::filesystem::path& get_sampling_getdist_python_filename() const { return(this->sampling_getdist_python_output); }

    //! get filename of the sample GetDist LaTeX output file
    const boost::filesystem::path& get_sampling_getdist_latex_filename() const { return(this->sampling_getdist_latex_output); }

    //! get header guard for translated core
    const std::string& get_core_guard() const { return(this->core_guard); }

    //! get header guard for translated implementation
    const std::string& get_implementation_guard() const { return(this->implementation_guard); }

    //! get header guard for translated core
    const std::string& get_constructor_guard() const { return(this->constructor_guard); }

    //! get header guard for the sampling output file
    const std::string& get_sampling_guard() const { return(this->sampling_guard); }

    // GET CONFIGURATION OPTIONS

  public:

    //! perform common-subexpression elimination?
    bool do_cse() const;

    //! annotate output?
    bool annotate() const;

    //! get size of index assignment at which unrolling is disabled
    unsigned int unroll_policy() const;

    //! get fast option
    bool fast() const;

    
    // PASS-THROUGH TO UNDERLYING MODEL DESCRIPTOR
    
  public:
    
    //! access to model descriptor
    model_descriptor& root;
    
    //! direct access to Lagrangian block
    lagrangian_block& model;
    
    //! direct access to metadata block
    metadata_block& meta;
    
    //! direct access to templates block
    templates_block& templates;
    
    //! direct access to misc block
    misc_block& misc;

    // direct access to the cosmology_block block
    cosmology_block& cosmo;



    // MESSAGING INTERFACE

  public:

    //! report a message
    void message(const std::string& m) { this->msg(m); }


    // GET POLICY OBJECTS ASSOCIATED WITH TRANSLATION UNIT

  public:

    //! get finder
    finder& get_finder() { return this->search_paths; }

    //! get output stack
    output_stack& get_stack() { return this->outstack; }

    //! get symbol factory
    symbol_factory& get_symbol_factory() { return this->sym_factory; }
    
    //! get version policy repository
    version_policy& get_version_policy() { return this->policy; }
    
    //! get argument cache
    argument_cache& get_argument_cache() { return this->cache; }


    // INTERNAL DATA

  private:

    //! filename of model description
    boost::filesystem::path filename;

    //! message handler
    message_handler msg;


    // HANDLERS FOR ERROR_CONTEXT

    //! error handler
    error_context::error_handler err;

    //! warning handler
    error_context::warning_handler wrn;


    // POLICY OBJECTS

    //! finder
    finder& search_paths;

    //! output_stack
    output_stack& outstack;

    //! symbol factory
    symbol_factory& sym_factory;

    //! argument cache
    argument_cache& cache;
    
    //! version policy registry
    version_policy& policy;


    // CORE, IMPLEMENTATION

    //! core output file name
    boost::filesystem::path core_output;

    //! implementation output file name
    boost::filesystem::path implementation_output;

    //! module constructor output file name
    boost::filesystem::path constructor_output;

    //! sampling output file
    boost::filesystem::path sampling_output;

    //! model_mcmc.ini output file
    boost::filesystem::path sampling_mcmc_output;

    //! values.ini output file
    boost::filesystem::path sampling_values_output;

    //! priors.ini output file
    boost::filesystem::path sampling_priors_output;

    //! CppTSample CMakeLists.txt output file
    boost::filesystem::path sampling_cmake_output;

    //! CppTSample GetDist Python plotting output file
    boost::filesystem::path sampling_getdist_python_output;

    //! CppTSample GetDist LaTeX paramnames output file
    boost::filesystem::path sampling_getdist_latex_output;

    //! core header guard
    std::string core_guard;

    //! implementation header guard
    std::string implementation_guard;

    //! model constructor header guard
    std::string constructor_guard;

    //! CppTSample sampling header guard
    std::string sampling_guard;
  };


#endif //CPPTRANSPORT_TRANSLATOR_DATA_H
