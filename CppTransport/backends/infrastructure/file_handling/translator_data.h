//
// Created by David Seery on 30/11/2015.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_TRANSLATOR_DATA_H
#define CPPTRANSPORT_TRANSLATOR_DATA_H


#include <functional>

#include "finder.h"
#include "output_stack.h"
#include "model_settings.h"
#include "symbol_factory.h"
#include "y_driver.h"
#include "error_context.h"
#include "contexted_value.h"
#include "argument_cache.h"


typedef std::function<void(const std::string&)> message_handler;


class translator_data
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    translator_data(const boost::filesystem::path& file,
                    error_context::error_handler e, error_context::warning_handler w,
                    message_handler m, finder& f, output_stack& os, symbol_factory& s, y::y_driver& drv,
                    argument_cache& c);

    //! destructor is default
    ~translator_data() = default;


    // GET ERROR_CONTEXT HANDLERS

  public:

    //! get error handler
    error_context::error_handler& get_error_handler() { return(this->err); }

    //! get warning handler
    error_context::warning_handler& get_warning_handler() { return(this->wrn); }


    // SET CORE, IMPLEMENTATION DATA

  public:

    void set_core_implementation(const boost::filesystem::path& co, const std::string& cg,
                                 const boost::filesystem::path& io, const std::string& ig);


    // GET CORE, IMPLEMENTATION DATA

  public:

    //! get filename of model descriptor
    const boost::filesystem::path& get_model_input() const { return(this->filename); }

    //! get filename of translated core
    const boost::filesystem::path& get_core_output() const { return(this->core_output); }

    //! get filename of translated implementation
    const boost::filesystem::path& get_implementation_output() const { return(this->implementation_output); }

    //! get header guard for translated core
    const std::string& get_core_guard() const { return(this->core_guard); }

    //! get header guard for translated implementation
    const std::string& get_implementation_guard() const { return(this->implementation_guard); }


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


    // GET MODEL DATA

  public:

    boost::optional< contexted_value<std::string>& > get_name() const;

    boost::optional< contexted_value<std::string>& > get_author() const;

    boost::optional< contexted_value<std::string>& > get_model() const;

    boost::optional< contexted_value<std::string>& > get_tag() const;

    unsigned int     get_number_fields() const;

    unsigned int     get_number_parameters() const;

    enum index_order get_index_order() const;

    const GiNaC::symbol& get_Mp_symbol() const;

    const GiNaC::ex get_potential() const;

    const symbol_list get_field_symbols() const;

    const symbol_list get_deriv_symbols() const;

    const symbol_list get_parameter_symbols() const;

    const std::vector<std::string> get_field_list() const;

    const std::vector<std::string> get_latex_list() const;

    const std::vector<std::string> get_param_list() const;

    const std::vector<std::string> get_platx_list() const;

    const struct stepper& get_background_stepper() const;

    const struct stepper& get_perturbations_stepper() const;


    // MESSAGING INTERFACE

  public:

    //! report a message
    void message(const std::string& m) { this->msg(m); }


    // GET UTILITY OBJECTS ASSOCIATED WITH TRANSLATION UNIT

  public:

    //! get finder
    finder& get_finder() { return(this->search_paths); }

    //! get output stack
    output_stack& get_stack() { return(this->o_stack); }

    //! get symbol factory
    symbol_factory& get_symbol_factory() { return(this->sym_factory); }


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


    // UTILITY OBJECTS

    //! finder
    finder& search_paths;

    //! output_stack
    output_stack& o_stack;

    //! symbol factory
    symbol_factory& sym_factory;

    //! semantic driver
    y::y_driver& driver;

    //! argument cache
    argument_cache& cache;


    // CORE, IMPLEMENTATION

    //! core output file name
    boost::filesystem::path core_output;

    //! implementation output file name
    boost::filesystem::path implementation_output;

    //! core header guard
    std::string core_guard;

    //! implementation header guard
    std::string implementation_guard;

  };


#endif //CPPTRANSPORT_TRANSLATOR_DATA_H
