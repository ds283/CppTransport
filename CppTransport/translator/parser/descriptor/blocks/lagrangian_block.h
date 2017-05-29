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
// --@@
//

#ifndef CPPTRANSPORT_LAGRANGIAN_BLOCK_H
#define CPPTRANSPORT_LAGRANGIAN_BLOCK_H


#include "contexted_value.h"
#include "model_settings.h"
#include "y_common.h"

#include "declarations.h"
#include "symbol_database.h"

#include "symbol_factory.h"
#include "symbol_list.h"
#include "version_policy.h"
#include "disable_warnings.h"

#include "ginac/ginac.h"


class lagrangian_block
  {

    // TYPES

  protected:

    //! typedef for field symbol table
    typedef symbol_database<field_declaration> field_symbol_table;

    //! typedef for parameter symbol table
    typedef symbol_database<parameter_declaration> parameter_symbol_table;

    //! typedef for subexpression symbol table
    typedef symbol_database<subexpr_declaration> subexpr_symbol_table;


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


    // SYMBOL SERVICES

  public:

    //! add symbol representing a field
    bool add_field(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, attributes& a);

    //! add symbol representing a parameter
    bool add_parameter(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, attributes& a);

    //! add symbol representing a subexpression
    bool add_subexpr(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, subexpr& e);

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
    bool set_potential(GiNaC::ex& V, const y::lexeme_type& l);

    //! get potential as a contexted value
    boost::optional< contexted_value<GiNaC::ex>& > get_potential() const;
    
    
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

    //! get list of symbols for parameters
    symbol_list get_param_symbols() const;

    //! get symbol for Planck mass M_P
    const GiNaC::symbol& get_Mp_symbol() const;


    // INTERNAL DATA

  private:
    
    // POLICY OBJECTS
    
    //! version policy registry
    version_policy& policy;
    

    // LAGRANGIAN DATA
    
    // TABLES

    //! symbol table: fields
    field_symbol_table fields;

    //! symbol table: parameters
    parameter_symbol_table parameters;

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
    std::unique_ptr< contexted_value<GiNaC::ex> > potential;


    // SYMBOL SERVICES

    //! symbol factory
    symbol_factory& sym_factory;

    //! symbol list: names of field derivatives
    symbol_list deriv_symbols;


    // SPECIAL RESERVED SYMBOLS

    //! symbol representing Planck mass
    GiNaC::symbol M_Planck;

  };


#endif //CPPTRANSPORT_LAGRANGIAN_BLOCK_H
