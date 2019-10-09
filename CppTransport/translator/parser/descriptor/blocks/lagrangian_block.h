//
// Created by David Seery on 24/05/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_LAGRANGIAN_BLOCK_H
#define CPPTRANSPORT_LAGRANGIAN_BLOCK_H


#include "contexted_value.h"
#include "model_settings.h"
#include "y_common.h"

#include "symbol_tables.h"
#include "declarations.h"

#include "symbol_factory.h"
#include "symbol_list.h"
#include "version_policy.h"
#include "disable_warnings.h"

#include "ginac/ginac.h"


class lagrangian_block
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    lagrangian_block(symbol_factory& s, version_policy& vp, error_context err_ctx);

    //! destructor is default
    ~lagrangian_block() = default;
    
    
    // GENERAL SERVICES
    
  public:
    
    //! perform validation
    validation_exceptions validate() const;

    //! factory function to make field_metric_base
    std::shared_ptr<field_metric_base> make_field_metric_base() const;


    // SYMBOL SERVICES

  public:

    //! add symbol representing a field
    bool add_field(const std::string& n, symbol_wrapper& s, const y::lexeme_type& p, std::shared_ptr<attributes> a);

    //! add symbol representing a parameter
    bool add_parameter(const std::string& n, symbol_wrapper& s, const y::lexeme_type& p, std::shared_ptr<attributes> a);

    //! add symbol representing a subexpression
    bool add_subexpr(const std::string& n, symbol_wrapper& s, const y::lexeme_type& p, std::shared_ptr<subexpr> e);

    //! check whether a given symbol name has been declared
    //! if the symbol exists, returns a boost::optional<> containing a reference
    //! to its declaration
    boost::optional<const declaration&> check_symbol_exists(const std::string& nm) const;
    
    //! freeze all tables
    void freeze_tables(const y::lexeme_type& l);
    
  protected:
    
    //! report that tables are frozen
    bool report_frozen(const y::lexeme_type& l);


    // LAGRANGIAN MANAGEMENT

  public:

    //! set the type of the Lagrangian
    void set_lagrangian_type(model_type t, const y::lexeme_type& l);
    
    //! get the type of the Lagrangian
    model_type get_lagrangian_type() const;
    
    //! set potential
    bool set_potential(const y::lexeme_type& l, std::shared_ptr<GiNaC::ex> V);

    //! get potential as a contexted value
    boost::optional< contexted_value< std::shared_ptr<GiNaC::ex> > > get_potential() const;
    
    //! set field-space metric
    bool set_metric(const y::lexeme_type& l, std::shared_ptr<field_metric> f);
    
    //! get field-space metric as contexted value
    boost::optional< contexted_value< std::shared_ptr<field_metric> > > get_metric() const;
    
    
    // FIELDS AND PARAMETERS

  public:

    //! get number of fields in the model
    unsigned int get_number_fields() const;

    //! get number of parameters in the model
    unsigned int get_number_params() const;

    //! get std::vector<> of field names
    std::vector<std::string> get_field_name_list() const;

    //! get_std::vector<> of LaTeX field names
    std::vector<std::string> get_field_latex_list() const;

    //! get std::vector<> of parameter names
    std::vector<std::string> get_param_name_list() const;

    //! get std::vector<> of LaTeX parameter names
    std::vector<std::string> get_param_latex_list() const;

    //! get list of symbols for fields
    symbol_list get_field_symbols() const;

    //! get list of symbols for field derivatives
    symbol_list get_deriv_symbols() const;

    //! get list of symbols for fields
    symbol_list get_field_deriv() const;

    //! get list of symbols for the field initial value
    symbol_list get_field_val() const;

    //! get list of symbols of prior names
    symbol_list get_prior_names() const;

    //! get list of symbols of prior names with latex
    symbol_list get_prior_latex() const;

    //! get list of symbols for the field initial derivative value
    symbol_list get_field_derivval() const;

    //! get list of symbols for the field initial prior
    symbol_list get_field_prior() const;

    //! get list of symbols for the field initial derivative prior
    symbol_list get_field_derivprior() const;

    //! get list of symbols for parameters
    symbol_list get_param_symbols() const;

    //! get list of symbols for the parameters values
    symbol_list get_param_values() const;

    //! get list of symbols for the parameters priors
    symbol_list get_param_priors() const;

    //! get symbol for Planck mass M_P
    const symbol_wrapper& get_Mp_symbol() const;


    // INTERNAL DATA

  private:
    
    // POLICY OBJECTS
    
    //! version policy registry
    version_policy& policy;
    

    // LAGRANGIAN DATA
    
    // TABLES

    //! symbol table: fields
    field_symbol_table fields;

    //! symbol list: names of all priors for cosmosis
    symbol_list priors;

    //! symbol list: names of all priors and associated LaTeX string for cosmosis
    symbol_list priors_with_latex;

    //! symbol list: names of field derivatives
    symbol_list fields_deriv;

    //! symbol list: names of field values for cosmosis
    symbol_list fields_vals;

    //! symbol list: names of field derivative values for cosmosis
    symbol_list fields_derivvals;

    //! symbol list: names of field priors for cosmosis
    symbol_list fields_priors;

    //! symbol list: names of field derivative priors for cosmosis
    symbol_list fields_derivpriors;

    //! symbol table: parameters
    parameter_symbol_table parameters;

    //! symbol list: names of parameters values for cosmosis
    symbol_list params_values;

    //! symbol list: names of parameters priors for cosmosis
    symbol_list params_priors;

    //! symbol table: reserved symbols (such as the Planck mass)
    parameter_symbol_table reserved;

    //! symbol table: subexpressions
    subexpr_symbol_table subexprs;
    
    //! flag to indicate whether symbol tables have been frozen -- can't define new fields or
    //! parameters after the potential or field-space metric have been specified
    std::unique_ptr< contexted_value<unsigned int> > symbols_frozen;
    
    // SETTINGS
    
    //! type of model
    std::unique_ptr< contexted_value<model_type> > type;

    //! store details of potentials
    std::unique_ptr< contexted_value< std::shared_ptr<GiNaC::ex> > > potential;
    
    //! store deatils of field-space metric
    std::unique_ptr< contexted_value< std::shared_ptr<field_metric> > > metric;


    // SYMBOL SERVICES

    //! symbol factory
    symbol_factory& sym_factory;

    //! symbol list: names of field derivatives
    symbol_list deriv_symbols;


    // SPECIAL RESERVED SYMBOLS

    //! symbol representing Planck mass
    symbol_wrapper M_Planck;

  };


#endif //CPPTRANSPORT_LAGRANGIAN_BLOCK_H
