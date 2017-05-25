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
// --@@
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


bool translator_data::do_cse() const
  {
    return(this->cache.do_cse());
  }


bool translator_data::annotate() const
  {
    return(this->cache.annotate());
  }


unsigned int translator_data::unroll_policy() const
  {
    return(this->cache.unroll_policy());
  }


bool translator_data::fast() const
  {
    return(this->cache.fast());
  }


boost::optional< contexted_value<std::string>& > translator_data::get_name() const
  {
    return(this->driver.get_descriptor().meta.get_name());
  }


const author_table& translator_data::get_author() const
  {
    return(this->driver.get_descriptor().meta.get_author());
  }


boost::optional< contexted_value<std::string>& > translator_data::get_model() const
  {
    return(this->driver.get_descriptor().templates.get_model());
  }


boost::optional< contexted_value<std::string>& > translator_data::get_citeguide() const
  {
    return(this->driver.get_descriptor().meta.get_citeguide());
  }


boost::optional< contexted_value<std::string>& > translator_data::get_description() const
  {
    return(this->driver.get_descriptor().meta.get_description());
  }


boost::optional< contexted_value<std::string>& > translator_data::get_license() const
  {
    return(this->driver.get_descriptor().meta.get_license());
  }


boost::optional< contexted_value<unsigned int>& > translator_data::get_revision() const
  {
    return(this->driver.get_descriptor().meta.get_revision());
  }


boost::optional< std::vector< contexted_value<std::string> >& > translator_data::get_references() const
  {
    return(this->driver.get_descriptor().meta.get_references());
  }


boost::optional< std::vector< contexted_value<std::string> >& > translator_data::get_urls() const
  {
    return(this->driver.get_descriptor().meta.get_urls());
  }


unsigned int translator_data::get_number_fields() const
  {
    return(this->driver.get_descriptor().model.get_number_fields());
  }


unsigned int translator_data::get_number_parameters() const
  {
    return(this->driver.get_descriptor().model.get_number_params());
  }


index_order translator_data::get_index_order() const
  {
    return(this->driver.get_descriptor().misc.get_indexorder());
  }


const GiNaC::symbol& translator_data::get_Mp_symbol() const
  {
    return(this->driver.get_descriptor().model.get_Mp_symbol());
  }


boost::optional< contexted_value<GiNaC::ex>& > translator_data::get_potential() const
  {
    return(this->driver.get_descriptor().model.get_potential());
  }


const symbol_list translator_data::get_field_symbols() const
  {
    return(this->driver.get_descriptor().model.get_field_symbols());
  }


const symbol_list translator_data::get_deriv_symbols() const
  {
    return(this->driver.get_descriptor().model.get_deriv_symbols());
  }


const symbol_list translator_data::get_parameter_symbols() const
  {
    return(this->driver.get_descriptor().model.get_param_symbols());
  }


const std::vector<std::string> translator_data::get_field_list() const
  {
    return(this->driver.get_descriptor().model.get_field_name_list());
  }


const std::vector<std::string> translator_data::get_latex_list() const
  {
    return(this->driver.get_descriptor().model.get_field_latex_list());
  }


const std::vector<std::string> translator_data::get_param_list() const
  {
    return(this->driver.get_descriptor().model.get_param_name_list());
  }


const std::vector<std::string> translator_data::get_platx_list() const
  {
    return(this->driver.get_descriptor().model.get_param_latex_list());
  }


boost::optional< contexted_value<stepper>& > translator_data::get_background_stepper() const
  {
    return(this->driver.get_descriptor().templates.get_background_stepper());
  }


boost::optional< contexted_value<stepper>& > translator_data::get_perturbations_stepper() const
  {
    return(this->driver.get_descriptor().templates.get_perturbations_stepper());
  }


void translator_data::set_core_implementation(const boost::filesystem::path& co, const std::string& cg,
                                              const boost::filesystem::path& io, const std::string& ig)
  {
    this->core_output           = co;
    this->core_guard            = cg;
    this->implementation_output = io;
    this->implementation_guard  = ig;
  }
