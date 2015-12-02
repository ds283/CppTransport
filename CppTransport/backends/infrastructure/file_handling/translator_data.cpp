//
// Created by David Seery on 30/11/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#include "translator_data.h"


translator_data::translator_data(const boost::filesystem::path& file,
                                 error_context::error_handler e, error_context::warning_handler w,
                                 message_handler m, finder& f, output_stack& os, symbol_factory& s, y::y_driver& drv,
                                 argument_cache& c)
  : filename(file),
    err(e),
    wrn(w),
    msg(m),
    search_paths(f),
    o_stack(os),
    sym_factory(s),
    driver(drv),
    cache(c)
  {
  }


bool translator_data::get_do_cse() const
  {
    return(this->cache.do_cse());
  }


boost::optional< contexted_value<std::string>& > translator_data::get_name() const
  {
    return(this->driver.get_script().get_name());
  }


boost::optional< contexted_value<std::string>& > translator_data::get_author() const
  {
    return(this->driver.get_script().get_author());
  }


boost::optional< contexted_value<std::string>& > translator_data::get_model() const
  {
    return(this->driver.get_script().get_model());
  }


boost::optional< contexted_value<std::string>& > translator_data::get_tag() const
  {
    return(this->driver.get_script().get_tag());
  }


unsigned int translator_data::get_number_fields() const
  {
    return(this->driver.get_script().get_number_fields());
  }


unsigned int translator_data::get_number_parameters() const
  {
    return(this->driver.get_script().get_number_params());
  }


enum indexorder translator_data::get_index_order() const
  {
    return(this->driver.get_script().get_indexorder());
  }


const GiNaC::symbol& translator_data::get_Mp_symbol() const
  {
    return(this->driver.get_script().get_Mp_symbol());
  }


const GiNaC::ex translator_data::get_potential() const
  {
    return(this->driver.get_script().get_potential());
  }


const std::vector<GiNaC::symbol> translator_data::get_field_symbols() const
  {
    return(this->driver.get_script().get_field_symbols());
  }


const std::vector<GiNaC::symbol> translator_data::get_deriv_symbols() const
  {
    return(this->driver.get_script().get_deriv_symbols());
  }


const std::vector<GiNaC::symbol> translator_data::get_parameter_symbols() const
  {
    return(this->driver.get_script().get_param_symbols());
  }


const std::vector<std::string> translator_data::get_field_list() const
  {
    return(this->driver.get_script().get_field_list());
  }


const std::vector<std::string> translator_data::get_latex_list() const
  {
    return(this->driver.get_script().get_latex_list());
  }


const std::vector<std::string> translator_data::get_param_list() const
  {
    return(this->driver.get_script().get_param_list());
  }


const std::vector<std::string> translator_data::get_platx_list() const
  {
    return(this->driver.get_script().get_platx_list());
  }


const struct stepper& translator_data::get_background_stepper() const
  {
    return(this->driver.get_script().get_background_stepper());
  }


const struct stepper& translator_data::get_perturbations_stepper() const
  {
    return(this->driver.get_script().get_perturbations_stepper());
  }


void translator_data::set_core_implementation(const boost::filesystem::path& co, const std::string& cg,
                                              const boost::filesystem::path& io, const std::string& ig)
  {
    this->core_output           = co;
    this->core_guard            = cg;
    this->implementation_output = io;
    this->implementation_guard  = ig;
  }
