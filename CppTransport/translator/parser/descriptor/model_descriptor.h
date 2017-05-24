//
// Created by David Seery on 18/06/2013.
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


#ifndef CPPTRANSPORT_MODEL_DESCRIPTOR_H
#define CPPTRANSPORT_MODEL_DESCRIPTOR_H


#include "lagrangian_block.h"
#include "metadata_block.h"
#include "templates_block.h"
#include "misc_block.h"


class model_descriptor
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor;
    //! symbol_factory is inherited from parent translation_unit
    //! error_context is passed down from parent translation_unit and is used to construct
    //! fake error contexts for default reserved symbols such as M_Planck
    model_descriptor(symbol_factory& s, error_context err_ctx);

    //! destructor is default
    ~model_descriptor() = default;


    // INTERFACE

  public:

    //! detect error condition
    bool failed() const { return this->err_count > 0; }
    
    
    // SYMBOL SERVICES
  
  public:
    
    //! add symbol representing a field
    bool add_field(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, attributes& a) { return this->lag.add_field(n,s,p,a); }
    
    //! add symbol representing a parameter
    bool add_parameter(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, attributes& a) { return this->lag.add_parameter(n,s,p,a); }
    
    //! add symbol representing a subexpression
    bool add_subexpr(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, subexpr& e) { return this->lag.add_subexpr(n,s,p,e); }
    
    //! check whether a given symbol name has been declared
    //! if the symbol exists, returns a boost::optional<> containing a reference
    //! to its declaration
    boost::optional<declaration&> check_symbol_exists(const std::string& nm) const { return this->lag.check_symbol_exists(nm); }
    
    
    // LAGRANGIAN MANAGEMENT
  
  public:
    
    //! set potential
    bool set_potential(GiNaC::ex& V, const y::lexeme_type& l) { return this->lag.set_potential(V,l); }
    
    //! get potential as a contexted value
    boost::optional< contexted_value<GiNaC::ex>& > get_potential() const { return this->lag.get_potential(); }
    
    //! unset potential
    void unset_potential() { this->lag.unset_potential(); }
    
    
    // FIELDS AND PARAMETERS
  
  public:
    
    //! get number of fields in the model
    unsigned int get_number_fields() const { return this->lag.get_number_fields(); }
    
    //! get number of parameters in the model
    unsigned int get_number_params() const { return this->lag.get_number_params(); }
    
    //! get std::vector<> of field names
    std::vector<std::string> get_field_name_list() const { return this->lag.get_field_name_list(); }
    
    //! get_std::vector<> of LaTeX field names
    std::vector<std::string> get_field_latex_list() const { return this->lag.get_field_latex_list(); }
    
    //! get std::vector<> of parameter names
    std::vector<std::string> get_param_name_list() const { return this->lag.get_param_name_list(); }
    
    //! get std::vector<> of LaTeX parameter names
    std::vector<std::string> get_param_latex_list() const { return this->lag.get_param_latex_list(); }
    
    //! get list of symbols for fields
    symbol_list get_field_symbols() const { return this->lag.get_field_symbols(); }
    
    //! get list of symbols for field derivatives
    symbol_list get_deriv_symbols() const { return this->lag.get_deriv_symbols(); }
    
    //! get list of symbols for parameters
    symbol_list get_param_symbols() const { return this->lag.get_param_symbols(); }
    
    //! get symbol for Planck mass M_P
    const GiNaC::symbol& get_Mp_symbol() const { return this->lag.get_Mp_symbol(); }
    
    
    // GET AND SET BASIC METADATA
  
  public:
    
    //! get model name (a textual string)
    bool set_name(const std::string& n, const y::lexeme_type& l) { return this->meta.set_name(n,l); }
    
    //! get model name as contexted value
    boost::optional< contexted_value<std::string>& > get_name() const { return this->meta.get_name(); }
    
    
    //! add an author
    bool add_author(const std::string& n, const y::lexeme_type& l, author& a) { return this->meta.add_author(n,l,a); }
    
    //! get author table
    const author_table& get_author() const { return this->meta.get_author(); }
    
    
    //! set citation guidance
    bool set_citeguide(const std::string& t, const y::lexeme_type& l) { return this->meta.set_citeguide(t,l); }
    
    //! get citation guidance as contexted value
    boost::optional< contexted_value<std::string>& > get_citeguide() const { return this->meta.get_citeguide(); }
    
    
    //! set model description
    bool set_description(const std::string& t, const y::lexeme_type& l) { return this->meta.set_description(t,l); }
    
    //! get model description as contexted value
    boost::optional< contexted_value<std::string>& > get_description() const { return this->meta.get_description(); }
    
    
    //! set model revision
    bool set_revision(int r, const y::lexeme_type& l) { return this->meta.set_revision(r,l); }
    
    //! get model revision as contexted value
    boost::optional< contexted_value<unsigned int>& > get_revision() const { return this->meta.get_revision(); }
    
    
    //! set model license string
    bool set_license(const std::string& t, const y::lexeme_type& l) { return this->meta.set_license(t,l); }
    
    //! get model license string as contexted value
    boost::optional< contexted_value<std::string>& > get_license() const { return this->meta.get_license(); }
    
    
    //! set model reference list
    bool set_references(const std::vector<contexted_value<std::string> >& refs) { return this->meta.set_references(refs); }
    
    //! get model reference list as contexted value
    boost::optional< std::vector< contexted_value<std::string> >& > get_references() const { return this->get_references(); }
    
    
    //! set model URL list
    bool set_urls(const std::vector<contexted_value<std::string> >& urls) { return this->meta.set_urls(urls); }
    
    //! get model URL list as contexted value
    boost::optional< std::vector< contexted_value<std::string> >& > get_urls() const { return this->meta.get_urls(); }
    
    
    // MISCELLANEOUS SETTINGS
  
  public:
    
    //! set index order flag (left-most first or right-most first)
    void set_indexorder(index_order o) { return this->misc.set_indexorder(o); }
    
    //! get index order flag
    index_order get_indexorder() const { return this->misc.get_indexorder(); }
    
    
    // TEMPLATE SPECIFICATION
  
  public:
    
    //! set core template name
    bool set_core(const std::string& c, const y::lexeme_type& l) { return this->templ.set_core(c,l); }
    
    //! get core template name as contexted value
    boost::optional< contexted_value<std::string>& > get_core() const { return this->templ.get_core(); }
    
    
    //! set implementation template name
    bool set_implementation(const std::string& i, const y::lexeme_type& l) { return this->templ.set_implementation(i,l); }
    
    //! get implementation template name as contexted value
    boost::optional< contexted_value<std::string>& > get_implementation() const { return this->templ.get_implementation(); }
    
    
    //! set model tag (an identifier)
    bool set_model(const std::string& m, const y::lexeme_type& l) { return this->templ.set_model(m,l); }
    
    //! get model tag as a contexted value
    boost::optional< contexted_value<std::string>& > get_model() const { return this->templ.get_model(); }
    
    
    // IMPLEMENTATION DATA
  
  public:
    
    //! set background stepper name
    bool set_background_stepper(stepper& s, const y::lexeme_type& l) { return this->templ.set_background_stepper(s,l); }
    
    //! get background stepper name as contexted value
    boost::optional< contexted_value<stepper>& > get_background_stepper() const { return this->templ.get_background_stepper(); }
    
    
    //! set perturbations stepper name
    bool set_perturbations_stepper(stepper& s, const y::lexeme_type& l) { return this->templ.set_perturbations_stepper(s,l); }
    
    //! get perturbations stepper name as contexted value
    boost::optional< contexted_value<stepper>& > get_perturbations_stepper() const { return this->templ.get_perturbations_stepper(); }


		// INTERNAL DATA

  private:
    
    //! count of errors encountered
    unsigned err_count;


    // FUNCTIONALITY BLOCKS

    //! Lagrangian management
    lagrangian_block lag;

    //! Metadata management
    metadata_block meta;

    //! Template management
    templates_block templ;

    //! Miscellaneous settings
    misc_block misc;

	};


#endif //CPPTRANSPORT_MODEL_DESCRIPTOR_H
